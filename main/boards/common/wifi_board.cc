#include "wifi_board.h"

#include "display.h"
#include "application.h"
#include "system_info.h"
#include "font_awesome_symbols.h"
#include "settings.h"
#include "assets/lang_config.h"
#include "board.h"
#include "language_runtime.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_http.h>
#include <esp_mqtt.h>
#include <esp_udp.h>
#include <tcp_transport.h>
#include <tls_transport.h>
#include <web_socket.h>
#include <esp_log.h>
#include <cJSON.h>

#include <wifi_station.h>
#include <wifi_configuration_ap.h>
#include <ssid_manager.h>

static const char *TAG = "WifiBoard";

WifiBoard::WifiBoard() {
    Settings settings("wifi", true);
    wifi_config_mode_ = settings.GetInt("force_ap") == 1;
    if (wifi_config_mode_) {
        ESP_LOGI(TAG, "force_ap is set to 1, reset to 0");
        settings.SetInt("force_ap", 0);
    }
}

std::string WifiBoard::GetBoardType() {
    return "wifi";
}

void WifiBoard::EnterWifiConfigMode() {
    auto& application = Application::GetInstance();
    application.SetDeviceState(kDeviceStateWifiConfiguring);

    auto& wifi_ap = WifiConfigurationAp::GetInstance();
    wifi_ap.SetLanguage(Lang::CODE);
    wifi_ap.SetSsidPrefix("Xiaozhi");
    wifi_ap.Start();

    // 显示 WiFi 配置 AP 的 SSID 和 Web 服务器 URL
    std::string hint = Lang::Strings::CONNECT_TO_HOTSPOT;
    hint += wifi_ap.GetSsid();
    hint += Lang::Strings::ACCESS_VIA_BROWSER;
    hint += wifi_ap.GetWebServerUrl();
    hint += "\n\n";
    
    // 播报配置 WiFi 的提示
    application.Alert(Lang::Strings::WIFI_CONFIG_MODE, hint.c_str(), "", Lang::Sounds::OGG_WIFICONFIG);
    
    // Wait forever until reset after configuration
    while (true) {
        int free_sram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        int min_free_sram = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);
        ESP_LOGI(TAG, "Free internal: %u minimal internal: %u", free_sram, min_free_sram);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void WifiBoard::StartNetwork() {
    // User can press BOOT button while starting to enter WiFi configuration mode
    if (wifi_config_mode_) {
        EnterWifiConfigMode();
        return;
    }

    // If no WiFi SSID is configured, enter WiFi configuration mode
    auto& ssid_manager = SsidManager::GetInstance();
    auto ssid_list = ssid_manager.GetSsidList();
    if (ssid_list.empty()) {
        wifi_config_mode_ = true;
        EnterWifiConfigMode();
        return;
    }

    auto& wifi_station = WifiStation::GetInstance();
    wifi_station.OnScanBegin([this]() {
        auto display = Board::GetInstance().GetDisplay();
        // 扫描阶段统一使用英文提示，直到确定目标 Wi-Fi 语言
        display->ShowNotification(Lang::Strings::SCANNING_WIFI, 30000);
    });
    wifi_station.OnConnect([this](const std::string& ssid) {
        auto display = Board::GetInstance().GetDisplay();
        // 如果即将连接的 SSID 与上次保存的一致且语言为中文，则在连接提示阶段就切到中文
        bool use_zh = LanguageRuntime::IsZhCNForSsid(ssid);
        std::string notification;
        if (use_zh) {
            notification = "正在连接 ";
        } else {
            notification = Lang::Strings::CONNECT_TO;
        }
        notification += ssid;
        notification += "...";
        display->ShowNotification(notification.c_str(), 30000);
    });
    wifi_station.OnConnected([this](const std::string& ssid) {
        auto display = Board::GetInstance().GetDisplay();
        bool use_zh = LanguageRuntime::IsZhCN();  // 此时已连接，可直接按当前语言环境
        std::string notification;
        if (use_zh) {
            notification = "已连接到 ";
        } else {
            notification = Lang::Strings::CONNECTED_TO;
        }
        notification += ssid;
        display->ShowNotification(notification.c_str(), 30000);
    });
    wifi_station.Start();

    // Try to connect to WiFi, if failed within 30 seconds, launch the WiFi configuration AP
    // 说明：
    // - 之前是 60 秒，时间较长，用户容易感觉“卡住”
    // - 现在改为 30 秒，更快进入配置热点模式，便于重新配置 Wi-Fi
    if (!wifi_station.WaitForConnected(30 * 1000)) {
        wifi_station.Stop();
        wifi_config_mode_ = true;
        EnterWifiConfigMode();
        return;
    }

    // Wi-Fi 已连接，基于外网 IP 自动检测一次语言环境（仅在 SSID 变化时才检测）
    auto ssid = wifi_station.GetSsid();
    // 需要读写语言设置，因此以 READWRITE 打开 NVS 命名空间
    Settings lang_settings("language", true);
    std::string last_ssid = lang_settings.GetString("last_ssid");
    std::string current_code = lang_settings.GetString("code");

    // 调试输出当前语言设置（不受日志等级影响）
    printf("Lang settings before GeoIP: ssid=\"%s\" last_ssid=\"%s\" code=\"%s\"\n",
           ssid.c_str(), last_ssid.c_str(), current_code.c_str());

    if (!ssid.empty() && ssid == last_ssid && !current_code.empty()) {
        printf("GeoIP skipped: SSID unchanged, keep language code: %s\n", current_code.c_str());
        ESP_LOGI(TAG, "SSID unchanged (%s), keep language code: %s", ssid.c_str(), current_code.c_str());
        return;
    }

    printf("GeoIP detecting language for SSID: %s\n", ssid.c_str());
    ESP_LOGI(TAG, "Detecting language by public IP for SSID: %s", ssid.c_str());
    auto& board = Board::GetInstance();
    std::unique_ptr<Http> http(board.CreateHttp());

    // 使用一个简单的 GeoIP 接口，根据返回的国家代码判断是否在中国
    const char* geoip_url = "http://ip-api.com/json/";
    if (!http->Open("GET", geoip_url)) {
        ESP_LOGW(TAG, "Failed to open GeoIP URL, fallback to compile-time language");
        return;
    }

    int status_code = http->GetStatusCode();
    std::string body = http->ReadAll();
    http->Close();

    if (status_code != 200) {
        printf("GeoIP HTTP status %d, body len=%zu\n", status_code, body.size());
        ESP_LOGW(TAG, "GeoIP HTTP status %d, body len=%zu", status_code, body.size());
        return;
    }

    std::string new_code = "en-US";  // 默认英文
    cJSON* root = cJSON_Parse(body.c_str());
    if (root != nullptr) {
        cJSON* country_code = cJSON_GetObjectItem(root, "countryCode");
        if (cJSON_IsString(country_code) && country_code->valuestring != nullptr) {
            std::string cc = country_code->valuestring;
            printf("GeoIP countryCode=%s\n", cc.c_str());
            ESP_LOGI(TAG, "GeoIP countryCode=%s", cc.c_str());
            if (cc == "CN") {
                new_code = "zh-CN";
            }
        }
        cJSON_Delete(root);
    } else {
        ESP_LOGW(TAG, "Failed to parse GeoIP JSON");
    }

    lang_settings.SetString("code", new_code);
    if (!ssid.empty()) {
        lang_settings.SetString("last_ssid", ssid);
    }
    printf("Language selected by IP: %s (SSID=%s)\n", new_code.c_str(), ssid.c_str());
    ESP_LOGI(TAG, "Language selected by IP: %s (SSID=%s)", new_code.c_str(), ssid.c_str());
}

Http* WifiBoard::CreateHttp() {
    return new EspHttp();
}

WebSocket* WifiBoard::CreateWebSocket() {
    Settings settings("websocket", false);
    std::string url = settings.GetString("url");
    if (url.find("wss://") == 0) {
        return new WebSocket(new TlsTransport());
    } else {
        return new WebSocket(new TcpTransport());
    }
    return nullptr;
}

Mqtt* WifiBoard::CreateMqtt() {
    return new EspMqtt();
}

Udp* WifiBoard::CreateUdp() {
    return new EspUdp();
}

const char* WifiBoard::GetNetworkStateIcon() {
    if (wifi_config_mode_) {
        return FONT_AWESOME_WIFI;
    }
    auto& wifi_station = WifiStation::GetInstance();
    if (!wifi_station.IsConnected()) {
        return FONT_AWESOME_WIFI_OFF;
    }
    int8_t rssi = wifi_station.GetRssi();
    if (rssi >= -60) {
        return FONT_AWESOME_WIFI;
    } else if (rssi >= -70) {
        return FONT_AWESOME_WIFI_FAIR;
    } else {
        return FONT_AWESOME_WIFI_WEAK;
    }
}

std::string WifiBoard::GetBoardJson() {
    // Set the board type for OTA
    auto& wifi_station = WifiStation::GetInstance();
    std::string board_json = std::string("{\"type\":\"" BOARD_TYPE "\",");
    board_json += "\"name\":\"" BOARD_NAME "\",";
    if (!wifi_config_mode_) {
        board_json += "\"ssid\":\"" + wifi_station.GetSsid() + "\",";
        board_json += "\"rssi\":" + std::to_string(wifi_station.GetRssi()) + ",";
        board_json += "\"channel\":" + std::to_string(wifi_station.GetChannel()) + ",";
        board_json += "\"ip\":\"" + wifi_station.GetIpAddress() + "\",";
    }
    board_json += "\"mac\":\"" + SystemInfo::GetMacAddress() + "\"}";
    return board_json;
}

void WifiBoard::SetPowerSaveMode(bool enabled) {
    auto& wifi_station = WifiStation::GetInstance();
    wifi_station.SetPowerSaveMode(enabled);
}

void WifiBoard::ResetWifiConfiguration() {
    // Set a flag and reboot the device to enter the network configuration mode
    {
        Settings settings("wifi", true);
        settings.SetInt("force_ap", 1);
    }
    GetDisplay()->ShowNotification(Lang::Strings::ENTERING_WIFI_CONFIG_MODE);
    vTaskDelay(pdMS_TO_TICKS(1000));
    // Reboot the device
    esp_restart();
}

std::string WifiBoard::GetDeviceStatusJson() {
    /*
     * 返回设备状态JSON
     * 
     * 返回的JSON结构如下：
     * {
     *     "audio_speaker": {
     *         "volume": 70
     *     },
     *     "screen": {
     *         "brightness": 100,
     *         "theme": "light"
     *     },
 

     *     "network": {
     *         "type": "wifi",
     *         "ssid": "Xiaozhi",
     *         "rssi": -60
     *     },
     *     "chip": {
     *         "temperature": 25
     *     }
     * }
     */
    auto& board = Board::GetInstance();
    auto root = cJSON_CreateObject();

    // Audio speaker
    auto audio_speaker = cJSON_CreateObject();
    auto audio_codec = board.GetAudioCodec();
    if (audio_codec) {
        cJSON_AddNumberToObject(audio_speaker, "volume", audio_codec->output_volume());
    }
    cJSON_AddItemToObject(root, "audio_speaker", audio_speaker);

    // Screen brightness
    auto backlight = board.GetBacklight();
    auto screen = cJSON_CreateObject();
    if (backlight) {
        cJSON_AddNumberToObject(screen, "brightness", backlight->brightness());
    }
    auto display = board.GetDisplay();
    if (display && display->height() > 64) { // For LCD display only
        cJSON_AddStringToObject(screen, "theme", display->GetTheme().c_str());
    }
    cJSON_AddItemToObject(root, "screen", screen);



    // Network
    auto network = cJSON_CreateObject();
    auto& wifi_station = WifiStation::GetInstance();
    cJSON_AddStringToObject(network, "type", "wifi");
    cJSON_AddStringToObject(network, "ssid", wifi_station.GetSsid().c_str());
    int rssi = wifi_station.GetRssi();
    if (rssi >= -60) {
        cJSON_AddStringToObject(network, "signal", "strong");
    } else if (rssi >= -70) {
        cJSON_AddStringToObject(network, "signal", "medium");
    } else {
        cJSON_AddStringToObject(network, "signal", "weak");
    }
    cJSON_AddItemToObject(root, "network", network);

    // Chip
    float esp32temp = 0.0f;
    if (board.GetTemperature(esp32temp)) {
        auto chip = cJSON_CreateObject();
        cJSON_AddNumberToObject(chip, "temperature", esp32temp);
        cJSON_AddItemToObject(root, "chip", chip);
    }

    auto json_str = cJSON_PrintUnformatted(root);
    std::string json(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);
    return json;
}

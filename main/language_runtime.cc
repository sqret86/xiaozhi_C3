#include "language_runtime.h"
#include "settings.h"
#include <wifi_station.h>

namespace LanguageRuntime {

bool IsZhCN() {
    Settings lang_settings("language", false);
    std::string code = lang_settings.GetString("code");
    if (code != "zh-CN") {
        return false;
    }

    // 只有当前 Wi-Fi 真的已连接时，才认为处于“中文环境”
    auto& wifi_station = WifiStation::GetInstance();
    return wifi_station.IsConnected();
}

bool IsZhCNForSsid(const std::string& ssid) {
    if (ssid.empty()) {
        return false;
    }
    Settings lang_settings("language", false);
    std::string code = lang_settings.GetString("code");
    if (code != "zh-CN") {
        return false;
    }
    std::string last_ssid = lang_settings.GetString("last_ssid");
    return !last_ssid.empty() && ssid == last_ssid;
}

}





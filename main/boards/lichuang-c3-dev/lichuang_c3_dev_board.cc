#include "wifi_board.h"
#include "audio_codecs/es8311_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "i2c_device.h"
#include "iot/thing_manager.h"

#include <esp_log.h>
#include <esp_lcd_panel_vendor.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>
#include <wifi_station.h>
#include <esp_err.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <malloc.h>

#define TAG "LichuangC3DevBoard"

LV_FONT_DECLARE(font_puhui_16_4);
LV_FONT_DECLARE(font_awesome_16_4);

class LichuangC3DevBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t codec_i2c_bus_;
    Button boot_button_;
    LcdDisplay* display_;

    void InitializeI2c() {
        // Initialize I2C peripheral
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = AUDIO_CODEC_I2C_SDA_PIN,
            .scl_io_num = AUDIO_CODEC_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &codec_i2c_bus_));
    }

    void InitializeSpi() {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_SPI_MOSI_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_SPI_SCK_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            // 在设备启动阶段按下 BOOT，强制进入 Wi-Fi 配置模式（无论之前是否已连接过 Wi-Fi）
            if (app.GetDeviceState() == kDeviceStateStarting) {
                ResetWifiConfiguration();
                return;
            }
            // 其余时间保持原有行为：短按 BOOT 作为对话/测试等控制键
            app.ToggleChatState();
        });
    }

    void InitializeDisplay() {
        ESP_LOGI(TAG, "Initializing ST7789 display for 1.69\" screen (240x280)");
        ESP_LOGI(TAG, "Display config: Width=%d, Height=%d, SwapXY=%s, MirrorX=%s, MirrorY=%s, OffsetX=%d, OffsetY=%d", 
                 DISPLAY_WIDTH, DISPLAY_HEIGHT, 
                 DISPLAY_SWAP_XY ? "true" : "false",
                 DISPLAY_MIRROR_X ? "true" : "false", 
                 DISPLAY_MIRROR_Y ? "true" : "false",
                 DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y);
        
        // Backlight GPIO will be configured by PwmBacklight class via LEDC (PWM)
        // Do NOT initialize GPIO here, it will conflict with LEDC

        // Initialize panel IO
        ESP_LOGI(TAG, "Install panel IO");
        esp_lcd_panel_io_handle_t panel_io = nullptr;
        esp_lcd_panel_io_spi_config_t io_config = {
            .cs_gpio_num = DISPLAY_SPI_CS_PIN,
            .dc_gpio_num = DISPLAY_DC_PIN,
            .spi_mode = 0,
            .pclk_hz = 20 * 1000 * 1000,  // 20MHz
            .trans_queue_depth = 10,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &panel_io));

        // Initialize ST7789 panel
        ESP_LOGI(TAG, "Install ST7789 panel driver");
        esp_lcd_panel_handle_t panel = nullptr;
        esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = -1,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
            .bits_per_pixel = 16,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io, &panel_config, &panel));
        
        // Reset and initialize panel
        ESP_LOGI(TAG, "Reset and initialize panel");
        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
        ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
        
        // Configure display orientation and properties
        ESP_LOGI(TAG, "Configure display properties");
        ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y));
        ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel, DISPLAY_SWAP_XY));
        ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel, true));
        
        // Set gap for 1.69" screen positioning
        ESP_LOGI(TAG, "Setting gap for proper display positioning: Y offset = %d", DISPLAY_OFFSET_Y);
        ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel, DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y));
        
        // Turn on display
        ESP_LOGI(TAG, "Turn on display");
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));
        
        // Backlight will be controlled by PwmBacklight (via GetBacklight()->SetBrightness())
        // Do NOT use gpio_set_level here, it conflicts with LEDC/PWM control
        
        // Create SpiLcdDisplay object
        ESP_LOGI(TAG, "Create SpiLcdDisplay object");
        display_ = new SpiLcdDisplay(panel_io, panel,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, 
                                    DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY,
                                    {
                                        .text_font = &font_puhui_16_4,
                                        .icon_font = &font_awesome_16_4,
                                        .emoji_font = font_emoji_32_init(),
                                    });
        ESP_LOGI(TAG, "ST7789 display initialization completed");
    }

    // 物联网初始化，添加对 AI 可见设备
    void InitializeIot() {
        auto& thing_manager = iot::ThingManager::GetInstance();
        thing_manager.AddThing(iot::CreateThing("Speaker"));
        thing_manager.AddThing(iot::CreateThing("Screen"));
    }

public:
    LichuangC3DevBoard() : boot_button_(BOOT_BUTTON_GPIO) {
        InitializeI2c();
        InitializeSpi();
        InitializeButtons();
        InitializeIot();
        GetBacklight()->SetBrightness(100);
        InitializeDisplay();
    }

    virtual AudioCodec* GetAudioCodec() override {
        static Es8311AudioCodec audio_codec(
            codec_i2c_bus_, 
            I2C_NUM_0, 
            AUDIO_INPUT_SAMPLE_RATE, 
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK, 
            AUDIO_I2S_GPIO_BCLK, 
            AUDIO_I2S_GPIO_WS, 
            AUDIO_I2S_GPIO_DOUT, 
            AUDIO_I2S_GPIO_DIN,
            AUDIO_CODEC_PA_PIN, 
            AUDIO_CODEC_ES8311_ADDR);
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        return display_;
    }
    
    virtual Backlight* GetBacklight() override {
        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        return &backlight;
    }
};

DECLARE_BOARD(LichuangC3DevBoard);

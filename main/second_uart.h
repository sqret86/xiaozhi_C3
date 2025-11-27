#ifndef SECOND_UART_H
#define SECOND_UART_H

#include <driver/uart.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>
#include <cstring>

// 第二串口配置 - ESP32C3只支持UART0和UART1
// 使用UART1，但配置到U0默认引脚GPIO21/20，释放GPIO18/19用于USB通信
#define SECOND_UART_NUM         UART_NUM_1
#define SECOND_UART_TX_PIN      21  // U0_TXD (原GPIO19改为GPIO21，避免占用USB D+)
#define SECOND_UART_RX_PIN      20  // U0_RXD (原GPIO18改为GPIO20，避免占用USB D-)
#define SECOND_UART_BAUD_RATE   115200
#define SECOND_UART_BUF_SIZE    1024

// 机器人控制指令
#define ROBOT_STAND_UP_CMD      "kup"
#define ROBOT_REST_CMD          "d"

class SecondUart {
private:
    static SecondUart* instance_;
    bool initialized_;
    std::string last_command_;
    
    SecondUart() : initialized_(false) {}
    
public:
    static SecondUart& GetInstance() {
        if (instance_ == nullptr) {
            instance_ = new SecondUart();
        }
        return *instance_;
    }
    
    // 初始化第二串口
    esp_err_t Initialize() {
        if (initialized_) {
            return ESP_OK;
        }
        
        // 首先确保卸载可能存在的UART驱动
        uart_driver_delete(SECOND_UART_NUM);
        
        // 配置UART参数 - ESP32C3使用UART1
        uart_config_t uart_config = {
            .baud_rate = SECOND_UART_BAUD_RATE,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 0,
            .source_clk = UART_SCLK_APB,
        };
        
        // 配置UART
        esp_err_t ret = uart_param_config(SECOND_UART_NUM, &uart_config);
        if (ret != ESP_OK) {
            ESP_LOGE("SecondUart", "Failed to configure UART1: %s", esp_err_to_name(ret));
            return ret;
        }
        
        // 安装UART驱动，设置TX和RX缓冲区（虽然只使用TX）
        ret = uart_driver_install(SECOND_UART_NUM, SECOND_UART_BUF_SIZE, SECOND_UART_BUF_SIZE, 0, NULL, 0);
        if (ret != ESP_OK) {
            ESP_LOGE("SecondUart", "Failed to install UART1 driver: %s", esp_err_to_name(ret));
            uart_driver_delete(SECOND_UART_NUM);
            return ret;
        }
        
        // 设置UART引脚
        ret = uart_set_pin(SECOND_UART_NUM, SECOND_UART_TX_PIN, SECOND_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        if (ret != ESP_OK) {
            ESP_LOGE("SecondUart", "Failed to set UART1 pins: %s", esp_err_to_name(ret));
            uart_driver_delete(SECOND_UART_NUM);
            return ret;
        }
        
        // 刷新UART缓冲区，确保干净的状态
        uart_flush(SECOND_UART_NUM);
        
        // 确保UART1不被用作控制台输出
        // 通过重新安装驱动并设置特定配置来隔离机器人控制通道
        
        initialized_ = true;
        ESP_LOGI("SecondUart", "Robot control UART1 initialized on pins TX:%d(U0_TXD), RX:%d(U0_RXD) - GPIO18/19 freed for USB", SECOND_UART_TX_PIN, SECOND_UART_RX_PIN);
        ESP_LOGI("SecondUart", "UART1 configured for robot control only - no system output should appear");
        
        return ESP_OK;
    }
    
    // 向第二串口发送数据
    void SendData(const char* data, size_t length) {
        if (!initialized_) {
            ESP_LOGW("SecondUart", "UART not initialized, attempting to initialize...");
            if (Initialize() != ESP_OK) {
                ESP_LOGE("SecondUart", "Failed to initialize UART");
                return;
            }
        }
        
        // 去重：若与上次完全相同则不重复发送
        if (last_command_.size() == length && memcmp(last_command_.data(), data, length) == 0) {
            ESP_LOGI("SecondUart", "Skip duplicate command: %.*s", (int)length, data);
            return;
        }

        int written = uart_write_bytes(SECOND_UART_NUM, data, length);
        if (written != length) {
            ESP_LOGW("SecondUart", "Only wrote %d bytes out of %d", written, length);
        } else {
            ESP_LOGI("SecondUart", "Sent robot command: %.*s (written %d bytes)", (int)length, data, written);
        }

        // 记录最近一次命令
        last_command_.assign(data, length);
    }
    
    // 向第二串口发送字符串
    void SendString(const std::string& str) {
        SendData(str.c_str(), str.length());
    }
    
    // 向第二串口发送字符串并添加换行
    void SendLine(const std::string& str) {
        std::string line = str + "\n";
        SendData(line.c_str(), line.length());
    }
    
    // 发送机器人起立指令 - 只发送纯指令，不包含换行符
    void SendStandUpCommand() {
        if (!initialized_) {
            ESP_LOGW("SecondUart", "UART not initialized, attempting to initialize...");
            if (Initialize() != ESP_OK) {
                ESP_LOGE("SecondUart", "Failed to initialize UART");
                return;
            }
        }
        
        // 去重：若与上次完全相同则不重复发送
        if (last_command_.size() == strlen(ROBOT_STAND_UP_CMD) && memcmp(last_command_.data(), ROBOT_STAND_UP_CMD, strlen(ROBOT_STAND_UP_CMD)) == 0) {
            ESP_LOGI("SecondUart", "Skip duplicate command: %s", ROBOT_STAND_UP_CMD);
            return;
        }

        // 强制刷新和清空UART缓冲区，确保完全清除任何残留数据
        uart_flush(SECOND_UART_NUM);
        vTaskDelay(pdMS_TO_TICKS(10));  // 等待10ms确保刷新完成
        
        // 重新刷新一次，确保绝对干净
        uart_flush(SECOND_UART_NUM);
        
        // 直接发送纯指令，不添加任何额外字符
        int written = uart_write_bytes(SECOND_UART_NUM, ROBOT_STAND_UP_CMD, strlen(ROBOT_STAND_UP_CMD));
        
        // 等待发送完成
        uart_wait_tx_done(SECOND_UART_NUM, pdMS_TO_TICKS(100));
        
        // 记录最近一次命令
        last_command_.assign(ROBOT_STAND_UP_CMD, strlen(ROBOT_STAND_UP_CMD));
        ESP_LOGI("SecondUart", "Sent pure stand up command: %s (written %d bytes)", ROBOT_STAND_UP_CMD, written);
    }
    
    // 发送机器人休息指令 - 只发送纯指令，不包含换行符
    void SendRestCommand() {
        if (!initialized_) {
            ESP_LOGW("SecondUart", "UART not initialized, attempting to initialize...");
            if (Initialize() != ESP_OK) {
                ESP_LOGE("SecondUart", "Failed to initialize UART");
                return;
            }
        }
        
        // 去重：若与上次完全相同则不重复发送
        if (last_command_.size() == strlen(ROBOT_REST_CMD) && memcmp(last_command_.data(), ROBOT_REST_CMD, strlen(ROBOT_REST_CMD)) == 0) {
            ESP_LOGI("SecondUart", "Skip duplicate command: %s", ROBOT_REST_CMD);
            return;
        }

        // 强制刷新和清空UART缓冲区，确保完全清除任何残留数据
        uart_flush(SECOND_UART_NUM);
        vTaskDelay(pdMS_TO_TICKS(10));  // 等待10ms确保刷新完成
        
        // 重新刷新一次，确保绝对干净
        uart_flush(SECOND_UART_NUM);
        
        // 直接发送纯指令，不添加任何额外字符
        int written = uart_write_bytes(SECOND_UART_NUM, ROBOT_REST_CMD, strlen(ROBOT_REST_CMD));
        
        // 等待发送完成
        uart_wait_tx_done(SECOND_UART_NUM, pdMS_TO_TICKS(100));
        
        // 记录最近一次命令
        last_command_.assign(ROBOT_REST_CMD, strlen(ROBOT_REST_CMD));
        ESP_LOGI("SecondUart", "Sent pure rest command: %s (written %d bytes)", ROBOT_REST_CMD, written);
    }
    
    // 反初始化（清理资源）
    void Deinitialize() {
        if (initialized_) {
            uart_driver_delete(SECOND_UART_NUM);
            initialized_ = false;
            ESP_LOGI("SecondUart", "Second UART deinitialized");
        }
    }
    
    ~SecondUart() {
        Deinitialize();
    }
};

// 静态成员声明（在头文件中）
// 定义将在对应的 .cc 文件中

// 便利函数，用于外部调用
namespace SecondUartUtils {
    // 初始化第二串口
    inline esp_err_t Init() {
        return SecondUart::GetInstance().Initialize();
    }
    
    // 发送机器人起立指令 - 只发送纯"kup"指令
    inline void SendStandUp() {
        SecondUart::GetInstance().SendStandUpCommand();
    }
    
    // 发送机器人休息指令 - 只发送纯"d"指令
    inline void SendRest() {
        SecondUart::GetInstance().SendRestCommand();
    }
}

// 宏定义，用于在现有代码中方便地添加机器人控制
#define ROBOT_STAND_UP() do { \
    SecondUart::GetInstance().SendStandUpCommand(); \
} while(0)

#define ROBOT_REST() do { \
    SecondUart::GetInstance().SendRestCommand(); \
} while(0)

#endif // SECOND_UART_H

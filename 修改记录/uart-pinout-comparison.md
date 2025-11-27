# ESP32-C3 UART引脚对比图

## 引脚配置对比

### 修改前配置 ❌

```
ESP32-C3                    机器人控制板
┌──────────┐               ┌──────────┐
│          │               │          │
│  GPIO18  ├──────────────►│   RX     │
│ (USB D-) │   UART1 RX    │          │
│          │               │          │
│  GPIO19  ├──────────────►│   TX     │
│ (USB D+) │   UART1 TX    │          │
│          │               │          │
└──────────┘               └──────────┘

⚠️ 问题：GPIO18/19被占用，USB功能受限
```

### 修改后配置 ✅

```
ESP32-C3                    机器人控制板
┌──────────┐               ┌──────────┐
│          │               │          │
│  GPIO18  │               │          │
│ (USB D-) │  空闲/USB用   │          │
│          │               │          │
│  GPIO19  │               │          │
│ (USB D+) │  空闲/USB用   │          │
│          │               │          │
│  GPIO20  ├──────────────►│   RX     │
│(U0_RXD)  │   UART1 RX    │          │
│          │               │          │
│  GPIO21  ├──────────────►│   TX     │
│(U0_TXD)  │   UART1 TX    │          │
│          │               │          │
└──────────┘               └──────────┘

✅ 优势：GPIO18/19释放，USB和UART同时可用
```

## 完整引脚功能表

| GPIO | 默认功能 | 修改前用途 | 修改后用途 | 状态 |
|------|----------|------------|------------|------|
| GPIO0 | BOOT | I2C SDA | I2C SDA | 无变化 |
| GPIO1 | - | I2C SCL | I2C SCL | 无变化 |
| GPIO2 | - | 背光PWM | 背光PWM | 无变化 |
| GPIO3 | - | SPI CLK | SPI CLK | 无变化 |
| GPIO4 | - | SPI MOSI | SPI MOSI | 无变化 |
| GPIO5 | - | SPI CS | SPI CS | 无变化 |
| GPIO6 | - | LCD DC | LCD DC | 无变化 |
| GPIO7 | - | I2S DOUT | I2S DOUT | 无变化 |
| GPIO8 | - | I2S WS | I2S WS | 无变化 |
| GPIO9 | - | BOOT按钮 | BOOT按钮 | 无变化 |
| GPIO10 | - | I2S DIN | I2S DIN | 无变化 |
| GPIO12 | - | I2S BCLK | I2S BCLK | 无变化 |
| GPIO13 | - | I2S MCLK | I2S MCLK | 无变化 |
| **GPIO18** | **USB D-** | **UART1 RX** ❌ | **USB专用** ✅ | **已释放** |
| **GPIO19** | **USB D+** | **UART1 TX** ❌ | **USB专用** ✅ | **已释放** |
| **GPIO20** | **U0_RXD** | **空闲** | **UART1 RX** ✅ | **新分配** |
| **GPIO21** | **U0_TXD** | **空闲** | **UART1 TX** ✅ | **新分配** |

## 信号流向图

### 数据流向

```
┌─────────────────────────────────────────────────────┐
│                    ESP32-C3                         │
│                                                     │
│  ┌────────┐        ┌────────┐        ┌──────┐     │
│  │ UART0  │───────►│ 系统   │        │ USB  │     │
│  │Console │        │ 日志   │        │Serial│     │
│  └────────┘        └────────┘        └───┬──┘     │
│                                           │        │
│  ┌────────┐        ┌────────┐        ┌───▼──┐     │
│  │ UART1  │───────►│ 机器人 │        │GPIO  │     │
│  │Robot   │        │ 指令   │        │18/19 │     │
│  └────┬───┘        └────────┘        └──────┘     │
│       │                                            │
│       │ GPIO21(TX)                                 │
│       │ GPIO20(RX)                                 │
└───────┼────────────────────────────────────────────┘
        │
        ▼
   ┌─────────┐
   │ 机器人  │
   │ 控制板  │
   └─────────┘
```

## 引脚电气特性

| 参数 | 值 | 说明 |
|------|-----|------|
| 电压等级 | 3.3V | GPIO电平 |
| UART波特率 | 115200 | 机器人控制串口 |
| 数据位 | 8 bits | UART配置 |
| 停止位 | 1 bit | UART配置 |
| 校验位 | None | 无校验 |
| 流控 | None | 无硬件流控 |

## 兼容性说明

### ✅ 兼容设备
- 立创ESP32-C3开发板
- 其他使用ESP32-C3的开发板
- 所有支持115200波特率的串口设备

### ⚠️ 注意事项
1. **电压兼容**: 确保控制板使用3.3V电平
2. **线序**: TX连RX，RX连TX（交叉连接）
3. **接地**: 务必共地(GND相连)
4. **电源**: 建议使用独立供电

## 接线示例

### 标准接线方式

```
ESP32-C3                     机器人控制板
┌──────────┐                ┌──────────┐
│ GPIO21   ├───────────────►│ RX       │ (交叉)
│ (TX)     │                │          │
│          │                │          │
│ GPIO20   │◄───────────────┤ TX       │ (交叉)
│ (RX)     │                │          │
│          │                │          │
│ GND      ├───────────────►│ GND      │ (共地)
│          │                │          │
└──────────┘                └──────────┘

注意：TX → RX, RX → TX (交叉连接)
```

### 错误接线 ❌

```
❌ 直连 (错误):
   ESP32-C3 TX → 控制板 TX
   ESP32-C3 RX → 控制板 RX

✅ 交叉 (正确):
   ESP32-C3 TX → 控制板 RX
   ESP32-C3 RX → 控制板 TX
```

## 测试验证

### 检查引脚配置

查看启动日志，应该看到：

```
I (xxx) SecondUart: Robot control UART1 initialized on pins TX:21(U0_TXD), RX:20(U0_RXD) - GPIO18/19 freed for USB
I (xxx) SecondUart: UART1 configured for robot control only - no system output should appear
```

### USB功能验证

```bash
# 检查USB设备
ls /dev/tty.usbserial* # macOS
ls /dev/ttyUSB* # Linux
```

### 串口通信测试

发送测试指令：
```c
SecondUart::GetInstance().SendString("kup");  // 起立
SecondUart::GetInstance().SendString("d");    // 休息
```

观察机器人是否正常响应。

## 故障排查

### 问题1: 机器人无响应

**可能原因:**
- 硬件未重新接线
- TX/RX接反了
- 未共地

**解决方法:**
1. 确认使用GPIO21/20
2. 检查交叉连接
3. 确保GND连接

### 问题2: USB无法识别

**可能原因:**
- GPIO18/19仍被其他功能占用
- USB线缆问题

**解决方法:**
1. 确认代码已更新
2. 重新编译烧录
3. 更换USB线缆

### 问题3: 编译错误

**可能原因:**
- 代码未正确更新

**解决方法:**
```bash
git pull
idf.py fullclean
idf.py build
```

## 参考链接

- [完整迁移文档](uart-pin-migration.md)
- [ESP32-C3技术手册](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_cn.pdf)
- [UART编程指南](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32c3/api-reference/peripherals/uart.html)


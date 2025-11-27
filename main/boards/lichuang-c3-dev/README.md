## 立创·实战派ESP32-C3开发板

1、开发板资料：https://wiki.lckfb.com/zh-hans/szpi-esp32c3

2、该开发板 flash 大小为 8MB，编译时注意选择合适的分区表：

```
Partition Table  --->
  Partition Table (Custom partition table CSV)  --->
  (partitions/v1/8m.csv) Custom partition CSV file
```

## UART引脚配置 (2025-10-22更新)

### 机器人控制串口 (UART1)

**当前配置：**
- TX引脚: **GPIO21** (U0_TXD位置)
- RX引脚: **GPIO20** (U0_RXD位置)
- 波特率: 115200

**历史修改：**
- 原配置：GPIO19(TX)、GPIO18(RX) - 与USB冲突
- 新配置：GPIO21(TX)、GPIO20(RX) - 释放USB引脚

⚠️ **硬件连接注意**：如果你使用机器人控制功能，请将控制板连接到GPIO21/20，而不是GPIO18/19。

📖 详细说明请参考：[UART引脚迁移文档](../../../修改记录/uart-pin-migration.md)

# 1.69寸 ST7789 V3 屏幕调试指南

## 当前配置状态

### 最新配置 (config.h)
- 分辨率: 240×280 (无swap_xy)
- 镜像: MirrorX=false, MirrorY=true
- 偏移: OffsetX=0, OffsetY=20
- 颜色反转: 关闭

### 初始化优化 (lichuang_c3_dev_board.cc)
- SPI模式: 0
- 时钟频率: 15MHz (降低提高稳定性)
- 添加了延时确保初始化稳定

## 系统性测试配置

如果当前配置不工作，请按顺序尝试以下配置组合：

### 配置组合 1 (当前)
```c
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  280
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y true
#define DISPLAY_SWAP_XY false
#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  20
// 颜色反转: false
```

### 配置组合 2 (如果组合1不工作)
```c
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  280
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY false
#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  20
// 同时修改代码：esp_lcd_panel_invert_color(panel, true);
```

### 配置组合 3 (使用swap_xy)
```c
#define DISPLAY_WIDTH   280
#define DISPLAY_HEIGHT  240
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY true
#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  20
// 颜色反转: false
```

### 配置组合 4 (经典竖屏配置)
```c
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  280
#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY false
#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0
// 颜色反转: true
```

### 配置组合 5 (零偏移测试)
```c
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  280
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y true
#define DISPLAY_SWAP_XY false
#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0
// 颜色反转: false
```

## 测试步骤

1. **修改配置**：在 `config.h` 中应用配置组合
2. **修改颜色反转**：在 `lichuang_c3_dev_board.cc` 中设置对应的颜色反转值
3. **编译烧录**：`idf.py build && idf.py flash monitor`
4. **观察日志**：查看配置输出和是否有错误
5. **检查显示**：是否有内容显示

## 调试输出示例

正常的日志输出应该类似：
```
I (xxx) LichuangC3DevBoard: Display config: Width=240, Height=280, SwapXY=false, MirrorX=false, MirrorY=true, OffsetX=0, OffsetY=20
I (xxx) LichuangC3DevBoard: Set color inversion: disabled (try for 1.69" screen)
I (xxx) LichuangC3DevBoard: ST7789 display initialization completed
```

## 高级调试

如果所有配置都不工作，可能需要：
1. 检查硬件连接（特别是DC、CS引脚）
2. 尝试更低的SPI时钟频率（10MHz或5MHz）
3. 添加硬件复位引脚（如果可用）
4. 检查屏幕是否真的是ST7789 V3

# UART引脚迁移更新日志

## [2025-10-22] GPIO引脚优化 - UART迁移

### 🎯 变更摘要
将机器人控制串口(UART1)从GPIO18/19迁移到GPIO21/20，释放USB通信引脚。

### 📝 详细变更

#### 修改的文件
1. **main/second_uart.h** - UART配置头文件

#### 引脚配置变更

| 项目 | 修改前 | 修改后 | 说明 |
|------|--------|--------|------|
| UART1 TX | GPIO19 | GPIO21 | 释放USB D+引脚 |
| UART1 RX | GPIO18 | GPIO20 | 释放USB D-引脚 |

### ⚠️ 破坏性变更

**硬件连接需要调整！**

如果你使用机器人控制功能，必须重新连接硬件：
- 将TX线从GPIO19改接到GPIO21
- 将RX线从GPIO18改接到GPIO20

### ✅ 影响范围

#### 受影响的功能
- 机器人控制指令发送 (需要重新接线)

#### 不受影响的功能
- 所有软件代码（无需修改）
- UART通信协议（保持不变）
- 其他GPIO功能（无冲突）

### 📚 相关文档
- [完整迁移文档](uart-pin-migration.md) - 详细的技术说明和过程
- [立创C3开发板说明](../main/boards/lichuang-c3-dev/README.md) - 板级配置
- [开发板指南](../main/boards/README.md) - 自定义开发板指南

### 🔍 技术背景

ESP32-C3的GPIO18/19是USB D-/D+引脚，用于：
- USB Serial/JTAG调试
- USB CDC通信
- 固件烧录

之前这两个引脚被UART1占用，导致USB功能受限。迁移到GPIO21/20后：
- USB功能完全可用
- UART1继续正常工作
- 无引脚冲突

### 👤 修改者
AI Assistant

### 📅 发布日期
2025年10月22日

---

## 如何升级

### 1. 更新代码
```bash
git pull
```

### 2. 重新编译
```bash
idf.py build
idf.py flash
```

### 3. 调整硬件连接
- 断开电源
- 将TX线从GPIO19移到GPIO21
- 将RX线从GPIO18移到GPIO20
- 重新上电测试

### 4. 验证功能
- 检查USB连接是否正常
- 测试机器人指令发送
- 查看串口日志确认初始化信息

---

## 回滚指南

如需回滚到旧版本，修改 `main/second_uart.h`:

```c
#define SECOND_UART_TX_PIN      19  // 回滚到GPIO19
#define SECOND_UART_RX_PIN      18  // 回滚到GPIO18
```

**注意**: 回滚后需要将硬件连接改回GPIO18/19。


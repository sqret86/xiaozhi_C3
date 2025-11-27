# 物联网控制模块

> ⚠️ **注意：本模块已不推荐使用。请使用"MCP协议"来实现物联网控制，获得更好的兼容性与功能支持。**

本模块实现了小智AI语音聊天机器人的物联网控制功能，使用户可以通过语音指令控制接入到ESP32开发板的各种物联网设备。

## 工作原理

整个物联网控制模块的工作流程如下：

1. **设备注册**：在开发板初始化阶段（如在`compact_wifi_board.cc`中），各种物联网设备通过`ThingManager`注册到系统中
2. **设备描述**：系统将设备描述信息（包括名称、属性、方法等）通过通信协议（如MQTT或WebSocket）发送给AI服务器
3. **用户交互**：用户通过语音与小智AI对话，表达控制物联网设备的意图
4. **命令执行**：AI服务器解析用户意图，生成控制命令，通过协议发送回ESP32，由`ThingManager`分发给对应的设备执行
5. **状态更新**：设备执行命令后，状态变化会通过`ThingManager`收集并发送回AI服务器，保持状态同步

## 核心组件

### ThingManager

`ThingManager`是物联网控制模块的核心管理类，采用单例模式实现：

- `AddThing`：注册物联网设备
- `GetDescriptorsJson`：获取所有设备的描述信息，用于向AI服务器报告设备能力
- `GetStatesJson`：获取所有设备的当前状态，可以选择只返回变化的部分
- `Invoke`：根据AI服务器下发的命令，调用对应设备的方法

### Thing

`Thing`是所有物联网设备的基类，提供了以下核心功能：

- 属性管理：通过`PropertyList`定义设备的可查询状态
- 方法管理：通过`MethodList`定义设备可执行的操作
- JSON序列化：将设备描述和状态转换为JSON格式，便于网络传输
- 命令执行：解析和执行来自AI服务器的指令

## 设备设计示例

### 屏幕（Screen）

屏幕是一个物联网设备示例，可以控制主题和亮度：

```cpp
class Screen : public Thing {
public:
    Screen() : Thing("Screen", "一个可以设置主题和亮度的屏幕") {
        // 定义属性：theme（当前主题）
        properties_.AddStringProperty("theme", "当前主题", [this]() -> std::string {
            auto theme = Board::GetInstance().GetDisplay()->GetTheme();
            return theme;
        });

        // 定义属性：brightness（当前亮度）
        properties_.AddNumberProperty("brightness", "当前亮度百分比", [this]() -> int {
            auto backlight = Board::GetInstance().GetBacklight();
            return backlight ? backlight->brightness() : 100;
        });

        // 定义方法：set_theme（设置主题）
        methods_.AddMethod("set_theme", "设置屏幕主题", ParameterList({
            Parameter("theme_name", "主题名称，可选值为'light'和'dark'", kValueTypeString, true)
        }), [this](const ParameterList& parameters) {
            std::string theme_name = static_cast<std::string>(parameters["theme_name"].string());
            auto display = Board::GetInstance().GetDisplay();
            if (display) {
                display->SetTheme(theme_name);
            }
        });
        
        // 定义方法：set_brightness（设置亮度）
        methods_.AddMethod("set_brightness", "设置屏幕亮度", ParameterList({
            Parameter("brightness", "0到100之间的整数", kValueTypeNumber, true)
        }), [this](const ParameterList& parameters) {
            uint8_t brightness = static_cast<uint8_t>(parameters["brightness"].number());
            auto backlight = Board::GetInstance().GetBacklight();
            if (backlight) {
                backlight->SetBrightness(brightness, true);
            }
        });
    }
};
```

用户可以通过语音指令如"小智，请把屏幕调暗一点"来控制屏幕亮度，或"小智，请切换到暗色主题"来设置主题。

### 扬声器（Speaker）

扬声器控制实现了音量调节功能：

```cpp
class Speaker : public Thing {
public:
    Speaker() : Thing("Speaker", "扬声器") {
        // 定义属性：volume（当前音量值）
        properties_.AddNumberProperty("volume", "当前音量值", [this]() -> int {
            auto codec = Board::GetInstance().GetAudioCodec();
            return codec->output_volume();
        });

        // 定义方法：SetVolume（设置音量）
        methods_.AddMethod("SetVolume", "设置音量", ParameterList({
            Parameter("volume", "0到100之间的整数", kValueTypeNumber, true)
        }), [this](const ParameterList& parameters) {
            auto codec = Board::GetInstance().GetAudioCodec();
            codec->SetOutputVolume(static_cast<uint8_t>(parameters["volume"].number()));
        });
    }
};
```

用户可以通过语音指令如"小智，把音量调到50"来控制扬声器的音量。

## 设计自定义物联网设备

要设计一个新的物联网设备，需要以下步骤：

1. **创建设备类**：继承`Thing`基类
2. **定义属性**：使用`properties_`添加设备的可查询状态
3. **定义方法**：使用`methods_`添加设备可执行的操作
4. **实现硬件控制**：在方法回调中实现对硬件的控制
5. **注册设备**：注册设备有两种方式（见下文），并在板级初始化中添加设备实例

### 两种设备注册方式

1. **使用DECLARE_THING宏**：适合通用设备类型
   ```cpp
   // 在设备实现文件末尾添加
   DECLARE_THING(MyDevice);
   
   // 然后在板级初始化中
   thing_manager.AddThing(iot::CreateThing("MyDevice"));
   ```

2. **直接创建设备实例**：适合特定于板级的设备
   ```cpp
   // 在板级初始化中
   auto my_device = new iot::MyDevice();
   thing_manager.AddThing(my_device);
   ```

### 设备实现位置建议

您可以根据设备的通用性选择不同的实现位置：

1. **通用设备**：放在`main/iot/things/`目录下，使用`DECLARE_THING`注册
2. **板级特定设备**：直接在板级目录(如`main/boards/your_board/`)中实现，使用直接创建实例的方式注册

这种灵活性允许您为不同的板设计特定的设备实现，同时保持通用设备的可重用性。

### 属性类型

物联网设备支持以下属性类型：

- **布尔值**（`kValueTypeBoolean`）：开关状态等
- **数值**（`kValueTypeNumber`）：温度、音量等
- **字符串**（`kValueTypeString`）：设备名称、状态描述等

### 方法参数

设备方法可以定义参数，支持以下参数类型：

- **布尔值**：开关等
- **数值**：温度、音量等
- **字符串**：命令、模式等

## 使用示例

在板级初始化代码（如`compact_wifi_board.cc`）中注册物联网设备：

```cpp
void InitializeIot() {
    auto& thing_manager = iot::ThingManager::GetInstance();
    thing_manager.AddThing(iot::CreateThing("Speaker"));
    thing_manager.AddThing(iot::CreateThing("Screen"));
}
```

之后，用户可以通过语音指令控制这些设备，例如：

- "小智，请调暗屏幕"
- "小智，切换到深色主题"
- "音量有点太小了"
- "把音量设置为80%"

AI服务器会将这些语音指令解析为对应的设备控制命令，通过协议发送给ESP32执行。

## 注意事项

### 大模型控制的随机性

由于语音控制由大型语言模型(LLM)处理，控制过程可能存在一定的随机性和不确定性。为了增强安全性和可靠性，请考虑以下建议：

1. **关键操作添加警示信息**：对于潜在危险或不可逆的操作，在方法描述中添加警示信息
   ```cpp
   methods_.AddMethod("PowerOff", "关闭系统电源[警告:此操作将导致设备完全关闭，请慎重使用]", 
                     ParameterList(), [this](const ParameterList& parameters) {
       // 关闭电源的实现
   });
   ```

2. **二次确认机制**：重要操作应在描述中明确要求二次确认
   ```cpp
   methods_.AddMethod("ResetToFactory", "恢复出厂设置[必须要用户二次确认]", 
                     ParameterList(), [this](const ParameterList& parameters) {
       // 恢复出厂设置的实现
   });
   ```

### 通信协议限制

当前IoT协议(1.0版本)存在以下限制：

1. **单向控制流**：大模型只能下发指令，无法立即获取指令执行结果
2. **状态更新延迟**：设备状态变更需要等到下一轮对话时，通过读取property属性值才能获知
3. **异步反馈**：如果需要操作结果反馈，必须通过设备属性的方式间接实现

### 最佳实践

1. **使用有意义的属性名称**：属性名称应清晰表达其含义，便于大模型理解和使用

2. **不产生歧义的方法描述**：为每个方法提供明确的自然语言描述，帮助大模型更准确地理解和调用
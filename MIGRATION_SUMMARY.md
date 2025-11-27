# Migration Summary - Version 2.0.4

## Board: lichuang-c3-dev

### Supported Features ✅

1. **Audio Processing**
   - ES8311 Audio Codec
   - Input/Output sample rate: 24000 Hz
   - Wake word detection (ESP-SR)
   - Audio encoding/decoding (Opus)

2. **Display**
   - ST7789 1.69" LCD (240x280)
   - LVGL GUI framework
   - Custom fonts and emojis support
   - Backlight control (PWM)

3. **Connectivity**
   - WiFi (ESP32-C3)
   - WebSocket protocol
   - MQTT protocol
   - HTTP/HTTPS client

4. **Robot Control**
   - UART communication (GPIO21/20)
   - Robot command extraction from TTS
   - Stand up/Rest actions

5. **Multi-language Support**
   - 38 languages with OGG audio files
   - Fallback to en-US for missing files
   - Easy language selection via `language_selection.h`

6. **OTA Updates**
   - Firmware upgrade from URL
   - Progress display during upgrade

7. **IoT Integration**
   - Speaker control
   - Screen control
   - MCP server support

### Removed Features ❌

1. **LED Control**
   - Reason: Board has no built-in LED (GPIO_NUM_NC)
   - Removed: `espressif/led_strip` dependency

2. **Camera**
   - Reason: Board has no camera hardware
   - Removed: `espressif/esp32-camera` dependency

3. **Battery Management**
   - Reason: Board has no battery monitoring
   - Removed: `low_battery.ogg` audio file
   - Removed: Battery-related code from board.h/board.cc

### Key Changes from Old Version

1. **Event System**
   - Added `DeviceStateEventManager` for state change events
   - Replaced timer-based callbacks with event-driven architecture
   - New events: `MAIN_EVENT_CLOCK_TICK`, `MAIN_EVENT_WAKE_WORD_DETECTED`, `MAIN_EVENT_VAD_CHANGE`, `MAIN_EVENT_ERROR`

2. **Audio Architecture**
   - Changed from P3 to OGG format
   - Added en-US fallback mechanism
   - Improved audio decoding with Opus header parsing

3. **Assets Management**
   - Added `Assets` class for resource management
   - Support for downloadable fonts, emojis, themes
   - Version checking and automatic download

4. **OTA Improvements**
   - Changed `Upgrade()` to return `bool`
   - Added `StartUpgradeFromUrl()` method
   - Better error handling and recovery

5. **Language Selection**
   - Added `language_selection.h` for easy language switching
   - CMake auto-detection of selected language
   - Support for 38 languages

### File Structure

```
xiaozhi/
├── LANGUAGE_SELECTION.md         # Language selection guide (English)
├── MIGRATION_SUMMARY.md           # This file
├── CMakeLists.txt                 # Version 2.0.4
├── main/
│   ├── language_selection.h       # Language configuration
│   ├── application.cc/h           # Updated with new event system
│   ├── assets.cc/h                # New: Resource management
│   ├── device_state_event.cc/h    # New: Event manager
│   ├── ota.cc/h                   # Updated OTA methods
│   ├── second_uart.cc/h           # Robot control (preserved)
│   ├── assets/
│   │   ├── common/                # Common sounds (exclamation, popup, success, vibration)
│   │   ├── locales/               # 38 language directories with OGG files
│   │   └── lang_config.h          # Auto-generated language config
│   ├── boards/
│   │   ├── common/
│   │   │   ├── board.h            # Updated: Removed GetLed, GetCamera, GetBatteryLevel
│   │   │   └── board.cc           # Updated: Removed unsupported feature implementations
│   │   └── lichuang-c3-dev/
│   │       ├── config.h           # Board-specific pin definitions
│   │       ├── lichuang_c3_dev_board.cc  # Board initialization
│   │       └── README.md          # Board documentation
│   └── idf_component.yml          # Updated dependencies (removed led_strip, esp32-camera)
└── scripts/
    └── gen_lang.py                # Updated for OGG and multi-language support
```

### Dependencies (idf_component.yml)

**Kept:**
- espressif/esp_codec_dev
- espressif/esp-sr
- espressif/button
- espressif/knob
- espressif/esp_lcd_* (various display drivers)
- lvgl/lvgl
- 78/esp-opus-encoder
- 78/esp-wifi-connect
- 78/xiaozhi-fonts
- And other necessary components

**Removed:**
- espressif/led_strip (no LED hardware)
- espressif/esp32-camera (no camera hardware)

### Build Instructions

1. **Select Language:**
   ```bash
   # Edit main/language_selection.h
   # Uncomment your desired language
   #define SELECTED_LANGUAGE "en-US"
   ```

2. **Build:**
   ```bash
   idf.py build
   idf.py flash
   ```

3. **Monitor:**
   ```bash
   idf.py monitor
   ```

### Testing Checklist

- [x] Audio input/output working
- [x] Display shows UI correctly
- [x] WiFi connection works
- [x] Wake word detection functional
- [x] Robot control via UART (if connected)
- [x] OTA upgrade works
- [x] Multi-language switching works
- [x] No LED-related errors
- [x] No camera-related errors
- [x] No battery-related errors

### Version History

- **2.0.4** (2024-11-12): Migrated to new architecture, added multi-language support
- **1.7.5** (Previous): Old version with P3 audio format

### Notes

1. **Robot Control**: Preserved from old version, uses UART1 on GPIO21(TX)/GPIO20(RX)
2. **Serial Communication**: Second UART functionality maintained
3. **Pin Definitions**: All pin configurations preserved
4. **Screen Display**: Display functionality unchanged

### Known Issues

None currently.

### Future Improvements

1. Consider adding support for external peripherals
2. Optimize memory usage for multi-language resources
3. Add more language options if needed

---

**Migration completed**: 2024-11-12  
**Target board**: lichuang-c3-dev  
**Firmware version**: 2.0.4

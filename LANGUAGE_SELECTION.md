# Language Selection Guide

## Introduction

This project supports 38 languages. You can select your preferred language by modifying the `language_selection.h` file.

## How to Use

### Method 1: Using language_selection.h File (Recommended)

1. Open the `main/language_selection.h` file
2. Find the language you want to use
3. Remove the `//` comment symbols at the beginning of that line
4. Make sure all other language lines remain commented (with `//`)
5. Save the file and rebuild the project

**Example:**

If you want to use English, modify as follows:

```cpp
// Simplified Chinese
// #define SELECTED_LANGUAGE "zh-CN"

// English (US) ← Uncomment this line
#define SELECTED_LANGUAGE "en-US"

// Japanese
// #define SELECTED_LANGUAGE "ja-JP"
```

### Method 2: Using Kconfig Configuration (Traditional Method)

If you delete the `language_selection.h` file, the system will automatically fall back to using Kconfig configuration:

```bash
idf.py menuconfig
# Navigate to: Component config → Xiaozhi Configuration → Default Language
```

## Supported Languages

| Language Code | Language Name |
|---------------|---------------|
| zh-CN | Simplified Chinese |
| zh-TW | Traditional Chinese |
| en-US | English (US) |
| ja-JP | Japanese |
| ko-KR | Korean |
| fr-FR | French |
| de-DE | German |
| es-ES | Spanish |
| it-IT | Italian |
| pt-PT | Portuguese |
| ru-RU | Russian |
| ar-SA | Arabic (Saudi Arabia) |
| hi-IN | Hindi |
| th-TH | Thai |
| vi-VN | Vietnamese |
| id-ID | Indonesian |
| ms-MY | Malay |
| fil-PH | Filipino |
| tr-TR | Turkish |
| pl-PL | Polish |
| nl-NL | Dutch |
| sv-SE | Swedish |
| nb-NO | Norwegian |
| da-DK | Danish |
| fi-FI | Finnish |
| el-GR | Greek |
| cs-CZ | Czech |
| hu-HU | Hungarian |
| ro-RO | Romanian |
| bg-BG | Bulgarian |
| hr-HR | Croatian |
| sk-SK | Slovak |
| sl-SI | Slovenian |
| sr-RS | Serbian |
| uk-UA | Ukrainian |
| fa-IR | Persian |
| he-IL | Hebrew |
| ca-ES | Catalan |

## Language Resources

Each language includes:

- **language.json**: Text string translations
- **.ogg audio files**: System sounds and voice prompts

If an audio file doesn't exist in the selected language, the system will automatically fall back to the corresponding file in `en-US`.

## Build Project

After changing the language selection, rebuild the project:

```bash
idf.py build
idf.py flash
```

## Important Notes

1. **Only one language at a time** - Make sure only one `SELECTED_LANGUAGE` line is uncommented

2. **Rebuild required** - You must rebuild the entire project after changing the language

3. **Fallback mechanism** - English versions are used if audio files are missing

4. **Custom languages** - To add a new language, create a corresponding folder in `main/assets/locales/`

## Troubleshooting

### Build Error: "No valid SELECTED_LANGUAGE found"

**Cause:** No language line is uncommented, or multiple lines are uncommented

**Solution:** Make sure only one `SELECTED_LANGUAGE` definition is uncommented in `language_selection.h`

### No Sound at Runtime

**Cause:** Audio files may be missing or in incorrect format

**Solution:**
1. Check if .ogg files exist in `main/assets/locales/[your-language]/`
2. Check serial logs to see if fallback language is being used

## Technical Details

### How it Works

1. CMake reads `language_selection.h` during build
2. Extracts the uncommented `SELECTED_LANGUAGE` value
3. Sets the corresponding `CONFIG_LANGUAGE_*` variable
4. Collects JSON and OGG files based on the language code
5. Generates `lang_config.h` header file for code use

### File Structure

```
main/
├── language_selection.h          # Language selection configuration
├── assets/
│   ├── common/                   # Common audio files (shared by all languages)
│   │   ├── exclamation.ogg
│   │   ├── popup.ogg
│   │   ├── success.ogg
│   │   ├── vibration.ogg
│   │   └── low_battery.ogg
│   ├── locales/                  # Multi-language resource directory
│   │   ├── zh-CN/
│   │   │   ├── language.json
│   │   │   └── *.ogg
│   │   ├── en-US/               # English (used as fallback)
│   │   │   ├── language.json
│   │   │   └── *.ogg
│   │   └── ...                  # Other 36 languages
│   └── lang_config.h            # Auto-generated language config header
└── CMakeLists.txt               # Build configuration (reads language selection)
```

## Contributing New Languages

If you want to add a new language to the project:

1. Create a new language directory under `main/assets/locales/` (e.g., `xx-XX`)
2. Copy `en-US/language.json` and translate all strings
3. Record or generate corresponding .ogg audio files
4. Add the new language option in `language_selection.h`
5. Add a corresponding `elseif` branch in `main/CMakeLists.txt` fallback logic
6. Submit a Pull Request

## Quick Start Examples

### Example 1: Switch to Japanese

1. Open `main/language_selection.h`
2. Comment out the current language:
   ```cpp
   // #define SELECTED_LANGUAGE "zh-CN"
   ```
3. Uncomment Japanese:
   ```cpp
   #define SELECTED_LANGUAGE "ja-JP"
   ```
4. Build:
   ```bash
   idf.py build
   idf.py flash
   ```

### Example 2: Switch to Spanish

1. Open `main/language_selection.h`
2. Find the Spanish line:
   ```cpp
   // Spanish
   // #define SELECTED_LANGUAGE "es-ES"
   ```
3. Remove the comment symbols `//`:
   ```cpp
   // Spanish
   #define SELECTED_LANGUAGE "es-ES"
   ```
4. Ensure all other languages remain commented
5. Build and flash

## Testing Recommendations

### 1. Verify Language Switch
```bash
# Clean build cache
idf.py fullclean

# Build
idf.py build

# Check the build log for language selection info
# You should see output like:
# -- Language selected from language_selection.h: zh-CN
```

### 2. Verify Audio Playback
- Flash the firmware and monitor serial output
- Trigger system sounds
- Confirm that the audio in the selected language is playing

### 3. Verify Fallback Mechanism
- Temporarily delete a .ogg file for a language
- Rebuild
- Confirm the system uses the corresponding file from en-US

## Best Practices

1. **Always use full clean when switching languages**
   ```bash
   idf.py fullclean
   idf.py build
   ```

2. **Keep only one language uncommented**
   - Double-check before building

3. **Test audio playback**
   - Verify all system sounds work correctly

4. **Use UTF-8 encoding**
   - Ensure `language_selection.h` is saved as UTF-8

5. **Check build logs**
   - Confirm the correct language is being selected

## Advantages

✅ **User-friendly**: Only need to edit one configuration file

✅ **Clear and intuitive**: All 38 languages are visible with descriptions

✅ **Highly automated**: CMake handles all the details automatically

✅ **Backward compatible**: Doesn't affect users who use Kconfig

✅ **Easy to maintain**: Add new languages in one place

✅ **Well documented**: Comprehensive usage instructions provided

## FAQ

**Q: Can I select multiple languages?**
A: No, only one language can be selected at a time.

**Q: Do I need to run menuconfig?**
A: No, just edit `language_selection.h` and rebuild.

**Q: What happens if I don't uncomment any language?**
A: The build will fail with an error message.

**Q: Can I add my own custom language?**
A: Yes, create a folder in `main/assets/locales/` and add your translations.

**Q: How do I revert to Kconfig method?**
A: Simply delete or rename the `language_selection.h` file.

---

**Project Version**: 2.0.4  
**Last Updated**: 2024-11-12  
**Maintained by**: Xiaozhi Project Team

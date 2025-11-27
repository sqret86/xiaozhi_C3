set(SOURCES 
            # 新版AudioService架构
            "audio/audio_service.cc"
            "audio/audio_codec.cc"
            "audio/codecs/es8311_audio_codec.cc"
            "audio/codecs/no_audio_codec.cc"
            "audio/processors/no_audio_processor.cc"
            "audio/processors/audio_debugger.cc"
            "audio/wake_words/esp_wake_word.cc"

            "display/display.cc"
            "display/lcd_display.cc"
            "protocols/protocol.cc"
            "protocols/mqtt_protocol.cc"
            "protocols/websocket_protocol.cc"
            "iot/thing.cc"
            "iot/thing_manager.cc"
            "mcp_server.cc"
            "system_info.cc"
            "application.cc"
            "ota.cc"
            "settings.cc"
            "main.cc"
            "language_runtime.cc"
            "second_uart.cc"
            "device_state_event.cc"
            "assets.cc"
            )

set(INCLUDE_DIRS "." "display" "audio" "audio/codecs" "audio/processors" "audio/wake_words" "audio_codecs" "protocols")

# 添加 IOT 相关文件
file(GLOB IOT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/iot/things/*.cc)
list(APPEND SOURCES ${IOT_SOURCES})

# 添加板级公共文件
file(GLOB BOARD_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/boards/common/*.cc)
list(APPEND SOURCES ${BOARD_COMMON_SOURCES})
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/boards/common)

# 根据 BOARD_TYPE 配置添加对应的板级文件
if(CONFIG_BOARD_TYPE_LICHUANG_C3_DEV)
    set(BOARD_TYPE "lichuang-c3-dev")
endif()
file(GLOB BOARD_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE}/*.c
)
list(APPEND SOURCES ${BOARD_SOURCES})

# 唤醒词配置已在audio/wake_words/中处理

# 从 language_selection.h 读取语言配置
set(LANG_SELECTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/language_selection.h")
if(EXISTS "${LANG_SELECTION_FILE}")
    file(READ "${LANG_SELECTION_FILE}" LANG_SELECTION_CONTENT)
    # 提取未注释的 SELECTED_LANGUAGE 的值
    # 匹配以 #define 开头（不是 //#define）的行，支持行首或换行后的匹配
    string(REGEX MATCH "(^|[\r\n])[ \t]*#define[ \t]+SELECTED_LANGUAGE[ \t]+\"([^\"]+)\"" LANG_MATCH "${LANG_SELECTION_CONTENT}")
    if(CMAKE_MATCH_2)
        set(LANG_DIR "${CMAKE_MATCH_2}")
        message(STATUS "Language selected from language_selection.h: ${LANG_DIR}")
        
        # 设置对应的 CONFIG_LANGUAGE_* 变量以保持兼容性
        string(REPLACE "-" "_" LANG_CONFIG_NAME "${LANG_DIR}")
        string(TOUPPER "${LANG_CONFIG_NAME}" LANG_CONFIG_NAME)
        set(CONFIG_LANGUAGE_${LANG_CONFIG_NAME} 1)
    else()
        message(FATAL_ERROR "No valid SELECTED_LANGUAGE found in ${LANG_SELECTION_FILE}. Please uncomment one language.")
    endif()
else()
    # 如果没有 language_selection.h，则回退到 Kconfig 配置
    message(STATUS "language_selection.h not found, using Kconfig configuration")
    if(CONFIG_LANGUAGE_ZH_CN)
        set(LANG_DIR "zh-CN")
    elseif(CONFIG_LANGUAGE_ZH_TW)
        set(LANG_DIR "zh-TW")
    elseif(CONFIG_LANGUAGE_EN_US)
        set(LANG_DIR "en-US")
    elseif(CONFIG_LANGUAGE_JA_JP)
        set(LANG_DIR "ja-JP")
    elseif(CONFIG_LANGUAGE_KO_KR)
        set(LANG_DIR "ko-KR")
    elseif(CONFIG_LANGUAGE_VI_VN)
        set(LANG_DIR "vi-VN")
    elseif(CONFIG_LANGUAGE_TH_TH)
        set(LANG_DIR "th-TH")
    elseif(CONFIG_LANGUAGE_DE_DE)
        set(LANG_DIR "de-DE")
    elseif(CONFIG_LANGUAGE_FR_FR)
        set(LANG_DIR "fr-FR")
    elseif(CONFIG_LANGUAGE_ES_ES)
        set(LANG_DIR "es-ES")
    elseif(CONFIG_LANGUAGE_IT_IT)
        set(LANG_DIR "it-IT")
    elseif(CONFIG_LANGUAGE_RU_RU)
        set(LANG_DIR "ru-RU")
    elseif(CONFIG_LANGUAGE_AR_SA)
        set(LANG_DIR "ar-SA")
    elseif(CONFIG_LANGUAGE_HI_IN)
        set(LANG_DIR "hi-IN")
    elseif(CONFIG_LANGUAGE_PT_PT)
        set(LANG_DIR "pt-PT")
    elseif(CONFIG_LANGUAGE_PL_PL)
        set(LANG_DIR "pl-PL")
    elseif(CONFIG_LANGUAGE_CS_CZ)
        set(LANG_DIR "cs-CZ")
    elseif(CONFIG_LANGUAGE_FI_FI)
        set(LANG_DIR "fi-FI")
    elseif(CONFIG_LANGUAGE_TR_TR)
        set(LANG_DIR "tr-TR")
    elseif(CONFIG_LANGUAGE_ID_ID)
        set(LANG_DIR "id-ID")
    elseif(CONFIG_LANGUAGE_UK_UA)
        set(LANG_DIR "uk-UA")
    elseif(CONFIG_LANGUAGE_RO_RO)
        set(LANG_DIR "ro-RO")
    elseif(CONFIG_LANGUAGE_BG_BG)
        set(LANG_DIR "bg-BG")
    elseif(CONFIG_LANGUAGE_CA_ES)
        set(LANG_DIR "ca-ES")
    elseif(CONFIG_LANGUAGE_DA_DK)
        set(LANG_DIR "da-DK")
    elseif(CONFIG_LANGUAGE_EL_GR)
        set(LANG_DIR "el-GR")
    elseif(CONFIG_LANGUAGE_FA_IR)
        set(LANG_DIR "fa-IR")
    elseif(CONFIG_LANGUAGE_FIL_PH)
        set(LANG_DIR "fil-PH")
    elseif(CONFIG_LANGUAGE_HE_IL)
        set(LANG_DIR "he-IL")
    elseif(CONFIG_LANGUAGE_HR_HR)
        set(LANG_DIR "hr-HR")
    elseif(CONFIG_LANGUAGE_HU_HU)
        set(LANG_DIR "hu-HU")
    elseif(CONFIG_LANGUAGE_MS_MY)
        set(LANG_DIR "ms-MY")
    elseif(CONFIG_LANGUAGE_NB_NO)
        set(LANG_DIR "nb-NO")
    elseif(CONFIG_LANGUAGE_NL_NL)
        set(LANG_DIR "nl-NL")
    elseif(CONFIG_LANGUAGE_SK_SK)
        set(LANG_DIR "sk-SK")
    elseif(CONFIG_LANGUAGE_SL_SI)
        set(LANG_DIR "sl-SI")
    elseif(CONFIG_LANGUAGE_SV_SE)
        set(LANG_DIR "sv-SE")
    elseif(CONFIG_LANGUAGE_SR_RS)
        set(LANG_DIR "sr-RS")
    endif()
endif()

# 定义生成路径
set(LANG_JSON "${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/${LANG_DIR}/language.json")
set(LANG_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/assets/lang_config.h")

# Collect current language audio files
file(GLOB LANG_SOUNDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/${LANG_DIR}/*.ogg)

# If not en-US, collect en-US audio files as fallback for missing files
if(NOT LANG_DIR STREQUAL "en-US")
    file(GLOB EN_US_SOUNDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/en-US/*.ogg)
    
    # Extract filenames (without path) from current language
    set(EXISTING_NAMES "")
    foreach(SOUND_FILE ${LANG_SOUNDS})
        get_filename_component(FILENAME ${SOUND_FILE} NAME)
        list(APPEND EXISTING_NAMES ${FILENAME})
    endforeach()
    
    # Only add en-US audio files that are missing in current language
    foreach(EN_SOUND ${EN_US_SOUNDS})
        get_filename_component(FILENAME ${EN_SOUND} NAME)
        if(NOT ${FILENAME} IN_LIST EXISTING_NAMES)
            list(APPEND LANG_SOUNDS ${EN_SOUND})
            message(STATUS "Using en-US fallback for missing audio: ${FILENAME}")
        endif()
    endforeach()
endif()

file(GLOB COMMON_SOUNDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/common/*.ogg)

# ------------------------------------------------------------------------------
# 额外：始终打包一套 zh-CN 的激活提示音和数字 0–9，用于运行时按 IP 切换中文播报
# 为了避免与默认语言的符号冲突，这里给 zh-CN 版本加上 “_zh” 后缀
# 生成到构建目录下，然后通过 EMBED_FILES 一并打包进固件
# ------------------------------------------------------------------------------
set(ZH_CN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/zh-CN")
set(ZH_CN_EXTRA_SOUNDS "")
foreach(name 0 1 2 3 4 5 6 7 8 9 activation)
    set(SRC_FILE "${ZH_CN_DIR}/${name}.ogg")
    if(EXISTS "${SRC_FILE}")
        set(DST_FILE "${CMAKE_CURRENT_BINARY_DIR}/${name}_zh.ogg")
        add_custom_command(
            OUTPUT "${DST_FILE}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SRC_FILE}" "${DST_FILE}"
            DEPENDS "${SRC_FILE}"
            COMMENT "Copy zh-CN sound ${name}.ogg -> ${name}_zh.ogg"
        )
        list(APPEND ZH_CN_EXTRA_SOUNDS "${DST_FILE}")
    endif()
endforeach()

idf_component_register(SRCS ${SOURCES}
                    EMBED_FILES ${LANG_SOUNDS} ${COMMON_SOUNDS} ${ZH_CN_EXTRA_SOUNDS}
                    INCLUDE_DIRS ${INCLUDE_DIRS}
                    WHOLE_ARCHIVE
                    )

# 使用 target_compile_definitions 来定义 BOARD_TYPE, BOARD_NAME
# 如果 BOARD_NAME 为空，则使用 BOARD_TYPE
if(NOT BOARD_NAME)
    set(BOARD_NAME ${BOARD_TYPE})
endif()
target_compile_definitions(${COMPONENT_LIB}
                    PRIVATE BOARD_TYPE=\"${BOARD_TYPE}\" BOARD_NAME=\"${BOARD_NAME}\"
                    )

# 添加生成规则
add_custom_command(
    OUTPUT ${LANG_HEADER}
    COMMAND python ${PROJECT_DIR}/scripts/gen_lang.py
            --language "${LANG_DIR}"
            --output "${LANG_HEADER}"
    DEPENDS
        ${LANG_JSON}
        ${LANG_SELECTION_FILE}
        ${PROJECT_DIR}/scripts/gen_lang.py
    COMMENT "Generating ${LANG_DIR} language config from language_selection.h"
)

# 强制建立生成依赖
add_custom_target(lang_header ALL
    DEPENDS ${LANG_HEADER}
)

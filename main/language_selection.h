/**
 * 语言选择配置文件 / Language Selection Configuration
 * 
 * 使用方法 / Usage:
 * 1. 找到你想要使用的语言 / Find the language you want to use
 * 2. 取消该行的注释（删除 // ）/ Uncomment that line (remove //)
 * 3. 确保其他语言都是注释状态 / Make sure all other languages are commented
 * 4. 重新编译项目 / Rebuild the project
 * 
 * 注意：同时只能选择一种语言 / Note: Only one language can be selected at a time
 */

#ifndef LANGUAGE_SELECTION_H
#define LANGUAGE_SELECTION_H

// ====================================================================================
// 选择你的语言 / SELECT YOUR LANGUAGE
// 取消注释下面其中一行 / Uncomment ONE of the lines below
// ====================================================================================

// 简体中文 (Simplified Chinese)
//#define SELECTED_LANGUAGE "zh-CN"

// 繁体中文 (Traditional Chinese)
//#define SELECTED_LANGUAGE "zh-TW"

// 英语 (English - US)
#define SELECTED_LANGUAGE "en-US"

// 日语 (Japanese)
//#define SELECTED_LANGUAGE "ja-JP"

// 韩语 (Korean)
// #define SELECTED_LANGUAGE "ko-KR"

// 法语 (French)
// #define SELECTED_LANGUAGE "fr-FR"

// 德语 (German)
// #define SELECTED_LANGUAGE "de-DE"

// 西班牙语 (Spanish)
// #define SELECTED_LANGUAGE "es-ES"

// 意大利语 (Italian)
// #define SELECTED_LANGUAGE "it-IT"

// 葡萄牙语 (Portuguese)
// #define SELECTED_LANGUAGE "pt-PT"

// 俄语 (Russian)
// #define SELECTED_LANGUAGE "ru-RU"

// 阿拉伯语 (Arabic - Saudi Arabia)
// #define SELECTED_LANGUAGE "ar-SA"

// 印地语 (Hindi)
// #define SELECTED_LANGUAGE "hi-IN"

// 泰语 (Thai)
// #define SELECTED_LANGUAGE "th-TH"

// 越南语 (Vietnamese)
// #define SELECTED_LANGUAGE "vi-VN"

// 印尼语 (Indonesian)
// #define SELECTED_LANGUAGE "id-ID"

// 马来语 (Malay)
// #define SELECTED_LANGUAGE "ms-MY"

// 菲律宾语 (Filipino)
// #define SELECTED_LANGUAGE "fil-PH"

// 土耳其语 (Turkish)
// #define SELECTED_LANGUAGE "tr-TR"

// 波兰语 (Polish)
// #define SELECTED_LANGUAGE "pl-PL"

// 荷兰语 (Dutch)
// #define SELECTED_LANGUAGE "nl-NL"

// 瑞典语 (Swedish)
// #define SELECTED_LANGUAGE "sv-SE"

// 挪威语 (Norwegian)
// #define SELECTED_LANGUAGE "nb-NO"

// 丹麦语 (Danish)
// #define SELECTED_LANGUAGE "da-DK"

// 芬兰语 (Finnish)
// #define SELECTED_LANGUAGE "fi-FI"

// 希腊语 (Greek)
// #define SELECTED_LANGUAGE "el-GR"

// 捷克语 (Czech)
// #define SELECTED_LANGUAGE "cs-CZ"

// 匈牙利语 (Hungarian)
// #define SELECTED_LANGUAGE "hu-HU"

// 罗马尼亚语 (Romanian)
// #define SELECTED_LANGUAGE "ro-RO"

// 保加利亚语 (Bulgarian)
// #define SELECTED_LANGUAGE "bg-BG"

// 克罗地亚语 (Croatian)
// #define SELECTED_LANGUAGE "hr-HR"

// 斯洛伐克语 (Slovak)
// #define SELECTED_LANGUAGE "sk-SK"

// 斯洛文尼亚语 (Slovenian)
// #define SELECTED_LANGUAGE "sl-SI"

// 塞尔维亚语 (Serbian)
// #define SELECTED_LANGUAGE "sr-RS"

// 乌克兰语 (Ukrainian)
// #define SELECTED_LANGUAGE "uk-UA"

// 波斯语 (Persian)
// #define SELECTED_LANGUAGE "fa-IR"

// 希伯来语 (Hebrew)
// #define SELECTED_LANGUAGE "he-IL"

// 加泰罗尼亚语 (Catalan)
// #define SELECTED_LANGUAGE "ca-ES"

// ====================================================================================
// 以下为自动配置，请勿修改 / AUTO-CONFIGURATION, DO NOT MODIFY BELOW
// ====================================================================================

// 语言代码映射 / Language code mapping
#if defined(SELECTED_LANGUAGE)
    #if SELECTED_LANGUAGE == "zh-CN"
        #define CONFIG_LANGUAGE_ZH_CN 1
    #elif SELECTED_LANGUAGE == "zh-TW"
        #define CONFIG_LANGUAGE_ZH_TW 1
    #elif SELECTED_LANGUAGE == "en-US"
        #define CONFIG_LANGUAGE_EN_US 1
    #elif SELECTED_LANGUAGE == "ja-JP"
        #define CONFIG_LANGUAGE_JA_JP 1
    #elif SELECTED_LANGUAGE == "ko-KR"
        #define CONFIG_LANGUAGE_KO_KR 1
    #elif SELECTED_LANGUAGE == "fr-FR"
        #define CONFIG_LANGUAGE_FR_FR 1
    #elif SELECTED_LANGUAGE == "de-DE"
        #define CONFIG_LANGUAGE_DE_DE 1
    #elif SELECTED_LANGUAGE == "es-ES"
        #define CONFIG_LANGUAGE_ES_ES 1
    #elif SELECTED_LANGUAGE == "it-IT"
        #define CONFIG_LANGUAGE_IT_IT 1
    #elif SELECTED_LANGUAGE == "pt-PT"
        #define CONFIG_LANGUAGE_PT_PT 1
    #elif SELECTED_LANGUAGE == "ru-RU"
        #define CONFIG_LANGUAGE_RU_RU 1
    #elif SELECTED_LANGUAGE == "ar-SA"
        #define CONFIG_LANGUAGE_AR_SA 1
    #elif SELECTED_LANGUAGE == "hi-IN"
        #define CONFIG_LANGUAGE_HI_IN 1
    #elif SELECTED_LANGUAGE == "th-TH"
        #define CONFIG_LANGUAGE_TH_TH 1
    #elif SELECTED_LANGUAGE == "vi-VN"
        #define CONFIG_LANGUAGE_VI_VN 1
    #elif SELECTED_LANGUAGE == "id-ID"
        #define CONFIG_LANGUAGE_ID_ID 1
    #elif SELECTED_LANGUAGE == "ms-MY"
        #define CONFIG_LANGUAGE_MS_MY 1
    #elif SELECTED_LANGUAGE == "fil-PH"
        #define CONFIG_LANGUAGE_FIL_PH 1
    #elif SELECTED_LANGUAGE == "tr-TR"
        #define CONFIG_LANGUAGE_TR_TR 1
    #elif SELECTED_LANGUAGE == "pl-PL"
        #define CONFIG_LANGUAGE_PL_PL 1
    #elif SELECTED_LANGUAGE == "nl-NL"
        #define CONFIG_LANGUAGE_NL_NL 1
    #elif SELECTED_LANGUAGE == "sv-SE"
        #define CONFIG_LANGUAGE_SV_SE 1
    #elif SELECTED_LANGUAGE == "nb-NO"
        #define CONFIG_LANGUAGE_NB_NO 1
    #elif SELECTED_LANGUAGE == "da-DK"
        #define CONFIG_LANGUAGE_DA_DK 1
    #elif SELECTED_LANGUAGE == "fi-FI"
        #define CONFIG_LANGUAGE_FI_FI 1
    #elif SELECTED_LANGUAGE == "el-GR"
        #define CONFIG_LANGUAGE_EL_GR 1
    #elif SELECTED_LANGUAGE == "cs-CZ"
        #define CONFIG_LANGUAGE_CS_CZ 1
    #elif SELECTED_LANGUAGE == "hu-HU"
        #define CONFIG_LANGUAGE_HU_HU 1
    #elif SELECTED_LANGUAGE == "ro-RO"
        #define CONFIG_LANGUAGE_RO_RO 1
    #elif SELECTED_LANGUAGE == "bg-BG"
        #define CONFIG_LANGUAGE_BG_BG 1
    #elif SELECTED_LANGUAGE == "hr-HR"
        #define CONFIG_LANGUAGE_HR_HR 1
    #elif SELECTED_LANGUAGE == "sk-SK"
        #define CONFIG_LANGUAGE_SK_SK 1
    #elif SELECTED_LANGUAGE == "sl-SI"
        #define CONFIG_LANGUAGE_SL_SI 1
    #elif SELECTED_LANGUAGE == "sr-RS"
        #define CONFIG_LANGUAGE_SR_RS 1
    #elif SELECTED_LANGUAGE == "uk-UA"
        #define CONFIG_LANGUAGE_UK_UA 1
    #elif SELECTED_LANGUAGE == "fa-IR"
        #define CONFIG_LANGUAGE_FA_IR 1
    #elif SELECTED_LANGUAGE == "he-IL"
        #define CONFIG_LANGUAGE_HE_IL 1
    #elif SELECTED_LANGUAGE == "ca-ES"
        #define CONFIG_LANGUAGE_CA_ES 1
    #else
        #error "Unknown language selected. Please check SELECTED_LANGUAGE value."
    #endif
#else
    #error "No language selected. Please uncomment one SELECTED_LANGUAGE line."
#endif

#endif // LANGUAGE_SELECTION_H


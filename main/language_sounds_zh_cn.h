// 运行时使用的简体中文提示音（独立于编译时默认语言）
// 这里只包含激活流程需要的音频：activation + 数字 0–9

#pragma once

#include <string_view>

namespace LangZhCN {
namespace Sounds {

extern const char activation_zh_ogg_start[] asm("_binary_activation_zh_ogg_start");
extern const char activation_zh_ogg_end[] asm("_binary_activation_zh_ogg_end");
static const std::string_view OGG_ACTIVATION {
    static_cast<const char*>(activation_zh_ogg_start),
    static_cast<size_t>(activation_zh_ogg_end - activation_zh_ogg_start)
};

extern const char _0_zh_ogg_start[] asm("_binary_0_zh_ogg_start");
extern const char _0_zh_ogg_end[] asm("_binary_0_zh_ogg_end");
static const std::string_view OGG_0 {
    static_cast<const char*>(_0_zh_ogg_start),
    static_cast<size_t>(_0_zh_ogg_end - _0_zh_ogg_start)
};

extern const char _1_zh_ogg_start[] asm("_binary_1_zh_ogg_start");
extern const char _1_zh_ogg_end[] asm("_binary_1_zh_ogg_end");
static const std::string_view OGG_1 {
    static_cast<const char*>(_1_zh_ogg_start),
    static_cast<size_t>(_1_zh_ogg_end - _1_zh_ogg_start)
};

extern const char _2_zh_ogg_start[] asm("_binary_2_zh_ogg_start");
extern const char _2_zh_ogg_end[] asm("_binary_2_zh_ogg_end");
static const std::string_view OGG_2 {
    static_cast<const char*>(_2_zh_ogg_start),
    static_cast<size_t>(_2_zh_ogg_end - _2_zh_ogg_start)
};

extern const char _3_zh_ogg_start[] asm("_binary_3_zh_ogg_start");
extern const char _3_zh_ogg_end[] asm("_binary_3_zh_ogg_end");
static const std::string_view OGG_3 {
    static_cast<const char*>(_3_zh_ogg_start),
    static_cast<size_t>(_3_zh_ogg_end - _3_zh_ogg_start)
};

extern const char _4_zh_ogg_start[] asm("_binary_4_zh_ogg_start");
extern const char _4_zh_ogg_end[] asm("_binary_4_zh_ogg_end");
static const std::string_view OGG_4 {
    static_cast<const char*>(_4_zh_ogg_start),
    static_cast<size_t>(_4_zh_ogg_end - _4_zh_ogg_start)
};

extern const char _5_zh_ogg_start[] asm("_binary_5_zh_ogg_start");
extern const char _5_zh_ogg_end[] asm("_binary_5_zh_ogg_end");
static const std::string_view OGG_5 {
    static_cast<const char*>(_5_zh_ogg_start),
    static_cast<size_t>(_5_zh_ogg_end - _5_zh_ogg_start)
};

extern const char _6_zh_ogg_start[] asm("_binary_6_zh_ogg_start");
extern const char _6_zh_ogg_end[] asm("_binary_6_zh_ogg_end");
static const std::string_view OGG_6 {
    static_cast<const char*>(_6_zh_ogg_start),
    static_cast<size_t>(_6_zh_ogg_end - _6_zh_ogg_start)
};

extern const char _7_zh_ogg_start[] asm("_binary_7_zh_ogg_start");
extern const char _7_zh_ogg_end[] asm("_binary_7_zh_ogg_end");
static const std::string_view OGG_7 {
    static_cast<const char*>(_7_zh_ogg_start),
    static_cast<size_t>(_7_zh_ogg_end - _7_zh_ogg_start)
};

extern const char _8_zh_ogg_start[] asm("_binary_8_zh_ogg_start");
extern const char _8_zh_ogg_end[] asm("_binary_8_zh_ogg_end");
static const std::string_view OGG_8 {
    static_cast<const char*>(_8_zh_ogg_start),
    static_cast<size_t>(_8_zh_ogg_end - _8_zh_ogg_start)
};

extern const char _9_zh_ogg_start[] asm("_binary_9_zh_ogg_start");
extern const char _9_zh_ogg_end[] asm("_binary_9_zh_ogg_end");
static const std::string_view OGG_9 {
    static_cast<const char*>(_9_zh_ogg_start),
    static_cast<size_t>(_9_zh_ogg_end - _9_zh_ogg_start)
};

}  // namespace Sounds
}  // namespace LangZhCN




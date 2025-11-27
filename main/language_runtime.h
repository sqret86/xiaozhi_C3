#ifndef LANGUAGE_RUNTIME_H
#define LANGUAGE_RUNTIME_H

#include <string>

namespace LanguageRuntime {

// 当前是否处于“中文环境”（language.code 为 zh-CN 且 Wi-Fi 已成功连接）
bool IsZhCN();

// 给定即将连接/已连接的 SSID，是否应视为中文环境（language.code 为 zh-CN 且与上次记录的 SSID 相同）
bool IsZhCNForSsid(const std::string& ssid);

}

#endif




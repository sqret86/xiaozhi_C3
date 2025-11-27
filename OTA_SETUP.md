# 自建 OTA 升级服务配置指南

## 概述

本项目使用 GitHub Pages 托管 OTA 版本检查 API，固件文件通过 GitHub Releases 分发。

## 架构

```
┌─────────────────┐     版本检查      ┌──────────────────────────┐
│   ESP32-C3      │ ───────────────▶ │  GitHub Pages            │
│   设备          │                   │  /api/ota/index.json     │
└─────────────────┘                   └──────────────────────────┘
         │                                       │
         │ 下载固件                               │ 返回版本信息
         ▼                                       ▼
┌─────────────────┐                   ┌──────────────────────────┐
│  GitHub         │                   │  {                       │
│  Releases       │ ◀─────────────── │    "firmware": {         │
│  xiaozhi.bin    │                   │      "version": "2.0.5", │
└─────────────────┘                   │      "url": "..."        │
                                      │    }                     │
         │                            │  }                       │
         │ 对话/激活                   └──────────────────────────┘
         ▼
┌─────────────────┐
│  小智官方服务器  │
│  (保持不变)     │
└─────────────────┘
```

## 配置步骤

### 1. 启用 GitHub Pages

1. 打开仓库 Settings → Pages
2. Source 选择 `Deploy from a branch`
3. Branch 选择 `main`，目录选择 `/ (root)`
4. 保存后等待部署完成

### 2. API 地址

部署完成后，OTA API 地址为：
```
https://sqret86.github.io/xiaozhi_C3/api/ota/
```

### 3. 发布新版本

#### 步骤 1：更新版本号

修改 `CMakeLists.txt` 中的版本号：
```cmake
set(PROJECT_VER "x.x.x")
```

#### 步骤 2：编译固件

```bash
source ~/esp/v5.4.1/esp-idf/export.sh
idf.py build
```

#### 步骤 3：创建 GitHub Release

```bash
# 使用 GitHub CLI
gh release create vX.X.X build/xiaozhi.bin --title "vX.X.X" --notes "更新说明"

# 或者在 GitHub 网页上手动创建 Release 并上传 xiaozhi.bin
```

#### 步骤 4：更新 API 版本文件

修改 `api/ota/index.json`：
```json
{
  "firmware": {
    "version": "X.X.X",
    "url": "https://github.com/sqret86/xiaozhi_C3/releases/download/vX.X.X/xiaozhi.bin"
  }
}
```

#### 步骤 5：推送更新

```bash
git add .
git commit -m "Release vX.X.X"
git push
```

## 文件说明

| 文件 | 说明 |
|-----|------|
| `api/ota/index.json` | OTA 版本检查 API 返回的 JSON |
| `api/index.html` | API 说明页面 |
| `CMakeLists.txt` | 固件版本号定义 |
| `sdkconfig` | OTA URL 配置 |
| `main/Kconfig.projbuild` | OTA URL 默认值 |

## 版本检查逻辑

设备启动后会：
1. 请求 `CONFIG_OTA_URL`（即 GitHub Pages API）
2. 比较返回的 `firmware.version` 与当前版本
3. 如果有新版本，下载 `firmware.url` 指向的固件
4. 写入 OTA 分区并重启

## 注意事项

1. **GitHub Pages 部署延迟**：更新 `index.json` 后可能需要几分钟才能生效
2. **固件大小限制**：GitHub Releases 单文件限制 2GB，足够使用
3. **HTTPS 必需**：ESP-IDF HTTP 客户端支持 HTTPS
4. **对话服务不变**：MQTT/WebSocket 仍然使用小智官方服务器

## 回滚到官方 OTA

如需恢复使用官方 OTA 服务，修改 `sdkconfig`：
```
CONFIG_OTA_URL="https://api.tenclass.net/xiaozhi/ota/"
```

然后重新编译烧录即可。


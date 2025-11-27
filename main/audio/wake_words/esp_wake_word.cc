#include "esp_wake_word.h"
#include <esp_log.h>
#include <esp_timer.h>


#define TAG "EspWakeWord"

EspWakeWord::EspWakeWord() {
}

EspWakeWord::~EspWakeWord() {
    if (wakenet_data_ != nullptr) {
        wakenet_iface_->destroy(wakenet_data_);
        esp_srmodel_deinit(wakenet_model_);
    }
}

bool EspWakeWord::Initialize(AudioCodec* codec, srmodel_list_t* models_list) {
    codec_ = codec;

    if (models_list == nullptr) {
        wakenet_model_ = esp_srmodel_init("model");
    } else {
        wakenet_model_ = models_list;
    }

    if (wakenet_model_ == nullptr || wakenet_model_->num == -1) {
        ESP_LOGE(TAG, "Failed to initialize wakenet model");
        return false;
    }
    if(wakenet_model_->num > 1) {
        ESP_LOGW(TAG, "More than one model found, using the first one");
    } else if (wakenet_model_->num == 0) {
        ESP_LOGE(TAG, "No model found");
        return false;
    }
    char *model_name = wakenet_model_->model_name[0];
    wakenet_iface_ = (esp_wn_iface_t*)esp_wn_handle_from_name(model_name);
    // 使用 DET_MODE_90（Normal），相对 DET_MODE_95 略微降低灵敏度，减少误触发
    wakenet_data_ = wakenet_iface_->create(model_name, DET_MODE_90);

    int frequency = wakenet_iface_->get_samp_rate(wakenet_data_);
    int audio_chunksize = wakenet_iface_->get_samp_chunksize(wakenet_data_);
    ESP_LOGI(TAG, "Wake word model: %s, freq: %d, chunksize: %d", model_name, frequency, audio_chunksize);

    // 打印模型中配置的唤醒词字符串，方便用户在串口中查看实际的唤醒短语
    char *wake_words = esp_srmodel_get_wake_words(wakenet_model_, model_name);
    if (wake_words != nullptr) {
        ESP_LOGI(TAG, "Available wake words: %s", wake_words);
    } else {
        ESP_LOGW(TAG, "Failed to get wake words from model");
    }

    return true;
}

void EspWakeWord::OnWakeWordDetected(std::function<void(const std::string& wake_word)> callback) {
    wake_word_detected_callback_ = callback;
}

void EspWakeWord::Start() {
    running_ = true;
}

void EspWakeWord::Stop() {
    running_ = false;
}

void EspWakeWord::Feed(const std::vector<int16_t>& data) {
    if (wakenet_data_ == nullptr || !running_) {
        return;
    }

    // 冷却时间：上次成功唤醒后的短时间内直接忽略，避免连续误触
    int64_t now_us = esp_timer_get_time();
    if (last_trigger_time_us_ != 0 &&
        now_us - last_trigger_time_us_ < static_cast<int64_t>(kMinTriggerIntervalMs_) * 1000) {
        return;
    }

    int res = wakenet_iface_->detect(wakenet_data_, (int16_t *)data.data());
    if (res > 0) {
        // 单帧检测到唤醒词即触发，具体灵敏度由 DET_MODE_90 控制
        last_detected_wake_word_ = wakenet_iface_->get_word_name(wakenet_data_, res);
        running_ = false;

        // 记录本次唤醒时间，用于后续冷却
        last_trigger_time_us_ = now_us;

        if (wake_word_detected_callback_) {
            wake_word_detected_callback_(last_detected_wake_word_);
        }
    }
}

size_t EspWakeWord::GetFeedSize() {
    if (wakenet_data_ == nullptr) {
        return 0;
    }
    return wakenet_iface_->get_samp_chunksize(wakenet_data_);
}

void EspWakeWord::EncodeWakeWordData() {
}

bool EspWakeWord::GetWakeWordOpus(std::vector<uint8_t>& opus) {
    return false;
}

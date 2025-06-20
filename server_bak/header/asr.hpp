#pragma once
#include "aip-cpp-sdk/speech.h"
#include "logger.hpp"

namespace liren {
    // ASRClient 类封装了百度语音识别 SDK 的接口
    class ASRClient 
    {
    public:
        using ptr = std::shared_ptr<ASRClient>;
    public: 
        ASRClient(const std::string &app_id,
                const std::string &api_key,
                const std::string &secret_key)
            : _client(app_id, api_key, secret_key) 
        {}

        std::string recognize(const std::string &speech_data, std::string &err)
        {
            // 基于语音识别 sdk 进行语音识别，获取识别后的文本内容
            Json::Value result = _client.recognize(speech_data, "pcm", 16000, aip::null);
            if (result["err_no"].asInt() != 0) {
                LOG_ERROR("语音识别失败：{}", result["err_msg"].asString());
                err = result["err_msg"].asString();
                return std::string();
            }
            return result["result"][0].asString();
        }
    private:
        aip::Speech _client;
    };
}
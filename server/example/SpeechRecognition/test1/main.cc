#include "../aip-cpp-sdk/speech.h"  // 引入百度语音识别 SDK 头文件

// 语音识别函数
void asr(aip::Speech &client)
{
    // 创建一个字符串变量来存储音频文件的内容
    std::string file_content;

    // 读取本地的音频文件（16k.pcm），并将其内容存入 file_content
    aip::get_file_content("16k.pcm", &file_content);

    // 调用百度语音识别 SDK 中的 recognize 方法进行语音识别
    // file_content：传入音频数据
    // "pcm"：音频格式，指示音频文件是 PCM 编码格式
    // 16000：音频采样率（16kHz）
    // aip::null：附加参数，通常为 null
    Json::Value result = client.recognize(file_content, "pcm", 16000, aip::null);

    // 检查识别结果的错误码，如果错误码不为 0，表示识别失败
    if (result["err_no"].asInt() != 0) {
        std::cout << result["err_msg"].asString() << std::endl;
        return;  
    }

    // 输出识别结果的第一个元素，通常是转录后的文本
    std::cout << result["result"][0].asString() << std::endl;
}

int main()
{
    // 设置应用的 APPID/AK/SK（需要从百度云获取）
    std::string app_id = "117644864";  
    std::string api_key = "5Bt1BCHIJcJIO2AoMbBsSqC9"; 
    std::string secret_key = "Y1gqrypoZHMWAoh6M20UH88wfbn89uRw";  

    // 初始化百度语音识别客户端对象，传入应用的 APPID、API 密钥和 Secret Key
    aip::Speech client(app_id, api_key, secret_key);

    asr(client); // 进行语音识别
    return 0;  
}

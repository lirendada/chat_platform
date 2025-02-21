#include "../../../header/asr.hpp"
#include "gflags/gflags.h"

DEFINE_string(app_id, "117644864", "语音平台应用ID");
DEFINE_string(api_key, "5Bt1BCHIJcJIO2AoMbBsSqC9", "语音平台API密钥");
DEFINE_string(secret_key, "Y1gqrypoZHMWAoh6M20UH88wfbn89uRw", "语音平台加密密钥");

DEFINE_bool(run_mode, false, "程序的运行模式，false-调试； true-发布；");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志输出等级");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);

    ASRClient client(FLAGS_app_id, FLAGS_api_key, FLAGS_secret_key);

    std::string file_content, err;
    aip::get_file_content("16k.pcm", &file_content);

    std::string res = client.recognize(file_content, err);
    if (res.empty())
    {
        LOG_ERROR("识别失败：{}", err);
        return -1;
    }
    std::cout << res << std::endl;
    return 0;
}
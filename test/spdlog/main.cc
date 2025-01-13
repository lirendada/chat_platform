#include "logger.hpp"
#include <gflags/gflags.h>

DEFINE_bool(run_mode, false, "程序运行模式：true为发布模式，false为调试模式");
DEFINE_string(log_file, "", "发布模式下用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下用于指定日志输出等级");

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);
    LOG_TRACE("你好啊，{}！", "liren");
    LOG_DEBUG("你好啊，{}！", "liren");
    LOG_INFO("你好啊，{}！", "liren");
    LOG_WARN("你好啊，{}！", "liren");
    LOG_ERROR("你好啊，{}！", "liren");
    LOG_CRITICAL("你好啊，{}！", "liren");
    return 0;
}
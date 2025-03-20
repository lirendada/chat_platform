#include "../../common/dms.hpp"
#include "gflags/gflags.h"


DEFINE_string(key_id, "LTAI5t6NF7vt499UeqYX6LB9", "平台访问密钥ID");
DEFINE_string(key_secret, "5hx1qvpXHDKfQDk73aJs6j53Q8KcF2", "平台访问密钥");

DEFINE_bool(run_mode, false, "程序的运行模式，false-调试； true-发布；");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志输出等级");

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);

    DMSClient client(FLAGS_key_id, FLAGS_key_secret);
    client.send("15929917272", "5678");

    return 0;
}
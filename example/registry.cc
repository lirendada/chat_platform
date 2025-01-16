#include "../header/etcd.hpp"
#include <gflags/gflags.h>
 
DEFINE_bool(run_mode, false, "程序的运行模式，false为调试模式，true为发布模式");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志的输出等级");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(key, "/service", "服务监控的主目录");
DEFINE_string(instance, "/friend/instance", "当前实例名称");
DEFINE_string(value, "127.0.0.1:8080", "主机地址");

int main(int argc, char* argv[])
{
    // 初始化
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);

    // 创建注册类对象并用智能指针保护起来
    Registry::ptr reg = std::make_shared<Registry>(FLAGS_etcd_host);
    reg->regiter(FLAGS_key + FLAGS_instance, FLAGS_value);

    std::this_thread::sleep_for(std::chrono::seconds(600));
    return 0;
}
#include "../../../header/rabbitmq.hpp"
#include "../../../header/logger.hpp"
#include <gflags/gflags.h>

using namespace liren;

DEFINE_string(user, "root", "rabbitmq访问用户名");
DEFINE_string(pswd, "123456", "rabbitmq访问密码");
DEFINE_string(host, "127.0.0.1:5672", "rabbitmq服务器地址信息 host:port");

DEFINE_bool(run_mode, false, "程序的运行模式，false-调试； true-发布；");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志输出等级");

void callback(const char *body, size_t sz)
{
    std::string msg;
    msg.assign(body, sz);
    LOG_DEBUG("{}", msg);
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);

    // 创建客户端对象
    MQClient client(FLAGS_user, FLAGS_pswd, FLAGS_host);

    // 声明交换机、队列及绑定
    client.declareComponents("test-exchange", "test-queue");

    // 订阅消息
    client.consume("test-queue", callback);
    std::this_thread::sleep_for(std::chrono::seconds(60));
    return 0;
}

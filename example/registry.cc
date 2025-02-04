#include <brpc/server.h>
#include <butil/logging.h>
#include <gflags/gflags.h>
#include "main.pb.h"
#include "../header/etcd.hpp"
 
DEFINE_bool(run_mode, false, "程序的运行模式，false为调试模式，true为发布模式");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志的输出等级");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(basedir, "/service", "服务监控的主目录");
DEFINE_string(instance, "/echo/instance", "当前实例名称");
DEFINE_string(host, "127.0.0.1:8080", "服务的主机地址");
DEFINE_int32(listen_port, 8080, "rpc服务器的监听端口");

// 别忘了要声明命名空间example
class EchoServiceImpl : public example::EchoService {
public:
    EchoServiceImpl() {}
    ~EchoServiceImpl() {}

    // 重写Echo接口，实现具体的业务逻辑，以响应客户端发起的 Echo RPC 调用
    void Echo(::google::protobuf::RpcController* controller,
                const ::example::EchoRequest* request,
                ::example::EchoResponse* response,
                ::google::protobuf::Closure* done)
    {
        // 1. 使用RAII方式自动释放done对象，内部会调用run()接口
        brpc::ClosureGuard guard(done);

        // 2. 进行请求输出和响应
        std::cout << "收到消息：" << request->message() << std::endl;
        response->set_message(request->message() + "这是一条用于测试的响应信息！");
    }
};

int main(int argc, char* argv[])
{
    // 初始化gflags
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);
    
    // 1. 关闭rpc自带的日志输出
    logging::LoggingSettings settings;
    settings.logging_dest = logging::LoggingDestination::LOG_TO_NONE;
    logging::InitLogging(settings);

    // 2. 创建rpc服务器对象
    brpc::Server server;

    // 3. 向服务器对象中新增EchoService服务（局部变量，不需要去服务器去清除服务对象）
    EchoServiceImpl impl;
    int ret = server.AddService(&impl, brpc::ServiceOwnership::SERVER_DOESNT_OWN_SERVICE);
    if(ret != 0) {
        std::cout << "添加rpc服务失败！\n";
        return -1;
    }

    // 4. 启动服务器
    brpc::ServerOptions options;
    options.num_threads = 1;
    ret = server.Start(FLAGS_listen_port, &options);
    if(ret != 0) {
        std::cout << "启动rpc服务器失败！\n";
        return -1;
    }

    // 5、注册服务（创建注册类对象并用智能指针保护起来）
    Registry::ptr reg = std::make_shared<Registry>(FLAGS_etcd_host);
    reg->regiter(FLAGS_basedir + FLAGS_instance, FLAGS_host);
    
    server.RunUntilAskedToQuit(); // 休眠等待运行结束
    return 0;
}
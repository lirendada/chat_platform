#include <brpc/server.h>
#include <butil/logging.h>
#include "main.pb.h"

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
        response->set_message("这是一条用于测试的响应信息！");
    }
};

int main(int argc, char* argv[])
{
    // 1. 关闭rpc自带的日志输出
    logging::LoggingSettings settings;
    settings.logging_dest = logging::LoggingDestination::LOG_TO_NONE;
    logging::InitLogging(settings);

    // 2. 创建服务器对象
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
    ret = server.Start(8080, &options);
    if(ret != 0) {
        std::cout << "启动rpc服务器失败！\n";
        return -1;
    }
    server.RunUntilAskedToQuit(); // 休眠等待运行结束
    return 0;
}
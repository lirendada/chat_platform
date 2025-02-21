#include <brpc/channel.h>
#include <thread>
#include "main.pb.h"

void callback(brpc::Controller* ctl, example::EchoResponse* resp)
{
    std::unique_ptr<brpc::Controller> ctl_guard(ctl);
    std::unique_ptr<example::EchoResponse> resp_guard(resp);
    if (ctl->Failed() == true) {
        std::cout << "Rpc调用失败：" << ctl->ErrorText() << std::endl;
        return;
    }
    std::cout << "收到响应: " << resp->message() << std::endl;
}

int main(int argc, char* argv[])
{
    // 1. 构造Channel信道，连接服务器
    brpc::Channel channel;
    int ret = channel.Init("127.0.0.1:8080", nullptr);
    if(ret != 0) {
        std::cout << "初始化信道失败！\n";
        return -1;
    }
    
    // 2. 构造EchoService_Stub对象，用于进行rpc调用
    example::EchoService_Stub stub(&channel);
    
    // 3. 进行Rpc调用（异步调用）
    example::EchoRequest req;
    req.set_message("你好，利刃！");
    brpc::Controller *ctl = new brpc::Controller();
    example::EchoResponse *resp = new example::EchoResponse();

    // 设置回调函数，然后返回Closure对象指针
    auto closure = google::protobuf::NewCallback(callback, ctl, resp); 
    stub.Echo(ctl, &req, resp, closure); 
    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}
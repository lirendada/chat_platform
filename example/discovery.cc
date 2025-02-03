#include "../header/etcd.hpp"
#include "../header/channel.hpp"
#include "main.pb.h"
#include <gflags/gflags.h>

DEFINE_bool(run_mode, false, "程序的运行模式，false为调试模式，true为发布模式");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志的输出等级");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(basedir, "/service", "服务监控的主目录");
DEFINE_string(call_service, "/service/echo", "测试代码中要获取的提供echo服务节点");

int main(int argc, char* argv[])
{
    // 初始化gflags
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);

    // 1、构造rpc信道管理对象，然后关注“/service/echo”这个服务
    ServiceManager::ptr sm = std::make_shared<ServiceManager>();
    sm->declared(FLAGS_call_service);
  
    // 2、绑定回调函数，构造服务发现对象
    auto put_cb = std::bind(&ServiceManager::online, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&ServiceManager::offline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    Discovery::ptr dis = std::make_shared<Discovery>(FLAGS_etcd_host, FLAGS_basedir, put_cb, del_cb);

    while(true)
    {
        // 3、通过rpc信道管理对象，获取提供Echo服务的信道对象
        auto channel = sm.get()->getChannel(FLAGS_call_service);
        if(!channel)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return -1;
        }

        // 4、构造EchoService_Stub对象，用于进行rpc调用（同步调用）
        example::EchoService_Stub stub(channel.get());
        example::EchoRequest req;
        req.set_message("你好，利刃！");
        brpc::Controller *ctl = new brpc::Controller();
        example::EchoResponse *resp = new example::EchoResponse();

        stub.Echo(ctl, &req, resp, nullptr); // 同步方式调用，不需要Closure对象
        if(ctl->Failed() == true) {
            std::cout << "Rpc调用失败：" << ctl->ErrorText() << std::endl;
            delete ctl; 
            delete resp;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        std::cout << "收到响应: " << resp->message() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
/*
 * 文件客户端单元测试程序
 * 作用：对文件存储子服务的四个业务接口（单文件/多文件上传和下载）进行单元测试
 * 测试使用 Google Test 框架进行断言，使用 gflags 管理命令行参数
 */

#include <gflags/gflags.h>       // 命令行参数解析库
#include <gtest/gtest.h>         // Google Test 单元测试框架
#include <thread>                // 用于线程操作，测试过程中可能需要等待
#include "etcd.hpp"              // 服务注册模块封装，负责服务注册和发现
#include "channel.hpp"           // RPC 信道封装，提供 RPC 通信通道
#include "logger.hpp"            // 日志模块封装，用于记录调试和错误日志
#include "file.pb.h"             // 文件服务相关的 protobuf 消息定义
#include "base.pb.h"             // 基础数据类型的 protobuf 定义
#include "utils.hpp"             // 工具类封装，包含读写文件、生成 UUID 等常用函数

// 定义命令行参数
DEFINE_bool(run_mode, false, "程序的运行模式，false-调试； true-发布；");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志输出等级");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(base_service, "/service", "服务监控根目录");
DEFINE_string(file_service, "/service/file_service", "文件服务的监控路径");

liren::ServiceManager::channel_ptr channel; // RPC 信道，用于后续调用文件服务的接口
std::string single_file_id; // 保存单个文件上传返回的文件ID，供后续下载测试使用

// 上传单个文件接口测试
TEST(put_test, single_file) 
{
    // 1. 读取当前目录下的 "Makefile" 文件数据
    std::string body;
    ASSERT_TRUE(liren::readFile("./Makefile", body));

    // 2. 构造文件服务的 RPC 客户端对象（Stub），通过全局信道channel获取
    liren::FileService_Stub stub(channel.get());

    // 构造上传单个文件的请求消息
    liren::PutSingleFileReq req;
    req.set_request_id("1111");   // 设置请求ID
    req.mutable_file_data()->set_file_name("Makefile");  // 设置文件名称
    req.mutable_file_data()->set_file_size(body.size()); // 设置文件大小
    req.mutable_file_data()->set_file_content(body);     // 设置文件内容

    // 3. 构造 brpc Controller、响应对象，并发起 RPC 调用
    brpc::Controller *cntl = new brpc::Controller();
    liren::PutSingleFileRsp *rsp = new liren::PutSingleFileRsp();
    stub.PutSingleFile(cntl, &req, rsp, nullptr);

    // 4. 断言 RPC 调用未失败，并检查上传是否成功
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());
    ASSERT_EQ(rsp->file_info().file_size(), body.size());
    ASSERT_EQ(rsp->file_info().file_name(), "Makefile");

    // 保存返回的文件ID，用于后续下载单文件测试
    single_file_id = rsp->file_info().file_id();
    LOG_DEBUG("文件ID：{}", rsp->file_info().file_id());
}

// 下载单个文件接口测试
TEST(get_test, single_file) 
{
    // 1. 构造 RPC 客户端对象
    liren::FileService_Stub stub(channel.get());
    liren::GetSingleFileReq req;
    liren::GetSingleFileRsp *rsp;

    req.set_request_id("2222");       // 设置请求ID
    req.set_file_id(single_file_id);  // 设置要下载的文件ID（之前上传返回的ID）

    // 2. 构造 Controller、响应对象，发起 RPC 调用
    brpc::Controller *cntl = new brpc::Controller();
    rsp = new liren::GetSingleFileRsp();
    stub.GetSingleFile(cntl, &req, rsp, nullptr);

    // 3. 检查 RPC 调用状态和响应内容
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());
    ASSERT_EQ(single_file_id, rsp->file_data().file_id());

    // 4. 将下载的文件数据写入到本地文件 "make_file_download"
    liren::writeFile("make_file_download", rsp->file_data().file_content());
}

// 保存上传多个文件返回的文件ID列表，用于后续下载测试
std::vector<std::string> multi_file_id;

// 单元测试：上传多个文件接口测试
TEST(put_test, multi_file) 
{
    // 1. 分别读取当前目录下的 "base.pb.h" 和 "file.pb.h" 文件数据
    std::string body1;
    ASSERT_TRUE(liren::readFile("./base.pb.h", body1));
    std::string body2;
    ASSERT_TRUE(liren::readFile("./file.pb.h", body2));

    // 2. 构造 RPC 客户端对象，构造上传多个文件请求消息
    liren::FileService_Stub stub(channel.get());
    liren::PutMultiFileReq req;
    req.set_request_id("3333");

    // 添加第一个文件数据
    auto file_data = req.add_file_data();
    file_data->set_file_name("base.pb.h");
    file_data->set_file_size(body1.size());
    file_data->set_file_content(body1);

    // 添加第二个文件数据
    file_data = req.add_file_data();
    file_data->set_file_name("file.pb.h");
    file_data->set_file_size(body2.size());
    file_data->set_file_content(body2);

    // 3. 构造 Controller、响应对象，并发起 RPC 调用
    brpc::Controller *cntl = new brpc::Controller();
    liren::PutMultiFileRsp *rsp = new liren::PutMultiFileRsp();
    stub.PutMultiFile(cntl, &req, rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());

    // 4. 检查响应，断言上传成功，同时保存返回的文件ID列表
    ASSERT_TRUE(rsp->success());
    for (int i = 0; i < rsp->file_info_size(); i++){
        multi_file_id.push_back(rsp->file_info(i).file_id());
        LOG_DEBUG("文件ID：{}", multi_file_id[i]);
    }
}

// 下载多个文件接口测试
TEST(get_test, multi_file) 
{
    // 1. 构造 RPC 客户端对象，构造下载多个文件请求消息
    liren::FileService_Stub stub(channel.get());
    liren::GetMultiFileReq req;
    liren::GetMultiFileRsp *rsp;
    req.set_request_id("4444");
    // 添加之前上传的多个文件ID
    req.add_file_id_list(multi_file_id[0]);
    req.add_file_id_list(multi_file_id[1]);

    // 2. 构造 Controller、响应对象，发起 RPC 调用
    brpc::Controller *cntl = new brpc::Controller();
    rsp = new liren::GetMultiFileRsp();
    stub.GetMultiFile(cntl, &req, rsp, nullptr);
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(rsp->success());

    // 3. 检查响应 map 中是否包含对应的文件数据
    ASSERT_TRUE(rsp->file_data().find(multi_file_id[0]) != rsp->file_data().end());
    ASSERT_TRUE(rsp->file_data().find(multi_file_id[1]) != rsp->file_data().end());

    // 4. 从响应 map 中提取文件数据，并将内容写入到本地文件中
    auto map = rsp->file_data();
    auto file_data1 = map[multi_file_id[0]];
    liren::writeFile("base_download_file1", file_data1.file_content());
    auto file_data2 = map[multi_file_id[1]];
    liren::writeFile("file_download_file2", file_data2.file_content());
}

int main(int argc, char *argv[])
{
    // 初始化
    testing::InitGoogleTest(&argc, argv);
    google::ParseCommandLineFlags(&argc, &argv, true);
    liren::init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);

    // 1. 构造 RPC 信道管理对象 ServiceManager
    auto sm = std::make_shared<liren::ServiceManager>();
    sm->declared(FLAGS_file_service);
    // 设置服务上线和下线的回调函数
    auto put_cb = std::bind(&liren::ServiceManager::online, sm.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&liren::ServiceManager::offline, sm.get(), std::placeholders::_1, std::placeholders::_2);
    
    // 2. 构造服务发现对象，使用 etcd 作为注册中心
    liren::Discovery::ptr dclient = std::make_shared<liren::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);
    
    // 3. 通过 ServiceManager 获取对应文件服务的 RPC 信道
    channel = sm->getChannel(FLAGS_file_service);
    if (!channel) {
        // 如果未获取到信道，等待1秒后退出（这里简化处理，实际应重试或报错）
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return -1;
    }
    
    // 4. 运行所有单元测试
    return RUN_ALL_TESTS();
}

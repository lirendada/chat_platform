#include "etcd.hpp"
#include "channel.hpp"
#include "utils.hpp"
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <thread>
#include "user.pb.h"
#include "base.pb.h"

DEFINE_bool(run_mode, false, "程序的运行模式，false-调试； true-发布；");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志输出等级");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(base_service, "/service", "服务监控根目录");
DEFINE_string(user_service, "/service/user_service", "服务监控根目录");

liren::ServiceManager::ptr _user_channels;

liren::UserInfo user_info;

std::string login_ssid;
std::string new_nickname = "亲爱的猪妈妈";

// TEST(用户子服务测试, 用户注册测试) {
//     auto channel = _user_channels->getChannel(FLAGS_user_service); // 获取通信信道
//     ASSERT_TRUE(channel);  // 断言信道不为空，否则测试失败
    
//     user_info.set_nickname("猪妈妈"); // 设置用户信息中的昵称为“猪妈妈”

//     // 构造用户注册请求对象
//     liren::UserRegisterReq req;
    
//     req.set_request_id(liren::uuid()); // 为请求设置一个唯一的请求ID，保证每次请求的唯一性
//     req.set_nickname(user_info.nickname()); 
//     req.set_password("123456");

//     liren::UserRegisterRsp rsp; // 定义响应对象，用于接收服务器端返回的信息
//     brpc::Controller cntl;      // 定义控制器，用于管理RPC调用过程中的状态和错误信息
//     liren::UserService_Stub stub(channel.get()); // 创建用户服务的Stub，通过通信信道与远程服务进行交互

//     // 发起用户注册RPC调用，传入控制器、请求、响应对象和空回调
//     stub.UserRegister(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }

// TEST(用户子服务测试, 用户登录测试) {
//     auto channel = _user_channels->getChannel(FLAGS_user_service); // 获取通信信道
//     ASSERT_TRUE(channel);

//     liren::UserLoginReq req;
//     req.set_request_id(liren::uuid());
//     req.set_nickname("猪妈妈");
//     req.set_password("123456");
//     liren::UserLoginRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.UserLogin(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
//     login_ssid = rsp.login_session_id();
// }

// TEST(用户子服务测试, 用户头像设置测试) {
//     auto channel = _user_channels->getChannel(FLAGS_user_service);//获取通信信道
//     ASSERT_TRUE(channel);

//     liren::SetUserAvatarReq req;
//     req.set_request_id(liren::uuid());
//     req.set_user_id(user_info.user_id());
//     req.set_session_id(login_ssid);
//     req.set_avatar(user_info.avatar());
//     liren::SetUserAvatarRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.SetUserAvatar(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }

// TEST(用户子服务测试, 用户签名设置测试) {
//     auto channel = _user_channels->getChannel(FLAGS_user_service);//获取通信信道
//     ASSERT_TRUE(channel);

//     liren::SetUserDescriptionReq req;
//     req.set_request_id(liren::uuid());
//     req.set_user_id(user_info.user_id());
//     req.set_session_id(login_ssid);
//     req.set_description(user_info.description());
//     liren::SetUserDescriptionRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.SetUserDescription(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }

// TEST(用户子服务测试, 用户昵称设置测试) {
//     auto channel = _user_channels->getChannel(FLAGS_user_service);//获取通信信道
//     ASSERT_TRUE(channel);

//     liren::SetUserNicknameReq req;
//     req.set_request_id(liren::uuid());
//     req.set_user_id(user_info.user_id());
//     req.set_session_id(login_ssid);
//     req.set_nickname(new_nickname);
//     liren::SetUserNicknameRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.SetUserNickname(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }


// TEST(用户子服务测试, 用户信息获取测试) {
//     auto channel = _user_channels->getChannel(FLAGS_user_service);//获取通信信道
//     ASSERT_TRUE(channel);

//     liren::GetUserInfoReq req;
//     req.set_request_id(liren::uuid());
//     req.set_user_id(user_info.user_id());
//     req.set_session_id(login_ssid);
//     liren::GetUserInfoRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.GetUserInfo(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
//     ASSERT_EQ(user_info.user_id(), rsp.user_info().user_id());
//     ASSERT_EQ(new_nickname, rsp.user_info().nickname());
//     ASSERT_EQ(user_info.description(), rsp.user_info().description());
//     ASSERT_EQ("", rsp.user_info().phone());
//     ASSERT_EQ(user_info.avatar(), rsp.user_info().avatar());
// }

// void set_user_avatar(const std::string &uid, const std::string &avatar) {
//     auto channel = _user_channels->getChannel(FLAGS_user_service);//获取通信信道
//     ASSERT_TRUE(channel);
//     liren::SetUserAvatarReq req;
//     req.set_request_id(liren::uuid());
//     req.set_user_id(uid);
//     req.set_session_id(login_ssid);
//     req.set_avatar(avatar);
//     liren::SetUserAvatarRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.SetUserAvatar(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }

// TEST(用户子服务测试, 批量用户信息获取测试) {
//     set_user_avatar("用户ID1", "小猪佩奇的头像数据");
//     set_user_avatar("用户ID2", "小猪乔治的头像数据");
//     auto channel = _user_channels->getChannel(FLAGS_user_service);//获取通信信道
//     ASSERT_TRUE(channel);

//     liren::GetMultiUserInfoReq req;
//     req.set_request_id(liren::uuid());
//     req.add_users_id("用户ID1");
//     req.add_users_id("用户ID2");
//     req.add_users_id("ee55-9043bfd7-0001");
//     liren::GetMultiUserInfoRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.GetMultiUserInfo(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
//     auto users_map = rsp.mutable_users_info();
//     liren::UserInfo fuser = (*users_map)["ee55-9043bfd7-0001"];
//     ASSERT_EQ(fuser.user_id(), "ee55-9043bfd7-0001");
//     ASSERT_EQ(fuser.nickname(), "猪爸爸");
//     ASSERT_EQ(fuser.description(), "");
//     ASSERT_EQ(fuser.phone(), "");
//     ASSERT_EQ(fuser.avatar(), "");

//     liren::UserInfo puser = (*users_map)["用户ID1"];
//     ASSERT_EQ(puser.user_id(), "用户ID1");
//     ASSERT_EQ(puser.nickname(), "小猪佩奇");
//     ASSERT_EQ(puser.description(), "这是一只小猪");
//     ASSERT_EQ(puser.phone(), "手机号1");
//     ASSERT_EQ(puser.avatar(), "小猪佩奇的头像数据");
    
//     liren::UserInfo quser = (*users_map)["用户ID2"];
//     ASSERT_EQ(quser.user_id(), "用户ID2");
//     ASSERT_EQ(quser.nickname(), "小猪乔治");
//     ASSERT_EQ(quser.description(), "这是一只小小猪");
//     ASSERT_EQ(quser.phone(), "手机号2");
//     ASSERT_EQ(quser.avatar(), "小猪乔治的头像数据");
// }

// std::string code_id;
// void get_code() {
//     auto channel = _user_channels->getChannel(FLAGS_user_service);//获取通信信道
//     ASSERT_TRUE(channel);

//     liren::PhoneVerifyCodeReq req;
//     req.set_request_id(liren::uuid());
//     req.set_phone_number(user_info.phone());
//     liren::PhoneVerifyCodeRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.GetPhoneVerifyCode(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
//     code_id = rsp.verify_code_id();
// }


// TEST(用户子服务测试, 手机号注册) {
//     get_code();
//     auto channel = _user_channels->choose(FLAGS_user_service);//获取通信信道
//     ASSERT_TRUE(channel);

//     liren::PhoneRegisterReq req;
//     req.set_request_id(liren::uuid());
//     req.set_phone_number(user_info.phone());
//     req.set_verify_code_id(code_id);
//     std::cout << "手机号注册，输入验证码：" << std::endl;
//     std::string code;
//     std::cin >> code;
//     req.set_verify_code(code);
//     liren::PhoneRegisterRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.PhoneRegister(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }
// TEST(用户子服务测试, 手机号登录) {
//     std::this_thread::sleep_for(std::chrono::seconds(3));
//     get_code();
//     auto channel = _user_channels->getChannel(FLAGS_user_service);//获取通信信道
//     ASSERT_TRUE(channel);

//     liren::PhoneLoginReq req;
//     req.set_request_id(liren::uuid());
//     req.set_phone_number(user_info.phone());
//     req.set_verify_code_id(code_id);
//     std::cout << "手机号登录，输入验证码：" << std::endl;
//     std::string code;
//     std::cin >> code;
//     req.set_verify_code(code);
//     liren::PhoneLoginRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.PhoneLogin(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
//     std::cout << "手机登录会话ID：" << rsp.login_session_id() << std::endl;
// }
// TEST(用户子服务测试, 手机号设置) {
//     std::this_thread::sleep_for(std::chrono::seconds(10));
//     get_code();
//     auto channel = _user_channels->getChannel(FLAGS_user_service);//获取通信信道
//     ASSERT_TRUE(channel);

//     liren::SetUserPhoneNumberReq req;
//     req.set_request_id(liren::uuid());
//     std::cout << "手机号设置时，输入用户ID：" << std::endl;
//     std::string user_id;
//     std::cin >> user_id;
//     req.set_user_id(user_id);
//     req.set_phone_number("18888888888");
//     req.set_phone_verify_code_id(code_id);
//     std::cout << "手机号设置时，输入验证码：" << std::endl;
//     std::string code;
//     std::cin >> code;
//     req.set_phone_verify_code(code);
//     liren::SetUserPhoneNumberRsp rsp;
//     brpc::Controller cntl;
//     liren::UserService_Stub stub(channel.get());
//     stub.SetUserPhoneNumber(&cntl, &req, &rsp, nullptr);
//     ASSERT_FALSE(cntl.Failed());
//     ASSERT_TRUE(rsp.success());
// }

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    liren::init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);

    // 1. 先构造Rpc信道管理对象
    _user_channels = std::make_shared<liren::ServiceManager>();
    _user_channels->declared(FLAGS_user_service);
    auto put_cb = std::bind(&liren::ServiceManager::online, _user_channels.get(), std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&liren::ServiceManager::offline, _user_channels.get(), std::placeholders::_1, std::placeholders::_2);
    
    // 2. 构造服务发现对象
    liren::Discovery::ptr dclient = std::make_shared<liren::Discovery>(FLAGS_etcd_host, FLAGS_base_service, put_cb, del_cb);
    
    user_info.set_nickname("猪妈妈");
    user_info.set_user_id("e893-13628387-0000");
    user_info.set_description("这是一个美丽的猪妈妈");
    user_info.set_phone("18063551124");
    user_info.set_avatar("猪妈妈头像数据");
    testing::InitGoogleTest(&argc, argv);
    LOG_DEBUG("开始测试！");
    return RUN_ALL_TESTS();
}
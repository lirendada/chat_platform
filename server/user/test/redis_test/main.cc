#include "../../../header/data_redis.hpp"   // 包含 Redis 操作封装头文件（定义了 Session、Status、Codes、RedisClientFactory 等类）
#include <gflags/gflags.h>                     
#include <thread>                            

DEFINE_bool(run_mode, false, "程序的运行模式，false-调试； true-发布；");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志输出等级");

DEFINE_string(ip, "127.0.0.1", "这是服务器的IP地址，格式：127.0.0.1");
DEFINE_int32(port, 6379, "这是服务器的端口, 格式: 8080");
DEFINE_int32(db, 0, "库的编号：默认0号");
DEFINE_bool(keep_alive, true, "是否进行长连接保活");

// session_test 函数用于测试 Session 类的功能：添加会话、删除会话和获取会话对应的用户ID
void session_test(const std::shared_ptr<sw::redis::Redis> &client) 
{
    // 创建 Session 对象，传入 Redis 客户端
    liren::Session ss(client);

    // 添加多个会话记录，键为会话ID，值为对应的用户ID
    ss.append("会话ID1", "用户ID1");
    ss.append("会话ID2", "用户ID2");
    ss.append("会话ID3", "用户ID3");
    ss.append("会话ID4", "用户ID4");

    // 删除部分会话记录
    ss.remove("会话ID2");
    ss.remove("会话ID3");

    // 分别获取各个会话对应的用户ID，并输出到控制台
    auto res1 = ss.uid("会话ID1");
    if (res1) std::cout << *res1 << std::endl;
    auto res2 = ss.uid("会话ID2");
    if (res2) std::cout << *res2 << std::endl;
    auto res3 = ss.uid("会话ID3");
    if (res3) std::cout << *res3 << std::endl;
    auto res4 = ss.uid("会话ID4");
    if (res4) std::cout << *res4 << std::endl;
}

// status_test 函数用于测试 Status 类的功能：添加在线状态、删除状态以及检测状态是否存在
void status_test(const std::shared_ptr<sw::redis::Redis> &client) 
{
    // 创建 Status 对象，传入 Redis 客户端
    liren::Status status(client);

    // 添加多个用户的在线状态记录（使用用户ID作为键，值为空字符串表示在线）
    status.append("用户ID1");
    status.append("用户ID2");
    status.append("用户ID3");
    
    // 删除其中一个用户的在线状态
    status.remove("用户ID2");

    // 检测各用户在线状态，并输出提示信息
    if (status.exists("用户ID1")) std::cout << "用户1在线！" << std::endl;
    if (status.exists("用户ID2")) std::cout << "用户2在线！" << std::endl;
    if (status.exists("用户ID3")) std::cout << "用户3在线！" << std::endl;
}

// code_test 函数用于测试 Codes 类的功能：添加验证码、删除验证码以及查询验证码，并测试验证码超时效果
void code_test(const std::shared_ptr<sw::redis::Redis> &client) 
{
    // 创建 Codes 对象，传入 Redis 客户端
    liren::Codes codes(client);

    // 添加多个验证码记录，键为验证码ID，值为验证码内容，默认设置了超时时间（一般为 300 秒）
    codes.append("验证码ID1", "验证码1");
    codes.append("验证码ID2", "验证码2");
    codes.append("验证码ID3", "验证码3");
    
    // 删除其中一个验证码记录
    codes.remove("验证码ID2");

    // 分别查询各验证码记录，并输出查询结果
    auto y1 = codes.code("验证码ID1");
    auto y2 = codes.code("验证码ID2");
    auto y3 = codes.code("验证码ID3");
    if (y1) std::cout << *y1 << std::endl;
    if (y2) std::cout << *y2 << std::endl;
    if (y3) std::cout << *y3 << std::endl;

    // 等待 4 秒（模拟验证码过期情况）
    std::this_thread::sleep_for(std::chrono::seconds(4));
    
    // 再次查询验证码记录，验证是否已过期
    auto y4 = codes.code("验证码ID1");
    auto y5 = codes.code("验证码ID2");
    auto y6 = codes.code("验证码ID3");
    if (!y4) std::cout << "验证码ID1不存在" << std::endl;
    if (!y5) std::cout << "验证码ID2不存在" << std::endl;
    if (!y6) std::cout << "验证码ID3不存在" << std::endl;
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
//  liren::init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);

    // 通过 RedisClientFactory 创建 Redis 客户端对象，使用命令行参数设置 IP、端口、数据库编号和长连接保活选项
    auto client = liren::RedisClientFactory::create(FLAGS_ip, FLAGS_port, FLAGS_db, FLAGS_keep_alive);

    // 运行不同的测试函数，可根据需要取消注释相应的测试
    session_test(client);
    status_test(client);
    code_test(client);   
    return 0;
}

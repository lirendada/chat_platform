#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
using namespace std;

int main()
{
    // 设置全局刷新策略
    spdlog::flush_every(std::chrono::seconds(1));       // 设置每秒刷新
    spdlog::flush_on(spdlog::level::level_enum::debug); // 设置为debug级别以上立刻刷新

    // 设置全局日志输出等级（可忽略，因为一般下面都会对特定的日志级别进行输出）
    spdlog::set_level(spdlog::level::level_enum::debug);

    // 创建同步日志器
    auto logger = spdlog::stdout_color_mt("default-logger");

    // 设置日志同步器的刷新策略、输出级别（这也是为什么上面不需要设置全局输出等级的原因）
    logger->flush_on(spdlog::level::level_enum::debug);
    logger->set_level(spdlog::level::level_enum::info);

    // 设置输出格式
    logger->set_pattern("[%H:%M:%S][%t][%-8l] %v");

    // 进行简单的日志输出
    logger->trace("你好！{}", "liren");
    logger->debug("你好！{}", "liren");
    logger->info("你好！{}", "liren");
    logger->warn("你好！{}", "liren");
    logger->error("你好！{}", "liren");
    logger->critical("你好！{}", "liren");
    std::cout << "同步日志输出演示完毕！" << std::endl;
    return 0;
}
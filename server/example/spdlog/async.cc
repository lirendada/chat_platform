#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
using namespace std;

int main()
{
    spdlog::flush_every(std::chrono::seconds(1));       
    spdlog::flush_on(spdlog::level::level_enum::debug); 

    // 唯一不同之处：创建异步日志器，传入模板参数即可
    auto logger = spdlog::stdout_color_mt<spdlog::async_factory>("default-logger");

    logger->flush_on(spdlog::level::level_enum::debug);
    logger->set_level(spdlog::level::level_enum::info);
    logger->set_pattern("[%H:%M:%S][%t][%-8l] %v");
    logger->trace("你好！{}", "liren");
    logger->debug("你好！{}", "liren");
    logger->info("你好！{}", "liren");
    logger->warn("你好！{}", "liren");
    logger->error("你好！{}", "liren");
    logger->critical("你好！{}", "liren");
    std::cout << "异步日志输出演示完毕！" << std::endl;
    return 0;
}
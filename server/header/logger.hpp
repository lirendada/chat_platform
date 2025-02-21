#pragma once
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <string>
#include <memory>

namespace liren {
    std::shared_ptr<spdlog::logger> logger;

    // mode：true表示发布模式，false表示调试模式
    void init_logger(bool mode, const std::string& file, int level)
    {
        if(mode == false) // 如果是调试模式，则建立标准输出日志器，输出等级为最低
        {
            logger = spdlog::stdout_color_mt("default-logger");
            logger->set_level(spdlog::level::level_enum::trace);
            logger->flush_on(spdlog::level::level_enum::trace);
        }
        else // 如果是发布模式，则建立文件输出日志器，输出等级根据参数决定
        {
            logger = spdlog::basic_logger_mt("default-logger", file);
            logger->set_level((spdlog::level::level_enum)level);
            logger->flush_on(spdlog::level::level_enum::trace);
        }
        logger->set_pattern("[%n][%H:%M:%S][%t][%-8l]%v");
    }

    #define LOG_TRACE(format, ...) logger->trace(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__);
    #define LOG_DEBUG(format, ...) logger->debug(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__);
    #define LOG_INFO(format, ...) logger->info(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__);
    #define LOG_WARN(format, ...) logger->warn(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__);
    #define LOG_ERROR(format, ...) logger->error(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__);
    #define LOG_CRITICAL(format, ...) logger->critical(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__);
}
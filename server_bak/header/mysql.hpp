#pragma once
#include <string>      
#include <memory>      
#include <cstdlib>     
#include <iostream>   
#include <odb/database.hxx>      // ODB ORM 框架中数据库抽象层定义
#include <odb/mysql/database.hxx> // ODB MySQL 数据库实现的定义
#include "logger.hpp"  // 日志模块封装，用于输出调试和错误日志

namespace liren 
{
    // ODBFactory 类封装了 ODB 数据库对象的创建逻辑
    // 通过该工厂方法可以创建 MySQL 数据库连接，并支持连接池功能
    class ODBFactory 
    {
    public:
        // 静态工厂方法 create 用于创建并返回一个数据库连接对象
        // 参数说明：
        //  - user: 数据库用户名
        //  - pswd: 数据库密码
        //  - host: 数据库主机地址
        //  - db: 数据库名称
        //  - cset: 数据库字符集（如 "utf8"）
        //  - port: 数据库端口号
        //  - conn_pool_count: 连接池中连接的数量
        static std::shared_ptr<odb::core::database> create(const std::string &user,
                                                           const std::string &pswd,
                                                           const std::string &host,
                                                           const std::string &db,
                                                           const std::string &cset,
                                                           int port,
                                                           int conn_pool_count)
        {
            // 创建一个 odb::mysql::connection_pool_factory 对象，用于管理数据库连接池
            // 使用 std::unique_ptr 管理连接池工厂的生命周期，并传入连接池连接数量参数
            std::unique_ptr<odb::mysql::connection_pool_factory> cpf(
                new odb::mysql::connection_pool_factory(conn_pool_count, 0));
            
            // 创建 MySQL 数据库对象，并传入相关参数：
            // user, pswd, db, host, port: 数据库连接基本信息
            // 空字符串 "" 表示默认的 socket 或其它参数（具体视 ODB 实现而定）
            // cset: 数据库字符集
            // 0: 可能表示客户端标志或其它配置信息（依据 ODB 接口文档）
            // std::move(cpf): 转移连接池工厂所有权，以便数据库对象内部使用连接池
            auto res = std::make_shared<odb::mysql::database>(user, pswd, 
                db, host, port, "", cset, 0, std::move(cpf));
            
            return res;
        }
    };
}

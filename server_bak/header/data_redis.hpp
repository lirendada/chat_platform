#include <sw/redis++/redis.h>   // 引入 Redis++ 库，用于与 Redis 数据库交互
#include <iostream>

namespace liren 
{
    // RedisClientFactory 类用于创建 Redis 客户端对象
    // 通过封装创建过程，简化外部调用时对 Redis 客户端的初始化
    class RedisClientFactory 
    {
    public:
        // 静态工厂方法，用于根据传入参数创建 Redis 客户端
        // 参数：
        //   - host: Redis 服务器地址
        //   - port: Redis 服务器端口
        //   - db: 使用的数据库编号
        //   - keep_alive: 是否启用长连接保持
        // 返回值：
        //   - std::shared_ptr<sw::redis::Redis>：返回创建的 Redis 客户端智能指针
        static std::shared_ptr<sw::redis::Redis> create(const std::string &host,
                                                        int port,
                                                        int db,
                                                        bool keep_alive) 
        {
            // 配置连接选项
            sw::redis::ConnectionOptions opts;
            opts.host = host;         // 设置 Redis 服务器地址
            opts.port = port;         // 设置 Redis 服务器端口
            opts.db = db;             // 指定使用的数据库编号
            opts.keep_alive = keep_alive; // 设置是否启用长连接保持

            // 创建 Redis 客户端对象，并封装到智能指针中返回
            auto res = std::make_shared<sw::redis::Redis>(opts);
            return res;
        }
    };

    // 封装基于 Redis 的会话管理操作
    // 用于保存和管理用户会话信息，支持添加、删除以及获取会话对应的用户ID
    class Session 
    {
    public:
        using ptr = std::shared_ptr<Session>;

        // 构造函数：传入 Redis 客户端对象，用于后续操作 Redis 数据库
        Session(const std::shared_ptr<sw::redis::Redis> &redis_client)
            : _redis_client(redis_client) 
        {}

        // append 方法：为给定的会话ID(ssid)存储对应的用户ID(uid)
        void append(const std::string &ssid, const std::string &uid) {
            _redis_client->set(ssid, uid);
        }

        // remove 方法：删除指定会话ID(ssid)对应的 Redis 键值对
        void remove(const std::string &ssid) {
            _redis_client->del(ssid);
        }

        // uid 方法：根据会话ID(ssid)获取存储的用户ID
        // 返回类型为 sw::redis::OptionalString，可以判断是否存在该键
        sw::redis::OptionalString uid(const std::string &ssid) {
            return _redis_client->get(ssid);
        }
    private:
        std::shared_ptr<sw::redis::Redis> _redis_client; // 保存 Redis 客户端对象，所有 Redis 操作均通过此对象进行
    };

    // 封装用户在线状态的管理操作
    // 通过 Redis 存储用户在线状态，支持添加、删除和检查状态是否存在
    class Status 
    {
    public:
        using ptr = std::shared_ptr<Status>;

        // 构造函数，传入 Redis 客户端对象
        Status(const std::shared_ptr<sw::redis::Redis> &redis_client)
            : _redis_client(redis_client) 
        {}

        // append 方法：设置指定用户ID(uid)的在线状态
        // 这里使用空字符串作为在线状态的标记
        void append(const std::string &uid) {
            _redis_client->set(uid, "");
        }

        // remove 方法：删除指定用户ID(uid)的在线状态记录
        void remove(const std::string &uid) {
            _redis_client->del(uid);
        }

        // exists 方法：检查指定用户ID(uid)是否存在在线状态记录
        // 通过获取键值判断是否存在
        bool exists(const std::string &uid) {
            auto res = _redis_client->get(uid);
            if (res) return true;
            return false;
        }
    private:
        std::shared_ptr<sw::redis::Redis> _redis_client; // Redis 客户端对象，用于执行状态相关的 Redis 操作
    };

    // 封装验证码操作
    // 支持添加、删除验证码，并设置验证码的有效期（超时自动失效）
    class Codes 
    {
    public:
        using ptr = std::shared_ptr<Codes>;

        // 构造函数：传入 Redis 客户端对象
        Codes(const std::shared_ptr<sw::redis::Redis> &redis_client)
            : _redis_client(redis_client) 
        {}

        // append 方法：添加验证码
        // 参数：
        //   - cid: 验证码对应的键（通常用于标识）
        //   - code: 验证码内容
        //   - t: 有效期，默认为 300000 毫秒（5 分钟）
        void append(const std::string &cid, const std::string &code, 
            const std::chrono::milliseconds &t = std::chrono::milliseconds(300000)) {
            // 设置键值对，并带有超时时间 t
            _redis_client->set(cid, code, t);
        }

        // remove 方法：删除指定验证码记录
        void remove(const std::string &cid) {
            _redis_client->del(cid);
        }

        // code 方法：根据验证码键(cid)获取验证码内容
        // 返回类型为 sw::redis::OptionalString，用于判断是否成功获取验证码
        sw::redis::OptionalString code(const std::string &cid)  {
            return _redis_client->get(cid);
        }
    private:
        std::shared_ptr<sw::redis::Redis> _redis_client; // 保存 Redis 客户端对象，供验证码相关操作使用
    };
} 

#pragma once
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>
#include <iostream>
#include <functional>
#include "logger.hpp"

namespace liren {
    class MQClient 
    {
    public:
        using ptr = std::shared_ptr<MQClient>;
        using MessageCallback = std::function<void(const char*, size_t)>; // 定义消息回调类型

        // 构造函数，初始化连接信息，连接到AMQP服务器
        MQClient(const std::string &user, 
                const std::string passwd,
                const std::string host) 
        {
            // 初始化Libev事件循环
            _loop = EV_DEFAULT;
            // 创建AMQP的LibEv事件循环处理器
            _handler = std::make_unique<AMQP::LibEvHandler>(_loop);

            // 构建AMQP连接地址（例如：amqp://root:123456@127.0.0.1:5672/）
            std::string url = "amqp://" + user + ":" + passwd + "@" + host + "/";
            AMQP::Address address(url);

            // 创建TCP连接
            _connection = std::make_unique<AMQP::TcpConnection>(_handler.get(), address);
            // 创建TCP通道
            _channel = std::make_unique<AMQP::TcpChannel>(_connection.get());

            // 启动一个新线程，运行事件循环
            _loop_thread = std::thread([this]() {
                ev_run(_loop, 0);
            });
        }

        // 析构函数，关闭事件循环并清理资源
        ~MQClient() 
        {
                // 初始化异步事件
                ev_async_init(&_async_watcher, watcher_callback);
                ev_async_start(_loop, &_async_watcher);
                ev_async_send(_loop, &_async_watcher);

                // 等待事件循环线程结束
                _loop_thread.join();
                _loop = nullptr;
        }

        // 声明交换机、队列及绑定
        void declareComponents(const std::string &exchange, // 交换机名称
                            const std::string &queue,    // 队列名称
                            const std::string &routing_key = "routing_key",
                            AMQP::ExchangeType echange_type = AMQP::ExchangeType::direct) // 一对一模式
        {
            // 声明交换机
            _channel->declareExchange(exchange, echange_type)
                .onError([](const char *message) {
                    LOG_ERROR("声明交换机失败：{}", message);
                    exit(0);
                })
                .onSuccess([exchange](){
                    LOG_ERROR("{} 交换机创建成功！", exchange);
                });

            // 声明队列
            _channel->declareQueue(queue)
                .onError([](const char *message) {
                    LOG_ERROR("声明队列失败：{}", message);
                    exit(0);
                })
                .onSuccess([queue](){
                    LOG_ERROR("{} 队列创建成功！", queue);
                });

            // 绑定交换机与队列
            _channel->bindQueue(exchange, queue, routing_key)
                .onError([exchange, queue](const char *message) {
                    LOG_ERROR("{} - {} 绑定失败：", exchange, queue);
                    exit(0);
                })
                .onSuccess([exchange, queue, routing_key](){
                    LOG_ERROR("{} - {} - {} 绑定成功！", exchange, queue, routing_key);
                });
        }

        // 发布消息到指定交换机
        bool publish(const std::string &exchange, 
                    const std::string &msg, 
                    const std::string &routing_key = "routing_key")
        {
            LOG_DEBUG("向交换机 {}-{} 发布消息！", exchange, routing_key);
            bool ret = _channel->publish(exchange, routing_key, msg);
            if (ret == false) {
                LOG_ERROR("{} 发布消息失败：", exchange);
                return false;
            }
            return true;
        }

        // 从指定队列消费消息
        void consume(const std::string &queue, const MessageCallback &cb) 
        {
            LOG_DEBUG("开始订阅 {} 队列消息！", queue);
            _channel->consume(queue, "consume-tag")  // 返回值为DeferredConsumer，而不是Deferred，所以要先用onReceived再用onError
                .onReceived([this, cb](const AMQP::Message &message, 
                                    uint64_t deliveryTag, 
                                    bool redelivered) 
                {
                    cb(message.body(), message.bodySize()); // 消息处理回调
                    _channel->ack(deliveryTag); // 确认消息处理
                })
                .onError([queue](const char *message){
                    LOG_ERROR("订阅 {} 队列消息失败: {}", queue, message);
                    exit(0);
                });
        }

    private:
        // Libev事件循环异步回调
        static void watcher_callback(struct ev_loop *loop, ev_async *watcher, int32_t revents) {
            ev_break(loop, EVBREAK_ALL);  // 停止事件循环
        }

    private:
        struct ev_async _async_watcher;  // 异步事件监视器
        struct ev_loop *_loop;           // Libev事件循环
        std::unique_ptr<AMQP::LibEvHandler> _handler;      // Libev事件处理器
        std::unique_ptr<AMQP::TcpConnection> _connection;  // TCP连接
        std::unique_ptr<AMQP::TcpChannel> _channel;        // AMQP通道
        std::thread _loop_thread;  // 事件循环线程
    };
}
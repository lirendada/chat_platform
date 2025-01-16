#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <etcd/Value.hpp>
#include <etcd/Watcher.hpp>
#include <functional>
#include "logger.hpp"

class Registry 
{
public:
    using ptr = std::shared_ptr<Registry>;

    // host：主机地址
    Registry(const std::string& host)
        : _client(std::make_shared<etcd::Client>(host))
        , _keepalive(_client->leasekeepalive(3).get())
        , _leaseid(_keepalive->Lease())
    {}

    ~Registry() {
        _keepalive->Cancel();
    }

    // 服务注册函数
    bool regiter(const std::string& key, const std::string& value)
    {
        // 将键值对添加到etcd服务中，然后进行判断即可
        auto resp = _client->put(key, value, _leaseid).get();
        if(resp.is_ok() == false) {
            LOG_ERROR("新增数据失败：{}", resp.error_message());
            return false;
        }
        LOG_DEBUG("新增数据成功：{}-{}", key, value);
        return true;
    }

private:
    std::shared_ptr<etcd::Client> _client;       // 客户端对象
    std::shared_ptr<etcd::KeepAlive> _keepalive; // 保活对象
    uint64_t _leaseid; // 租约id
};


class Discovery
{
public:
    using ptr = std::shared_ptr<Discovery>;
    using NotifyCallback = std::function<void(std::string, std::string)>; // 参数为键值对

    Discovery(const std::string& host,      // 主机地址
              const std::string& basedir,   // 表示主目录
              const NotifyCallback& put_cb, // 新增键值对后的回调处理
              const NotifyCallback& del_cb) // 删除键值对后的回调处理
        : _client(std::make_shared<etcd::Client>(host))
        , _put_cb(put_cb)
        , _del_cb(del_cb)
    {
        // 1. 先进行服务发现，获取到当前已有的数据
        auto resp = _client->ls(basedir).get();
        if(resp.is_ok() == false) {
            LOG_ERROR("获取键值对数据失败：{}", resp.error_message());
        }

        // 对获取到的数据进行回调处理
        for(int i = 0; i < resp.keys().size(); ++i) {
            if(_put_cb) _put_cb(resp.key(i), resp.value(i).as_string());
        }

        // 2. 然后进行事件监控，监控数据发生的改变并调用回调进行处理
        _watcher = std::make_shared<etcd::Watcher>(*_client.get(), basedir,
            std::bind(&Discovery::callback, this, std::placeholders::_1), true);
    }

    ~Discovery() {
        _watcher->Cancel();
    }

private:
    void callback(const etcd::Response& resp)
    {
        if(resp.is_ok() == false)
        {
            LOG_ERROR("收到一个错误的事件通知：{}", resp.error_message())
            return;
        }

        // 遍历resp中的所有的Event对象观察键值对变化情况
        for(auto const& es : resp.events())
        {
            if(es.event_type() == etcd::Event::EventType::PUT)
            {
                if(_put_cb) _put_cb(es.kv().key(), es.kv().as_string());
                LOG_DEBUG("新增键值对：{}-{}", es.kv().key(), es.kv().as_string());
            }
            else if(es.event_type() == etcd::Event::EventType::DELETE_)
            {
                if(_put_cb) _put_cb(es.prev_kv().key(), es.prev_kv().as_string());
                LOG_DEBUG("删除键值对：{}-{}", es.prev_kv().key(), es.prev_kv().as_string());
            }
        }
    }

private:
    std::shared_ptr<etcd::Client> _client;   // 客户端对象
    std::shared_ptr<etcd::Watcher> _watcher; // 监听对象

    NotifyCallback _put_cb, _del_cb; // 两个回调函数
};
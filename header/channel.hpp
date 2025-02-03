#pragma once
#include <brpc/channel.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include "logger.hpp"

// 单个服务的信道管理类
class ChannelManager {
public:
    using channel_ptr = std::shared_ptr<brpc::Channel>;
    using ptr = std::shared_ptr<ChannelManager>;
public:
    ChannelManager(const std::string& service_name)
        : _service_name(service_name)
        , _index(0)
    {}

    // 服务上线了一个节点，则调用append新增信道
    void append(const std::string& host)
    {
        // 构造初始化Channel信道
        channel_ptr channel = std::make_shared<brpc::Channel>();
        brpc::ChannelOptions options;
        options.connect_timeout_ms = -1;
        options.timeout_ms = -1;
        options.max_retry = 3;
        options.protocol = "baidu_std";
        int ret = channel->Init(host.c_str(), &options);
        if(ret != 0) {
            LOG_ERROR("初始化{}-{}信道失败！", _service_name, host);
            return;
        }

        // 先加锁再添加信息
        std::unique_lock<std::mutex> lock(_mtx);
        _channels.push_back(channel);
        _hosts[host] = channel;
    }

    // 服务下线了一个节点，则调用remove释放信道
    void remove(const std::string& host)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        auto it = _hosts.find(host);
        if(it == _hosts.end())
        {
            LOG_WARN("删除{}-{}信道失败，没有找到该信道！", _service_name, host);
            return;
        }

        for(auto ait = _channels.begin(); ait != _channels.end(); ++ait)
            if(*ait == it->second)
            {
                _channels.erase(ait);
                break;
            }
        _hosts.erase(it);
    }

    // 通过RR轮转策略，获取一个Channel用于发起对应服务的rpc调用
    channel_ptr get()
    {
        std::unique_lock<std::mutex> lock(_mtx);
        if(_channels.size() == 0)
        {
            LOG_ERROR("当前无信道可用，已为你创建新信道");
            return channel_ptr();
        }
        auto it = _channels[_index];
        _index = (_index + 1) % _channels.size();
        return it;
    }
private:
    std::mutex _mtx;
    int32_t _index;                                      // 当前轮转下标计数器
    std::string _service_name;                           // 服务名
    std::vector<channel_ptr> _channels;                  // 存放channel的集合
    std::unordered_map<std::string, channel_ptr> _hosts; // 存放主机号与channel的映射关系
};

// 总体服务的信道管理类
class ServiceManager {
public:
    using channel_ptr = std::shared_ptr<brpc::Channel>;
    using ptr = std::shared_ptr<ServiceManager>;
public:
    // 获取对应服务的一个channel对象，用于rpc调用
    ChannelManager::channel_ptr getChannel(const std::string& service_name)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        auto it = _services.find(service_name);
        if(it == _services.end())
        {
            LOG_ERROR("没有提供{}服务的节点", service_name);
            return ChannelManager::channel_ptr();
        }
        return it->second->get();
    }

    // 声明关注哪些服务的上下线调用，不关注的服务不需要处理
    void declared(const std::string& service_name)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _follow_services.insert(service_name);
    }

    // 服务上线时调用的回调接口：为服务添加主机地址（即etcd.hpp中Discovery类的put_cb对象）
    void online(const std::string& instance_name, const std::string& host)
    {
        std::string service_name = instance_to_service(instance_name);
        ChannelManager::ptr service;
        {
            std::unique_lock<std::mutex> lock(_mtx);
            auto fit = _follow_services.find(service_name);
            if(fit == _follow_services.end())
            {
                LOG_DEBUG("{}-{}服务上线了，但是当前并不关心！\n", service_name, host);
                return;
            }

            auto it = _services.find(service_name);
            if(it == _services.end())
            {
                // 说明是新添加的服务节点，此时创建并且插入即可
                service = std::make_shared<ChannelManager>(service_name);
                _services[service_name] = service;
            }
            else
                service = it->second;
        }
        service->append(host);
        LOG_DEBUG("{}-{}服务上线新节点，进行添加管理！", service_name, host);
    }

    // 服务下线时调用的回调接口，删除服务节点（即etcd.hpp中Discovery类的del_cb对象）
    void offline(const std::string& instance_name, const std::string& host)
    {
        std::string service_name = instance_to_service(instance_name);
        ChannelManager::ptr service;
        {
            std::unique_lock<std::mutex> lock(_mtx);
            auto fit = _follow_services.find(service_name);
            if(fit == _follow_services.end())
            {
                LOG_DEBUG("{}-{}服务下线了，但是当前并不关心！", service_name, host);
                return;
            }

            auto it = _services.find(service_name);
            if(it == _services.end())
            {
                LOG_WARN("删除服务节点失败：没找到{}节点", service_name);
                return;
            }
            service = it->second;
        }
        service->remove(host);
        LOG_DEBUG("{}-{} 服务下线节点，进行删除管理！", service_name, host);
    }
private:
    // 将实例名转化为服务名（实际上就是去掉最后一个'/'后面的内容
    std::string instance_to_service(const std::string& instance)
    {
        auto pos = instance.find_last_of('/');
        return instance.substr(0, pos);
    }
private:
    std::mutex _mtx;
    std::unordered_set<std::string> _follow_services;               // 存放关注的服务集合
    std::unordered_map<std::string, ChannelManager::ptr> _services; // 存放服务名和ChannelManager映射的集合
};
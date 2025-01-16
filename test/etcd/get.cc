#include <etcd/Client.hpp>
#include <etcd/Value.hpp>
#include <etcd/Response.hpp>
#include <etcd/Watcher.hpp>

void callback(const etcd::Response& resp)
{
    if(resp.is_ok() == false)
    {
        std::cout << "收到一个错误的事件通知：" << resp.error_message() << std::endl;
        return;
    }

    // 遍历resp中的所有的Event对象观察键值对变化情况
    for(auto const& es : resp.events())
    {
        if(es.event_type() == etcd::Event::EventType::PUT)
        {
            std::cout << "键值对发生了变化：\n";
            std::cout << "  之前的键值对：" << es.prev_kv().key() << " : " << es.prev_kv().as_string() << std::endl;
            std::cout << "  现在的键值对：" << es.kv().key() << " : " << es.kv().as_string() << std::endl;
        }
        else if(es.event_type() == etcd::Event::EventType::DELETE_)
        {
            std::cout << "键值对被删除：\n";
            std::cout << "  之前的键值对：" << es.prev_kv().key() << " : " << es.prev_kv().as_string() << std::endl;
            std::cout << "  现在的键值对：" << es.kv().key() << " : " << es.kv().as_string() << std::endl;
        }
    }
}

int main()
{
    // 1. 实例化客户端对象
    std::string addr = "http://127.0.0.1:2379";
    etcd::Client client(addr);

    // 2. 获取键值对信息
    auto resp = client.ls("/service").get();
    if(resp.is_ok() == false) {
        std::cout << "获取键值对数据失败：" << resp.error_message() << std::endl;
        return -1;
    }

    for(int i = 0; i < resp.keys().size(); ++i)
    {
        std::cout << "键：" << resp.value(i).key() << "  ";
        std::cout << "值：" << resp.value(i).as_string() << std::endl;
    }

    // 3. 创建watcher对象来监听键值对变化
    auto watcher = etcd::Watcher(client, "/service", callback, true);
    watcher.Wait();

    return 0;
}
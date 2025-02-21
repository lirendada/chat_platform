#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>

int main()
{
    // 1. 实例化客户端对象
    std::string addr = "http://127.0.0.1:2379";
    etcd::Client client(addr);

    // 2. 获取租约保活对象，设置租约时间为3秒
    auto keepalive = client.leasekeepalive(3).get();
    
    // 3. 获取租约id
    auto lease_id = keepalive->Lease();

    // 4. 向etcd新增数据
    auto resp1 = client.put("/service/user", "127.0.0.1:8080", lease_id).get();
    if(resp1.is_ok() == false) {
        std::cout << "新增数据失败：" << resp1.error_message() << std::endl;
        return -1;
    }else {
        std::cout << "新增数据成功！" << std::endl;
    }

    auto resp2 = client.put("/service/friend", "127.0.0.1:9090", lease_id).get();
    if(resp2.is_ok() == false) {
        std::cout << "新增数据失败：" << resp2.error_message() << std::endl;
        return -1;
    }else {
        std::cout << "新增数据成功！" << std::endl;
    }

    // 让线程睡眠，观察现象
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "put线程退出！" << std::endl;

    return 0;
}
#include <brpc/server.h>
#include <butil/logging.h>

#include "etcd.hpp"     // 服务注册模块封装，负责将服务注册到注册中心（如 etcd）
#include "logger.hpp"   // 日志模块封装，用于记录调试和错误日志
#include "utils.hpp"    // 工具类封装，包含读写文件、生成 UUID 等常用函数
#include "base.pb.h"    // 基础 protobuf 定义（如通用数据结构）
#include "file.pb.h"    // 文件操作相关的 protobuf 消息定义

namespace liren
{
    // FileServiceImpl 类实现了文件操作 RPC 服务接口，
    // 主要处理文件的上传与下载业务逻辑
    class FileServiceImpl : public liren::FileService 
    {
    public:
        // 构造函数：接收文件存储目录路径，
        // 创建该目录（如果不存在），并确保路径以 '/' 结尾，方便后续拼接文件名
        FileServiceImpl(const std::string &storage_path)
            : _storage_path(storage_path)
        {
            // 设置 umask 为 0，确保创建目录的权限不受当前进程 umask 限制
            umask(0);
            // 创建目录，权限设置为 0775（所有者可读写执行，组用户可读写执行，其他用户可读执行）
            mkdir(storage_path.c_str(), 0775);
            if (_storage_path.back() != '/') _storage_path.push_back('/');
        }

        ~FileServiceImpl(){}

        // 下载单个文件
        // 业务流程：
        // 1. 从请求中提取文件 ID（文件名）
        // 2. 拼接出完整的文件路径，并读取文件内容
        // 3. 根据读取结果组织响应，成功返回文件数据，失败则返回错误信息
        void GetSingleFile(google::protobuf::RpcController* controller,
                           const ::liren::GetSingleFileReq* request,
                           ::liren::GetSingleFileRsp* response,
                           ::google::protobuf::Closure* done) 
        {
            // 利用 ClosureGuard 确保 RPC 回调最终被调用，防止资源泄漏
            brpc::ClosureGuard rpc_guard(done);
            // 设置响应中的请求 ID，方便请求跟踪
            response->set_request_id(request->request_id());

            // 1. 获取文件 ID（在这里即文件名）
            std::string fid = request->file_id();
            std::string filename = _storage_path + fid; // 拼接完整的文件路径

            // 2. 读取文件内容到字符串 body
            std::string body;
            bool ret = readFile(filename, body);
            if (ret == false) {
                response->set_success(false);
                response->set_errmsg("读取文件数据失败！");
                LOG_ERROR("{} 读取文件数据失败！", request->request_id());
                return;
            }

            // 3. 组织响应，设置成功标志及返回的文件数据
            response->set_success(true);
            response->mutable_file_data()->set_file_id(fid);
            response->mutable_file_data()->set_file_content(body);
        }
        
        // 下载多个文件
        // 业务流程：
        //  遍历请求中的文件 ID 列表，依次读取每个文件的数据，并将其存入响应中的映射表中
        void GetMultiFile(google::protobuf::RpcController* controller,
                          const ::liren::GetMultiFileReq* request,
                          ::liren::GetMultiFileRsp* response,
                          ::google::protobuf::Closure* done) 
        {
            brpc::ClosureGuard rpc_guard(done);
            response->set_request_id(request->request_id());

            // 遍历请求中的所有文件 ID
            for (int i = 0; i < request->file_id_list_size(); i++) 
            {
                std::string fid = request->file_id_list(i);
                std::string filename = _storage_path + fid;
                std::string body;

                // 读取文件内容
                bool ret = readFile(filename, body);
                if (ret == false) {
                    response->set_success(false);
                    response->set_errmsg("读取文件数据失败！");
                    LOG_ERROR("{} 读取文件数据失败！", request->request_id());
                    return;
                }

                // 构造文件下载数据对象，并设置文件 ID 和内容
                FileDownloadData data;
                data.set_file_id(fid);
                data.set_file_content(body);

                // 将该数据插入到响应中的映射 map 中，键为文件 ID
                response->mutable_file_data()->insert({fid, data});
            }
            response->set_success(true);
        }

        // 上传单个文件
        // 业务流程：
        //  1. 生成唯一的文件 ID（UUID）作为文件名及标识符
        //  2. 将请求中的文件数据写入到指定文件中
        //  3. 组织响应，返回文件的元信息（如文件大小、文件名等）
        void PutSingleFile(google::protobuf::RpcController* controller,
                           const ::liren::PutSingleFileReq* request,
                           ::liren::PutSingleFileRsp* response,
                           ::google::protobuf::Closure* done) 
        {
            brpc::ClosureGuard rpc_guard(done);
            response->set_request_id(request->request_id());

            // 1. 生成唯一的文件 ID，并拼接生成文件路径
            std::string fid = uuid();
            std::string filename = _storage_path + fid;

            // 2. 从请求中取出文件数据，并写入到磁盘中
            bool ret = writeFile(filename, request->file_data().file_content());
            if (ret == false) {
                response->set_success(false);
                response->set_errmsg("读取文件数据失败！");
                LOG_ERROR("{} 写入文件数据失败！", request->request_id());
                return;
            }

            // 3. 构造响应：设置成功标志，并返回文件的元信息
            response->set_success(true);
            response->mutable_file_info()->set_file_id(fid);
            response->mutable_file_info()->set_file_size(request->file_data().file_size());
            response->mutable_file_info()->set_file_name(request->file_data().file_name());
        }

        // 上传多个文件
        // 业务流程：
        //  遍历每个上传文件数据，为每个文件生成唯一 ID，
        //  写入磁盘，并将对应的文件元信息添加到响应中
        void PutMultiFile(google::protobuf::RpcController* controller,
                          const ::liren::PutMultiFileReq* request,
                          ::liren::PutMultiFileRsp* response,
                          ::google::protobuf::Closure* done) 
        {
            brpc::ClosureGuard rpc_guard(done);
            response->set_request_id(request->request_id());

            // 遍历所有上传的文件数据
            for (int i = 0; i < request->file_data_size(); i++) 
            {
                // 为每个文件生成唯一文件 ID
                std::string fid = uuid();
                std::string filename = _storage_path + fid;

                // 写入文件内容到磁盘
                bool ret = writeFile(filename, request->file_data(i).file_content());
                if (ret == false) {
                    response->set_success(false);
                    response->set_errmsg("读取文件数据失败！");
                    LOG_ERROR("{} 写入文件数据失败！", request->request_id());
                    return;
                }

                // 将文件的元信息添加到响应中
                liren::FileMessageInfo *info  = response->add_file_info();
                info->set_file_id(fid);
                info->set_file_size(request->file_data(i).file_size());
                info->set_file_name(request->file_data(i).file_name());
            }
            response->set_success(true);
        }
    private:
        std::string _storage_path; // 文件存储目录路径，所有文件将存储在此目录下
    };

    // FileServer 类封装了文件存储子服务的 RPC 服务器功能
    class FileServer 
    {
    public:
        using ptr = std::shared_ptr<FileServer>;
        FileServer(const Registry::ptr &reg_client,
                   const std::shared_ptr<brpc::Server> &server)
            : _reg_client(reg_client)
            , _rpc_server(server)
        {}
        ~FileServer(){}

        // 启动 RPC 服务器，服务器将一直运行，直到收到退出指令
        void start() {
            _rpc_server->RunUntilAskedToQuit();
        }
    private:
        Registry::ptr _reg_client;                   // 服务注册客户端，用于将服务信息注册到注册中心
        std::shared_ptr<brpc::Server> _rpc_server;   // brpc 服务器对象，负责处理 RPC 请求
    };

    // FileServerBuilder 类用于构造和配置文件存储子服务
    // 包括服务注册和 RPC 服务器的初始化，确保各个模块正确组装
    class FileServerBuilder 
    {
    public:
        // 构造服务注册客户端对象，并在注册中心中注册文件服务
        // 参数：
        //  - reg_host: 注册中心地址
        //  - service_name: 服务名称（例如 "/service/file"）
        //  - access_host: 当前服务的访问地址
        void make_reg_object(const std::string &reg_host,
                             const std::string &service_name,
                             const std::string &access_host) 
        {
            _reg_client = std::make_shared<Registry>(reg_host);
            _reg_client->regiter(service_name, access_host);
        }

        // 构造 RPC 服务器对象，并启动服务
        // 参数：
        //  - port: 监听端口
        //  - timeout: 空闲连接超时时间（秒）
        //  - num_threads: 服务器工作线程数
        //  - path: 文件存储目录，默认为 "./data/"（可以自定义目录）
        void make_rpc_server(uint16_t port, int32_t timeout, 
                             uint8_t num_threads, const std::string &path = "./data/") 
        {
            _rpc_server = std::make_shared<brpc::Server>();
            // 创建 FileServiceImpl 实例，传入文件存储路径
            FileServiceImpl *file_service = new FileServiceImpl(path);

            // 将文件服务实例添加到 RPC 服务器中，服务器拥有该实例的生命周期
            int ret = _rpc_server->AddService(file_service, 
                                              brpc::ServiceOwnership::SERVER_OWNS_SERVICE);
            if (ret == -1) {
                LOG_ERROR("添加Rpc服务失败！");
                abort();
            }
            
            // 设置服务器运行参数：空闲超时和工作线程数量，启动服务器，监听指定端口；若启动失败则退出程序
            brpc::ServerOptions options;
            options.idle_timeout_sec = timeout;
            options.num_threads = num_threads;
            ret = _rpc_server->Start(port, &options);
            if (ret == -1) {
                LOG_ERROR("服务启动失败！");
                abort();
            }
        }

        // 构建 FileServer 对象，前提是注册模块和 RPC 服务器都已初始化
        FileServer::ptr build() 
        {
            if (!_reg_client) {
                LOG_ERROR("还未初始化服务注册模块！");
                abort();
            }
            if (!_rpc_server) {
                LOG_ERROR("还未初始化RPC服务器模块！");
                abort();
            }
            FileServer::ptr server = std::make_shared<FileServer>(_reg_client, _rpc_server);
            return server;
        }
    private:
        Registry::ptr _reg_client;                   // 服务注册客户端对象
        std::shared_ptr<brpc::Server> _rpc_server;   // brpc 服务器对象
    };
}
// 实现语音识别子服务，利用brpc搭建RPC服务，集成ASR模块和服务注册功能

#include <brpc/server.h>
#include <butil/logging.h>

#include "asr.hpp"      // 语音识别模块封装（负责调用底层ASR引擎，转录语音数据）
#include "etcd.hpp"     // 服务注册模块封装（用于向etcd注册服务，让其他服务可以发现它）
#include "logger.hpp"   // 日志模块封装（统一管理日志输出，让调试更轻松）
#include "speech.pb.h"  // protobuf框架代码（定义了RPC请求和响应的数据结构）

namespace liren {
    // SpeechServiceImpl 类实现了 SpeechService 接口
    // 该类负责处理语音识别的RPC调用
    class SpeechServiceImpl : public liren::SpeechService 
    {
    public:
        // 构造函数：接收一个语音识别客户端ASRClient智能指针，后续用于调用语音识别模块
        SpeechServiceImpl(const ASRClient::ptr &asr_client)
            : _asr_client(asr_client)
        {}

        ~SpeechServiceImpl(){}

        // 实现语音转文字的RPC接口
        // 参数说明：
        // - controller: RPC调用上下文（用于控制和传递RPC调用信息）
        // - request: 客户端发起的语音识别请求，其中包含语音数据和请求ID
        // - response: 服务器返回的响应对象，用于保存识别结果或错误信息
        // - done: RPC调用结束后的回调函数，确保资源释放
        void SpeechRecognition(google::protobuf::RpcController* controller,
                               const ::liren::SpeechRecognitionReq* request,
                               ::liren::SpeechRecognitionRsp* response,
                               ::google::protobuf::Closure* done) 
        {
            // 记录接收到语音转文字请求的调试日志
            LOG_DEBUG("收到语音转文字请求！");
            // 使用ClosureGuard确保即使在异常情况下也能正确调用done回调结束RPC调用
            brpc::ClosureGuard rpc_guard(done);

            // 1. 从请求中提取语音数据，然后调用ASR模块进行识别
            std::string err;  // 用于保存错误信息
            std::string res = _asr_client->recognize(request->speech_content(), err);
            
            // 如果识别结果为空，则表示识别失败
            if (res.empty()) {
                LOG_ERROR("{} 语音识别失败！", request->request_id());
                response->set_request_id(request->request_id());
                response->set_success(false);
                response->set_errmsg("语音识别失败:" + err);
                return;
            }

            // 2. 如果识别成功，则构造成功的响应信息
            response->set_request_id(request->request_id());
            response->set_success(true);
            response->set_recognition_result(res);
        }
    private:
        ASRClient::ptr _asr_client; // 保存语音识别模块的客户端对象，用于调用识别接口
    };

    // SpeechServer 类封装了RPC服务器对象，用于启动并管理整个语音识别服务
    class SpeechServer 
    {
    public:
        // 定义智能指针别名，方便管理对象生命周期
        using ptr = std::shared_ptr<SpeechServer>;
        
        // 构造函数：传入ASR客户端、服务注册客户端和RPC服务器对象
        SpeechServer(const ASRClient::ptr asr_client, 
                     const Registry::ptr &reg_client,
                     const std::shared_ptr<brpc::Server> &server)
            : _asr_client(asr_client)
            , _reg_client(reg_client)
            , _rpc_server(server)
        {}

        ~SpeechServer(){}

        // 启动RPC服务器，服务将一直运行直到收到退出信号
        void start() {
            _rpc_server->RunUntilAskedToQuit();
        }
    private:
        ASRClient::ptr _asr_client;                  // 语音识别客户端对象
        Registry::ptr _reg_client;                   // 服务注册客户端对象
        std::shared_ptr<brpc::Server> _rpc_server;   // brpc服务器对象
    };

    // SpeechServerBuilder 类用于分步构建SpeechServer实例，
    // 包括创建ASR客户端、服务注册客户端以及RPC服务器
    class SpeechServerBuilder 
    {
    public:
        // 构造语音识别客户端对象
        // 参数：
        // - app_id: 应用ID
        // - api_key: API密钥
        // - secret_key: 秘密密钥
        void make_asr_object(const std::string &app_id,
                             const std::string &api_key,
                             const std::string &secret_key) 
        {
            _asr_client = std::make_shared<ASRClient>(app_id, api_key, secret_key);
        }

        // 构造服务注册客户端对象，并在注册中心中注册服务
        // 参数：
        // - reg_host: 注册中心的主机地址
        // - service_name: 服务名称（例如"SpeechService"）
        // - access_host: 当前服务的访问地址
        void make_reg_object(const std::string &reg_host,
                             const std::string &service_name,
                             const std::string &access_host) 
        {
            _reg_client = std::make_shared<Registry>(reg_host);
            _reg_client->regiter(service_name, access_host);
        }

        // 构造并启动RPC服务器
        // 参数：
        // - port: 监听端口
        // - timeout: 空闲连接超时时间（秒）
        // - num_threads: 服务器工作线程数
        void make_rpc_server(uint16_t port, int32_t timeout, uint8_t num_threads) 
        {
            // 如果ASR客户端尚未初始化，则直接报错退出
            if (!_asr_client) {
                LOG_ERROR("还未初始化语音识别模块！");
                abort();
            }
            // 创建brpc服务器对象
            _rpc_server = std::make_shared<brpc::Server>();
            
            // 实例化语音识别服务实现对象，并将其注册到RPC服务器中
            SpeechServiceImpl *speech_service = new SpeechServiceImpl(_asr_client);
            int ret = _rpc_server->AddService(speech_service, 
                brpc::ServiceOwnership::SERVER_OWNS_SERVICE);
            if (ret == -1) {
                LOG_ERROR("添加Rpc服务失败！");
                abort();
            }
            
            // 设置服务器选项：包括空闲超时和工作线程数
            brpc::ServerOptions options;
            options.idle_timeout_sec = timeout;
            options.num_threads = num_threads;
            
            // 启动服务器，监听指定端口；如果启动失败，则报错退出
            ret = _rpc_server->Start(port, &options);
            if (ret == -1) {
                LOG_ERROR("服务启动失败！");
                abort();
            }
        }

        // 构建SpeechServer对象，确保所有依赖模块均已正确初始化
        SpeechServer::ptr build() {
            if (!_asr_client) {
                LOG_ERROR("还未初始化语音识别模块！");
                abort();
            }
            if (!_reg_client) {
                LOG_ERROR("还未初始化服务注册模块！");
                abort();
            }
            if (!_rpc_server) {
                LOG_ERROR("还未初始化RPC服务器模块！");
                abort();
            }
            // 构建并返回封装了所有模块的SpeechServer实例
            SpeechServer::ptr server = std::make_shared<SpeechServer>(_asr_client, _reg_client, _rpc_server);
            return server;
        }
    private:
        ASRClient::ptr _asr_client;                   // 存储语音识别客户端对象
        Registry::ptr _reg_client;                    // 存储服务注册客户端对象
        std::shared_ptr<brpc::Server> _rpc_server;      // 存储RPC服务器对象
    };
}
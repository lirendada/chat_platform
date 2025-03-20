#include "elastic.hpp"   // 引入 Elasticlient 客户端相关定义（ElasticSearch 操作封装）
#include "user.hxx"       // 用户实体类定义（用于ES中操作的用户数据）
// #include "message.hxx"    // 消息实体类定义（用于ES中操作的消息数据）

namespace liren 
{
    // ESClientFactory 类用于创建 Elasticsearch 客户端对象
    // 封装了通过一组主机地址创建 Elasticlient 客户端的过程
    class ESClientFactory 
    {
    public:
        // 静态方法 create：传入主机列表，返回一个 Elasticlient 客户端的智能指针
        static std::shared_ptr<elasticlient::Client> create(const std::vector<std::string> host_list) {
            return std::make_shared<elasticlient::Client>(host_list);
        }
    };

    // ESUser 类封装了针对用户数据在 Elasticsearch 中的操作
    // 包括创建用户索引和插入/更新用户数据，以及基于条件搜索用户数据
    class ESUser 
    {
    public:
        using ptr = std::shared_ptr<ESUser>;

        // 构造函数：接收一个 Elasticlient 客户端对象，后续所有操作都依赖此客户端
        ESUser(const std::shared_ptr<elasticlient::Client> &client)
            : _es_client(client)
        {}

        // createIndex 方法用于创建用户信息索引
        // 通过链式调用定义各个字段的映射规则，并最终调用 create() 创建索引
        bool createIndex() 
        {
            bool ret = ESIndex(_es_client, "user")
                .append("user_id", "keyword", "standard", true)      // user_id：关键字类型，分词器 standard，必须字段
                .append("nickname")                                     // nickname：默认映射（例如 text 类型）
                .append("phone", "keyword", "standard", true)           // phone：关键字类型，必须字段
                .append("description", "text", "standard", false)       // description：文本类型，可选字段
                .append("avatar_id", "keyword", "standard", false)      // avatar_id：关键字类型，可选字段
                .create();
            if (ret == false) {
                LOG_INFO("用户信息索引创建失败!");
                return false;
            }
            LOG_INFO("用户信息索引创建成功!");
            return true;
        }

        // appendData 方法用于新增或更新用户数据
        // 参数为用户ID、手机号、昵称、描述和头像ID
        bool appendData(const std::string &uid,
                        const std::string &phone,
                        const std::string &nickname,
                        const std::string &description,
                        const std::string &avatar_id) 
        {
            bool ret = ESInsert(_es_client, "user")
                .append("user_id", uid)
                .append("nickname", nickname)
                .append("phone", phone)
                .append("description", description)
                .append("avatar_id", avatar_id)
                .insert(uid);  // 使用 uid 作为文档的唯一标识符插入数据
            if (ret == false) {
                LOG_ERROR("用户数据插入/更新失败!");
                return false;
            }
            LOG_INFO("用户数据新增/更新成功!");
            return true;
        }

        // search 方法用于基于关键字搜索用户数据
        // 参数 key 为搜索关键字，uid_list 为排除的用户ID列表（must_not 条件）
        // 返回值为符合条件的 User 对象的 vector
        std::vector<User> search(const std::string &key, const std::vector<std::string> &uid_list) 
        {
            std::vector<User> res;
            // 使用 ESSearch 对象构造查询请求
            // 通过 append_should_match 对多个字段进行模糊匹配
            // append_must_not_terms 用于排除指定 uid_list 中的用户ID
            Json::Value json_user = ESSearch(_es_client, "user")
                .append_should_match("phone.keyword", key)
                .append_should_match("user_id.keyword", key)
                .append_should_match("nickname", key)
                .append_must_not_terms("user_id.keyword", uid_list)
                .search();
            // 判断返回结果是否为数组类型
            if (json_user.isArray() == false) {
                LOG_ERROR("用户搜索结果为空，或者结果不是数组类型");
                return res;
            }
            int sz = json_user.size();
            LOG_DEBUG("检索结果条目数量：{}", sz);
            // 遍历查询结果，构造 User 对象，并添加到返回 vector 中
            for (int i = 0; i < sz; i++) {
                User user;
                user.user_id(json_user[i]["_source"]["user_id"].asString());
                user.nickname(json_user[i]["_source"]["nickname"].asString());
                user.description(json_user[i]["_source"]["description"].asString());
                user.phone(json_user[i]["_source"]["phone"].asString());
                user.avatar_id(json_user[i]["_source"]["avatar_id"].asString());
                res.push_back(user);
            }
            return res;
        }
    private:
        // 保存 Elasticlient 客户端对象，后续所有 ES 操作都通过该客户端进行
        std::shared_ptr<elasticlient::Client> _es_client;
    };

    // ESMessage 类封装了针对消息数据在 Elasticsearch 中的操作
    // 包括创建消息索引、插入/更新消息数据、删除消息数据以及基于条件搜索消息
    // class ESMessage {
    //     public:
    //         using ptr = std::shared_ptr<ESMessage>;

    //         // 构造函数：接收 Elasticlient 客户端对象
    //         ESMessage(const std::shared_ptr<elasticlient::Client> &es_client):
    //             _es_client(es_client){}

    //         // createIndex 方法用于创建消息信息索引
    //         // 定义各字段的映射规则，最后调用 create() 创建索引
    //         bool createIndex() {
    //             bool ret = ESIndex(_es_client, "message")
    //                 .append("user_id", "keyword", "standard", false)         // user_id：关键字类型，可选字段
    //                 .append("message_id", "keyword", "standard", false)         // message_id：关键字类型，可选字段
    //                 .append("create_time", "long", "standard", false)           // create_time：长整型，用于记录时间戳
    //                 .append("chat_session_id", "keyword", "standard", true)     // chat_session_id：关键字类型，必须字段
    //                 .append("content")                                          // content：默认映射，文本内容
    //                 .create();
    //             if (ret == false) {
    //                 LOG_INFO("消息信息索引创建失败!");
    //                 return false;
    //             }
    //             LOG_INFO("消息信息索引创建成功!");
    //             return true;
    //         }

    //         // appendData 方法用于新增或更新消息数据
    //         // 参数包括用户ID、消息ID、创建时间、会话ID和消息内容
    //         bool appendData(const std::string &user_id,
    //             const std::string &message_id,
    //             const long create_time,
    //             const std::string &chat_session_id,
    //             const std::string &content) {
    //             bool ret = ESInsert(_es_client, "message")
    //                 .append("message_id", message_id)
    //                 .append("create_time", create_time)
    //                 .append("user_id", user_id)
    //                 .append("chat_session_id", chat_session_id)
    //                 .append("content", content)
    //                 .insert(message_id);  // 使用 message_id 作为文档的唯一标识符插入数据
    //             if (ret == false) {
    //                 LOG_ERROR("消息数据插入/更新失败!");
    //                 return false;
    //             }
    //             LOG_INFO("消息数据新增/更新成功!");
    //             return true;
    //         }

    //         // remove 方法用于删除指定消息数据
    //         // 参数 mid 为消息ID，通过该 ID 删除对应文档
    //         bool remove(const std::string &mid) {
    //             bool ret = ESRemove(_es_client, "message").remove(mid);
    //             if (ret == false) {
    //                 LOG_ERROR("消息数据删除失败!");
    //                 return false;
    //             }
    //             LOG_INFO("消息数据删除成功!");
    //             return true;
    //         }

    //         // search 方法用于基于关键字搜索消息数据
    //         // 参数 key 为搜索关键字，ssid 为会话ID（必须匹配该会话）
    //         // 返回值为符合条件的 Message 对象的 vector
    //         std::vector<liren::Message> search(const std::string &key, const std::string &ssid) {
    //             std::vector<liren::Message> res;
    //             // 构造搜索请求：必须匹配 chat_session_id，同时对 content 字段进行模糊匹配
    //             Json::Value json_user = ESSearch(_es_client, "message")
    //                 .append_must_term("chat_session_id.keyword", ssid)
    //                 .append_must_match("content", key)
    //                 .search();
    //             if (json_user.isArray() == false) {
    //                 LOG_ERROR("用户搜索结果为空，或者结果不是数组类型");
    //                 return res;
    //             }
    //             int sz = json_user.size();
    //             LOG_DEBUG("检索结果条目数量：{}", sz);
    //             // 遍历搜索结果，将每条消息的各字段提取并封装到 Message 对象中
    //             for (int i = 0; i < sz; i++) {
    //                 liren::Message message;
    //                 message.user_id(json_user[i]["_source"]["user_id"].asString());
    //                 message.message_id(json_user[i]["_source"]["message_id"].asString());
    //                 // 将时间戳转换为 ptime 对象
    //                 boost::posix_time::ptime ctime(boost::posix_time::from_time_t(
    //                     json_user[i]["_source"]["create_time"].asInt64()));
    //                 message.create_time(ctime);
    //                 message.session_id(json_user[i]["_source"]["chat_session_id"].asString());
    //                 message.content(json_user[i]["_source"]["content"].asString());
    //                 res.push_back(message);
    //             }
    //             return res;
    //         }
    //     private:
    //         // 保存 Elasticlient 客户端对象，所有 ES 操作均通过该对象执行
    //         std::shared_ptr<elasticlient::Client> _es_client;
    // };
} 
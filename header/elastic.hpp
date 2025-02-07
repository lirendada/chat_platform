#pragma once
#include <elasticlient/client.h>
#include <cpr/cpr.h>
#include <json/json.h>
#include <iostream>
#include <sstream>
#include <memory>
#include "logger.hpp"

/**
 * @brief 将 Json::Value 对象序列化为字符串。
 * @param val 需要序列化的 Json::Value 对象。
 * @param dst 输出参数，存储序列化后的字符串。
 * @return 如果序列化成功返回 true，否则返回 false。
 */
bool Serialize(const Json::Value &val, std::string &dst)
{
    // 定义 Json::StreamWriter 工厂类 Json::StreamWriterBuilder
    Json::StreamWriterBuilder swb;
    std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
    
    // 通过 Json::StreamWriter 中的 write 接口进行序列化
    std::stringstream ss;
    int ret = sw->write(val, &ss);
    if (ret != 0) {
        std::cout << "Json 序列化失败！\n";
        return false;
    }
    dst = ss.str();
    return true;
}

/**
 * @brief 将字符串反序列化为 Json::Value 对象。
 * @param src 需要反序列化的字符串。
 * @param val 输出参数，存储反序列化后的 Json::Value 对象。
 * @return 如果反序列化成功返回 true，否则返回 false。
 */
bool UnSerialize(const std::string &src, Json::Value &val)
{
    Json::CharReaderBuilder crb;
    std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
    std::string err;
    bool ret = cr->parse(src.c_str(), src.c_str() + src.size(), &val, &err);
    if (ret == false) {
        std::cout << "Json 反序列化失败: " << err << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief ESIndex 类用于创建和管理 Elasticsearch 索引。
 */
class ESIndex 
{
public:
    /**
     * @brief 构造函数，初始化索引名称、类型和 Elasticsearch 客户端。
     * @param client Elasticsearch 客户端对象的共享指针。
     * @param name 索引名称。
     * @param type 索引类型，默认为 "_doc"。
     */
    ESIndex(std::shared_ptr<elasticlient::Client> &client, 
                const std::string &name, 
                const std::string &type = "_doc")
            : _name(name)
            , _type(type)
            , _client(client) 
    {
        // 初始化索引的 settings 部分，配置分词器
        Json::Value analysis;
        Json::Value analyzer;
        Json::Value ik;
        Json::Value tokenizer;
        tokenizer["tokenizer"] = "ik_max_word";  // 使用 ik_max_word 分词器
        ik["ik"] = tokenizer;
        analyzer["analyzer"] = ik;
        analysis["analysis"] = analyzer;
        _index["settings"] = analysis;  // 将分词器配置添加到索引 settings 中
    }

    /**
     * @brief 向索引中添加字段。
     * @param key 字段名称。
     * @param type 字段类型，默认为 "text"。
     * @param analyzer 分词器，默认为 "ik_max_word"。
     * @param enabled 是否启用字段，默认为 true。
     * @return 返回当前对象的引用，支持链式调用。
     */
    ESIndex& append(const std::string &key, 
                    const std::string &type = "text", 
                    const std::string &analyzer = "ik_max_word", 
                    bool enabled = true)
    {
        Json::Value fields;
        fields["type"] = type;  // 设置字段类型
        fields["analyzer"] = analyzer;  // 设置分词器
        if (enabled == false) 
            fields["enabled"] = enabled;  // 设置字段是否启用
        _properties[key] = fields;  // 将字段添加到 properties 中
        return *this;
    }

    /**
     * @brief 创建 Elasticsearch 索引。
     * @param index_id 索引的 ID，默认为 "default_index_id"。
     * @return 如果索引创建成功返回 true，否则返回 false。
     */
    bool create(const std::string &index_id = "default_index_id") 
    {
        // 将 mappings 部分添加到索引配置中
        Json::Value mappings;
        mappings["dynamic"] = true;            // 允许动态映射
        mappings["properties"] = _properties;  // 添加字段属性
        _index["mappings"] = mappings;

        // 将索引配置序列化为字符串
        std::string body;
        bool ret = Serialize(_index, body);
        if (ret == false) {
            LOG_ERROR("索引序列化失败！");
            return false;
        }
        LOG_DEBUG("{}", body);

        // 发起创建索引请求
        try {
            auto rsp = _client->index(_name, _type, index_id, body);
            if (rsp.status_code < 200 || rsp.status_code >= 300) {
                LOG_ERROR("创建 ES 索引 {} 失败，响应状态码异常: {}", _name, rsp.status_code);
                return false;
            }
        } catch(std::exception &e) {
            LOG_ERROR("创建 ES 索引 {} 失败: {}", _name, e.what());
            return false;
        }
        return true;
    }

private:
    std::string _name;  // 索引名称
    std::string _type;  // 索引类型
    Json::Value _properties;  // 索引字段属性
    Json::Value _index;  // 索引配置
    std::shared_ptr<elasticlient::Client> _client;  // Elasticsearch 客户端
};
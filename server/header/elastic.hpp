#pragma once
#include <elasticlient/client.h>
#include <cpr/cpr.h>
#include <json/json.h>
#include <iostream>
#include <sstream>
#include <memory>
#include "logger.hpp"

namespace liren {
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
            LOG_ERROR("Json 序列化失败！");
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
            LOG_ERROR("Json 序列化失败：{}", err);
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
            // LOG_DEBUG("{}", body);

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

    // ESInsert类用于将数据插入到 Elasticsearch 中
    class ESInsert 
    {
    public:
        // 构造函数，初始化 Elasticsearch 客户端、索引名称、类型（默认为 "_doc"）
        ESInsert(std::shared_ptr<elasticlient::Client> &client, 
                const std::string &name, 
                const std::string &type = "_doc")
            : _name(name)          
            , _type(type)          
            , _client(client)      
        {}

        // 模板函数，用于向待插入的 JSON 数据中追加键值对
        template<typename T>
        ESInsert &append(const std::string &key, const T &val)
        {
            _item[key] = val;  // 将键值对插入到内部的 JSON 对象 _item 中
            return *this;      // 返回当前对象的引用，支持链式调用
        }

        // 插入数据到 Elasticsearch 中，返回是否成功
        bool insert(const std::string id = "") 
        {
            std::string body;   // 保存序列化后的 JSON 字符串
            bool ret = Serialize(_item, body);  // 序列化 _item 为 JSON 格式的字符串
            if (ret == false) { 
                LOG_ERROR("索引序列化失败！");  
                return false; 
            }
            // LOG_DEBUG("{}", body);  
            
            // 发起插入请求
            try {
                // 使用客户端发起索引请求，将数据插入到指定的索引中
                auto rsp = _client->index(_name, _type, id, body);
                if (rsp.status_code < 200 || rsp.status_code >= 300) {  // 如果响应状态码不在成功范围内
                    LOG_ERROR("新增数据 {} 失败，响应状态码异常: {}", body, rsp.status_code);  
                    return false;  
                }
            } catch(std::exception &e) {  
                LOG_ERROR("新增数据 {} 失败: {}", body, e.what());  
                return false;  
            }
            return true;  
        }

    private:
        std::string _name;                 // 索引名称
        std::string _type;                 // 索引类型
        Json::Value _item;                 // 待插入的数据，存储为 JSON 对象
        std::shared_ptr<elasticlient::Client> _client;  // Elasticsearch 客户端
    };

    class ESRemove {
    public:
        ESRemove(std::shared_ptr<elasticlient::Client> &client, 
                const std::string &name, 
                const std::string &type = "_doc")
            : _name(name)          
            , _type(type)          
            , _client(client)      
        {}

        bool remove(const std::string &id) {
            try {
                auto rsp = _client->remove(_name, _type, id);
                if (rsp.status_code < 200 || rsp.status_code >= 300) {
                    LOG_ERROR("删除数据 {} 失败，响应状态码异常: {}", id, rsp.status_code);
                    return false;
                }
            } catch(std::exception &e) {
                LOG_ERROR("删除数据 {} 失败: {}", id, e.what());
                return false;
            }
            return true;
        }
    private:
        std::string _name;
        std::string _type;
        std::shared_ptr<elasticlient::Client> _client;
    };

    class ESSearch 
    {
    public:
        // 构造函数，初始化搜索类所需的基本参数，包括客户端、索引名称和类型
        ESSearch(std::shared_ptr<elasticlient::Client> &client, 
                const std::string &name, 
                const std::string &type = "_doc")
            : _name(name)           
            , _type(type)           
            , _client(client)       
        {}

        // 添加一个“must_not”条件，意味着这些条件的字段值不能匹配
        ESSearch& append_must_not_terms(const std::string &key, const std::vector<std::string> &vals) {
            Json::Value fields;
            // 将所有的条件值添加到对应的字段
            for (const auto& val : vals) {
                fields[key].append(val);
            }
            Json::Value terms;
            terms["terms"] = fields;  // 构建terms查询条件
            _must_not.append(terms);  // 将条件添加到must_not数组中
            return *this;  // 返回当前对象，支持链式调用
        }

        // 添加一个“should”条件，表示这些条件值是“推荐匹配”项（即加分项）
        ESSearch& append_should_match(const std::string &key, const std::string &val) {
            Json::Value field;
            field[key] = val;  // 设置匹配条件的字段和值
            Json::Value match;
            match["match"] = field;  // 构建match查询条件
            _should.append(match);   // 将条件添加到should数组中
            return *this;  // 返回当前对象，支持链式调用
        }

        // 添加一个“must”条件，表示这些条件值是“必须匹配”项
        ESSearch& append_must_term(const std::string &key, const std::string &val) {
            Json::Value field;
            field[key] = val;  // 设置匹配条件的字段和值
            Json::Value term;
            term["term"] = field;  // 构建term查询条件
            _must.append(term);    // 将条件添加到must数组中
            return *this;  // 返回当前对象，支持链式调用
        }

        // 添加一个“must”条件（match类型），表示这些条件值是“必须匹配”项
        ESSearch& append_must_match(const std::string &key, const std::string &val){
            Json::Value field;
            field[key] = val;  // 设置匹配条件的字段和值
            Json::Value match;
            match["match"] = field;  // 构建match查询条件
            _must.append(match);     // 将条件添加到must数组中
            return *this;  // 返回当前对象，以支持链式调用
        }

        // 发起实际的搜索请求，并返回结果
        Json::Value search(){
            // 搭建搜索请求的正文框架
            Json::Value cond;
            if (_must_not.empty() == false) cond["must_not"] = _must_not;  // 如果有must_not条件，添加到查询中
            if (_should.empty() == false) cond["should"] = _should;        // 如果有should条件，添加到查询中
            if (_must.empty() == false) cond["must"] = _must;              // 如果有must条件，添加到查询中
            Json::Value query;
            query["bool"] = cond;  // 将上述条件组成一个bool查询
            Json::Value root;
            root["query"] = query; // 将查询条件放入query中

            // 序列化查询条件为字符串
            std::string body;
            bool ret = Serialize(root, body);
            if (ret == false) {
                LOG_ERROR("索引序列化失败！");  
                return Json::Value();
            }
            // LOG_DEBUG("{}", body);  

            // 发起搜索请求
            cpr::Response rsp;
            try {
                rsp = _client->search(_name, _type, body);  // 使用Elasticsearch客户端发起搜索
                if (rsp.status_code < 200 || rsp.status_code >= 300) {
                    LOG_ERROR("检索数据 {} 失败，响应状态码异常: {}", body, rsp.status_code);  
                    return Json::Value();
                }
            } catch(std::exception &e) {
                LOG_ERROR("检索数据 {} 失败: {}", body, e.what());  
                return Json::Value();
            }

            // 需要对响应正文进行反序列化
            // LOG_DEBUG("检索响应正文: [{}]", rsp.text);  // 可选，调试时可以打印响应的正文
            Json::Value json_res;
            ret = UnSerialize(rsp.text, json_res);  
            if (ret == false) {
                LOG_ERROR("检索数据 {} 结果反序列化失败", rsp.text);  
                return Json::Value();
            }
            return json_res["hits"]["hits"];  // 返回命中的结果部分
        }

    private:
        std::string _name;  // 索引名称
        std::string _type;  // 文档类型
        Json::Value _must_not;  // 存放“must_not”条件的数组
        Json::Value _should;    // 存放“should”条件的数组
        Json::Value _must;      // 存放“must”条件的数组
        std::shared_ptr<elasticlient::Client> _client;  // Elasticsearch客户端
    };
}
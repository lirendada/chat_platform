#include "mysql.hpp"         // MySQL 数据库相关头文件（封装了 MySQL 连接和操作）
#include "user.hxx"          // 用户实体类声明
#include "user-odb.hxx"      // 用户实体类的 ODB 映射声明

// 业务功能包括：
//  用户注册、用户登录、验证码获取、手机号注册、手机号登录、获取用户信息、用户信息修改等
//  同时支持通过昵称、手机号、用户ID以及多个用户ID获取用户信息

namespace liren 
{
    // UserTable 类封装了对用户表的数据库操作，提供插入、更新以及多种查询接口
    class UserTable 
    {
    public:
        using ptr = std::shared_ptr<UserTable>;

        // 构造函数：通过传入的数据库对象初始化 UserTable
        UserTable(const std::shared_ptr<odb::core::database> &db)
            : _db(db)
        {}

        // 插入用户记录
        // 参数：user - 需要插入的用户对象（以智能指针形式传入）
        // 返回值：成功返回 true，失败返回 false
        bool insert(const std::shared_ptr<User> &user) 
        {
            try {
                odb::transaction trans(_db->begin()); // 开启一个数据库事务，确保操作的原子性
                _db->persist(*user); // 持久化用户对象，将其写入数据库
                trans.commit();      // 提交事务，保存数据
            } catch (std::exception &e) {
                LOG_ERROR("新增用户失败 {}:{}！", user->nickname(), e.what());
                return false;
            }
            return true;
        }

        // 更新用户记录
        // 参数：user - 需要更新的用户对象
        // 返回值：成功返回 true，失败返回 false
        bool update(const std::shared_ptr<User> &user) 
        {
            try {
                odb::transaction trans(_db->begin());
                _db->update(*user); // 更新用户对象数据
                trans.commit();
            } catch (std::exception &e) {
                LOG_ERROR("更新用户失败 {}:{}！", user->nickname(), e.what());
                return false;
            }
            return true;
        }

        // 根据昵称查询用户记录
        // 参数：nickname - 用户的昵称
        // 返回值：若查询成功，返回查询到的用户对象；否则返回空指针
        std::shared_ptr<User> select_by_nickname(const std::string &nickname) 
        {
            std::shared_ptr<User> res;
            try {
                odb::transaction trans(_db->begin());

                // 定义查询类型别名，便于构造查询条件
                typedef odb::query<User> query;
                typedef odb::result<User> result;
                // 利用 ODB 提供的 query_one 方法查询符合条件的用户记录
                res.reset(_db->query_one<User>(query::nickname == nickname));

                trans.commit();
            } catch (std::exception &e) {
                LOG_ERROR("通过昵称查询用户失败 {}:{}！", nickname, e.what());
            }
            return res;
        }

        // 根据手机号查询用户记录
        // 参数：phone - 用户的手机号
        // 返回值：查询到的用户对象指针，如果查询失败则返回空指针
        std::shared_ptr<User> select_by_phone(const std::string &phone) 
        {
            std::shared_ptr<User> res;
            try {
                odb::transaction trans(_db->begin());
                typedef odb::query<User> query;
                typedef odb::result<User> result;
                res.reset(_db->query_one<User>(query::phone == phone));
                trans.commit();
            } catch (std::exception &e) {
                LOG_ERROR("通过手机号查询用户失败 {}:{}！", phone, e.what());
            }
            return res;
        }

        // 根据用户ID查询用户记录
        // 参数：user_id - 用户的唯一标识ID
        // 返回值：若查询成功，返回对应的用户对象；否则返回空指针
        std::shared_ptr<User> select_by_id(const std::string &user_id) 
        {
            std::shared_ptr<User> res;
            try {
                odb::transaction trans(_db->begin());
                typedef odb::query<User> query;
                typedef odb::result<User> result;
                res.reset(_db->query_one<User>(query::user_id == user_id));
                trans.commit();
            } catch (std::exception &e) {
                LOG_ERROR("通过用户ID查询用户失败 {}:{}！", user_id, e.what());
            }
            return res;
        }

        // 根据多个用户ID查询用户记录
        // 参数：id_list - 用户ID列表，返回所有匹配的用户记录
        // 返回值：用户对象的 vector，如果列表为空或查询失败，返回空 vector
        std::vector<User> select_multi_users(const std::vector<std::string> &id_list) 
        {
            // 如果传入的 id_list 为空，直接返回空 vector
            if (id_list.empty()) {
                return std::vector<User>();
            }
            std::vector<User> res;
            try {
                odb::transaction trans(_db->begin());
                typedef odb::query<User> query;
                typedef odb::result<User> result;

                // 构造 SQL 中 IN 条件的字符串：例如 "user_id in ('id1','id2',...)"
                std::stringstream ss;
                ss << "user_id in (";
                for (const auto &id : id_list) {
                    ss << "'" << id << "',";
                }

                // 移除最后多余的逗号，并添加右括号
                std::string condition = ss.str();
                condition.pop_back();
                condition += ")";

                // 利用自定义条件进行查询
                result r(_db->query<User>(condition));
                // 遍历查询结果，存入返回 vector 中
                for (result::iterator i(r.begin()); i != r.end(); ++i) {
                    res.push_back(*i);
                }

                trans.commit();
            } catch (std::exception &e) {
                LOG_ERROR("通过用户ID批量查询用户失败:{}！", e.what());
            }
            return res;
        }

    private:
        // 数据库对象指针，所有数据库操作均通过此对象执行
        std::shared_ptr<odb::core::database> _db;
    };

} 

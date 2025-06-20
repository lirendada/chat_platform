#include "../../../header/mysql_user.hpp"
#include "../../../odb/user.hxx"
#include "user-odb.hxx"
#include <gflags/gflags.h>

DEFINE_bool(run_mode, false, "程序的运行模式，false-调试； true-发布；");
DEFINE_string(log_file, "", "发布模式下，用于指定日志的输出文件");
DEFINE_int32(log_level, 0, "发布模式下，用于指定日志输出等级");

void insert(liren::UserTable &user) {
    auto user1 = std::make_shared<liren::User>("uid1", "昵称1", "123456");
    user.insert(user1);
    
    auto user2 = std::make_shared<liren::User>("uid2", "15566667777");
    user.insert(user2);
}

void update_by_id(liren::UserTable &user_tb) {
    auto user = user_tb.select_by_id("uid1");
    user->description("我是一个风一样的男子！！");
    user_tb.update(user);
}
void update_by_phone(liren::UserTable &user_tb) {
    auto user = user_tb.select_by_phone("15566667777");
    user->password("22223333");
    user_tb.update(user);
}
void update_by_nickname(liren::UserTable &user_tb) {
    auto user = user_tb.select_by_nickname("uid2");
    user->nickname("昵称2");
    user_tb.update(user);
}

void select_users(liren::UserTable &user_tb) {
    std::vector<std::string> id_list = {"uid1", "uid2"};
    auto res = user_tb.select_multi_users(id_list);
    for (auto user : res) {
        std::cout << user.nickname() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    liren::init_logger(FLAGS_run_mode, FLAGS_log_file, FLAGS_log_level);

    auto db = liren::ODBFactory::create("root", "TThh1314520!", "127.0.0.1", "liren", "utf8", 0, 1);
    
    liren::UserTable user(db);

    // insert(user);
    // update_by_id(user);
    // update_by_phone(user);
    // update_by_nickname(user);
    select_users(user);
    return 0;
}
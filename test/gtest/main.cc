#include <iostream>
#include <string>
#include <gtest/gtest.h>
using namespace std;

int add(int a, int b)
{
    return a + b;
}

TEST(测试名称, 加法用例测试) {
    ASSERT_EQ(add(1, 2), 3);
    ASSERT_LT(add(1, 2), 4);
    ASSERT_LT(add(1, 2), 2);
    ASSERT_NE(add(1, 2), 2);
    cout << "执行到尾部！" << endl;
}

TEST(测试名称, 字符串拼接) {
    std::string a = "Hello, ";
    std::string b = "World!";
    ASSERT_EQ(a + b, "Hello, World!");  // 检查字符串拼接
}

int main(int argc, char* argv[])
{
    // 初始化
    testing::InitGoogleTest(&argc, argv);

    // 运行所有测试样例
    return RUN_ALL_TESTS();
}
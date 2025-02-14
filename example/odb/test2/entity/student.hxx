#pragma once  
#include <string>
#include <cstddef> // std::size_t
#include <boost/date_time/posix_time/posix_time.hpp>  // 引入 boost 日期时间库，用于处理时间相关类型
#include <odb/nullable.hxx>  // ODB 库的可空类型，用于表示可以为空的字段
#include <odb/core.hxx>  // ODB 核心库，提供 ORM（对象关系映射）功能

// ODB 注解，指定此类映射为数据库中的一张表
#pragma db object
class Student
{
public:
    // 默认构造函数
    Student() {}

    // 带参数的构造函数，初始化学生信息
    Student(unsigned long sn, const std::string &name, unsigned short age, unsigned long cid):
        _sn(sn), _name(name), _age(age), _classes_id(cid)
    {}

    unsigned long sn() { return _sn; }
    void sn(unsigned long num) { _sn = num; }

    std::string name() { return _name; }
    void name(const std::string &name) { _name = name; }

    odb::nullable<unsigned short> age() { return _age; }
    void age(unsigned short num) { _age = num; }

    unsigned long classes_id() { return _classes_id; }
    void classes_id(unsigned long cid) { _classes_id = cid; }
private:
    // ODB需要访问私有成员
    friend class odb::access;

    // 数据库中自动生成的主键 ID
    #pragma db id auto
    unsigned long _id;

    // 学号，唯一约束
    #pragma db unique
    unsigned long _sn;

    // 学生姓名
    std::string _name;

    // 学生年龄，使用 nullable 类型，表示此字段可能为空
    odb::nullable<unsigned short> _age;

    // 班级 ID，索引字段
    #pragma db index
    unsigned long _classes_id;
};

// ODB注解，指定此类映射为数据库中的一张表
#pragma db object
class Classes 
{
public:
    Classes() {}
    Classes(const std::string &name) : _name(name) {}

    std::string name() { return _name; }
    void name(const std::string &name) { _name = name; }
private:
    // ODB需要访问私有成员
    friend class odb::access;

    // 数据库中自动生成的主键 ID
    #pragma db id auto
    unsigned long _id;

    // 班级名称
    std::string _name;
};

// 查询所有的学生信息，并显示班级名称
// 使用 ODB 的视图（view），通过关联 Student 和 Classes 表
#pragma db view object(Student)\  
                object(Classes = classes : Student::_classes_id == classes::_id)\  
                query((?))  
struct classes_student {
    // 映射查询结果中的字段到结构体成员
    #pragma db column(Student::_id)  // 学生 ID
    unsigned long id;

    #pragma db column(Student::_sn)  // 学生学号
    unsigned long sn;

    #pragma db column(Student::_name)  // 学生姓名
    std::string name;

    #pragma db column(Student::_age)  // 学生年龄（可能为空）
    odb::nullable<unsigned short> age;

    #pragma db column(classes::_name)  // 班级名称
    std::string classes_name;
};

// 查询所有学生姓名，外部调用时传入的过滤条件
#pragma db view query("select name from Student" + (?))  // 查询语句与外部传入的过滤条件拼接
struct all_name {
    // 映射查询结果中的学生姓名字段
    std::string name;
};

// 生成 ODB 的 MySQL 相关数据库模式和查询代码
// `-d mysql` 表示数据库类型为 MySQL，`--std c++11` 表示使用 C++11 标准，`--generate-query` 生成查询接口，`--generate-schema` 生成数据库模式
// `--profile boost/date-time` 表示使用 Boost 日期时间库配置，`student.hxx` 是头文件路径
// odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time student.hxx

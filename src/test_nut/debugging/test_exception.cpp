
#include <nut/unittest/unittest.h>
#include <nut/debugging/exception.h>
#include <exception>
#include <stdexcept>
#include <cstring>

using namespace std;
using namespace nut;

class TestException : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_constructor);
        NUT_REGISTER_CASE(test_move_constructor);
        NUT_REGISTER_CASE(test_getters);
        NUT_REGISTER_CASE(test_what);
        NUT_REGISTER_CASE(test_source_info);
        NUT_REGISTER_CASE(test_throw_catch);
        NUT_REGISTER_CASE(test_macro);
    }

    void test_constructor()
    {
        Exception e1(100, "test error message");
        NUT_TA(e1.get_code() == 100);
        NUT_TA(e1.get_message() == "test error message");
        NUT_TA(e1.get_source_path() == nullptr);
        NUT_TA(e1.get_source_line() == -1);
        NUT_TA(e1.get_source_func() == nullptr);

        Exception e2(200, "another error", __FILE__, __LINE__, __FUNCTION__);
        NUT_TA(e2.get_code() == 200);
        NUT_TA(e2.get_message() == "another error");
        NUT_TA(e2.get_source_path() != nullptr);
        NUT_TA(e2.get_source_line() > 0);
        NUT_TA(e2.get_source_func() != nullptr);
    }

    void test_move_constructor()
    {
        string msg = "move message";
        Exception e(300, std::move(msg), __FILE__, __LINE__, __FUNCTION__);
        NUT_TA(e.get_code() == 300);
        NUT_TA(e.get_message() == "move message");
        // msg 应该被移动了，但标准未规定移动后内容，所以不检查
    }

    void test_getters()
    {
        const char* file = __FILE__;
        int line = __LINE__ + 1;
        const char* func = __FUNCTION__;
        Exception e(400, "getters test", file, line, func);

        NUT_TA(e.get_code() == 400);
        NUT_TA(e.get_message() == "getters test");
        NUT_TA(e.get_source_path() == file);
        NUT_TA(e.get_source_line() == line);
        NUT_TA(e.get_source_func() == func);

        // 测试 get_source_file() 提取文件名
        const char* filename = e.get_source_file();
        NUT_TA(filename != nullptr);
        // 文件名应该包含 "test_exception.cpp"
        string fn_str(filename);
        NUT_TA(fn_str.find("test_exception.cpp") != string::npos);
    }

    void test_what()
    {
        Exception e(500, "what test message");
        const char* what_str = e.what();
        NUT_TA(what_str != nullptr);
        NUT_TA(strcmp(what_str, "what test message") == 0);

        // what() 应该返回 message 的 c_str()
        NUT_TA(what_str == e.get_message().c_str());
    }

    void test_source_info()
    {
        Exception e1(600, "no source info");
        NUT_TA(e1.get_source_file() == nullptr);
        NUT_TA(e1.get_source_path() == nullptr);
        NUT_TA(e1.get_source_line() == -1);
        NUT_TA(e1.get_source_func() == nullptr);

        Exception e2(700, "with source info", "test_file.cpp", 42, "test_function");
        NUT_TA(e2.get_source_path() != nullptr);
        NUT_TA(strcmp(e2.get_source_path(), "test_file.cpp") == 0);
        NUT_TA(e2.get_source_line() == 42);
        NUT_TA(e2.get_source_func() != nullptr);
        NUT_TA(strcmp(e2.get_source_func(), "test_function") == 0);

        // get_source_file() 应该提取文件名
        const char* filename = e2.get_source_file();
        NUT_TA(filename != nullptr);
        NUT_TA(strcmp(filename, "test_file.cpp") == 0);
    }

    void test_throw_catch()
    {
        bool caught = false;
        try
        {
            throw Exception(800, "throw test", __FILE__, __LINE__, __FUNCTION__);
        }
        catch (const Exception& e)
        {
            caught = true;
            NUT_TA(e.get_code() == 800);
            NUT_TA(e.get_message() == "throw test");
        }
        catch (...)
        {
            NUT_TA(false); // 应该捕获 Exception
        }
        NUT_TA(caught);

        // 测试继承自 std::exception
        caught = false;
        try
        {
            throw Exception(900, "std::exception test");
        }
        catch (const std::exception& e)
        {
            caught = true;
            const char* msg = e.what();
            NUT_TA(msg != nullptr);
            NUT_TA(strcmp(msg, "std::exception test") == 0);
        }
        NUT_TA(caught);
    }

    void test_macro()
    {
        // 测试 THROW_ON_FALSE 宏
        bool thrown = false;
        try
        {
            THROW_ON_FALSE(false, 1000);
        }
        catch (const Exception& e)
        {
            thrown = true;
            NUT_TA(e.get_code() == 1000);
            NUT_TA(e.get_message() == "false");
            NUT_TA(e.get_source_path() != nullptr);
            NUT_TA(e.get_source_line() > 0);
            NUT_TA(e.get_source_func() != nullptr);
        }
        NUT_TA(thrown);

        // 测试条件为 true 时不抛出
        thrown = false;
        try
        {
            THROW_ON_FALSE(true, 2000);
            thrown = false; // 不应该到这里
        }
        catch (const Exception&)
        {
            thrown = true;
        }
        NUT_TA(!thrown);

        // 测试复杂表达式
        thrown = false;
        try
        {
            int x = 5;
            int y = 10;
            THROW_ON_FALSE(x > y, 3000);
        }
        catch (const Exception& e)
        {
            thrown = true;
            NUT_TA(e.get_code() == 3000);
            NUT_TA(strstr(e.get_message().c_str(), "x > y") != nullptr);
        }
        NUT_TA(thrown);
    }
};

NUT_REGISTER_FIXTURE(TestException, "debugging,quiet")


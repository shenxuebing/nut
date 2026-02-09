
#include <nut/unittest/unittest.h>
#include <nut/debugging/source_location.h>
#include <cstring>

using namespace std;
using namespace nut;

class TestSourceLocation : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_constructor);
        NUT_REGISTER_CASE(test_getters);
        NUT_REGISTER_CASE(test_comparison);
        NUT_REGISTER_CASE(test_to_string);
        NUT_REGISTER_CASE(test_macros);
    }

    void test_constructor()
    {
        SourceLocation loc(__FILE__, __LINE__, __FUNCTION__);
        NUT_TA(loc.get_file_path() == __FILE__);
        NUT_TA(loc.get_line_number() == __LINE__ - 1);
        NUT_TA(loc.get_function_name() == __FUNCTION__);

        SourceLocation loc2("test_file.cpp", 42, "test_func");
        NUT_TA(strcmp(loc2.get_file_path(), "test_file.cpp") == 0);
        NUT_TA(loc2.get_line_number() == 42);
        NUT_TA(strcmp(loc2.get_function_name(), "test_func") == 0);

        SourceLocation loc3(nullptr, -1, nullptr);
        NUT_TA(loc3.get_file_path() == nullptr);
        NUT_TA(loc3.get_line_number() == -1);
        NUT_TA(loc3.get_function_name() == nullptr);
    }

    void test_getters()
    {
        SourceLocation loc("/path/to/test_file.cpp", 100, "test_function");
        
        NUT_TA(strcmp(loc.get_file_path(), "/path/to/test_file.cpp") == 0);
        NUT_TA(loc.get_line_number() == 100);
        NUT_TA(strcmp(loc.get_function_name(), "test_function") == 0);

        // 测试 get_file_name() 提取文件名
        const char* filename = loc.get_file_name();
        NUT_TA(filename != nullptr);
        NUT_TA(strcmp(filename, "test_file.cpp") == 0);

        // 测试 Windows 路径
        SourceLocation loc2("C:\\path\\to\\file.cpp", 200, "func");
        NUT_TA(strcmp(loc2.get_file_name(), "file.cpp") == 0);

        // 测试 Unix 路径
        SourceLocation loc3("/usr/local/include/file.h", 300, "func");
        NUT_TA(strcmp(loc3.get_file_name(), "file.h") == 0);

        // 测试相对路径
        SourceLocation loc4("subdir/file.cpp", 400, "func");
        NUT_TA(strcmp(loc4.get_file_name(), "file.cpp") == 0);
    }

    void test_comparison()
    {
        SourceLocation loc1("file.cpp", 10, "func");
        SourceLocation loc2("file.cpp", 10, "func");
        SourceLocation loc3("file.cpp", 20, "func");
        SourceLocation loc4("other.cpp", 10, "func");
        SourceLocation loc5("file.cpp", 10, "other");

        NUT_TA(loc1 == loc2);
        NUT_TA(!(loc1 != loc2));

        NUT_TA(loc1 != loc3);
        NUT_TA(!(loc1 == loc3));

        NUT_TA(loc1 != loc4);
        NUT_TA(loc1 != loc5);

        // 测试 nullptr
        SourceLocation loc6(nullptr, -1, nullptr);
        SourceLocation loc7(nullptr, -1, nullptr);
        SourceLocation loc8("file.cpp", -1, nullptr);

        NUT_TA(loc6 == loc7);
        NUT_TA(loc6 != loc8);

        // 测试相同路径但不同行
        SourceLocation loc9("file.cpp", 1, "func");
        SourceLocation loc10("file.cpp", 2, "func");
        NUT_TA(loc9 != loc10);
    }

    void test_to_string()
    {
        SourceLocation loc("test_file.cpp", 42, "test_function");
        string str = loc.to_string();
        NUT_TA(str.find("test_file.cpp") != string::npos);
        NUT_TA(str.find("42") != string::npos);
        NUT_TA(str.find("test_function") != string::npos);
        NUT_TA(str.find("()") != string::npos); // 函数名后应该有 ()

        // 测试无函数名的情况
        SourceLocation loc2("file.cpp", 100, nullptr);
        string str2 = loc2.to_string();
        NUT_TA(str2.find("file.cpp") != string::npos);
        NUT_TA(str2.find("100") != string::npos);
        // 不应该包含函数名部分
        NUT_TA(str2.find("()") == string::npos);

        // 测试路径包含目录的情况
        SourceLocation loc3("/path/to/file.cpp", 200, "func");
        string str3 = loc3.to_string();
        // to_string 应该只包含文件名，不包含路径
        NUT_TA(str3.find("file.cpp") != string::npos);
        NUT_TA(str3.find("/path/to/") == string::npos);
    }

    void test_macros()
    {
        // 测试 NUT_SOURCE_LOCATION 宏
        SourceLocation loc = NUT_SOURCE_LOCATION;
        NUT_TA(loc.get_file_path() != nullptr);
        NUT_TA(loc.get_line_number() > 0);
        NUT_TA(loc.get_function_name() != nullptr);

        // 验证行号在当前行附近
        int current_line = __LINE__ - 3; // 减去一些偏移
        NUT_TA(loc.get_line_number() >= current_line);

        // 测试 NUT_SOURCE_LOCATION_ARGS 宏（作为参数使用）
        SourceLocation loc2(NUT_SOURCE_LOCATION_ARGS);
        NUT_TA(loc2.get_file_path() != nullptr);
        NUT_TA(loc2.get_line_number() > 0);
        NUT_TA(loc2.get_function_name() != nullptr);
    }
};

NUT_REGISTER_FIXTURE(TestSourceLocation, "debugging,quiet")


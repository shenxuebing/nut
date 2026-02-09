/**
 * NUT Debugging 模块使用示例
 * 
 * 演示如何使用 debugging 目录中的各个 API：
 * - Exception: 带调试信息的异常类
 * - SourceLocation: 源码位置追踪
 * - Backtrace: 调用栈追踪
 * - DestroyChecker: 对象生命周期检查器
 * - ProcAddrMaps: 进程地址映射（仅Linux）
 */

#include <iostream>
#include <string>
#include <fstream>

// 包含 NUT debugging 模块
#include <nut/debugging/exception.h>
#include <nut/debugging/source_location.h>
#include <nut/debugging/backtrace.h>
#include <nut/debugging/destroy_checker.h>

#if NUT_PLATFORM_OS_LINUX
#include <nut/debugging/proc_addr_maps.h>
#endif

using namespace std;
using namespace nut;

// ============================================================================
// 1. Exception 使用示例
// ============================================================================

void example_exception_basic()
{
    cout << "\n=== Exception 基本用法 ===" << endl;
    
    try
    {
        // 最简单的异常
        throw Exception(100, "简单的错误消息");
    }
    catch (const Exception& e)
    {
        cout << "错误码: " << e.get_code() << endl;
        cout << "错误消息: " << e.get_message() << endl;
        cout << "源文件: " << (e.get_source_file() ? e.get_source_file() : "未指定") << endl;
    }
}

void example_exception_detailed()
{
    cout << "\n=== Exception 详细用法（带调试信息）===" << endl;
    
    try
    {
        throw Exception(
            200,
            "文件操作失败",
            __FILE__,    // 自动获取当前文件
            __LINE__,    // 自动获取当前行号
            __FUNCTION__ // 自动获取当前函数名
        );
    }
    catch (const Exception& e)
    {
        cout << "错误码: " << e.get_code() << endl;
        cout << "错误消息: " << e.get_message() << endl;
        cout << "源文件: " << (e.get_source_file() ? e.get_source_file() : "未指定") << endl;
        cout << "完整路径: " << (e.get_source_path() ? e.get_source_path() : "未指定") << endl;
        cout << "行号: " << e.get_source_line() << endl;
        cout << "函数: " << (e.get_source_func() ? e.get_source_func() : "未指定") << endl;
    }
}

void example_throw_on_false()
{
    cout << "\n=== THROW_ON_FALSE 宏用法 ===" << endl;
    
    int x = 5;
    int y = 10;
    
    try
    {
        // 如果条件为 false，自动抛出异常
        THROW_ON_FALSE(x > y, 300);  // x > y 为 false，会抛出异常
    }
    catch (const Exception& e)
    {
        cout << "捕获到异常！" << endl;
        cout << "错误码: " << e.get_code() << endl;
        cout << "条件表达式: " << e.get_message() << endl;
        cout << "位置: " << e.get_source_file() << ":" << e.get_source_line() << endl;
    }
}

// ============================================================================
// 2. SourceLocation 使用示例
// ============================================================================

void example_source_location()
{
    cout << "\n=== SourceLocation 使用示例 ===" << endl;
    
    // 方式1: 使用宏自动获取当前位置
    SourceLocation loc = NUT_SOURCE_LOCATION;
    cout << "当前位置: " << loc.to_string() << endl;
    cout << "文件名: " << loc.get_file_name() << endl;
    cout << "行号: " << loc.get_line_number() << endl;
    cout << "函数: " << (loc.get_function_name() ? loc.get_function_name() : "未指定") << endl;
    
    // 方式2: 手动构造
    SourceLocation loc2("example.cpp", 42, "example_function");
    cout << "\n手动构造的位置: " << loc2.to_string() << endl;
    
    // 方式3: 比较
    SourceLocation loc3("file.cpp", 10, "func");
    SourceLocation loc4("file.cpp", 10, "func");
    cout << "\n位置比较: " << (loc3 == loc4 ? "相同" : "不同") << endl;
}

void log_with_location(const string& message)
{
    SourceLocation loc = NUT_SOURCE_LOCATION;
    cout << "[" << loc.get_file_name() 
         << ":" << loc.get_line_number() 
         << "] " << message << endl;
}

// ============================================================================
// 3. Backtrace 使用示例
// ============================================================================

void function_level_3()
{
    cout << "\n=== Backtrace 使用示例 ===" << endl;
    cout << "当前位置: function_level_3" << endl;
    
    // 方式1: 直接打印调用栈
    cout << "\n调用栈（直接打印）:" << endl;
    Backtrace::print_stack();
    
    // 方式2: 获取调用栈字符串
    cout << "\n调用栈（字符串形式）:" << endl;
    string stack = Backtrace::backtrace();
    cout << stack << endl;
    
    // 方式3: 跳过顶层几帧
    cout << "\n调用栈（跳过2层）:" << endl;
    string stack2 = Backtrace::backtrace(2);
    cout << stack2 << endl;
}

void function_level_2()
{
    function_level_3();
}

void function_level_1()
{
    function_level_2();
}

// ============================================================================
// 4. DestroyChecker 使用示例
// ============================================================================

class SafeResource
{
    NUT_DEBUGGING_DESTROY_CHECKER;  // 添加生命周期检查器
    
private:
    string resource_name_;
    
public:
    SafeResource(const string& name) : resource_name_(name)
    {
        cout << "创建资源: " << resource_name_ << endl;
    }
    
    void use()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;  // 检查资源是否仍然有效
        cout << "使用资源: " << resource_name_ << endl;
    }
    
    ~SafeResource()
    {
        cout << "销毁资源: " << resource_name_ << endl;
    }
};

void example_destroy_checker()
{
    cout << "\n=== DestroyChecker 使用示例 ===" << endl;
    
    {
        SafeResource resource("test_resource");
        resource.use();  // 正常使用，对象有效
        
        // 资源会在作用域结束时自动销毁
    }
    
    // resource.use();  // 如果取消注释，在 Debug 模式下会触发 assert
}

// ============================================================================
// 5. ProcAddrMaps 使用示例（仅Linux）
// ============================================================================

#if NUT_PLATFORM_OS_LINUX
void example_proc_addr_maps()
{
    cout << "\n=== ProcAddrMaps 使用示例（Linux）===" << endl;
    
    ProcAddrMaps& maps = ProcAddrMaps::instance();
    
    if (!maps.is_valid())
    {
        cout << "ProcAddrMaps 实例无效" << endl;
        return;
    }
    
    // 获取可执行文件路径
    const string& exec_path = maps.get_exec_path();
    cout << "可执行文件路径: " << exec_path << endl;
    
    // 加载所有模块映射
    maps.load();
    
    // 尝试查找一些常见的库
    const char* libs[] = {
        "libc.so",
        "libstdc++.so",
        "libpthread.so"
    };
    
    for (size_t i = 0; i < sizeof(libs) / sizeof(libs[0]); ++i)
    {
        ProcAddrMaps::addr_type addr = 0;
        if (maps.find(libs[i], &addr))
        {
            cout << libs[i] << " 加载地址: 0x" << hex << addr << dec << endl;
        }
    }
}
#endif

// ============================================================================
// 6. 综合示例：完整的错误处理
// ============================================================================

class DataProcessor
{
public:
    void process_file(const string& filename)
    {
        SourceLocation loc = NUT_SOURCE_LOCATION;
        cout << "\n=== 综合示例：处理文件 ===" << endl;
        cout << "调用位置: " << loc.to_string() << endl;
        
        // 使用 THROW_ON_FALSE 进行参数验证
        THROW_ON_FALSE(!filename.empty(), 1001);
        
        ifstream file(filename);
        if (!file.is_open())
        {
            throw Exception(
                1002,
                "无法打开文件: " + filename,
                __FILE__, __LINE__, __FUNCTION__
            );
        }
        
        // 模拟处理
        cout << "文件处理成功: " << filename << endl;
    }
    
    void process_with_error_handling(const string& filename)
    {
        try
        {
            process_file(filename);
        }
        catch (const Exception& e)
        {
            // 记录详细的错误信息
            cout << "\n错误报告:" << endl;
            cout << "  错误码: " << e.get_code() << endl;
            cout << "  错误消息: " << e.get_message() << endl;
            cout << "  位置: " << e.get_source_file() 
                 << ":" << e.get_source_line() << endl;
            cout << "  函数: " << e.get_source_func() << endl;
            
            // 打印调用栈
            cout << "\n调用栈:" << endl;
            Backtrace::print_stack();
        }
    }
};

// ============================================================================
// 主函数
// ============================================================================

int main()
{
    cout << "NUT Debugging 模块使用示例" << endl;
    cout << "============================" << endl;
    
    // 1. Exception 示例
    example_exception_basic();
    example_exception_detailed();
    example_throw_on_false();
    
    // 2. SourceLocation 示例
    example_source_location();
    log_with_location("这是一条带位置信息的日志");
    
    // 3. Backtrace 示例
    function_level_1();
    
    // 4. DestroyChecker 示例
    example_destroy_checker();
    
#if NUT_PLATFORM_OS_LINUX
    // 5. ProcAddrMaps 示例（仅Linux）
    example_proc_addr_maps();
#endif
    
    // 6. 综合示例
    DataProcessor processor;
    processor.process_with_error_handling("test.txt");
    processor.process_with_error_handling("");  // 会触发异常
    
    return 0;
}


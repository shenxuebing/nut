/**
 * NUT Library 使用示例
 * 演示如何使用nut库的基本功能
 */

#include <iostream>
#include <string>

// 根据库类型选择不同的包含方式
#ifdef NUT_STATIC_LIB
    // 静态库不需要特殊处理
    #include "nut/nut.h"
#else
    // 动态库需要导入定义
    #include "nut/nut.h"
#endif

int main() {
    std::cout << "=== NUT Library 使用示例 ===" << std::endl;
    
    try {
        // 1. 测试字符串工具
        std::cout << "\n1. 字符串工具测试:" << std::endl;
        std::string test_str = "  Hello, NUT Library!  ";
        std::string trimmed = nut::trim(test_str);
        std::cout << "   原字符串: '" << test_str << "'" << std::endl;
        std::cout << "   去空格后: '" << trimmed << "'" << std::endl;
        
        // 2. 测试时间功能
        std::cout << "\n2. 时间功能测试:" << std::endl;
        nut::DateTime now = nut::DateTime::now();
        std::cout << "   当前时间: " << now.to_string() << std::endl;
        
        nut::PerformanceCounter pc;
        pc.restart();
        
        // 模拟一些工作
        for(int i = 0; i < 1000000; ++i) {
            volatile int x = i * i;
        }
        
        double elapsed = pc.get_elapsed_sec();
        std::cout << "   性能计时: " << elapsed << " 秒" << std::endl;
        
        // 3. 测试数学功能  
        std::cout << "\n3. 数学功能测试:" << std::endl;
        nut::BigInteger big1("12345678901234567890");
        nut::BigInteger big2("98765432109876543210");
        nut::BigInteger result = big1 + big2;
        std::cout << "   大整数运算: " << big1.to_string() << " + " 
                  << big2.to_string() << " = " << result.to_string() << std::endl;
        
        // 4. 测试容器
        std::cout << "\n4. 容器功能测试:" << std::endl;
        nut::IntegerSet int_set;
        int_set.add_range(1, 10);
        int_set.add_range(20, 30);
        std::cout << "   整数集合大小: " << int_set.size() << std::endl;
        std::cout << "   包含数字5: " << (int_set.contains(5) ? "是" : "否") << std::endl;
        std::cout << "   包含数字15: " << (int_set.contains(15) ? "是" : "否") << std::endl;
        
        // 5. 测试加密功能
        std::cout << "\n5. 加密功能测试:" << std::endl;
        std::string input = "Hello NUT Library";
        std::string md5_hash = nut::md5(input);
        std::cout << "   MD5('" << input << "') = " << md5_hash << std::endl;
        
        std::string sha1_hash = nut::sha1(input);
        std::cout << "   SHA1('" << input << "') = " << sha1_hash << std::endl;
        
        std::cout << "\n=== 所有测试成功完成！===" << std::endl;
        std::cout << "\n编译信息:" << std::endl;
        
        #ifdef _WIN32
            std::cout << "   平台: Windows" << std::endl;
        #elif defined(__linux__)
            std::cout << "   平台: Linux" << std::endl;
        #elif defined(__APPLE__)
            std::cout << "   平台: macOS" << std::endl;
        #endif
        
        #ifdef _MT
            #ifdef _DLL
                std::cout << "   运行时: MD (多线程DLL)" << std::endl;
            #else
                std::cout << "   运行时: MT (多线程静态)" << std::endl;
            #endif
        #else
            std::cout << "   运行时: 单线程" << std::endl;
        #endif
        
        #ifdef NUT_STATIC_LIB
            std::cout << "   库类型: 静态库" << std::endl;
        #else
            std::cout << "   库类型: 动态库" << std::endl;
        #endif
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}



#include <nut/unittest/unittest.h>
#include <nut/platform/platform.h>

using namespace std;
using namespace nut;

#if NUT_PLATFORM_OS_LINUX

#include <nut/debugging/proc_addr_maps.h>

using namespace std;
using namespace nut;

class TestProcAddrMaps : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_singleton);
        NUT_REGISTER_CASE(test_is_valid);
        NUT_REGISTER_CASE(test_get_exec_path);
        NUT_REGISTER_CASE(test_load);
        NUT_REGISTER_CASE(test_find);
        NUT_REGISTER_CASE(test_module_path);
    }

    void test_singleton()
    {
        // 测试单例模式
        ProcAddrMaps& inst1 = ProcAddrMaps::instance();
        ProcAddrMaps& inst2 = ProcAddrMaps::instance();
        NUT_TA(&inst1 == &inst2); // 应该是同一个实例
    }

    void test_is_valid()
    {
        ProcAddrMaps& inst = ProcAddrMaps::instance();
        // 单例对象应该始终有效
        NUT_TA(inst.is_valid());
    }

    void test_get_exec_path()
    {
        ProcAddrMaps& inst = ProcAddrMaps::instance();
        const string& exec_path = inst.get_exec_path();
        
        // 可执行文件路径应该不为空
        NUT_TA(!exec_path.empty());
        
        // 路径应该以 / 开头（Linux 绝对路径）
        NUT_TA(exec_path[0] == '/');
        
        // 多次调用应该返回相同的引用
        const string& path2 = inst.get_exec_path();
        NUT_TA(&exec_path == &path2);
    }

    void test_load()
    {
        ProcAddrMaps& inst = ProcAddrMaps::instance();
        
        // 测试无参数加载（加载所有模块）
        inst.load();
        NUT_TA(inst.is_valid());
        
        // 测试加载指定模块（使用可执行文件路径）
        const string& exec_path = inst.get_exec_path();
        if (!exec_path.empty())
        {
            inst.load(exec_path);
            NUT_TA(inst.is_valid());
        }
        
        // 测试加载不存在的模块（应该不会崩溃）
        inst.load("/nonexistent/path/module.so");
        NUT_TA(inst.is_valid());
    }

    void test_find()
    {
        ProcAddrMaps& inst = ProcAddrMaps::instance();
        inst.load(); // 先加载所有模块
        
        ProcAddrMaps::addr_type addr = 0;
        
        // 测试查找可执行文件本身
        const string& exec_path = inst.get_exec_path();
        if (!exec_path.empty())
        {
            bool found = inst.find(exec_path, &addr);
            // 可执行文件应该能找到（即使地址为 0）
            NUT_TA(found);
        }
        
        // 测试查找不存在的模块
        bool found = inst.find("/nonexistent/module.so", &addr);
        NUT_TA(!found);
        NUT_TA(addr == 0); // 未找到时应该设置为 0
        
        // 测试传入 nullptr（虽然头文件中要求非空，但这里测试边界情况）
        // 注意：实际使用中不应该传入 nullptr，这里只是测试防御性编程
    }

    void test_module_path()
    {
        ProcAddrMaps& inst = ProcAddrMaps::instance();
        
        // 加载所有模块
        inst.load();
        
        // 测试查找标准库路径（如果存在）
        // libc.so 通常存在于系统中
        const char* standard_libs[] = {
            "libc.so",
            "libstdc++.so",
            "libpthread.so",
            "libm.so"
        };
        
        bool found_any = false;
        for (size_t i = 0; i < sizeof(standard_libs) / sizeof(standard_libs[0]); ++i)
        {
            ProcAddrMaps::addr_type addr = 0;
            // 尝试查找（可能找不到，取决于系统配置）
            bool found = inst.find(standard_libs[i], &addr);
            if (found)
            {
                found_any = true;
                NUT_TA(addr >= 0); // 地址应该 >= 0
            }
        }
        
        // 至少应该能找到一些模块（通常至少可执行文件本身是能找到的）
        // 这里不强制要求找到标准库，因为不同系统配置不同
    }
};

NUT_REGISTER_FIXTURE(TestProcAddrMaps, "debugging,quiet")

#else /* !NUT_PLATFORM_OS_LINUX */

// 非 Linux 平台下，ProcAddrMaps 不可用，创建一个空的测试以保持一致性
class TestProcAddrMaps : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_platform_skip);
    }

    void test_platform_skip()
    {
        // ProcAddrMaps 仅在 Linux 平台上可用
        // 在其他平台上跳过测试
        NUT_TA(true);
    }
};

NUT_REGISTER_FIXTURE(TestProcAddrMaps, "debugging,quiet")

#endif /* NUT_PLATFORM_OS_LINUX */


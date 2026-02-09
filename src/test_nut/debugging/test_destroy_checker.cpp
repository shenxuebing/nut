
#include <nut/unittest/unittest.h>
#include <nut/debugging/destroy_checker.h>
#include <cassert>

using namespace std;
using namespace nut;

class TestDestroyChecker : public TestFixture
{
    virtual void register_cases() noexcept override
    {
        NUT_REGISTER_CASE(test_basic);
        NUT_REGISTER_CASE(test_assert_alive);
        NUT_REGISTER_CASE(test_macro);
        NUT_REGISTER_CASE(test_destruction);
    }

    void test_basic()
    {
        DestroyChecker checker;
        // 对象构造后应该可以正常调用 assert_alive
        checker.assert_alive();

        // 对象在作用域内应该一直有效
        {
            DestroyChecker checker2;
            checker2.assert_alive();
        }
        // checker2 已析构，但 checker 仍然有效
        checker.assert_alive();
    }

    void test_assert_alive()
    {
        DestroyChecker checker;
        
        // 在对象生命周期内，assert_alive 不应该抛出异常
        // 注意：在 NDEBUG 模式下，assert 可能被禁用，但方法仍然可以被调用
        #ifndef NDEBUG
        // 在调试模式下，assert_alive 应该正常工作
        checker.assert_alive();
        #endif
    }

    void test_macro()
    {
        // 测试 NUT_DEBUGGING_DESTROY_CHECKER 宏
        #ifndef NDEBUG
        NUT_DEBUGGING_DESTROY_CHECKER;
        NUT_DEBUGGING_ASSERT_ALIVE;
        #endif

        // 宏应该可以编译通过（在 NDEBUG 模式下可能是空的）
        #ifdef NDEBUG
        // 在发布模式下，宏应该是空的，所以不会创建对象
        NUT_DEBUGGING_DESTROY_CHECKER;
        NUT_DEBUGGING_ASSERT_ALIVE;
        #endif
    }

    void test_destruction()
    {
        // 测试对象析构不会崩溃
        // 注意：DestroyChecker 的析构函数会在调试模式下检查对象是否被正确构造
        {
            DestroyChecker checker;
            checker.assert_alive();
        }
        // checker 已析构，应该不会有问题

        // 测试多个对象
        DestroyChecker c1;
        {
            DestroyChecker c2;
            c1.assert_alive();
            c2.assert_alive();
        }
        c1.assert_alive();

        // 测试在函数中使用
        helper_function();
    }

private:
    void helper_function()
    {
        DestroyChecker local_checker;
        local_checker.assert_alive();
    }
};

NUT_REGISTER_FIXTURE(TestDestroyChecker, "debugging,quiet")


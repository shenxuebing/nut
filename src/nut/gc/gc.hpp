﻿/**
 * @file -
 * @author jingqi
 * @date 2011-11-11 18:49
 */

#ifndef ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_
#define ___HEADFILE___B77B6DC2_6C30_4732_943B_117AF7E35B83_

#include <stdint.h>
#include <stdlib.h>
#include <new>

#include <nut/memtool/refarg.hpp>
#include <nut/debugging/destroy_checker.hpp>
#include <nut/threading/lockfree/atomic.hpp>
#include <nut/mem/memory_allocator.hpp>

#include "enref.hpp"
#include "ref.hpp"

namespace nut
{

/**
 * 可引用计数对象包装器
 *
 * @note
 * 使用包装器来包装的好处：
 * 1. 析构函数的部分见gc_new的note
 * 2. 如果对象是多继承的，计数器仍然只有一份，仍然能正常运行
 */
template <typename T>
class GCWrapper : public T
{
protected:
    // 引用计数
    int volatile m_ref_count;

    // 内存分配器
    const ref<memory_allocator> m_alloc;

    // 避免多次销毁的检查器
    NUT_DEBUGGING_DESTROY_CHECKER

public:
    GCWrapper(memory_allocator *ma)
        : T(), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1>
    GCWrapper(memory_allocator *ma, Arg1 arg1)
        : T(RefargTraits<Arg1>::value(arg1)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3)),
        m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
        RefargTraits<Arg4>::value(arg4)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
        RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
        RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6)),
        m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
        RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
        RefargTraits<Arg7>::value(arg7)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7, Arg8 arg8)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
        RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
        RefargTraits<Arg7>::value(arg7), RefargTraits<Arg8>::value(arg8)), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
              Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
        RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
        RefargTraits<Arg7>::value(arg7), RefargTraits<Arg8>::value(arg8), RefargTraits<Arg9>::value(arg9)),
        m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
        GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
                  Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9, Arg10 arg10)
        : T(RefargTraits<Arg1>::value(arg1), RefargTraits<Arg2>::value(arg2), RefargTraits<Arg3>::value(arg3),
        RefargTraits<Arg4>::value(arg4), RefargTraits<Arg5>::value(arg5), RefargTraits<Arg6>::value(arg6),
        RefargTraits<Arg7>::value(arg7), RefargTraits<Arg8>::value(arg8), RefargTraits<Arg9>::value(arg9),
        RefargTraits<Arg10>::value(arg10)), m_ref_count(0), m_alloc(ma)
    {}

    virtual int add_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return atomic_add(&m_ref_count, 1) + 1;
    }

    virtual int rls_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = atomic_add(&m_ref_count, -1) - 1;
        if (0 == ret)
        {
            ref<memory_allocator> alloc = m_alloc;
            this->~GCWrapper();
            if (alloc.is_not_null())
                alloc->free(this);
            else
                ::free(this);
        }
        return ret;
    }

    virtual int get_ref() const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return m_ref_count;
    }
};

template <typename T>
class GCWrapper<enref<T> > : public enref<T>
{
protected:
    // 引用计数
    int volatile m_ref_count;

    // 内存分配器
    memory_allocator *m_alloc;

    //* 检查多次销毁的检查器
    NUT_DEBUGGING_DESTROY_CHECKER

public:
    GCWrapper(memory_allocator *ma)
        : enref<T>(), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1>
    GCWrapper(memory_allocator *ma, Arg1 arg1)
        : enref<T>(arg1), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2)
        : enref<T>(arg1, arg2), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : enref<T>(arg1, arg2, arg3), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        : enref<T>(arg1, arg2, arg3, arg4), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        : enref<T>(arg1, arg2, arg3, arg4, arg5), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6>
        GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
                  Arg6 arg6)
        : enref<T>(arg1, arg2, arg3, arg4, arg5, arg6), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7>
        GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
                  Arg6 arg6, Arg7 arg7)
        : enref<T>(arg1, arg2, arg3, arg4, arg5, arg6, arg7), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
              typename Arg6, typename Arg7, typename Arg8>
        GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
                  Arg6 arg6, Arg7 arg7, Arg8 arg8)
        : enref<T>(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
        typename Arg6, typename Arg7, typename Arg8, typename Arg9>
        GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
        Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9)
        : enref<T>(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9), m_ref_count(0), m_alloc(ma)
    {}

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5,
        typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
        GCWrapper(memory_allocator *ma, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5,
        Arg6 arg6, Arg7 arg7, Arg8 arg8, Arg9 arg9, Arg10 arg10)
        : enref<T>(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10), m_ref_count(0), m_alloc(ma)
    {}

    virtual int add_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return atomic_add(&m_ref_count, 1) + 1;
    }

    virtual int rls_ref()
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        const int ret = atomic_add(&m_ref_count, -1) - 1;
        if (0 == ret)
        {
            ref<memory_allocator> alloc = m_alloc;
            this->~GCWrapper();
            if (alloc.is_not_null())
                alloc->free(this);
            else
                ::free(this);
        }
        return ret;
    }

    virtual int get_ref() const
    {
        NUT_DEBUGGING_ASSERT_ALIVE;
        return m_ref_count;
    }
};

}

// 即使用户类的构造函数是private的，也能用 gc_new
#define NUT_GC_PRIVATE_GCNEW template <typename ___T> friend class nut::GCWrapper;

#define GC_NEW(ma, type, ...) (type*) (new ((nut::GCWrapper<type >*) nut::ma_alloc((ma), sizeof(nut::GCWrapper<type >))) nut::GCWrapper<type >((ma), ##__VA_ARGS__))

#endif /* head file guarder */

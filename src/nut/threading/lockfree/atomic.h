﻿/**
 *  基本上大部分的无锁并发数据结构都是依靠处理器提供的 CAS(compare and swap)操作来
 * 实现的; 包括原子加、原子减、自旋锁等也是依靠这个来实现的。
 */

#ifndef ___HEADFILE_CBAE01C9_CF0C_4836_A4DC_E7B0934DEA6E_
#define ___HEADFILE_CBAE01C9_CF0C_4836_A4DC_E7B0934DEA6E_

#include <nut/nut_config.h>
#include <nut/platform/platform.h>
#include <nut/platform/stdint_traits.h>

namespace nut
{

/**
 * 指针CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(void * volatile *dest, void *oldval, void *newval);


#if NUT_PLATFORM_BITS_64

/**
 * 128位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(int128_t volatile *dest, int128_t oldval, int128_t newval);


/**
 * 128位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(uint128_t volatile *dest, uint128_t oldval, uint128_t newval);

#endif

/**
 * 64位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(int64_t volatile *dest, int64_t oldval, int64_t newval);

/**
 * 64位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(uint64_t volatile *dest, uint64_t oldval, uint64_t newval);

/**
 * 32位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(int32_t volatile *dest, int32_t oldval, int32_t newval);

/**
 * 32位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(uint32_t volatile *dest, uint32_t oldval, uint32_t newval);

/**
 * 16位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(int16_t volatile *dest, int16_t oldval, int16_t newval);

/**
 * 16位CAS操作
 *
 * @return 操作成功则返回true
 */
NUT_API bool atomic_cas(uint16_t volatile *dest, uint16_t oldval, uint16_t newval);


#if NUT_PLATFORM_BITS_64

/**
 * 128位原子加
 *
 * @return 返回旧值
 */
NUT_API int128_t atomic_add(int128_t volatile *addend, int128_t value);

/**
 * 128位原子加
 *
 * @return 返回旧值
 */
NUT_API uint128_t atomic_add(uint128_t volatile *addend, uint128_t value);

#endif

/**
 * 64位原子加
 *
 * @return 返回旧值
 */
NUT_API int64_t atomic_add(int64_t volatile *addend, int64_t value);

/**
 * 64位原子加
 *
 * @return 返回旧值
 */
NUT_API uint64_t atomic_add(uint64_t volatile *addend, uint64_t value);

/**
 * 32位原子加
 *
 * @return 返回旧值
 */
NUT_API int32_t atomic_add(int32_t volatile *addend, int32_t value);

/**
 * 32位原子加
 *
 * @return 返回旧值
 */
NUT_API uint32_t atomic_add(uint32_t volatile *addend, uint32_t value);

/**
 * 16位原子加
 *
 * @return 返回旧值
 */
NUT_API int16_t atomic_add(int16_t volatile *addend, int16_t value);

/**
 * 16位原子加
 *
 * @return 返回旧值
 */
NUT_API uint16_t atomic_add(uint16_t volatile *addend, uint16_t value);

/**
 * 为了避免 ABA 问题而引入的带标签的指针
 *
 * 解决 ABA 问题有几个方案：
 * 1. 由于指针地址一般是对齐的，用指针末尾的几个 bit 做一个时间戳 tag
 *    缺点是 tag 位数少，可能 wrap 后依然出现 ABA 问题; 如果指针不对齐(不推荐，但是有可能出现)，则
 *    不适用
 * 2. 在指针后附加一个大字段来表示时间戳，也就是下面的方案。
 *    缺点是，操作位数变长，性能有所损失
 */
template <typename T>
union TagedPtr
{
    /** 整体对应的CAS操作数类型 */
#if NUT_PLATFORM_BITS_64
    typedef uint64_t tag_type;
    typedef uint128_t cas_type;
#elif NUT_PLATFORM_BITS_32
    typedef uint32_t tag_type;
    typedef uint64_t cas_type;
#elif NUT_PLATFORM_BITS_16
    typedef uint16_t tag_type;
    typedef uint32_t cas_type;
#else
#   error platform not supported!
#endif

    struct
    {
        T *ptr; // union 内部无法重复赋初值(字段 cas 已经初始化)
        tag_type tag;
    };

    // FIXME 在 Windows 系统上的 MinGW 编译器有个bug: 会忽略 union 内的 "就地初始化"
    //       (c++11新特性，直接在成员后加 "=" 号来初始化)。
    //       虽然在编译时没有报错，但是会导致该成员在运行时处于未初始化状态
    cas_type cas;

    TagedPtr()
        : cas(0) // NOTE 使用构造函数的初始化列表来避免上述"就地初始化"问题
    {}

    TagedPtr(T *p, tag_type t)
        : ptr(p), tag(t)
    {}

    TagedPtr(cas_type c)
        : cas(c)
    {}
};

static_assert(sizeof(void*) == sizeof(TagedPtr<void>::tag_type), "TagedPtr 数据结构对齐问题");
static_assert(sizeof(void*) * 2 == sizeof(TagedPtr<void>::cas_type), "TagedPtr 数据结构对齐问题");
static_assert(sizeof(TagedPtr<void>) == sizeof(TagedPtr<void>::cas_type), "TagedPtr 数据结构对齐问题");

}

#endif /* head file guarder */

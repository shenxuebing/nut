﻿
#ifndef ___HEADFILE_B56CC2C0_68D4_44FD_AD11_BBA330875876_
#define ___HEADFILE_B56CC2C0_68D4_44FD_AD11_BBA330875876_

#include <time.h>
#include <string>
#include <chrono>

#include "../platform/platform.h"

#if !NUT_PLATFORM_CC_VC
#   include <sys/time.h>
#endif

#include "../nut_config.h"


#define __NSECS_PER_SEC  1000000000L

namespace nut
{

class NUT_API TimeDiff
{
public:
    constexpr TimeDiff() noexcept
        : _seconds(0), _nanoseconds(0)
    {}

    constexpr explicit TimeDiff(double s) noexcept
        : _seconds(s), _nanoseconds((s - (time_t) s) * __NSECS_PER_SEC)
    {}

    /**
     * @param s 秒
     * @param ns 纳秒
     */
    explicit TimeDiff(time_t s, long ns) noexcept;

    constexpr bool operator==(const TimeDiff& x) const noexcept
    {
        return _seconds == x._seconds && _nanoseconds == x._nanoseconds;
    }

    constexpr bool operator!=(const TimeDiff& x) const noexcept
    {
        return !(*this == x);
    }

    constexpr bool operator<(const TimeDiff& x) const noexcept
    {
        return ((_seconds < x._seconds) ||
                (_seconds == x._seconds && _nanoseconds < x._nanoseconds));
    }

    constexpr bool operator>(const TimeDiff& x) const noexcept
    {
        return x < *this;
    }

    constexpr bool operator<=(const TimeDiff& x) const noexcept
    {
        return !(x < *this);
    }

    constexpr bool operator>=(const TimeDiff& x) const noexcept
    {
        return !(*this < x);
    }

    TimeDiff operator+(const TimeDiff& x) const noexcept;
    TimeDiff operator+(double seconds) const noexcept;

    TimeDiff operator-(const TimeDiff& x) const noexcept;
    TimeDiff operator-(double seconds) const noexcept;

    constexpr TimeDiff operator*(double scale) const noexcept
    {
        return TimeDiff(to_double() * scale);
    }

    constexpr TimeDiff operator/(double scale) const noexcept
    {
        return TimeDiff(to_double() / scale);
    }

    TimeDiff operator-() const noexcept;

    TimeDiff& operator+=(const TimeDiff& x) noexcept;
    TimeDiff& operator-=(const TimeDiff& x) noexcept;
    TimeDiff& operator*=(double scale) noexcept;
    TimeDiff& operator/=(double scale) noexcept;

    void set(double s) noexcept;
    void set(time_t s, long ns) noexcept;

#if !NUT_PLATFORM_CC_VC
    void set(const struct timeval& tv) noexcept;
    void set(const struct timespec& tv) noexcept;
    void to_timeval(struct timeval *tv) noexcept;
    void to_timespec(struct timespec *tv) noexcept;
#endif

    constexpr time_t get_seconds() const noexcept
    {
        return _seconds;
    }

    constexpr long get_nanoseconds() const noexcept
    {
        return _nanoseconds;
    }

    constexpr double to_double() const noexcept
    {
        return ((double) _seconds) + _nanoseconds / (double) __NSECS_PER_SEC;
    }

    // example: "12.2345"
    std::string to_string() const noexcept;

protected:
    /**
     * 规范化
     */
    void normalize() noexcept;

public:
    static const TimeDiff ZERO;

protected:
    time_t _seconds;
    long _nanoseconds;
};



class NUT_API TimeDiffEx
{
public:
    TimeDiffEx() : m_begin(std::chrono::high_resolution_clock::now()) {}


    void reset()
    {
        m_begin = std::chrono::high_resolution_clock::now();
    }


    // 默认输出毫秒.
    template<typename Duration = std::chrono::milliseconds>
    long long elapsed() const
    {
        return std::chrono::duration_cast<Duration>(std::chrono::high_resolution_clock::now() - m_begin).count();
    }


    // 微秒.
    long long elapsedMicroseconds() const
    {
        return elapsed<std::chrono::microseconds>();
    }


    // 纳秒.
    long long elapsedNanoSeconds() const
    {
        return elapsed<std::chrono::nanoseconds>();
    }


    // 秒.
    long long elapsedSeconds() const
    {
        return elapsed<std::chrono::seconds>();
    }


    // 分.
    long long elapsedMinutes() const
    {
        return elapsed<std::chrono::minutes>();
    }


    // 时.
    long long elapsedHours() const
    {
        return elapsed<std::chrono::hours>();
    }


private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
};


}

#undef __NSECS_PER_SEC

#endif

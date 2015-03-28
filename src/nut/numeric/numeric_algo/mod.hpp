﻿/**
 * @file -
 * @author jingqi
 * @date 2012-11-25
 * @last-edit 2015-01-06 22:43:39 jingqi
 */

#ifndef ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_
#define ___HEADFILE_058D89EB_50A2_4934_AF92_FC4F82613999_

#include "../big_integer.hpp"
#include "gcd.hpp"

// 优化程度，>= 0
#define OPTIMIZE_LEVEL 1000

namespace nut
{

/**
 * 模乘之前的预计算表计算
 */
template <size_t C>
struct ModMultiplyPreBuildTable
{
    const rc_ptr<memory_allocator> alloc;

#if (OPTIMIZE_LEVEL == 0)
    size_t hight, width;
    BigInteger *table;

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n, memory_allocator *ma = NULL)
        : alloc(ma), hight(0), width(0), table(NULL)
    {
        assert(a.is_positive() && n.is_positive() && a < n); // 一定要保证 a<n ，以便进行模加运算

        hight = (32 * n.significant_words_length() + C - 1) / C;
        width = (1 << C) - 1;

        const size_t count = hight * width;
        table = (BigInteger*) ma_alloc(alloc.pointer(), sizeof(BigInteger) * count);
        assert(NULL != table);

        // 填充第一行
        new (table + 0) BigInteger(a);
        for (size_t i = 1; i < width; ++i)
        {
            new (table + i) BigInteger(table[0] + table[i - 1]);
            if (table[i] >= n)
                table[i] -= n;
        }
        // 填充其他行
        for (size_t i = 1; i < hight; ++i)
        {
            const size_t base_off = i * width;
            new (table + base_off) BigInteger(table[base_off - width] + table[base_off - 1]);
            if (table[base_off] >= n)
                table[base_off] -= n;

            for (size_t j = 1; j < width; ++j)
            {
                new (table + base_off + j) BigInteger(table[base_off] + table[base_off + j - 1]);
                if (table[base_off + j] >= n)
                    table[base_off + j] -= n;
            }
        }
    }

    ~ModMultiplyPreBuildTable()
    {
        if (NULL != table)
        {
            for (size_t i = 0, count = hight * width; i < count; ++i)
                (table + i)->~BigInteger();
            ma_free(alloc.pointer(), table);
            table = NULL;
        }
    }

    const BigInteger& at(size_t i, size_t j) const
    {
        assert(i < hight && j < width);
        return table[i * width + j];
    }
#else
    size_t hight, width;
    BigInteger **table;
    BigInteger mod;

    ModMultiplyPreBuildTable(const BigInteger& a, const BigInteger& n)
        : alloc(a.allocator()), hight(0), width(0), table(NULL)
    {
        assert(a.is_positive() && n.is_positive() && a < n); // 一定要保证 a<n ，以便进行模加运算

        hight = (n.bit_length() + C - 1) / C;
        width = (1 << C) - 1;

        const size_t count = hight * width;
        table = (BigInteger**) ma_alloc(alloc.pointer(), sizeof(BigInteger*) * count);
        assert(NULL != table);
        ::memset(table, 0, sizeof(BigInteger*) * count);

        table[0] = MA_NEW(alloc.pointer(), BigInteger, a);
        mod = n;
    }

    ~ModMultiplyPreBuildTable()
    {
        if (NULL != table)
        {
            for (size_t i = 0; i < hight; ++i)
            {
                for (size_t j = 0; j < width; ++j)
                {
                    if (NULL != table[i * width + j])
                    {
                        MA_DELETE(alloc.pointer(), table[i * width + j], BigInteger);
                        table[i * width + j] = NULL;
                    }
                }
            }
            ma_free(alloc.pointer(), table);
            table = NULL;
        }
    }

    const BigInteger& at(size_t i, size_t j) const
    {
        assert(i < hight && j < width);

        const size_t base_off = i * width;
        if (NULL == table[base_off + j])
        {
            if (j == 0)
            {
                table[base_off] = (BigInteger*) ma_alloc(alloc.pointer(), sizeof(BigInteger));
                new (table[base_off]) BigInteger(at(i - 1, 0) + at(i - 1, width - 1));
            }
            else
            {
                table[base_off + j] = (BigInteger*) ma_alloc(alloc.pointer(), sizeof(BigInteger));
                new (table[base_off + j]) BigInteger(at(i, 0) + at(i, j - 1));
            }

            if (*table[base_off + j] >= mod) // 模加
                *table[base_off + j] -= mod;
        }
        return *table[base_off + j];
    }
#endif

private:
    ModMultiplyPreBuildTable(const ModMultiplyPreBuildTable<C>&);
};

/**
* 预算表法求乘模
* 参见 《公开密钥密码算法及其快速实现》.周玉洁.冯国登.国防工业出版社.2002 P57
 */
template <size_t C>
void mod_multiply(const BigInteger& b, const BigInteger& n, const ModMultiplyPreBuildTable<C>& table, BigInteger *rs)
{
    assert(NULL != rs);
    assert(b.is_positive() && n.is_positive() && b < n); // 一定要保证 b<n ,以便优化模加运算

    BigInteger s(0, b.allocator());
    size_t limit = (b.bit_length() + C - 1) / C;
    if (table.hight < limit)
        limit = table.hight;
    for (size_t i = 0; i < limit; ++i)
    {
        uint32_t j = 0; // bits window
        for (size_t k = 1; k <= C; ++k)
        {
            j <<= 1;
            j |= b.bit_at(i * C + (C - k));
        }
        if (j > 0)
        {
            s += table.at(i, j - 1);
            if (s >= n)
                s -= n;
        }
    }

    *rs = s;
}

/**
 * 蒙哥马利算法
 * {t + [(t mod r) * n' mod r] * n} / r
 */
inline void _montgomery(const BigInteger& t, size_t rlen, const BigInteger& n, const BigInteger& nn, BigInteger *rs)
{
    assert(NULL != rs);
    assert(t.is_positive() && rlen > 0 && n.is_positive() && nn.is_positive());
    typedef BigInteger::word_type word_type;

    // 计算 t % r
    size_t min_sig = (rlen + 8 * sizeof(word_type) - 1) / (8 * sizeof(word_type));
    if (t.significant_words_length() < min_sig)
        min_sig = t.significant_words_length();
    BigInteger s(t.data(), min_sig, true, t.allocator());
    s.limit_positive_bits_to(rlen);

    s.multiply_to_len(nn, rlen); // rs = (rs * nn) % r
    s *= n;
    s += t;
    s >>= rlen;

    if (s >= n)
        s -= n;

    *rs = s;
}

/**
 * 变形的蒙哥马利算法
 *
 * 算法来源:
 *      王金荣，周赟，王红霞. Montgomery模平方算法及其应用[J]. 计算机工程，2007，33(24)：155 - 156
 */
inline void _montgomery(const BigInteger& t, const BigInteger& n, BigInteger::word_type nn, BigInteger *_rs)
{
    assert(NULL != _rs);
    assert(t.is_positive() && n.is_positive() && nn > 0);
    typedef BigInteger::word_type word_type;
    typedef BigInteger::dword_type dword_type;

    const size_t r_word_count = n.significant_words_length();
    BigInteger rs(t);
    rs.resize(r_word_count * 2 + 1);
    for (size_t i = 0; i < r_word_count; ++i)
    {
        const word_type op1 = static_cast<word_type>(rs.word_at(i) * nn);
        if (0 == op1)
            continue;

        word_type carry = 0;
        for (size_t j = 0; j < r_word_count; ++j)
        {
            dword_type op2 = n.word_at(j);
            op2 *= op1;
            op2 += rs.word_at(i + j);
            op2 += carry;

            rs.data()[i + j] = static_cast<word_type>(op2);
            carry = static_cast<word_type>(op2 >> (8 * sizeof(word_type)));
        }

        for (size_t j = i; j < r_word_count; ++j)
        {
            if (0 == carry)
                break;

            dword_type op = rs.word_at(j + r_word_count);
            op += carry;

            rs.data()[j + r_word_count] = static_cast<word_type>(op);
            carry = static_cast<word_type>(op >> (8 * sizeof(word_type)));
        }
    }

    rs >>= 8 * sizeof(word_type) * r_word_count;

    while (rs >= n)
        rs -= n;

    *_rs = rs;
}

/**
 * 已知:
 *      n为奇数, r=2**p，(n,r互质)
 * 求 rr, nn 使得:
 *      r * rr - n * nn = 1 (rr为r模n的逆元，nn为n模r的负逆元)
 *
 * 算法来源：
 *      雷明，叶新，张焕国. Montgomery算法及其快速实现[J]. 计算机工程，2003，29(14)：46
 *
 * @return rr, nn 都为正数
 */
inline void _mont_extended_euclid(size_t rlen, const BigInteger& n, BigInteger *_rr, BigInteger *_nn)
{
    assert(NULL != _rr || NULL != _nn);

    BigInteger r(1, n.allocator());
    r <<= rlen;

    /**
     * 构建恒等式并计算：
     * (1) r * rr - n * nn = s
     * (2) r * n - n * r = 0
     * ->等式(1)用于变形，等式(2)用于加到等式(1)上以帮助其变形
     * ->初始时 rr=n+1, nn=r, s=r
     */
    BigInteger rr(n), nn(r);
    ++rr;
    size_t slen = rlen;
    while (slen > 0)
    {
        const int lb = rr.lowest_bit();
        assert(lb >= 0);
        if (lb > 0)
        {
            const int min_shift = (lb < (int) slen ? lb : (int) slen);
            rr >>= min_shift;
            nn >>= min_shift;
            slen -= min_shift;
        }
        else
        {
            rr += n;
            nn += r;
        }
    }

    if (NULL != _rr)
        *_rr = rr;
    if (NULL != _nn)
        *_nn = nn;
}

/**
 * 滑动窗口算法、蒙哥马利算法的预算表
 */
struct MontgomeryPreBuildTable
{
    const rc_ptr<memory_allocator> alloc;
    BigInteger **table;
    size_t size;

    MontgomeryPreBuildTable(size_t wnd_sz, const BigInteger& m, size_t rlen,
            const BigInteger& n, const BigInteger& nn, memory_allocator *ma = NULL)
        : alloc(ma), table(NULL), size(0)
    {
        assert(0 < wnd_sz && wnd_sz < 16);

        size = 1 << (wnd_sz - 1);
        table = (BigInteger**) ma_alloc(alloc.pointer(), sizeof(BigInteger*) * size);
        assert(NULL != table);
        ::memset(table, 0, sizeof(BigInteger*) * size);

        table[0] = (BigInteger*) ma_alloc(alloc.pointer(), sizeof(BigInteger));
        new (table[0]) BigInteger(m);
        BigInteger mm(0, m.allocator());
        _montgomery(m * m, rlen, n, nn, &mm);
        for (size_t i = 1; i < size; ++i)
        {
            table[i] = (BigInteger*) ma_alloc(alloc.pointer(), sizeof(BigInteger));
            new (table[i]) BigInteger(0, m.allocator());
            _montgomery(*table[i - 1] * mm, rlen, n, nn, table[i]);
        }
    }

    ~MontgomeryPreBuildTable()
    {
        if (NULL != table)
        {
            for (size_t i = 0; i < size; ++i)
            {
                if (NULL != table[i])
                {
                    table[i]->~BigInteger();
                    ma_free(alloc.pointer(), table[i]);
                    table[i] = NULL;
                }
            }
            ma_free(alloc.pointer(), table);
            table = NULL;
            size = 0;
        }
    }

    const BigInteger& at(size_t i) const
    {
        assert(i < size);
        return *table[i];
    }

private:
    MontgomeryPreBuildTable(const MontgomeryPreBuildTable&);
};

/**
 * 计算滑动窗口算法的最佳窗口大小
 */
inline size_t _best_wnd(size_t bit_len)
{
    // 参考 java 的 BigInteger.bnExpModThreshTable
    static const size_t TBL[] = {
        7, 25, 81, 241, 673, 1793
    };

    int left = -1, right = sizeof(TBL) / sizeof(TBL[0]);
    while (right - left > 1)
    {
        const int mid = (left + right) / 2;
        if (TBL[mid] == bit_len)
            return mid + 1;
        else if (TBL[mid] < bit_len)
            left = mid;
        else
            right = mid;
    }
    return left + 2;
}

/**
 * 使用 Montgomery 算法优化
 */
inline void _odd_mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n, BigInteger *rs)
{
    assert(NULL != rs);
    assert(a.is_positive() && b.is_positive() && n.is_positive());
    assert(a < n && n.bit_at(0) == 1);

#if (OPTIMIZE_LEVEL == 0)
    // 预运算
    const size_t rlen = n.bit_length();
    BigInteger r(1), nn;
    r <<= rlen;
    extended_euclid(r, n, NULL, NULL, &nn);
    if (nn.is_positive())
        nn = r - (nn % r);
    else
        nn = (-nn) % r;

    // 循环计算
    const BigInteger m = (a << rlen) % n;
    BigInteger ret(m);
    for (size_t i = b.bit_length() - 1; i > 0; --i)
    {
        _montgomery(ret * ret, rlen, n, nn, &ret);
        if (0 != b.bit_at(i - 1))
            _montgomery(ret * m, rlen, n, nn, &ret);
    }

    // 处理返回值
    _montgomery(ret, rlen, n, nn, rs);
    return;
#elif (OPTIMIZE_LEVEL == 1)
    /**
     * 变形的蒙哥马利算法
     *      在这里其效率低于原始的蒙哥马利算法, 可能是非等长的大整数运算效率高于定长大整数运算的缘故
     */

    // 预运算
    const size_t r_word_count = n.significant_words_length();
    BigInteger nn;
    _mont_extended_euclid(8 * sizeof(BigInteger::word_type), BigInteger(n.word_at(0)), NULL, &nn);
    BigInteger::word_type nnn = nn.word_at(0);

    // 循环计算
    const BigInteger m = (a << (8 * sizeof(BigInteger::word_type) * r_word_count)) % n;
    BigInteger ret(m);
    for (int i = ((int) b.bit_length()) - 2; i >= 0; --i)
    {
        ret *= ret;
        _montgomery(ret, n, nnn, &ret);
        if (0 != b.bit_at(i))
        {
            ret *= m;
            _montgomery(ret, n, nnn, &ret);
        }
    }

    // 处理返回值
    _montgomery(ret, n, nnn, rs);
    return;
#elif (OPTIMIZE_LEVEL == 2)
    /**
     * 特殊的扩展欧几里得算法
     */

    // 预运算
    const size_t rlen = n.bit_length();
    BigInteger nn;
    _mont_extended_euclid(rlen, n, NULL, &nn); // 特殊的欧几里得算法
    nn.limit_positive_bits_to(rlen);

    // 循环计算
    const BigInteger m = (a << rlen) % n;
    BigInteger ret(m);
    for (int i = ((int) b.bit_length()) - 2; i >= 0; --i)
    {
        ret *= ret;
        _montgomery(ret, rlen, n, nn, &ret);
        if (0 != b.bit_at(i))
        {
            ret *= m;
            _montgomery(ret, rlen, n, nn, &ret);
        }
    }

    // 处理返回值
    _montgomery(ret, rlen, n, nn, rs);
    return;
#else
    /**
     * 使用滑动窗口算法优化
     */

    // 准备蒙哥马利相关变量
    const size_t rlen = n.bit_length();
    BigInteger nn(0, a.allocator());
    _mont_extended_euclid(rlen, n, NULL, &nn);
    nn.limit_positive_bits_to(rlen);

    // 准备预运算表
    int bits_left = b.bit_length() - 1; // 剩余还未处理的比特数
    assert(bits_left >= 0);
    const size_t wnd_size = _best_wnd(bits_left); // 滑动窗口大小
    const uint32_t WND_MASK = ~(((uint32_t) 1) << wnd_size);
    const BigInteger m = (a << rlen) % n;
    const MontgomeryPreBuildTable tbl(wnd_size, m, rlen, n, nn, n.allocator());

    // 计算过程
    BigInteger ret(m);
    uint32_t wnd = 0; // 比特窗口
    size_t squre_count = 0; // 需要平方的次数
    while (bits_left > 0)
    {
        wnd <<= 1;
        wnd &= WND_MASK;
        if (0 != b.bit_at(--bits_left)) // b 的最后一个比特必定为1，因为 b 为奇数
        {
            wnd |= (uint32_t) 1;

            ++squre_count;
            bool term = true;
            for (size_t i = 1;
                 squre_count + i <= wnd_size && bits_left >= (int) i;
                ++i)
            {
                if (0 != b.bit_at(bits_left - i))
                {
                    term = false;
                    break;
                }
            }

            if (term)
            {
                for (size_t i = 0; i < squre_count; ++i)
                {
                    ret *= ret;
                    _montgomery(ret, rlen, n, nn, &ret);
                }
                squre_count = 0;

                ret *= tbl.at(wnd >> 1);
                _montgomery(ret, rlen, n, nn, &ret);
                wnd = 0;
            }
        }
        else if (0 == squre_count) // 窗口之间的0
        {
            ret *= ret;
            _montgomery(ret, rlen, n, nn, &ret);
        }
        else // 窗口内部的0
        {
            ++squre_count;
        }
    }

    // 处理返回值
    _montgomery(ret, rlen, n, nn, rs);
    return;
#endif
}

/**
 * 计算 (a ** b) mod (2 ** p)
 */
inline void _mod_pow_2(const BigInteger& a, const BigInteger& b, size_t p, BigInteger *rs)
{
    assert(NULL != rs);
    assert(a.is_positive() && b.is_positive() && p > 0);

    BigInteger ret(1, a.allocator());
    for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
    {
        ret.multiply_to_len(ret, p);

        if (0 != b.bit_at(i - 1))
            ret.multiply_to_len(a, p);
    }

    *rs = ret;
}

/**
 * 求(a**b)%n，即a的b次方(模n)
 * 参见 《现代计算机常用数据结构和算法》.潘金贵.顾铁成.南京大学出版社.1994 P576
 */
inline void mod_pow(const BigInteger& a, const BigInteger& b, const BigInteger& n, BigInteger *rs)
{
    assert(NULL != rs);
    assert(a.is_positive() && b.is_positive() && n.is_positive());

    if (b.is_zero())
    {
        *rs = (n == 1 ? 0 : 1);
        return;
    }
    else if (a == 1)
    {
        *rs = (n == 1 ? 0 : 1);
        return;
    }
    else if (a.is_zero())
    {
        rs->set_zero();
        return;
    }

#if (OPTIMIZE_LEVEL == 0)
    BigInteger ret(1);
    for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
    {
        ret = (ret * ret) % n;
        if (0 != b.bit_at(i - 1))
            ret = (ret * a) % n;
    }
    *rs = ret;
    return;
#elif (OPTIMIZE_LEVEL == 1)
    const size_t bbc = b.bit_count();
     if (bbc > 400) // 400 是一个经验数据
    {
        ModMultiplyPreBuildTable<4,MemAlloc> table(a % n, n, n.allocator()); /// 经测试，预算表模板参数取4比较合适
        BigInteger ret(1);
        for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
        {
            ret = (ret * ret) % n;
            if (0 != b.bit_at(i - 1))
                mod_multiply(ret, n, table, &ret);
        }
        *rs = ret;
        return;
    }
    else
    {
        BigInteger ret(1);
        for (size_t i = b.bit_length(); i > 0; --i) // 从高位向低有效位取bit
        {
            ret = (ret * ret) % n;
            if (0 != b.bit_at(i - 1))
                ret = (ret * a) % n;
        }
        *rs = ret;
        return;
    }
#else
    // 模是奇数，应用蒙哥马利算法
    if (n.bit_at(0) == 1)
    {
        _odd_mod_pow(a < n ? a : a % n, b, n, rs);
        return;
    }

    // 模是偶数，应用中国余数定理
    const size_t p = n.lowest_bit();
    BigInteger n1(n), n2(1, a.allocator());
    n1 >>= p;
    n2 <<= p;

    BigInteger a1(0, a.allocator());
    if (n1 != 1)
        _odd_mod_pow(a % n1, b, n1, &a1);

    BigInteger a2(0, a.allocator());
    _mod_pow_2((a < n ? a : a % n), b, p, &a2);

    BigInteger y1(0, a.allocator());
    extended_euclid(n2, n1, NULL, &y1, NULL);
    if (y1 < 0)
    {
        // y1 = n1 + (y1 % n1);
        y1 %= n1;
        y1 += n1;
    }
    BigInteger y2(0,a.allocator());
    extended_euclid(n1, n2, NULL, &y2, NULL);
    if (y2 < 0)
    {
        // y2 = n2 + (y2 % n2);
        y2 %= n2;
        y2 += n2;
    }

    // *rs = (a1 * n2 * y1 + a2 * n1 * y2) % n
    *rs = a1 * n2 * y1 + a2 * n1 * y2;
    *rs %= n;
    return;
#endif
}

}

#undef OPTIMIZE_LEVEL

#endif

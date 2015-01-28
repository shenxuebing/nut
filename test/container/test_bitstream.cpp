﻿#include <iostream>

#include <nut/unittest/unittest.hpp>

#include <nut/container/bit_stream.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestBitStream)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void set_up() {}

    void tear_down() {}

    void test_smoking()
    {
        BitStream bs("1010100");
        NUT_TA(bs.to_string() == "1010100");
        bs.append(false);
        bs.append(true);
        NUT_TA(bs.to_string() == "101010001");

        NUT_TA(bs.bit_at(2) && ! bs.bit_at(5));

        bs.append(bs);
        NUT_TA(bs.to_string() == "101010001101010001");

        NUT_TA(bs.bit1_count() == 8 && bs.bit0_count() == 10);
    }
};

NUT_REGISTER_FIXTURE(TestBitStream, "container, quiet")

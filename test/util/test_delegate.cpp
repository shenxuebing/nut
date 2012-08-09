
#include <nut/unittest/unittest.hpp>

#include <iostream>
#include <nut/util/delegate.hpp>

using namespace std;
using namespace nut;

static int g_t = 0;

int tf(int a)
{
    g_t += 1;
    return a + 1;
}

NUT_FIXTURE(TestDelegate)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()


    void setUp() {}
    void tearDown() {}

    int tm(int a)
    {
        g_t += 10;
        return a;
    }

    void tv()
    {
        g_t += 100;
    }

    void testSmoking()
    {
        delegate<void()> d1;
        d1.connect(this, &TestDelegate::tv);
        NUT_TA(g_t == 0);
        d1();
        NUT_TA(g_t == 100);

        delegate<int(int)> d2;
        d2.connect(this, &TestDelegate::tm);
        d2.connect(&tf);
        int rs = d2(3);
        NUT_TA(g_t == 111 && rs == 4);

        d2.disconnect(&tf);
        rs = d2(5);
        NUT_TA(g_t == 121 && rs == 5);

        d2.disconnect(this, &TestDelegate::tm);
        NUT_TA(!d2.isConnected());
        rs = d2(12);
        NUT_TA(g_t == 121 && rs == 0);
    }

};

NUT_REGISTER_FIXTURE(TestDelegate, "util, quiet")

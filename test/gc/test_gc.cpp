
#include <nut/unittest/unittest.hpp>

#include <nut/gc/gc.hpp>

using namespace nut;

static int obj_count = 0;

NUT_FIXTURE(TestGC)
{
    NUT_CASES_BEGIN()
    NUT_CASE(testSmoking)
    NUT_CASES_END()

    class B
    {
        DECLARE_GC_ENABLE
    };

    class A : public B
    {
    public:
        A() { ++obj_count; }
        ~A() { --obj_count; }
    };

    void setUp() {}
    void tearDown() {}

    void testSmoking() {
        NUT_TA(0 == obj_count);

        {
            ref<A> pa = gc_new<A>();
            ref<A> pb;
            NUT_TA(1 == obj_count);

            {
                ref<A> pc = gc_new<A>();
                NUT_TA(2 == obj_count);

                ref<A> pd = gc_new<A>();
                NUT_TA(3 == obj_count);
                pb = pd;
            }

            NUT_TA(2 == obj_count);

            pb = NULL;
            NUT_TA(1 == obj_count);
        }

        NUT_TA(0 == obj_count);
    }
};

NUT_REGISTER_FIXTURE(TestGC, "gc, quiet")

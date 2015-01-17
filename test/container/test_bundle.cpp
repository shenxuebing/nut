﻿
#include <nut/unittest/unittest.hpp>

#include <nut/container/bundle.hpp>

using namespace std;
using namespace nut;

NUT_FIXTURE(TestBundle)
{
    NUT_CASES_BEGIN()
    NUT_CASE(test_smoking)
    NUT_CASES_END()

    void set_up() {}

    void tear_down() {}

    void test_smoking()
    {
        Bundle<> *b = Bundle<>::create();
		b->set_value<int>("int", 23);
		NUT_TA(b->has_key("int"));
		NUT_TA(b->get_value<int>("int") == 23);

		b->set_value<string>("str", "abc");
		NUT_TA(b->has_key("str"));
		NUT_TA(b->get_value<string>("str") == "abc");

		b->clear();
		NUT_TA(!b->has_key("int"));

        b->rls_ref();
    }
};

NUT_REGISTER_FIXTURE(TestBundle, "container, quiet")

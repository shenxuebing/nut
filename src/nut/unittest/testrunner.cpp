﻿
#include <assert.h>
#include <string.h>

#include "testrunner.h"
#include "testregister.h"
#include "testfixture.h"

namespace nut
{

TestRunner::TestRunner(ITestLogger& logger)
    : m_logger(logger)
{}

void TestRunner::run_group(const char *group_name)
{
    assert(NULL != group_name);

    m_logger.start(group_name, NULL, NULL);

    TestRegister *pregister = *(TestRegister**)nut_get_register_header();
    while (NULL != pregister)
    {
        if (pregister->match_group(group_name))
        {
            m_logger.enter_fixture(pregister->get_fixture_name());

            TestFixture *fixture = pregister->new_fixture();
            assert(NULL != fixture);
            int total = fixture->___run_case(m_logger, -2, NULL);

            for (int i = 0; i < total; ++i) {
                if (NULL == fixture)
                    fixture = pregister->new_fixture();
                fixture->___run_case(m_logger, i, NULL);
                pregister->delete_fixture(fixture);
                fixture = NULL;
            }
            if (NULL != fixture)
                pregister->delete_fixture(fixture);

            m_logger.leave_fixture();
        }
        pregister = pregister->get_next_register();
    }

    m_logger.finish();
}

void TestRunner::run_fixture(const char *fixture_name)
{
    assert(NULL != fixture_name);

    m_logger.start(NULL, fixture_name, NULL);

    TestRegister *pregister = *(TestRegister**)nut_get_register_header();
    while (NULL != pregister)
    {
        if (0 == ::strcmp(fixture_name, pregister->get_fixture_name()))
        {
            m_logger.enter_fixture(fixture_name);

            TestFixture *fixture = pregister->new_fixture();
            assert(NULL != fixture);
            int total = fixture->___run_case(m_logger, -2, NULL);

            for (int i = 0; i < total; ++i) {
                if (NULL == fixture)
                    fixture = pregister->new_fixture();
                fixture->___run_case(m_logger, i, NULL);
                pregister->delete_fixture(fixture);
                fixture = NULL;
            }
            if (NULL != fixture)
                pregister->delete_fixture(fixture);

            m_logger.leave_fixture();
            break;
        }
        pregister = pregister->get_next_register();
    }

    m_logger.finish();
}

void TestRunner::run_case(const char *fixture_name, const char *case_name)
{
    assert(NULL != fixture_name && NULL != case_name);

    m_logger.start(NULL, fixture_name, case_name);

    TestRegister *pregister = *(TestRegister**)nut_get_register_header();
    while (NULL != pregister)
    {
        if (0 == ::strcmp(fixture_name, pregister->get_fixture_name()))
        {
            m_logger.enter_fixture(fixture_name);

            TestFixture *fixture = pregister->new_fixture();
            fixture->___run_case(m_logger, -1, case_name);
            pregister->delete_fixture(fixture);

            m_logger.leave_fixture();
            break;
        }
        pregister = pregister->get_next_register();
    }

    m_logger.finish();
}

}

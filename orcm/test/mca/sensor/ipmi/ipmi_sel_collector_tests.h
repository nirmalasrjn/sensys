/*
 * Copyright (c) 2015      Intel, Inc. All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef IPMI_SEL_COLLECTOR_TESTS_H
#define IPMI_SEL_COLLECTOR_TESTS_H

#include "gtest/gtest.h"

class ut_ipmi_sel_collection: public testing::Test
{
    protected: // gtest required methods

        static void SetUpTestCase();
        static void TearDownTestCase();

    public: // Helper Functions
        static bool WildcardCompare(const char* value, const char* pattern);
}; // class

#endif // IPMI_SEL_COLLECTOR_TESTS_H_INCLUDED

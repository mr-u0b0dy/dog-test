/* SPDX-License-Identifier: Apache-2.0 */
/* Copyright 2026 dog-test contributors */

#include <stdio.h>
#include <string.h>
#include "dogtest/test_case.h"

#ifndef DT_MAX_TESTS
#define DT_MAX_TESTS 256
#endif

static const dt_test_case_t* g_tests[DT_MAX_TESTS];
static size_t g_test_count = 0;

void dt_register_test(const dt_test_case_t* test_case) {
    if (g_test_count >= DT_MAX_TESTS) {
        fprintf(stderr,
                "dogtest: WARNING – test registry full (%d); \"%s\" was NOT registered\n",
                DT_MAX_TESTS,
                (test_case && test_case->name) ? test_case->name : "(null)");
        return;
    }
    g_tests[g_test_count++] = test_case;
}

size_t dt_registered_test_count(void) {
    return g_test_count;
}

const dt_test_case_t* dt_registered_test_at(size_t index) {
    if (index >= g_test_count) {
        return 0;
    }

    return g_tests[index];
}

void dt_reset_registry(void) {
    memset(g_tests, 0, sizeof(g_tests));
    g_test_count = 0;
}

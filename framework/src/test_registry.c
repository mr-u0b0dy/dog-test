#include <stdio.h>
#include <string.h>
#include "hiltest/test_case.h"

#ifndef HT_MAX_TESTS
#define HT_MAX_TESTS 256
#endif

static const ht_test_case_t* g_tests[HT_MAX_TESTS];
static size_t g_test_count = 0;

void ht_register_test(const ht_test_case_t* test_case) {
    if (g_test_count >= HT_MAX_TESTS) {
        fprintf(stderr,
                "hiltest: WARNING – test registry full (%d); \"%s\" was NOT registered\n",
                HT_MAX_TESTS,
                (test_case && test_case->name) ? test_case->name : "(null)");
        return;
    }
    g_tests[g_test_count++] = test_case;
}

size_t ht_registered_test_count(void) {
    return g_test_count;
}

const ht_test_case_t* ht_registered_test_at(size_t index) {
    if (index >= g_test_count) {
        return 0;
    }

    return g_tests[index];
}

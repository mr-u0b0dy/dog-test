#include <stdio.h>
#include "hiltest/assert.h"

static const char* g_last_file;
static uint32_t g_last_line;
static const char* g_last_expr;
static const char* g_last_message;

void ht_fail_impl(const char* file, uint32_t line, const char* expr, const char* message) {
    g_last_file = file;
    g_last_line = line;
    g_last_expr = expr;
    g_last_message = message;
}

void ht_emit_result_start(const ht_test_case_t* test_case) {
    const char* protocol = (test_case->monitor != 0 && test_case->monitor->protocol != 0)
        ? test_case->monitor->protocol
        : "none";
    printf("HT_EVENT start name=%s reset=%d monitor=%s\n", test_case->name, (int)test_case->reset_mode, protocol);
}

void ht_emit_result_pass(const ht_test_case_t* test_case) {
    printf("HT_EVENT pass name=%s\n", test_case->name);
}

void ht_emit_result_fail(const ht_test_case_t* test_case, const char* reason) {
    printf("HT_EVENT fail name=%s reason=%s file=%s line=%lu expr=%s msg=%s\n",
           test_case->name,
           reason,
           (g_last_file != 0) ? g_last_file : "n/a",
           (unsigned long)g_last_line,
           (g_last_expr != 0) ? g_last_expr : "n/a",
           (g_last_message != 0) ? g_last_message : "n/a");
}

int ht_run_all_tests(void) {
    size_t test_count = ht_registered_test_count();
    int failed = 0;

    for (size_t i = 0; i < test_count; ++i) {
        const ht_test_case_t* test_case = ht_registered_test_at(i);
        if (test_case == 0 || test_case->run == 0) {
            continue;
        }

        g_last_file = 0;
        g_last_line = 0;
        g_last_expr = 0;
        g_last_message = 0;

        ht_emit_result_start(test_case);
        if (test_case->run() == HT_TEST_PASSED) {
            ht_emit_result_pass(test_case);
        } else {
            ht_emit_result_fail(test_case, "assert");
            failed++;
        }
    }

    printf("HT_EVENT summary total=%lu failed=%d\n", (unsigned long)test_count, failed);
    return failed;
}

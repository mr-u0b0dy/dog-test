#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "hiltest/assert.h"

/* ── Non-fatal expectation counter ─────────────────────────────────── */
int ht_expect_failure_count = 0;

static const char* g_last_file;
static uint32_t g_last_line;
static const char* g_last_expr;
static const char* g_last_message;

/* Comparison failure values (set by ht_fail_cmp). */
static int  g_has_cmp_values;
static long g_cmp_expected;
static long g_cmp_actual;

/* String comparison failure values (set by ht_fail_str). */
static int  g_has_str_values;
static const char* g_str_expected;
static const char* g_str_actual;

void ht_fail_impl(const char* file, uint32_t line, const char* expr, const char* message) {
    g_last_file    = file;
    g_last_line    = line;
    g_last_expr    = expr;
    g_last_message = message;
    g_has_cmp_values = 0;
    g_has_str_values = 0;
}

void ht_fail_cmp(const char* file, uint32_t line,
                 const char* expr,
                 long expected, long actual,
                 const char* message) {
    g_last_file    = file;
    g_last_line    = line;
    g_last_expr    = expr;
    g_last_message = message;
    g_has_cmp_values = 1;
    g_cmp_expected   = expected;
    g_cmp_actual     = actual;
    g_has_str_values = 0;
}

void ht_fail_str(const char* file, uint32_t line,
                 const char* expr,
                 const char* expected, const char* actual,
                 const char* message) {
    g_last_file    = file;
    g_last_line    = line;
    g_last_expr    = expr;
    g_last_message = message;
    g_has_cmp_values = 0;
    g_has_str_values = 1;
    g_str_expected   = expected;
    g_str_actual     = actual;
}

/* ── HT_EVENT output helpers ───────────────────────────────────────── */

/** Print a key=value pair, quoting the value if it contains spaces or '='. */
static void _emit_kv(const char* key, const char* value) {
    if (value == 0) {
        printf(" %s=n/a", key);
        return;
    }
    /* Check if quoting is needed */
    int needs_quote = 0;
    for (const char* p = value; *p; ++p) {
        if (*p == ' ' || *p == '=' || *p == '"') {
            needs_quote = 1;
            break;
        }
    }
    if (needs_quote) {
        printf(" %s=\"", key);
        for (const char* p = value; *p; ++p) {
            if (*p == '"') printf("\\\"");
            else putchar(*p);
        }
        putchar('"');
    } else {
        printf(" %s=%s", key, value);
    }
}

void ht_emit_result_start(const ht_test_case_t* test_case) {
    const char* protocol = (test_case->monitor != 0 && test_case->monitor->protocol != 0)
        ? test_case->monitor->protocol
        : "none";
    printf("HT_EVENT start");
    _emit_kv("name", test_case->name);
    printf(" reset=%d", (int)test_case->reset_mode);
    _emit_kv("monitor", protocol);
    if (test_case->tags != 0) {
        _emit_kv("tags", test_case->tags);
    }
    putchar('\n');
}

void ht_emit_result_pass(const ht_test_case_t* test_case) {
    printf("HT_EVENT pass");
    _emit_kv("name", test_case->name);
    putchar('\n');
}

void ht_emit_result_fail(const ht_test_case_t* test_case, const char* reason) {
    printf("HT_EVENT fail");
    _emit_kv("name", test_case->name);
    _emit_kv("reason", reason);
    _emit_kv("file", g_last_file);
    printf(" line=%" PRIu32, g_last_line);
    _emit_kv("expr", g_last_expr);
    _emit_kv("msg", g_last_message);
    if (g_has_cmp_values) {
        printf(" expected=%ld actual=%ld", g_cmp_expected, g_cmp_actual);
    }
    if (g_has_str_values) {
        _emit_kv("expected_str", g_str_expected);
        _emit_kv("actual_str", g_str_actual);
    }
    putchar('\n');
}

void ht_emit_result_skip(const ht_test_case_t* test_case, const char* reason) {
    printf("HT_EVENT skip");
    _emit_kv("name", test_case->name);
    _emit_kv("reason", reason ? reason : g_last_message);
    putchar('\n');
}

static void _reset_fail_state(void) {
    g_last_file      = 0;
    g_last_line      = 0;
    g_last_expr      = 0;
    g_last_message   = 0;
    g_has_cmp_values = 0;
    g_has_str_values = 0;
}

/** Check if a comma-separated @p tags string contains @p tag. */
static int _has_tag(const char* tags, const char* tag) {
    if (tags == 0 || tag == 0) return 0;
    size_t tag_len = strlen(tag);
    const char* p = tags;
    while (*p) {
        /* skip leading whitespace/commas */
        while (*p == ',' || *p == ' ') p++;
        if (*p == '\0') break;
        const char* end = p;
        while (*end && *end != ',') end++;
        size_t span = (size_t)(end - p);
        /* trim trailing whitespace */
        while (span > 0 && p[span - 1] == ' ') span--;
        if (span == tag_len && strncmp(p, tag, tag_len) == 0) return 1;
        p = end;
    }
    return 0;
}

/**
 * Internal runner.  If @p filter is NULL every test is executed.
 * Otherwise only tests whose name contains @p filter run.
 * If @p tag is non-NULL, only tests with matching tags run.
 */
static int _run_tests_impl(const char* filter, const char* tag) {
    size_t test_count = ht_registered_test_count();
    int failed  = 0;
    int skipped = 0;
    int ran     = 0;

    for (size_t i = 0; i < test_count; ++i) {
        const ht_test_case_t* tc = ht_registered_test_at(i);
        if (tc == 0 || tc->run == 0) {
            continue;
        }

        /* Name filter: skip tests that don't match. */
        if (filter != 0 && strstr(tc->name, filter) == 0) {
            continue;
        }

        /* Tag filter: skip tests without the requested tag. */
        if (tag != 0 && !_has_tag(tc->tags, tag)) {
            continue;
        }

        _reset_fail_state();
        ht_expect_failure_count = 0;
        ht_emit_result_start(tc);

        /* Setup fixture. */
        if (tc->setup != 0 && tc->setup() != 0) {
            ht_emit_result_fail(tc, "setup");
            failed++;
            ran++;
            continue;
        }

        ht_test_result_t result = tc->run();

        /* Teardown fixture (always runs, even on failure). */
        if (tc->teardown != 0) {
            tc->teardown();
        }

        /* Promote to failure if any HT_EXPECT_* checks failed. */
        if (result == HT_TEST_PASSED && ht_expect_failure_count > 0) {
            result = HT_TEST_FAILED;
        }

        switch (result) {
            case HT_TEST_PASSED:
                ht_emit_result_pass(tc);
                break;
            case HT_TEST_SKIPPED:
                ht_emit_result_skip(tc, g_last_message);
                skipped++;
                break;
            case HT_TEST_FAILED:
            default:
                ht_emit_result_fail(tc, "assert");
                failed++;
                break;
        }
        ran++;
    }

    printf("HT_EVENT summary total=%d passed=%d failed=%d skipped=%d\n",
           ran, ran - failed - skipped, failed, skipped);
    return failed;
}

int ht_run_all_tests(void) {
    return _run_tests_impl((const char*)0, (const char*)0);
}

int ht_run_tests_filtered(const char* filter) {
    return _run_tests_impl(filter, (const char*)0);
}

int ht_run_tests_by_tag(const char* tag) {
    return _run_tests_impl((const char*)0, tag);
}

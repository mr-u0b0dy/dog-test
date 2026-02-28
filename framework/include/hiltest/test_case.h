#ifndef HILTEST_TEST_CASE_H
#define HILTEST_TEST_CASE_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HT_RESET_NONE = 0,
    HT_RESET_SOFT = 1,
    HT_RESET_HARD = 2
} ht_reset_mode_t;

typedef enum {
    HT_TEST_PASSED  = 0,
    HT_TEST_FAILED  = 1,
    HT_TEST_SKIPPED = 2
} ht_test_result_t;

typedef ht_test_result_t (*ht_test_fn_t)(void);

/** Optional setup/teardown callbacks (return non-zero to abort). */
typedef int (*ht_fixture_fn_t)(void);

typedef struct {
    const char* protocol;
    const char* channels;
    const char* trigger;
    uint32_t timeout_ms;
} ht_monitor_spec_t;

typedef struct {
    const char*            name;
    ht_test_fn_t           run;
    ht_reset_mode_t        reset_mode;
    const ht_monitor_spec_t* monitor;
    ht_fixture_fn_t        setup;      /**< called before run(); may be NULL */
    ht_fixture_fn_t        teardown;   /**< called after run();  may be NULL */
    const char*            tags;       /**< comma-separated tags; may be NULL */
} ht_test_case_t;

void ht_register_test(const ht_test_case_t* test_case);
size_t ht_registered_test_count(void);
const ht_test_case_t* ht_registered_test_at(size_t index);

void ht_emit_result_start(const ht_test_case_t* test_case);
void ht_emit_result_pass(const ht_test_case_t* test_case);
void ht_emit_result_fail(const ht_test_case_t* test_case, const char* reason);
void ht_emit_result_skip(const ht_test_case_t* test_case, const char* reason);

/**
 * Run all registered tests.
 * If @p filter is non-NULL only tests whose name contains @p filter are run.
 * Returns the number of failures.
 */
int ht_run_all_tests(void);
int ht_run_tests_filtered(const char* filter);

/**
 * Run only tests whose tags field contains @p tag.
 * Returns the number of failures.
 */
int ht_run_tests_by_tag(const char* tag);

/* ── Registration macros ───────────────────────────────────────────── */

/** Full-form: specify name, reset mode, monitor pointer, and function name. */
#define HT_TEST(name_literal, reset_mode_literal, monitor_ptr_literal, fn_name) \
    static ht_test_result_t fn_name(void); \
    static const ht_test_case_t fn_name##_meta = { \
        name_literal, fn_name, reset_mode_literal, monitor_ptr_literal, \
        (ht_fixture_fn_t)0, (ht_fixture_fn_t)0, (const char*)0 \
    }; \
    static void fn_name##_register(void) __attribute__((constructor)); \
    static void fn_name##_register(void) { ht_register_test(&fn_name##_meta); } \
    static ht_test_result_t fn_name(void)

/** Full-form with tags for grouping/filtering. */
#define HT_TEST_T(name_literal, reset_mode_literal, monitor_ptr_literal, tags_literal, fn_name) \
    static ht_test_result_t fn_name(void); \
    static const ht_test_case_t fn_name##_meta = { \
        name_literal, fn_name, reset_mode_literal, monitor_ptr_literal, \
        (ht_fixture_fn_t)0, (ht_fixture_fn_t)0, tags_literal \
    }; \
    static void fn_name##_register(void) __attribute__((constructor)); \
    static void fn_name##_register(void) { ht_register_test(&fn_name##_meta); } \
    static ht_test_result_t fn_name(void)

/** Extended form with setup/teardown fixtures. */
#define HT_TEST_F(name_literal, reset_mode_literal, monitor_ptr_literal, \
                  setup_fn, teardown_fn, fn_name) \
    static ht_test_result_t fn_name(void); \
    static const ht_test_case_t fn_name##_meta = { \
        name_literal, fn_name, reset_mode_literal, monitor_ptr_literal, \
        (setup_fn), (teardown_fn), (const char*)0 \
    }; \
    static void fn_name##_register(void) __attribute__((constructor)); \
    static void fn_name##_register(void) { ht_register_test(&fn_name##_meta); } \
    static ht_test_result_t fn_name(void)

/** Extended form with setup/teardown fixtures and tags. */
#define HT_TEST_FT(name_literal, reset_mode_literal, monitor_ptr_literal, \
                   setup_fn, teardown_fn, tags_literal, fn_name) \
    static ht_test_result_t fn_name(void); \
    static const ht_test_case_t fn_name##_meta = { \
        name_literal, fn_name, reset_mode_literal, monitor_ptr_literal, \
        (setup_fn), (teardown_fn), tags_literal \
    }; \
    static void fn_name##_register(void) __attribute__((constructor)); \
    static void fn_name##_register(void) { ht_register_test(&fn_name##_meta); } \
    static ht_test_result_t fn_name(void)

/* ── Convenience entry-point macro ─────────────────────────────────── */

/**
 * HT_MAIN() – provides a standard main() that emits the boot event,
 * runs all tests, and returns the failure count as the exit code.
 */
#define HT_MAIN() \
    int main(void) { \
        puts("HT_EVENT boot"); \
        return ht_run_all_tests(); \
    }

#ifdef __cplusplus
}
#endif

#endif

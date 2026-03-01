/* SPDX-License-Identifier: Apache-2.0 */
/* Copyright 2026 dog-test contributors */

#ifndef DOGTEST_TEST_CASE_H
#define DOGTEST_TEST_CASE_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DT_RESET_NONE = 0,
    DT_RESET_SOFT = 1,
    DT_RESET_HARD = 2
} dt_reset_mode_t;

typedef enum {
    DT_TEST_PASSED  = 0,
    DT_TEST_FAILED  = 1,
    DT_TEST_SKIPPED = 2
} dt_test_result_t;

typedef dt_test_result_t (*dt_test_fn_t)(void);

/** Optional setup/teardown callbacks (return non-zero to abort). */
typedef int (*dt_fixture_fn_t)(void);

typedef struct {
    const char* protocol;
    const char* channels;
    const char* trigger;
    uint32_t timeout_ms;
} dt_monitor_spec_t;

typedef struct {
    const char*            name;
    dt_test_fn_t           run;
    dt_reset_mode_t        reset_mode;
    const dt_monitor_spec_t* monitor;
    dt_fixture_fn_t        setup;      /**< called before run(); may be NULL */
    dt_fixture_fn_t        teardown;   /**< called after run();  may be NULL */
    const char*            tags;       /**< comma-separated tags; may be NULL */
} dt_test_case_t;

void dt_register_test(const dt_test_case_t* test_case);
size_t dt_registered_test_count(void);
const dt_test_case_t* dt_registered_test_at(size_t index);

/** Reset the test registry (clear all registrations). Useful for testing. */
void dt_reset_registry(void);

void dt_emit_result_start(const dt_test_case_t* test_case);
void dt_emit_result_pass(const dt_test_case_t* test_case);
void dt_emit_result_fail(const dt_test_case_t* test_case, const char* reason);
void dt_emit_result_skip(const dt_test_case_t* test_case, const char* reason);

/**
 * Run all registered tests.
 * If @p filter is non-NULL only tests whose name contains @p filter are run.
 * Returns the number of failures.
 */
int dt_run_all_tests(void);
int dt_run_tests_filtered(const char* filter);

/**
 * Run only tests whose tags field contains @p tag.
 * Returns the number of failures.
 */
int dt_run_tests_by_tag(const char* tag);

/* ── Registration macros ───────────────────────────────────────────── */

/** Full-form: specify name, reset mode, monitor pointer, and function name. */
#define DT_TEST(name_literal, reset_mode_literal, monitor_ptr_literal, fn_name) \
    static dt_test_result_t fn_name(void); \
    static const dt_test_case_t fn_name##_meta = { \
        name_literal, fn_name, reset_mode_literal, monitor_ptr_literal, \
        (dt_fixture_fn_t)0, (dt_fixture_fn_t)0, (const char*)0 \
    }; \
    static void fn_name##_register(void) __attribute__((constructor)); \
    static void fn_name##_register(void) { dt_register_test(&fn_name##_meta); } \
    static dt_test_result_t fn_name(void)

/** Full-form with tags for grouping/filtering. */
#define DT_TEST_T(name_literal, reset_mode_literal, monitor_ptr_literal, tags_literal, fn_name) \
    static dt_test_result_t fn_name(void); \
    static const dt_test_case_t fn_name##_meta = { \
        name_literal, fn_name, reset_mode_literal, monitor_ptr_literal, \
        (dt_fixture_fn_t)0, (dt_fixture_fn_t)0, tags_literal \
    }; \
    static void fn_name##_register(void) __attribute__((constructor)); \
    static void fn_name##_register(void) { dt_register_test(&fn_name##_meta); } \
    static dt_test_result_t fn_name(void)

/** Extended form with setup/teardown fixtures. */
#define DT_TEST_F(name_literal, reset_mode_literal, monitor_ptr_literal, \
                  setup_fn, teardown_fn, fn_name) \
    static dt_test_result_t fn_name(void); \
    static const dt_test_case_t fn_name##_meta = { \
        name_literal, fn_name, reset_mode_literal, monitor_ptr_literal, \
        (setup_fn), (teardown_fn), (const char*)0 \
    }; \
    static void fn_name##_register(void) __attribute__((constructor)); \
    static void fn_name##_register(void) { dt_register_test(&fn_name##_meta); } \
    static dt_test_result_t fn_name(void)

/** Extended form with setup/teardown fixtures and tags. */
#define DT_TEST_FT(name_literal, reset_mode_literal, monitor_ptr_literal, \
                   setup_fn, teardown_fn, tags_literal, fn_name) \
    static dt_test_result_t fn_name(void); \
    static const dt_test_case_t fn_name##_meta = { \
        name_literal, fn_name, reset_mode_literal, monitor_ptr_literal, \
        (setup_fn), (teardown_fn), tags_literal \
    }; \
    static void fn_name##_register(void) __attribute__((constructor)); \
    static void fn_name##_register(void) { dt_register_test(&fn_name##_meta); } \
    static dt_test_result_t fn_name(void)

/* ── Convenience entry-point macro ─────────────────────────────────── */

/**
 * DT_MAIN() – provides a standard main() that emits the boot event,
 * runs all tests, and returns the failure count as the exit code.
 */
#define DT_MAIN() \
    int main(void) { \
        puts("DT_EVENT boot"); \
        return dt_run_all_tests(); \
    }

#ifdef __cplusplus
}
#endif

#endif

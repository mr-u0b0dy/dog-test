/**
 * test_framework.c – host-side unit tests for the dogtest framework.
 *
 * Tests the registry, assertions (fatal + non-fatal), runner output,
 * and tag-based filtering without requiring any target hardware.
 */
/* SPDX-License-Identifier: Apache-2.0 */
/* Copyright 2026 dog-test contributors */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dogtest/assert.h"
#include "dogtest/test_case.h"

/* ── Registry tests ────────────────────────────────────────────────── */

DT_TEST("registry_count", DT_RESET_NONE, NULL, test_registry_count) {
    /* At least this test itself must be registered. */
    DT_ASSERT_GT((long)dt_registered_test_count(), 0);
    return DT_TEST_PASSED;
}

DT_TEST("registry_lookup", DT_RESET_NONE, NULL, test_registry_lookup) {
    const dt_test_case_t* tc = dt_registered_test_at(0);
    DT_ASSERT_NOT_NULL(tc);
    DT_ASSERT_NOT_NULL(tc->name);
    return DT_TEST_PASSED;
}

DT_TEST("registry_bounds", DT_RESET_NONE, NULL, test_registry_bounds) {
    const dt_test_case_t* tc = dt_registered_test_at(99999);
    DT_ASSERT_NULL(tc);
    return DT_TEST_PASSED;
}

/* ── Assertion macro tests ─────────────────────────────────────────── */

DT_TEST("assert_eq_pass", DT_RESET_NONE, NULL, test_assert_eq_pass) {
    DT_ASSERT_EQ(42, 42);
    return DT_TEST_PASSED;
}

DT_TEST("assert_ne_pass", DT_RESET_NONE, NULL, test_assert_ne_pass) {
    DT_ASSERT_NE(1, 2);
    return DT_TEST_PASSED;
}

DT_TEST("assert_gt_ge_lt_le", DT_RESET_NONE, NULL, test_assert_relational) {
    DT_ASSERT_GT(10, 5);
    DT_ASSERT_GE(10, 10);
    DT_ASSERT_GE(10, 5);
    DT_ASSERT_LT(5, 10);
    DT_ASSERT_LE(10, 10);
    DT_ASSERT_LE(5, 10);
    return DT_TEST_PASSED;
}

DT_TEST("assert_true_false", DT_RESET_NONE, NULL, test_assert_bool) {
    DT_ASSERT_TRUE(1);
    DT_ASSERT_FALSE(0);
    return DT_TEST_PASSED;
}

DT_TEST("assert_null_not_null", DT_RESET_NONE, NULL, test_assert_ptr) {
    int x = 1;
    DT_ASSERT_NOT_NULL(&x);
    DT_ASSERT_NULL(NULL);
    return DT_TEST_PASSED;
}

DT_TEST("assert_str_eq", DT_RESET_NONE, NULL, test_assert_str_eq) {
    DT_ASSERT_STR_EQ("hello", "hello");
    return DT_TEST_PASSED;
}

DT_TEST("assert_str_contains", DT_RESET_NONE, NULL, test_assert_str_contains) {
    DT_ASSERT_STR_CONTAINS("hello world", "world");
    return DT_TEST_PASSED;
}

DT_TEST("assert_eq_u32", DT_RESET_NONE, NULL, test_assert_eq_u32) {
    uint32_t a = 0xDEADBEEF;
    uint32_t b = 0xDEADBEEF;
    DT_ASSERT_EQ_U32(a, b);
    return DT_TEST_PASSED;
}

DT_TEST("assert_float_near", DT_RESET_NONE, NULL, test_assert_float_near) {
    DT_ASSERT_FLOAT_NEAR(3.14, 3.14159, 0.01);
    return DT_TEST_PASSED;
}

DT_TEST("assert_in_range", DT_RESET_NONE, NULL, test_assert_in_range) {
    DT_ASSERT_IN_RANGE(5, 1, 10);
    DT_ASSERT_IN_RANGE(1, 1, 10);
    DT_ASSERT_IN_RANGE(10, 1, 10);
    return DT_TEST_PASSED;
}

DT_TEST("assert_mem_eq", DT_RESET_NONE, NULL, test_assert_mem_eq) {
    uint8_t a[] = {0x01, 0x02, 0x03};
    uint8_t b[] = {0x01, 0x02, 0x03};
    DT_ASSERT_MEM_EQ(a, b, sizeof(a));
    return DT_TEST_PASSED;
}

DT_TEST("assert_msg", DT_RESET_NONE, NULL, test_assert_msg) {
    DT_ASSERT_MSG(1 == 1, "one should equal one");
    return DT_TEST_PASSED;
}

/* ── Non-fatal DT_EXPECT_* tests ──────────────────────────────────── */

DT_TEST("expect_all_pass", DT_RESET_NONE, NULL, test_expect_all_pass) {
    DT_EXPECT_TRUE(1);
    DT_EXPECT_FALSE(0);
    DT_EXPECT_EQ(42, 42);
    DT_EXPECT_NE(1, 2);
    DT_EXPECT_GT(10, 5);
    DT_EXPECT_GE(10, 10);
    DT_EXPECT_GE(10, 5);
    DT_EXPECT_LT(5, 10);
    DT_EXPECT_LE(10, 10);
    DT_EXPECT_LE(5, 10);
    DT_EXPECT_NULL(NULL);
    int x = 1;
    DT_EXPECT_NOT_NULL(&x);
    DT_EXPECT_STR_EQ("a", "a");
    DT_EXPECT_STR_CONTAINS("hello world", "world");
    DT_EXPECT_MSG(1, "ok");
    DT_EXPECT_EQ_U32(0xDEADBEEF, 0xDEADBEEF);
    DT_EXPECT_FLOAT_NEAR(3.14, 3.14159, 0.01);
    DT_EXPECT_IN_RANGE(5, 1, 10);
    uint8_t a1[] = {1, 2, 3};
    uint8_t b1[] = {1, 2, 3};
    DT_EXPECT_MEM_EQ(a1, b1, sizeof(a1));
    return DT_TEST_PASSED;
}

/* ── Tag tests ─────────────────────────────────────────────────────── */

DT_TEST_T("tagged_uart", DT_RESET_NONE, NULL, "uart,smoke", test_tagged_uart) {
    DT_ASSERT_TRUE(1);
    return DT_TEST_PASSED;
}

DT_TEST_T("tagged_spi", DT_RESET_NONE, NULL, "spi,smoke", test_tagged_spi) {
    DT_ASSERT_TRUE(1);
    return DT_TEST_PASSED;
}

/* ── Skip test ─────────────────────────────────────────────────────── */

DT_TEST("skip_example", DT_RESET_NONE, NULL, test_skip_example) {
    DT_SKIP("demonstration of skip");
    return DT_TEST_PASSED;
}

/* ── Fixture test ──────────────────────────────────────────────────── */

static int fixture_counter = 0;

static int fixture_setup(void) {
    fixture_counter = 100;
    return 0;
}

static int fixture_teardown(void) {
    fixture_counter = 0;
    return 0;
}

DT_TEST_F("fixture_test", DT_RESET_NONE, NULL,
           fixture_setup, fixture_teardown, test_fixture) {
    DT_ASSERT_EQ(100, fixture_counter);
    return DT_TEST_PASSED;
}

/* ── Fixture + tags (DT_TEST_FT) test ─────────────────────────────── */

DT_TEST_FT("fixture_tagged", DT_RESET_NONE, NULL,
            fixture_setup, fixture_teardown, "unit,fixture", test_fixture_tagged) {
    DT_ASSERT_EQ(100, fixture_counter);
    return DT_TEST_PASSED;
}

/* ── Failure-path tests ────────────────────────────────────────────── */

/*
 * These use helper functions to exercise failing assertions without
 * failing the entire test suite.  The helpers are NOT registered as
 * DT_TESTs — they are called directly and their return values inspected.
 */

static dt_test_result_t _helper_assert_eq_fail(void) {
    DT_ASSERT_EQ(1, 2);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_str_eq_fail(void) {
    DT_ASSERT_STR_EQ("hello", "world");
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_skip(void) {
    DT_SKIP("intentional skip");
    return DT_TEST_PASSED;
}

DT_TEST("fail_assert_eq_returns_failed", DT_RESET_NONE, NULL, test_fail_assert_eq) {
    dt_test_result_t r = _helper_assert_eq_fail();
    DT_ASSERT_EQ((long)r, (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_assert_str_eq_returns_failed", DT_RESET_NONE, NULL, test_fail_assert_str_eq) {
    dt_test_result_t r = _helper_assert_str_eq_fail();
    DT_ASSERT_EQ((long)r, (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("skip_returns_skipped", DT_RESET_NONE, NULL, test_skip_returns_skipped) {
    dt_test_result_t r = _helper_skip();
    DT_ASSERT_EQ((long)r, (long)DT_TEST_SKIPPED);
    return DT_TEST_PASSED;
}

/* ── Expect promotion test ─────────────────────────────────────────── */

DT_TEST("expect_promotion", DT_RESET_NONE, NULL, test_expect_promotion) {
    /* Save and reset the global counter so we can test independently. */
    int saved = dt_expect_failure_count;
    dt_expect_failure_count = 0;

    /* A failing DT_EXPECT (run inline, not via helper — it won't return). */
    DT_EXPECT_EQ(1, 2);   /* should fail non-fatally */
    DT_ASSERT_EQ(dt_expect_failure_count, 1);

    /* Restore so this test itself doesn't get promoted as failed. */
    dt_expect_failure_count = saved;
    return DT_TEST_PASSED;
}

/* ── Filtered / tag-based runner tests ─────────────────────────────── */

DT_TEST("run_filtered_match", DT_RESET_NONE, NULL, test_run_filtered) {
    /* Filter for a name that matches a known test but NOT this one. */
    int failures = dt_run_tests_filtered("tagged_uart");
    DT_ASSERT_EQ(failures, 0);
    return DT_TEST_PASSED;
}

DT_TEST("run_filtered_nomatch", DT_RESET_NONE, NULL, test_run_filtered_unknown) {
    /* No registered test should match this filter. 0 ran → 0 failed. */
    int failures = dt_run_tests_filtered("_xyzzy_nonexistent_filter_");
    DT_ASSERT_EQ(failures, 0);
    return DT_TEST_PASSED;
}

DT_TEST("run_by_tag_smoke", DT_RESET_NONE, NULL, test_run_by_tag_smoke) {
    /* We have tests tagged "smoke" (tagged_uart, tagged_spi). */
    int failures = dt_run_tests_by_tag("smoke");
    DT_ASSERT_EQ(failures, 0);
    return DT_TEST_PASSED;
}

DT_TEST("run_by_tag_nomatch", DT_RESET_NONE, NULL, test_run_by_tag_unknown) {
    int failures = dt_run_tests_by_tag("_xyzzy_nonexistent_tag_");
    DT_ASSERT_EQ(failures, 0);
    return DT_TEST_PASSED;
}

/* ── Version macro tests ───────────────────────────────────────────── */

DT_TEST("version_macros", DT_RESET_NONE, NULL, test_version_macros) {
    DT_ASSERT_EQ(DOGTEST_VERSION_MAJOR, 0);
    DT_ASSERT_EQ(DOGTEST_VERSION_MINOR, 1);
    DT_ASSERT_EQ(DOGTEST_VERSION_PATCH, 0);
    DT_ASSERT_STR_EQ(DOGTEST_VERSION_STRING, "0.1.0");
    return DT_TEST_PASSED;
}

/* ── 64-bit assertion tests ────────────────────────────────────────── */

DT_TEST("assert_eq_i64_pass", DT_RESET_NONE, NULL, test_assert_eq_i64) {
    int64_t a = INT64_C(0x100000000);
    int64_t b = INT64_C(0x100000000);
    DT_ASSERT_EQ_I64(a, b);
    return DT_TEST_PASSED;
}

DT_TEST("assert_ne_i64_pass", DT_RESET_NONE, NULL, test_assert_ne_i64) {
    DT_ASSERT_NE_I64(INT64_C(1), INT64_C(2));
    return DT_TEST_PASSED;
}

DT_TEST("assert_eq_u64_pass", DT_RESET_NONE, NULL, test_assert_eq_u64) {
    uint64_t a = UINT64_C(0xDEADBEEFCAFE);
    uint64_t b = UINT64_C(0xDEADBEEFCAFE);
    DT_ASSERT_EQ_U64(a, b);
    return DT_TEST_PASSED;
}

DT_TEST("assert_ne_u64_pass", DT_RESET_NONE, NULL, test_assert_ne_u64) {
    DT_ASSERT_NE_U64(UINT64_C(1), UINT64_C(2));
    return DT_TEST_PASSED;
}

/* ── Bitmask assertion tests ───────────────────────────────────────── */

DT_TEST("assert_bits_pass", DT_RESET_NONE, NULL, test_assert_bits) {
    uint32_t reg = 0b11010110;
    DT_ASSERT_BITS(0xFF, 0b11010110, reg);
    /* Check only the lower nibble */
    DT_ASSERT_BITS(0x0F, 0x06, reg);
    return DT_TEST_PASSED;
}

/* ── Array assertion tests ─────────────────────────────────────────── */

DT_TEST("assert_array_eq_pass", DT_RESET_NONE, NULL, test_assert_array_eq) {
    int a[] = {10, 20, 30, 40};
    int b[] = {10, 20, 30, 40};
    DT_ASSERT_ARRAY_EQ(a, b, 4);
    return DT_TEST_PASSED;
}

/* ── New expect variants ───────────────────────────────────────────── */

DT_TEST("expect_new_macros_pass", DT_RESET_NONE, NULL, test_expect_new) {
    int saved = dt_expect_failure_count;
    dt_expect_failure_count = 0;

    DT_EXPECT_EQ_I64(INT64_C(42), INT64_C(42));
    DT_EXPECT_NE_I64(INT64_C(1), INT64_C(2));
    DT_EXPECT_EQ_U64(UINT64_C(99), UINT64_C(99));
    DT_EXPECT_NE_U64(UINT64_C(1), UINT64_C(2));
    DT_EXPECT_BITS(0xFF, 0xAB, 0xAB);
    int a[] = {1, 2, 3};
    int b[] = {1, 2, 3};
    DT_EXPECT_ARRAY_EQ(a, b, 3);

    DT_ASSERT_EQ(dt_expect_failure_count, 0);
    dt_expect_failure_count = saved;
    return DT_TEST_PASSED;
}

/* ── Additional failure-path helpers ───────────────────────────────── */

static dt_test_result_t _helper_assert_ne_fail(void) {
    DT_ASSERT_NE(5, 5);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_gt_fail(void) {
    DT_ASSERT_GT(3, 10);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_lt_fail(void) {
    DT_ASSERT_LT(10, 3);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_ge_fail(void) {
    DT_ASSERT_GE(3, 10);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_le_fail(void) {
    DT_ASSERT_LE(10, 3);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_true_fail(void) {
    DT_ASSERT_TRUE(0);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_false_fail(void) {
    DT_ASSERT_FALSE(1);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_null_fail(void) {
    int x = 1;
    DT_ASSERT_NULL(&x);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_not_null_fail(void) {
    DT_ASSERT_NOT_NULL(NULL);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_float_near_fail(void) {
    DT_ASSERT_FLOAT_NEAR(1.0, 2.0, 0.01);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_in_range_fail(void) {
    DT_ASSERT_IN_RANGE(100, 1, 10);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_bits_fail(void) {
    DT_ASSERT_BITS(0xFF, 0xAA, 0x55);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_array_eq_fail(void) {
    int a[] = {1, 2, 3};
    int b[] = {1, 9, 3};
    DT_ASSERT_ARRAY_EQ(a, b, 3);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_eq_i64_fail(void) {
    DT_ASSERT_EQ_I64(INT64_C(1), INT64_C(2));
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_eq_u64_fail(void) {
    DT_ASSERT_EQ_U64(UINT64_C(1), UINT64_C(2));
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_mem_eq_fail(void) {
    uint8_t a[] = {1, 2, 3};
    uint8_t b[] = {1, 9, 3};
    DT_ASSERT_MEM_EQ(a, b, 3);
    return DT_TEST_PASSED;
}

static dt_test_result_t _helper_assert_str_contains_fail(void) {
    DT_ASSERT_STR_CONTAINS("hello", "xyz");
    return DT_TEST_PASSED;
}

DT_TEST("fail_ne_returns_failed", DT_RESET_NONE, NULL, test_fail_ne) {
    DT_ASSERT_EQ((long)_helper_assert_ne_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_gt_returns_failed", DT_RESET_NONE, NULL, test_fail_gt) {
    DT_ASSERT_EQ((long)_helper_assert_gt_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_lt_returns_failed", DT_RESET_NONE, NULL, test_fail_lt) {
    DT_ASSERT_EQ((long)_helper_assert_lt_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_ge_returns_failed", DT_RESET_NONE, NULL, test_fail_ge) {
    DT_ASSERT_EQ((long)_helper_assert_ge_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_le_returns_failed", DT_RESET_NONE, NULL, test_fail_le) {
    DT_ASSERT_EQ((long)_helper_assert_le_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_true_returns_failed", DT_RESET_NONE, NULL, test_fail_true) {
    DT_ASSERT_EQ((long)_helper_assert_true_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_false_returns_failed", DT_RESET_NONE, NULL, test_fail_false) {
    DT_ASSERT_EQ((long)_helper_assert_false_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_null_returns_failed", DT_RESET_NONE, NULL, test_fail_null) {
    DT_ASSERT_EQ((long)_helper_assert_null_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_not_null_returns_failed", DT_RESET_NONE, NULL, test_fail_not_null) {
    DT_ASSERT_EQ((long)_helper_assert_not_null_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_float_near_returns_failed", DT_RESET_NONE, NULL, test_fail_float) {
    DT_ASSERT_EQ((long)_helper_assert_float_near_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_in_range_returns_failed", DT_RESET_NONE, NULL, test_fail_in_range) {
    DT_ASSERT_EQ((long)_helper_assert_in_range_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_bits_returns_failed", DT_RESET_NONE, NULL, test_fail_bits) {
    DT_ASSERT_EQ((long)_helper_assert_bits_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_array_eq_returns_failed", DT_RESET_NONE, NULL, test_fail_array_eq) {
    DT_ASSERT_EQ((long)_helper_assert_array_eq_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_eq_i64_returns_failed", DT_RESET_NONE, NULL, test_fail_eq_i64) {
    DT_ASSERT_EQ((long)_helper_assert_eq_i64_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_eq_u64_returns_failed", DT_RESET_NONE, NULL, test_fail_eq_u64) {
    DT_ASSERT_EQ((long)_helper_assert_eq_u64_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_mem_eq_returns_failed", DT_RESET_NONE, NULL, test_fail_mem_eq) {
    DT_ASSERT_EQ((long)_helper_assert_mem_eq_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_TEST("fail_str_contains_returns_failed", DT_RESET_NONE, NULL, test_fail_str_contains) {
    DT_ASSERT_EQ((long)_helper_assert_str_contains_fail(), (long)DT_TEST_FAILED);
    return DT_TEST_PASSED;
}

DT_MAIN()

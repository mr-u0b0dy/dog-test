/**
 * test_framework.c – host-side unit tests for the hiltest framework.
 *
 * Tests the registry, assertions (fatal + non-fatal), runner output,
 * and tag-based filtering without requiring any target hardware.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hiltest/assert.h"
#include "hiltest/test_case.h"

/* ── Registry tests ────────────────────────────────────────────────── */

HT_TEST("registry_count", HT_RESET_NONE, NULL, test_registry_count) {
    /* At least this test itself must be registered. */
    HT_ASSERT_GT((long)ht_registered_test_count(), 0);
    return HT_TEST_PASSED;
}

HT_TEST("registry_lookup", HT_RESET_NONE, NULL, test_registry_lookup) {
    const ht_test_case_t* tc = ht_registered_test_at(0);
    HT_ASSERT_NOT_NULL(tc);
    HT_ASSERT_NOT_NULL(tc->name);
    return HT_TEST_PASSED;
}

HT_TEST("registry_bounds", HT_RESET_NONE, NULL, test_registry_bounds) {
    const ht_test_case_t* tc = ht_registered_test_at(99999);
    HT_ASSERT_NULL(tc);
    return HT_TEST_PASSED;
}

/* ── Assertion macro tests ─────────────────────────────────────────── */

HT_TEST("assert_eq_pass", HT_RESET_NONE, NULL, test_assert_eq_pass) {
    HT_ASSERT_EQ(42, 42);
    return HT_TEST_PASSED;
}

HT_TEST("assert_ne_pass", HT_RESET_NONE, NULL, test_assert_ne_pass) {
    HT_ASSERT_NE(1, 2);
    return HT_TEST_PASSED;
}

HT_TEST("assert_gt_ge_lt_le", HT_RESET_NONE, NULL, test_assert_relational) {
    HT_ASSERT_GT(10, 5);
    HT_ASSERT_GE(10, 10);
    HT_ASSERT_GE(10, 5);
    HT_ASSERT_LT(5, 10);
    HT_ASSERT_LE(10, 10);
    HT_ASSERT_LE(5, 10);
    return HT_TEST_PASSED;
}

HT_TEST("assert_true_false", HT_RESET_NONE, NULL, test_assert_bool) {
    HT_ASSERT_TRUE(1);
    HT_ASSERT_FALSE(0);
    return HT_TEST_PASSED;
}

HT_TEST("assert_null_not_null", HT_RESET_NONE, NULL, test_assert_ptr) {
    int x = 1;
    HT_ASSERT_NOT_NULL(&x);
    HT_ASSERT_NULL(NULL);
    return HT_TEST_PASSED;
}

HT_TEST("assert_str_eq", HT_RESET_NONE, NULL, test_assert_str_eq) {
    HT_ASSERT_STR_EQ("hello", "hello");
    return HT_TEST_PASSED;
}

HT_TEST("assert_str_contains", HT_RESET_NONE, NULL, test_assert_str_contains) {
    HT_ASSERT_STR_CONTAINS("hello world", "world");
    return HT_TEST_PASSED;
}

HT_TEST("assert_eq_u32", HT_RESET_NONE, NULL, test_assert_eq_u32) {
    uint32_t a = 0xDEADBEEF;
    uint32_t b = 0xDEADBEEF;
    HT_ASSERT_EQ_U32(a, b);
    return HT_TEST_PASSED;
}

HT_TEST("assert_float_near", HT_RESET_NONE, NULL, test_assert_float_near) {
    HT_ASSERT_FLOAT_NEAR(3.14, 3.14159, 0.01);
    return HT_TEST_PASSED;
}

HT_TEST("assert_in_range", HT_RESET_NONE, NULL, test_assert_in_range) {
    HT_ASSERT_IN_RANGE(5, 1, 10);
    HT_ASSERT_IN_RANGE(1, 1, 10);
    HT_ASSERT_IN_RANGE(10, 1, 10);
    return HT_TEST_PASSED;
}

HT_TEST("assert_mem_eq", HT_RESET_NONE, NULL, test_assert_mem_eq) {
    uint8_t a[] = {0x01, 0x02, 0x03};
    uint8_t b[] = {0x01, 0x02, 0x03};
    HT_ASSERT_MEM_EQ(a, b, sizeof(a));
    return HT_TEST_PASSED;
}

HT_TEST("assert_msg", HT_RESET_NONE, NULL, test_assert_msg) {
    HT_ASSERT_MSG(1 == 1, "one should equal one");
    return HT_TEST_PASSED;
}

/* ── Non-fatal HT_EXPECT_* tests ──────────────────────────────────── */

HT_TEST("expect_all_pass", HT_RESET_NONE, NULL, test_expect_all_pass) {
    HT_EXPECT_TRUE(1);
    HT_EXPECT_FALSE(0);
    HT_EXPECT_EQ(42, 42);
    HT_EXPECT_NE(1, 2);
    HT_EXPECT_GT(10, 5);
    HT_EXPECT_GE(10, 10);
    HT_EXPECT_GE(10, 5);
    HT_EXPECT_LT(5, 10);
    HT_EXPECT_LE(10, 10);
    HT_EXPECT_LE(5, 10);
    HT_EXPECT_NULL(NULL);
    int x = 1;
    HT_EXPECT_NOT_NULL(&x);
    HT_EXPECT_STR_EQ("a", "a");
    HT_EXPECT_STR_CONTAINS("hello world", "world");
    HT_EXPECT_MSG(1, "ok");
    HT_EXPECT_EQ_U32(0xDEADBEEF, 0xDEADBEEF);
    HT_EXPECT_FLOAT_NEAR(3.14, 3.14159, 0.01);
    HT_EXPECT_IN_RANGE(5, 1, 10);
    uint8_t a1[] = {1, 2, 3};
    uint8_t b1[] = {1, 2, 3};
    HT_EXPECT_MEM_EQ(a1, b1, sizeof(a1));
    return HT_TEST_PASSED;
}

/* ── Tag tests ─────────────────────────────────────────────────────── */

HT_TEST_T("tagged_uart", HT_RESET_NONE, NULL, "uart,smoke", test_tagged_uart) {
    HT_ASSERT_TRUE(1);
    return HT_TEST_PASSED;
}

HT_TEST_T("tagged_spi", HT_RESET_NONE, NULL, "spi,smoke", test_tagged_spi) {
    HT_ASSERT_TRUE(1);
    return HT_TEST_PASSED;
}

/* ── Skip test ─────────────────────────────────────────────────────── */

HT_TEST("skip_example", HT_RESET_NONE, NULL, test_skip_example) {
    HT_SKIP("demonstration of skip");
    return HT_TEST_PASSED;
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

HT_TEST_F("fixture_test", HT_RESET_NONE, NULL,
           fixture_setup, fixture_teardown, test_fixture) {
    HT_ASSERT_EQ(100, fixture_counter);
    return HT_TEST_PASSED;
}

/* ── Fixture + tags (HT_TEST_FT) test ─────────────────────────────── */

HT_TEST_FT("fixture_tagged", HT_RESET_NONE, NULL,
            fixture_setup, fixture_teardown, "unit,fixture", test_fixture_tagged) {
    HT_ASSERT_EQ(100, fixture_counter);
    return HT_TEST_PASSED;
}

/* ── Failure-path tests ────────────────────────────────────────────── */

/*
 * These use helper functions to exercise failing assertions without
 * failing the entire test suite.  The helpers are NOT registered as
 * HT_TESTs — they are called directly and their return values inspected.
 */

static ht_test_result_t _helper_assert_eq_fail(void) {
    HT_ASSERT_EQ(1, 2);
    return HT_TEST_PASSED;
}

static ht_test_result_t _helper_assert_str_eq_fail(void) {
    HT_ASSERT_STR_EQ("hello", "world");
    return HT_TEST_PASSED;
}

static ht_test_result_t _helper_skip(void) {
    HT_SKIP("intentional skip");
    return HT_TEST_PASSED;
}

HT_TEST("fail_assert_eq_returns_failed", HT_RESET_NONE, NULL, test_fail_assert_eq) {
    ht_test_result_t r = _helper_assert_eq_fail();
    HT_ASSERT_EQ((long)r, (long)HT_TEST_FAILED);
    return HT_TEST_PASSED;
}

HT_TEST("fail_assert_str_eq_returns_failed", HT_RESET_NONE, NULL, test_fail_assert_str_eq) {
    ht_test_result_t r = _helper_assert_str_eq_fail();
    HT_ASSERT_EQ((long)r, (long)HT_TEST_FAILED);
    return HT_TEST_PASSED;
}

HT_TEST("skip_returns_skipped", HT_RESET_NONE, NULL, test_skip_returns_skipped) {
    ht_test_result_t r = _helper_skip();
    HT_ASSERT_EQ((long)r, (long)HT_TEST_SKIPPED);
    return HT_TEST_PASSED;
}

/* ── Expect promotion test ─────────────────────────────────────────── */

HT_TEST("expect_promotion", HT_RESET_NONE, NULL, test_expect_promotion) {
    /* Save and reset the global counter so we can test independently. */
    int saved = ht_expect_failure_count;
    ht_expect_failure_count = 0;

    /* A failing HT_EXPECT (run inline, not via helper — it won't return). */
    HT_EXPECT_EQ(1, 2);   /* should fail non-fatally */
    HT_ASSERT_EQ(ht_expect_failure_count, 1);

    /* Restore so this test itself doesn't get promoted as failed. */
    ht_expect_failure_count = saved;
    return HT_TEST_PASSED;
}

/* ── Filtered / tag-based runner tests ─────────────────────────────── */

HT_TEST("run_filtered_match", HT_RESET_NONE, NULL, test_run_filtered) {
    /* Filter for a name that matches a known test but NOT this one. */
    int failures = ht_run_tests_filtered("tagged_uart");
    HT_ASSERT_EQ(failures, 0);
    return HT_TEST_PASSED;
}

HT_TEST("run_filtered_nomatch", HT_RESET_NONE, NULL, test_run_filtered_unknown) {
    /* No registered test should match this filter. 0 ran → 0 failed. */
    int failures = ht_run_tests_filtered("_xyzzy_nonexistent_filter_");
    HT_ASSERT_EQ(failures, 0);
    return HT_TEST_PASSED;
}

HT_TEST("run_by_tag_smoke", HT_RESET_NONE, NULL, test_run_by_tag_smoke) {
    /* We have tests tagged "smoke" (tagged_uart, tagged_spi). */
    int failures = ht_run_tests_by_tag("smoke");
    HT_ASSERT_EQ(failures, 0);
    return HT_TEST_PASSED;
}

HT_TEST("run_by_tag_nomatch", HT_RESET_NONE, NULL, test_run_by_tag_unknown) {
    int failures = ht_run_tests_by_tag("_xyzzy_nonexistent_tag_");
    HT_ASSERT_EQ(failures, 0);
    return HT_TEST_PASSED;
}

HT_MAIN()

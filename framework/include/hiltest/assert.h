/* SPDX-License-Identifier: Apache-2.0 */
/* Copyright 2026 dog-test contributors */

#ifndef HILTEST_ASSERT_H
#define HILTEST_ASSERT_H

/* ── Version information ───────────────────────────────────────────── */
#define HILTEST_VERSION_MAJOR 0
#define HILTEST_VERSION_MINOR 1
#define HILTEST_VERSION_PATCH 0
#define HILTEST_VERSION_STRING "0.1.0"

#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include "hiltest/test_case.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Low-level failure reporters.
 * ht_fail_impl  – plain expression + message (used by boolean assertions).
 * ht_fail_cmp   – comparison failure with stringified actual/expected values.
 * ht_fail_str   – string comparison failure with actual string values.
 */
void ht_fail_impl(const char* file, uint32_t line, const char* expr, const char* message);
void ht_fail_cmp(const char* file, uint32_t line,
                 const char* expr,
                 long expected, long actual,
                 const char* message);
void ht_fail_str(const char* file, uint32_t line,
                 const char* expr,
                 const char* expected, const char* actual,
                 const char* message);

/** Float comparison failure reporter with expected, actual, and tolerance. */
void ht_fail_float(const char* file, uint32_t line,
                   const char* expr,
                   double expected, double actual, double tolerance,
                   const char* message);

/** 64-bit signed comparison failure reporter. */
void ht_fail_cmp64(const char* file, uint32_t line,
                   const char* expr,
                   int64_t expected, int64_t actual,
                   const char* message);

/** 64-bit unsigned comparison failure reporter. */
void ht_fail_cmp_u64(const char* file, uint32_t line,
                     const char* expr,
                     uint64_t expected, uint64_t actual,
                     const char* message);

/** Skip-reason recorder (does not pollute fail state). */
void ht_skip_impl(const char* reason);

/**
 * Non-fatal expectation support.
 * When an HT_EXPECT_* macro fails, it records the failure here instead of
 * returning HT_TEST_FAILED.  The runner checks this flag after the test
 * function returns HT_TEST_PASSED and downgrades to HT_TEST_FAILED.
 */
extern int ht_expect_failure_count;

/* ── Boolean assertions (fatal) ────────────────────────────────────── */

#define HT_ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, "expected true"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_FALSE(expr) \
    do { \
        if ((expr)) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, "expected false"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── Comparison assertions (fatal, print expected vs actual) ───────── */

#define HT_ASSERT_EQ(expected, actual) \
    do { \
        long _ht_e = (long)(expected); \
        long _ht_a = (long)(actual); \
        if (_ht_e != _ht_a) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                        "expected " #expected " == " #actual); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_NE(expected, actual) \
    do { \
        long _ht_e = (long)(expected); \
        long _ht_a = (long)(actual); \
        if (_ht_e == _ht_a) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                        "expected " #expected " != " #actual); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_GT(a, b) \
    do { \
        long _ht_a = (long)(a); \
        long _ht_b = (long)(b); \
        if (!(_ht_a > _ht_b)) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " > " #b, _ht_a, _ht_b, \
                        "expected " #a " > " #b); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_GE(a, b) \
    do { \
        long _ht_a = (long)(a); \
        long _ht_b = (long)(b); \
        if (!(_ht_a >= _ht_b)) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " >= " #b, _ht_a, _ht_b, \
                        "expected " #a " >= " #b); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_LT(a, b) \
    do { \
        long _ht_a = (long)(a); \
        long _ht_b = (long)(b); \
        if (!(_ht_a < _ht_b)) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " < " #b, _ht_a, _ht_b, \
                        "expected " #a " < " #b); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_LE(a, b) \
    do { \
        long _ht_a = (long)(a); \
        long _ht_b = (long)(b); \
        if (!(_ht_a <= _ht_b)) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " <= " #b, _ht_a, _ht_b, \
                        "expected " #a " <= " #b); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── Typed comparison assertions (unsigned 32-bit) ─────────────────── */

#define HT_ASSERT_EQ_U32(expected, actual) \
    do { \
        uint32_t _ht_e = (uint32_t)(expected); \
        uint32_t _ht_a = (uint32_t)(actual); \
        if (_ht_e != _ht_a) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, \
                        (long)_ht_e, (long)_ht_a, \
                        "expected " #expected " == " #actual " (u32)"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── Floating-point assertion ──────────────────────────────────────── */

#define HT_ASSERT_FLOAT_NEAR(expected, actual, epsilon) \
    do { \
        double _ht_e = (double)(expected); \
        double _ht_a = (double)(actual); \
        double _ht_eps = (double)(epsilon); \
        if (fabs(_ht_e - _ht_a) > _ht_eps) { \
            ht_fail_float(__FILE__, (uint32_t)__LINE__, \
                          #actual, _ht_e, _ht_a, _ht_eps, \
                          "float mismatch: |" #expected " - " #actual "| > " #epsilon); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── 64-bit assertions ─────────────────────────────────────────────── */

#define HT_ASSERT_EQ_I64(expected, actual) \
    do { \
        int64_t _ht_e = (int64_t)(expected); \
        int64_t _ht_a = (int64_t)(actual); \
        if (_ht_e != _ht_a) { \
            ht_fail_cmp64(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                          "expected " #expected " == " #actual " (i64)"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_NE_I64(expected, actual) \
    do { \
        int64_t _ht_e = (int64_t)(expected); \
        int64_t _ht_a = (int64_t)(actual); \
        if (_ht_e == _ht_a) { \
            ht_fail_cmp64(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                          "expected " #expected " != " #actual " (i64)"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_EQ_U64(expected, actual) \
    do { \
        uint64_t _ht_e = (uint64_t)(expected); \
        uint64_t _ht_a = (uint64_t)(actual); \
        if (_ht_e != _ht_a) { \
            ht_fail_cmp_u64(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                            "expected " #expected " == " #actual " (u64)"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_NE_U64(expected, actual) \
    do { \
        uint64_t _ht_e = (uint64_t)(expected); \
        uint64_t _ht_a = (uint64_t)(actual); \
        if (_ht_e == _ht_a) { \
            ht_fail_cmp_u64(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                            "expected " #expected " != " #actual " (u64)"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── Bitmask assertion ─────────────────────────────────────────────── */

#define HT_ASSERT_BITS(mask, expected, actual) \
    do { \
        unsigned long _ht_m = (unsigned long)(mask); \
        unsigned long _ht_e = (unsigned long)(expected) & _ht_m; \
        unsigned long _ht_a = (unsigned long)(actual) & _ht_m; \
        if (_ht_e != _ht_a) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, \
                        "(" #actual " & " #mask ")", \
                        (long)_ht_e, (long)_ht_a, \
                        "bitmask mismatch for mask " #mask); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── Array assertion (reports index of first difference) ───────────── */

#define HT_ASSERT_ARRAY_EQ(expected, actual, count) \
    do { \
        for (size_t _ht_i = 0; _ht_i < (size_t)(count); _ht_i++) { \
            if ((expected)[_ht_i] != (actual)[_ht_i]) { \
                ht_fail_cmp(__FILE__, (uint32_t)__LINE__, \
                            #actual "[" #count "]", \
                            (long)(expected)[_ht_i], (long)(actual)[_ht_i], \
                            "array mismatch at index"); \
                return HT_TEST_FAILED; \
            } \
        } \
    } while (0)

/* ── Range assertion ───────────────────────────────────────────────── */

#define HT_ASSERT_IN_RANGE(val, lo, hi) \
    do { \
        long _ht_v = (long)(val); \
        long _ht_lo = (long)(lo); \
        long _ht_hi = (long)(hi); \
        if (_ht_v < _ht_lo || _ht_v > _ht_hi) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #val, _ht_lo, _ht_v, \
                        "expected " #lo " <= " #val " <= " #hi); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── Memory assertion ──────────────────────────────────────────────── */

#define HT_ASSERT_MEM_EQ(expected, actual, len) \
    do { \
        if (memcmp((expected), (actual), (len)) != 0) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, \
                         #actual, "memory mismatch over " #len " bytes"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── Pointer assertions ────────────────────────────────────────────── */

#define HT_ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != 0) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #ptr, "expected NULL"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == 0) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #ptr, "expected non-NULL"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── String assertions ─────────────────────────────────────────────── */

#define HT_ASSERT_STR_EQ(expected, actual) \
    do { \
        const char* _ht_e = (expected); \
        const char* _ht_a = (actual); \
        if (_ht_e == 0 || _ht_a == 0 || strcmp(_ht_e, _ht_a) != 0) { \
            ht_fail_str(__FILE__, (uint32_t)__LINE__, #actual, \
                        _ht_e ? _ht_e : "(null)", \
                        _ht_a ? _ht_a : "(null)", \
                        "string mismatch"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_STR_CONTAINS(haystack, needle) \
    do { \
        const char* _ht_h = (haystack); \
        const char* _ht_n = (needle); \
        if (_ht_h == 0 || _ht_n == 0 || strstr(_ht_h, _ht_n) == 0) { \
            ht_fail_str(__FILE__, (uint32_t)__LINE__, #haystack, \
                        _ht_n ? _ht_n : "(null)", \
                        _ht_h ? _ht_h : "(null)", \
                        "string does not contain expected substring"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── Custom-message assertion ──────────────────────────────────────── */

#define HT_ASSERT_MSG(expr, message) \
    do { \
        if (!(expr)) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, (message)); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

/* ── Skip macro (returns HT_TEST_SKIPPED from the test function) ─── */

#define HT_SKIP(reason) \
    do { \
        ht_skip_impl((reason)); \
        return HT_TEST_SKIPPED; \
    } while (0)

/* ═══════════════════════════════════════════════════════════════════
 *  Non-fatal HT_EXPECT_* variants
 *
 *  These record a failure (incrementing ht_expect_failure_count) but
 *  do NOT return from the test function, allowing subsequent checks
 *  to execute.  The runner promotes the test to HT_TEST_FAILED after
 *  it returns if any expectations failed.
 * ═══════════════════════════════════════════════════════════════════ */

#define HT_EXPECT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, "expected true"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_FALSE(expr) \
    do { \
        if ((expr)) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, "expected false"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_EQ(expected, actual) \
    do { \
        long _ht_e = (long)(expected); \
        long _ht_a = (long)(actual); \
        if (_ht_e != _ht_a) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                        "expected " #expected " == " #actual); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_NE(expected, actual) \
    do { \
        long _ht_e = (long)(expected); \
        long _ht_a = (long)(actual); \
        if (_ht_e == _ht_a) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                        "expected " #expected " != " #actual); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_GT(a, b) \
    do { \
        long _ht_a = (long)(a); \
        long _ht_b = (long)(b); \
        if (!(_ht_a > _ht_b)) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " > " #b, _ht_a, _ht_b, \
                        "expected " #a " > " #b); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_NULL(ptr) \
    do { \
        if ((ptr) != 0) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #ptr, "expected NULL"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == 0) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #ptr, "expected non-NULL"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_STR_EQ(expected, actual) \
    do { \
        const char* _ht_e = (expected); \
        const char* _ht_a = (actual); \
        if (_ht_e == 0 || _ht_a == 0 || strcmp(_ht_e, _ht_a) != 0) { \
            ht_fail_str(__FILE__, (uint32_t)__LINE__, #actual, \
                        _ht_e ? _ht_e : "(null)", \
                        _ht_a ? _ht_a : "(null)", \
                        "string mismatch"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_MSG(expr, message) \
    do { \
        if (!(expr)) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, (message)); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_GE(a, b) \
    do { \
        long _ht_a = (long)(a); \
        long _ht_b = (long)(b); \
        if (!(_ht_a >= _ht_b)) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " >= " #b, _ht_a, _ht_b, \
                        "expected " #a " >= " #b); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_LT(a, b) \
    do { \
        long _ht_a = (long)(a); \
        long _ht_b = (long)(b); \
        if (!(_ht_a < _ht_b)) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " < " #b, _ht_a, _ht_b, \
                        "expected " #a " < " #b); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_LE(a, b) \
    do { \
        long _ht_a = (long)(a); \
        long _ht_b = (long)(b); \
        if (!(_ht_a <= _ht_b)) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " <= " #b, _ht_a, _ht_b, \
                        "expected " #a " <= " #b); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_EQ_U32(expected, actual) \
    do { \
        uint32_t _ht_e = (uint32_t)(expected); \
        uint32_t _ht_a = (uint32_t)(actual); \
        if (_ht_e != _ht_a) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, \
                        (long)_ht_e, (long)_ht_a, \
                        "expected " #expected " == " #actual " (u32)"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_FLOAT_NEAR(expected, actual, epsilon) \
    do { \
        double _ht_e = (double)(expected); \
        double _ht_a = (double)(actual); \
        double _ht_eps = (double)(epsilon); \
        if (fabs(_ht_e - _ht_a) > _ht_eps) { \
            ht_fail_float(__FILE__, (uint32_t)__LINE__, \
                          #actual, _ht_e, _ht_a, _ht_eps, \
                          "float mismatch: |" #expected " - " #actual "| > " #epsilon); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_IN_RANGE(val, lo, hi) \
    do { \
        long _ht_v = (long)(val); \
        long _ht_lo = (long)(lo); \
        long _ht_hi = (long)(hi); \
        if (_ht_v < _ht_lo || _ht_v > _ht_hi) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, #val, _ht_lo, _ht_v, \
                        "expected " #lo " <= " #val " <= " #hi); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_MEM_EQ(expected, actual, len) \
    do { \
        if (memcmp((expected), (actual), (len)) != 0) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, \
                         #actual, "memory mismatch over " #len " bytes"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_STR_CONTAINS(haystack, needle) \
    do { \
        const char* _ht_h = (haystack); \
        const char* _ht_n = (needle); \
        if (_ht_h == 0 || _ht_n == 0 || strstr(_ht_h, _ht_n) == 0) { \
            ht_fail_str(__FILE__, (uint32_t)__LINE__, #haystack, \
                        _ht_n ? _ht_n : "(null)", \
                        _ht_h ? _ht_h : "(null)", \
                        "string does not contain expected substring"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

/* ── 64-bit non-fatal assertions ───────────────────────────────────── */

#define HT_EXPECT_EQ_I64(expected, actual) \
    do { \
        int64_t _ht_e = (int64_t)(expected); \
        int64_t _ht_a = (int64_t)(actual); \
        if (_ht_e != _ht_a) { \
            ht_fail_cmp64(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                          "expected " #expected " == " #actual " (i64)"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_NE_I64(expected, actual) \
    do { \
        int64_t _ht_e = (int64_t)(expected); \
        int64_t _ht_a = (int64_t)(actual); \
        if (_ht_e == _ht_a) { \
            ht_fail_cmp64(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                          "expected " #expected " != " #actual " (i64)"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_EQ_U64(expected, actual) \
    do { \
        uint64_t _ht_e = (uint64_t)(expected); \
        uint64_t _ht_a = (uint64_t)(actual); \
        if (_ht_e != _ht_a) { \
            ht_fail_cmp_u64(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                            "expected " #expected " == " #actual " (u64)"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_NE_U64(expected, actual) \
    do { \
        uint64_t _ht_e = (uint64_t)(expected); \
        uint64_t _ht_a = (uint64_t)(actual); \
        if (_ht_e == _ht_a) { \
            ht_fail_cmp_u64(__FILE__, (uint32_t)__LINE__, #actual, _ht_e, _ht_a, \
                            "expected " #expected " != " #actual " (u64)"); \
            ht_expect_failure_count++; \
        } \
    } while (0)

/* ── Bitmask / array non-fatal assertions ──────────────────────────── */

#define HT_EXPECT_BITS(mask, expected, actual) \
    do { \
        unsigned long _ht_m = (unsigned long)(mask); \
        unsigned long _ht_e = (unsigned long)(expected) & _ht_m; \
        unsigned long _ht_a = (unsigned long)(actual) & _ht_m; \
        if (_ht_e != _ht_a) { \
            ht_fail_cmp(__FILE__, (uint32_t)__LINE__, \
                        "(" #actual " & " #mask ")", \
                        (long)_ht_e, (long)_ht_a, \
                        "bitmask mismatch for mask " #mask); \
            ht_expect_failure_count++; \
        } \
    } while (0)

#define HT_EXPECT_ARRAY_EQ(expected, actual, count) \
    do { \
        for (size_t _ht_i = 0; _ht_i < (size_t)(count); _ht_i++) { \
            if ((expected)[_ht_i] != (actual)[_ht_i]) { \
                ht_fail_cmp(__FILE__, (uint32_t)__LINE__, \
                            #actual "[" #count "]", \
                            (long)(expected)[_ht_i], (long)(actual)[_ht_i], \
                            "array mismatch at index"); \
                ht_expect_failure_count++; \
                break; \
            } \
        } \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif

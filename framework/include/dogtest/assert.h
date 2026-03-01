/* SPDX-License-Identifier: Apache-2.0 */
/* Copyright 2026 dog-test contributors */

#ifndef DOGTEST_ASSERT_H
#define DOGTEST_ASSERT_H

/* ── Version information ───────────────────────────────────────────── */
#define DOGTEST_VERSION_MAJOR 0
#define DOGTEST_VERSION_MINOR 1
#define DOGTEST_VERSION_PATCH 0
#define DOGTEST_VERSION_STRING "0.1.0"

#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include "dogtest/test_case.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Low-level failure reporters.
 * dt_fail_impl  – plain expression + message (used by boolean assertions).
 * dt_fail_cmp   – comparison failure with stringified actual/expected values.
 * dt_fail_str   – string comparison failure with actual string values.
 */
void dt_fail_impl(const char* file, uint32_t line, const char* expr, const char* message);
void dt_fail_cmp(const char* file, uint32_t line,
                 const char* expr,
                 long expected, long actual,
                 const char* message);
void dt_fail_str(const char* file, uint32_t line,
                 const char* expr,
                 const char* expected, const char* actual,
                 const char* message);

/** Float comparison failure reporter with expected, actual, and tolerance. */
void dt_fail_float(const char* file, uint32_t line,
                   const char* expr,
                   double expected, double actual, double tolerance,
                   const char* message);

/** 64-bit signed comparison failure reporter. */
void dt_fail_cmp64(const char* file, uint32_t line,
                   const char* expr,
                   int64_t expected, int64_t actual,
                   const char* message);

/** 64-bit unsigned comparison failure reporter. */
void dt_fail_cmp_u64(const char* file, uint32_t line,
                     const char* expr,
                     uint64_t expected, uint64_t actual,
                     const char* message);

/** Skip-reason recorder (does not pollute fail state). */
void dt_skip_impl(const char* reason);

/**
 * Non-fatal expectation support.
 * When an DT_EXPECT_* macro fails, it records the failure here instead of
 * returning DT_TEST_FAILED.  The runner checks this flag after the test
 * function returns DT_TEST_PASSED and downgrades to DT_TEST_FAILED.
 */
extern int dt_expect_failure_count;

/* ── Boolean assertions (fatal) ────────────────────────────────────── */

#define DT_ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, "expected true"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_FALSE(expr) \
    do { \
        if ((expr)) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, "expected false"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── Comparison assertions (fatal, print expected vs actual) ───────── */

#define DT_ASSERT_EQ(expected, actual) \
    do { \
        long _dt_e = (long)(expected); \
        long _dt_a = (long)(actual); \
        if (_dt_e != _dt_a) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                        "expected " #expected " == " #actual); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_NE(expected, actual) \
    do { \
        long _dt_e = (long)(expected); \
        long _dt_a = (long)(actual); \
        if (_dt_e == _dt_a) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                        "expected " #expected " != " #actual); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_GT(a, b) \
    do { \
        long _dt_a = (long)(a); \
        long _dt_b = (long)(b); \
        if (!(_dt_a > _dt_b)) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " > " #b, _dt_a, _dt_b, \
                        "expected " #a " > " #b); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_GE(a, b) \
    do { \
        long _dt_a = (long)(a); \
        long _dt_b = (long)(b); \
        if (!(_dt_a >= _dt_b)) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " >= " #b, _dt_a, _dt_b, \
                        "expected " #a " >= " #b); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_LT(a, b) \
    do { \
        long _dt_a = (long)(a); \
        long _dt_b = (long)(b); \
        if (!(_dt_a < _dt_b)) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " < " #b, _dt_a, _dt_b, \
                        "expected " #a " < " #b); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_LE(a, b) \
    do { \
        long _dt_a = (long)(a); \
        long _dt_b = (long)(b); \
        if (!(_dt_a <= _dt_b)) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " <= " #b, _dt_a, _dt_b, \
                        "expected " #a " <= " #b); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── Typed comparison assertions (unsigned 32-bit) ─────────────────── */

#define DT_ASSERT_EQ_U32(expected, actual) \
    do { \
        uint32_t _dt_e = (uint32_t)(expected); \
        uint32_t _dt_a = (uint32_t)(actual); \
        if (_dt_e != _dt_a) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, \
                        (long)_dt_e, (long)_dt_a, \
                        "expected " #expected " == " #actual " (u32)"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── Floating-point assertion ──────────────────────────────────────── */

#define DT_ASSERT_FLOAT_NEAR(expected, actual, epsilon) \
    do { \
        double _dt_e = (double)(expected); \
        double _dt_a = (double)(actual); \
        double _dt_eps = (double)(epsilon); \
        if (fabs(_dt_e - _dt_a) > _dt_eps) { \
            dt_fail_float(__FILE__, (uint32_t)__LINE__, \
                          #actual, _dt_e, _dt_a, _dt_eps, \
                          "float mismatch: |" #expected " - " #actual "| > " #epsilon); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── 64-bit assertions ─────────────────────────────────────────────── */

#define DT_ASSERT_EQ_I64(expected, actual) \
    do { \
        int64_t _dt_e = (int64_t)(expected); \
        int64_t _dt_a = (int64_t)(actual); \
        if (_dt_e != _dt_a) { \
            dt_fail_cmp64(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                          "expected " #expected " == " #actual " (i64)"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_NE_I64(expected, actual) \
    do { \
        int64_t _dt_e = (int64_t)(expected); \
        int64_t _dt_a = (int64_t)(actual); \
        if (_dt_e == _dt_a) { \
            dt_fail_cmp64(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                          "expected " #expected " != " #actual " (i64)"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_EQ_U64(expected, actual) \
    do { \
        uint64_t _dt_e = (uint64_t)(expected); \
        uint64_t _dt_a = (uint64_t)(actual); \
        if (_dt_e != _dt_a) { \
            dt_fail_cmp_u64(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                            "expected " #expected " == " #actual " (u64)"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_NE_U64(expected, actual) \
    do { \
        uint64_t _dt_e = (uint64_t)(expected); \
        uint64_t _dt_a = (uint64_t)(actual); \
        if (_dt_e == _dt_a) { \
            dt_fail_cmp_u64(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                            "expected " #expected " != " #actual " (u64)"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── Bitmask assertion ─────────────────────────────────────────────── */

#define DT_ASSERT_BITS(mask, expected, actual) \
    do { \
        unsigned long _dt_m = (unsigned long)(mask); \
        unsigned long _dt_e = (unsigned long)(expected) & _dt_m; \
        unsigned long _dt_a = (unsigned long)(actual) & _dt_m; \
        if (_dt_e != _dt_a) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, \
                        "(" #actual " & " #mask ")", \
                        (long)_dt_e, (long)_dt_a, \
                        "bitmask mismatch for mask " #mask); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── Array assertion (reports index of first difference) ───────────── */

#define DT_ASSERT_ARRAY_EQ(expected, actual, count) \
    do { \
        for (size_t _dt_i = 0; _dt_i < (size_t)(count); _dt_i++) { \
            if ((expected)[_dt_i] != (actual)[_dt_i]) { \
                dt_fail_cmp(__FILE__, (uint32_t)__LINE__, \
                            #actual "[" #count "]", \
                            (long)(expected)[_dt_i], (long)(actual)[_dt_i], \
                            "array mismatch at index"); \
                return DT_TEST_FAILED; \
            } \
        } \
    } while (0)

/* ── Range assertion ───────────────────────────────────────────────── */

#define DT_ASSERT_IN_RANGE(val, lo, hi) \
    do { \
        long _dt_v = (long)(val); \
        long _dt_lo = (long)(lo); \
        long _dt_hi = (long)(hi); \
        if (_dt_v < _dt_lo || _dt_v > _dt_hi) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #val, _dt_lo, _dt_v, \
                        "expected " #lo " <= " #val " <= " #hi); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── Memory assertion ──────────────────────────────────────────────── */

#define DT_ASSERT_MEM_EQ(expected, actual, len) \
    do { \
        if (memcmp((expected), (actual), (len)) != 0) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, \
                         #actual, "memory mismatch over " #len " bytes"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── Pointer assertions ────────────────────────────────────────────── */

#define DT_ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != 0) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, #ptr, "expected NULL"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == 0) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, #ptr, "expected non-NULL"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── String assertions ─────────────────────────────────────────────── */

#define DT_ASSERT_STR_EQ(expected, actual) \
    do { \
        const char* _dt_e = (expected); \
        const char* _dt_a = (actual); \
        if (_dt_e == 0 || _dt_a == 0 || strcmp(_dt_e, _dt_a) != 0) { \
            dt_fail_str(__FILE__, (uint32_t)__LINE__, #actual, \
                        _dt_e ? _dt_e : "(null)", \
                        _dt_a ? _dt_a : "(null)", \
                        "string mismatch"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

#define DT_ASSERT_STR_CONTAINS(haystack, needle) \
    do { \
        const char* _dt_h = (haystack); \
        const char* _dt_n = (needle); \
        if (_dt_h == 0 || _dt_n == 0 || strstr(_dt_h, _dt_n) == 0) { \
            dt_fail_str(__FILE__, (uint32_t)__LINE__, #haystack, \
                        _dt_n ? _dt_n : "(null)", \
                        _dt_h ? _dt_h : "(null)", \
                        "string does not contain expected substring"); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── Custom-message assertion ──────────────────────────────────────── */

#define DT_ASSERT_MSG(expr, message) \
    do { \
        if (!(expr)) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, (message)); \
            return DT_TEST_FAILED; \
        } \
    } while (0)

/* ── Skip macro (returns DT_TEST_SKIPPED from the test function) ─── */

#define DT_SKIP(reason) \
    do { \
        dt_skip_impl((reason)); \
        return DT_TEST_SKIPPED; \
    } while (0)

/* ═══════════════════════════════════════════════════════════════════
 *  Non-fatal DT_EXPECT_* variants
 *
 *  These record a failure (incrementing dt_expect_failure_count) but
 *  do NOT return from the test function, allowing subsequent checks
 *  to execute.  The runner promotes the test to DT_TEST_FAILED after
 *  it returns if any expectations failed.
 * ═══════════════════════════════════════════════════════════════════ */

#define DT_EXPECT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, "expected true"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_FALSE(expr) \
    do { \
        if ((expr)) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, "expected false"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_EQ(expected, actual) \
    do { \
        long _dt_e = (long)(expected); \
        long _dt_a = (long)(actual); \
        if (_dt_e != _dt_a) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                        "expected " #expected " == " #actual); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_NE(expected, actual) \
    do { \
        long _dt_e = (long)(expected); \
        long _dt_a = (long)(actual); \
        if (_dt_e == _dt_a) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                        "expected " #expected " != " #actual); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_GT(a, b) \
    do { \
        long _dt_a = (long)(a); \
        long _dt_b = (long)(b); \
        if (!(_dt_a > _dt_b)) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " > " #b, _dt_a, _dt_b, \
                        "expected " #a " > " #b); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_NULL(ptr) \
    do { \
        if ((ptr) != 0) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, #ptr, "expected NULL"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == 0) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, #ptr, "expected non-NULL"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_STR_EQ(expected, actual) \
    do { \
        const char* _dt_e = (expected); \
        const char* _dt_a = (actual); \
        if (_dt_e == 0 || _dt_a == 0 || strcmp(_dt_e, _dt_a) != 0) { \
            dt_fail_str(__FILE__, (uint32_t)__LINE__, #actual, \
                        _dt_e ? _dt_e : "(null)", \
                        _dt_a ? _dt_a : "(null)", \
                        "string mismatch"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_MSG(expr, message) \
    do { \
        if (!(expr)) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, #expr, (message)); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_GE(a, b) \
    do { \
        long _dt_a = (long)(a); \
        long _dt_b = (long)(b); \
        if (!(_dt_a >= _dt_b)) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " >= " #b, _dt_a, _dt_b, \
                        "expected " #a " >= " #b); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_LT(a, b) \
    do { \
        long _dt_a = (long)(a); \
        long _dt_b = (long)(b); \
        if (!(_dt_a < _dt_b)) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " < " #b, _dt_a, _dt_b, \
                        "expected " #a " < " #b); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_LE(a, b) \
    do { \
        long _dt_a = (long)(a); \
        long _dt_b = (long)(b); \
        if (!(_dt_a <= _dt_b)) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #a " <= " #b, _dt_a, _dt_b, \
                        "expected " #a " <= " #b); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_EQ_U32(expected, actual) \
    do { \
        uint32_t _dt_e = (uint32_t)(expected); \
        uint32_t _dt_a = (uint32_t)(actual); \
        if (_dt_e != _dt_a) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #actual, \
                        (long)_dt_e, (long)_dt_a, \
                        "expected " #expected " == " #actual " (u32)"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_FLOAT_NEAR(expected, actual, epsilon) \
    do { \
        double _dt_e = (double)(expected); \
        double _dt_a = (double)(actual); \
        double _dt_eps = (double)(epsilon); \
        if (fabs(_dt_e - _dt_a) > _dt_eps) { \
            dt_fail_float(__FILE__, (uint32_t)__LINE__, \
                          #actual, _dt_e, _dt_a, _dt_eps, \
                          "float mismatch: |" #expected " - " #actual "| > " #epsilon); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_IN_RANGE(val, lo, hi) \
    do { \
        long _dt_v = (long)(val); \
        long _dt_lo = (long)(lo); \
        long _dt_hi = (long)(hi); \
        if (_dt_v < _dt_lo || _dt_v > _dt_hi) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, #val, _dt_lo, _dt_v, \
                        "expected " #lo " <= " #val " <= " #hi); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_MEM_EQ(expected, actual, len) \
    do { \
        if (memcmp((expected), (actual), (len)) != 0) { \
            dt_fail_impl(__FILE__, (uint32_t)__LINE__, \
                         #actual, "memory mismatch over " #len " bytes"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_STR_CONTAINS(haystack, needle) \
    do { \
        const char* _dt_h = (haystack); \
        const char* _dt_n = (needle); \
        if (_dt_h == 0 || _dt_n == 0 || strstr(_dt_h, _dt_n) == 0) { \
            dt_fail_str(__FILE__, (uint32_t)__LINE__, #haystack, \
                        _dt_n ? _dt_n : "(null)", \
                        _dt_h ? _dt_h : "(null)", \
                        "string does not contain expected substring"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

/* ── 64-bit non-fatal assertions ───────────────────────────────────── */

#define DT_EXPECT_EQ_I64(expected, actual) \
    do { \
        int64_t _dt_e = (int64_t)(expected); \
        int64_t _dt_a = (int64_t)(actual); \
        if (_dt_e != _dt_a) { \
            dt_fail_cmp64(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                          "expected " #expected " == " #actual " (i64)"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_NE_I64(expected, actual) \
    do { \
        int64_t _dt_e = (int64_t)(expected); \
        int64_t _dt_a = (int64_t)(actual); \
        if (_dt_e == _dt_a) { \
            dt_fail_cmp64(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                          "expected " #expected " != " #actual " (i64)"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_EQ_U64(expected, actual) \
    do { \
        uint64_t _dt_e = (uint64_t)(expected); \
        uint64_t _dt_a = (uint64_t)(actual); \
        if (_dt_e != _dt_a) { \
            dt_fail_cmp_u64(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                            "expected " #expected " == " #actual " (u64)"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_NE_U64(expected, actual) \
    do { \
        uint64_t _dt_e = (uint64_t)(expected); \
        uint64_t _dt_a = (uint64_t)(actual); \
        if (_dt_e == _dt_a) { \
            dt_fail_cmp_u64(__FILE__, (uint32_t)__LINE__, #actual, _dt_e, _dt_a, \
                            "expected " #expected " != " #actual " (u64)"); \
            dt_expect_failure_count++; \
        } \
    } while (0)

/* ── Bitmask / array non-fatal assertions ──────────────────────────── */

#define DT_EXPECT_BITS(mask, expected, actual) \
    do { \
        unsigned long _dt_m = (unsigned long)(mask); \
        unsigned long _dt_e = (unsigned long)(expected) & _dt_m; \
        unsigned long _dt_a = (unsigned long)(actual) & _dt_m; \
        if (_dt_e != _dt_a) { \
            dt_fail_cmp(__FILE__, (uint32_t)__LINE__, \
                        "(" #actual " & " #mask ")", \
                        (long)_dt_e, (long)_dt_a, \
                        "bitmask mismatch for mask " #mask); \
            dt_expect_failure_count++; \
        } \
    } while (0)

#define DT_EXPECT_ARRAY_EQ(expected, actual, count) \
    do { \
        for (size_t _dt_i = 0; _dt_i < (size_t)(count); _dt_i++) { \
            if ((expected)[_dt_i] != (actual)[_dt_i]) { \
                dt_fail_cmp(__FILE__, (uint32_t)__LINE__, \
                            #actual "[" #count "]", \
                            (long)(expected)[_dt_i], (long)(actual)[_dt_i], \
                            "array mismatch at index"); \
                dt_expect_failure_count++; \
                break; \
            } \
        } \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif

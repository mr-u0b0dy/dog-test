#ifndef HILTEST_ASSERT_H
#define HILTEST_ASSERT_H

#include <stdint.h>
#include "hiltest/test_case.h"

#ifdef __cplusplus
extern "C" {
#endif

void ht_fail_impl(const char* file, uint32_t line, const char* expr, const char* message);

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

#define HT_ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #actual, "expected equality"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#define HT_ASSERT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            ht_fail_impl(__FILE__, (uint32_t)__LINE__, #actual, "expected inequality"); \
            return HT_TEST_FAILED; \
        } \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif

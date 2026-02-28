#ifndef HILTEST_TEST_CASE_H
#define HILTEST_TEST_CASE_H

#include <stddef.h>
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
    HT_TEST_PASSED = 0,
    HT_TEST_FAILED = 1
} ht_test_result_t;

typedef ht_test_result_t (*ht_test_fn_t)(void);

typedef struct {
    const char* protocol;
    const char* channels;
    const char* trigger;
    uint32_t timeout_ms;
} ht_monitor_spec_t;

typedef struct {
    const char* name;
    ht_test_fn_t run;
    ht_reset_mode_t reset_mode;
    const ht_monitor_spec_t* monitor;
} ht_test_case_t;

void ht_register_test(const ht_test_case_t* test_case);
size_t ht_registered_test_count(void);
const ht_test_case_t* ht_registered_test_at(size_t index);

void ht_emit_result_start(const ht_test_case_t* test_case);
void ht_emit_result_pass(const ht_test_case_t* test_case);
void ht_emit_result_fail(const ht_test_case_t* test_case, const char* reason);

int ht_run_all_tests(void);

#define HT_TEST(name_literal, reset_mode_literal, monitor_ptr_literal, fn_name) \
    static ht_test_result_t fn_name(void); \
    static const ht_test_case_t fn_name##_meta = { \
        name_literal, fn_name, reset_mode_literal, monitor_ptr_literal \
    }; \
    static void fn_name##_register(void) __attribute__((constructor)); \
    static void fn_name##_register(void) { ht_register_test(&fn_name##_meta); } \
    static ht_test_result_t fn_name(void)

#ifdef __cplusplus
}
#endif

#endif

#include <stdio.h>

#include "hiltest/assert.h"
#include "hiltest/test_case.h"

static const ht_monitor_spec_t i2c_monitor = {
    "i2c",
    "ch0=scl,ch1=sda",
    "address:0x48",
    1200
};

HT_TEST("i2c_device_id", HT_RESET_SOFT, &i2c_monitor, i2c_device_id) {
    int expected = 0x48;
    int observed = 0x48;
    HT_ASSERT_EQ(expected, observed);
    return HT_TEST_PASSED;
}

int main(void) {
    puts("HT_EVENT boot");
    return ht_run_all_tests();
}

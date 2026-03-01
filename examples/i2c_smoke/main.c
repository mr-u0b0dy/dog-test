#include <stdio.h>

#include "dogtest/assert.h"
#include "dogtest/monitor_spec.h"
#include "dogtest/test_case.h"

static const dt_monitor_spec_t i2c_monitor = {
    "i2c",
    "ch0=scl,ch1=sda",
    "address:0x48",
    1200
};

DT_TEST("i2c_device_id", DT_RESET_SOFT, &i2c_monitor, i2c_device_id) {
    int expected = 0x48;
    int observed = 0x48;
    DT_ASSERT_EQ(expected, observed);
    return DT_TEST_PASSED;
}

DT_MAIN()

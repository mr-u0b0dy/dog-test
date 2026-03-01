#include <stdio.h>

#include "dogtest/assert.h"
#include "dogtest/monitor_spec.h"
#include "dogtest/test_case.h"

static const dt_monitor_spec_t spi_monitor = {
    "spi",
    "ch0=clk,ch1=mosi,ch2=miso,ch3=cs",
    "cs-falling",
    1200
};

DT_TEST("spi_transfer_crc", DT_RESET_HARD, &spi_monitor, spi_transfer_crc) {
    int expected_crc = 0xAB;
    int actual_crc = 0xAB;
    DT_ASSERT_EQ(expected_crc, actual_crc);
    DT_ASSERT_NE(0x00, actual_crc);
    return DT_TEST_PASSED;
}

DT_MAIN()

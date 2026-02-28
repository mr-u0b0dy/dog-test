#include <stdio.h>

#include "hiltest/assert.h"
#include "hiltest/test_case.h"

static const ht_monitor_spec_t spi_monitor = {
    "spi",
    "ch0=clk,ch1=mosi,ch2=miso,ch3=cs",
    "cs-falling",
    1200
};

HT_TEST("spi_transfer_crc", HT_RESET_HARD, &spi_monitor, spi_transfer_crc) {
    int expected_crc = 0xAB;
    int actual_crc = 0xAB;
    HT_ASSERT_EQ(expected_crc, actual_crc);
    HT_ASSERT_NE(0x00, actual_crc);
    return HT_TEST_PASSED;
}

int main(void) {
    puts("HT_EVENT boot");
    return ht_run_all_tests();
}

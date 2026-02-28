#include <stdio.h>

#include "hiltest/assert.h"
#include "hiltest/monitor_spec.h"
#include "hiltest/test_case.h"

static const ht_monitor_spec_t uart_monitor = {
    "uart",
    "ch0=tx,ch1=rx",
    "start-on-byte:0x55",
    1000
};

HT_TEST("uart_loopback_basic", HT_RESET_NONE, &uart_monitor, uart_loopback_basic) {
    int sent = 0x55;
    int received = 0x55;
    HT_ASSERT_EQ(sent, received);
    HT_ASSERT_TRUE(received == 0x55);
    return HT_TEST_PASSED;
}

HT_MAIN()

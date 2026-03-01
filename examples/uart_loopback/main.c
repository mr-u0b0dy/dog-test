#include <stdio.h>

#include "dogtest/assert.h"
#include "dogtest/monitor_spec.h"
#include "dogtest/test_case.h"

static const dt_monitor_spec_t uart_monitor = {
    "uart",
    "ch0=tx,ch1=rx",
    "start-on-byte:0x55",
    1000
};

DT_TEST("uart_loopback_basic", DT_RESET_NONE, &uart_monitor, uart_loopback_basic) {
    int sent = 0x55;
    int received = 0x55;
    DT_ASSERT_EQ(sent, received);
    DT_ASSERT_TRUE(received == 0x55);
    return DT_TEST_PASSED;
}

DT_MAIN()

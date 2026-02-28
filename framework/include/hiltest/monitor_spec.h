#ifndef HILTEST_MONITOR_SPEC_H
#define HILTEST_MONITOR_SPEC_H

#include "hiltest/test_case.h"

#define HT_MONITOR_UART(channels_literal, trigger_literal, timeout_literal) \
    &(ht_monitor_spec_t){ "uart", channels_literal, trigger_literal, timeout_literal }

#define HT_MONITOR_I2C(channels_literal, trigger_literal, timeout_literal) \
    &(ht_monitor_spec_t){ "i2c", channels_literal, trigger_literal, timeout_literal }

#define HT_MONITOR_SPI(channels_literal, trigger_literal, timeout_literal) \
    &(ht_monitor_spec_t){ "spi", channels_literal, trigger_literal, timeout_literal }

#endif

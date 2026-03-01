/* SPDX-License-Identifier: Apache-2.0 */
/* Copyright 2026 dog-test contributors */

#ifndef DOGTEST_MONITOR_SPEC_H
#define DOGTEST_MONITOR_SPEC_H

#include "dogtest/test_case.h"

#define DT_MONITOR_UART(channels_literal, trigger_literal, timeout_literal) \
    &(dt_monitor_spec_t){ "uart", channels_literal, trigger_literal, timeout_literal }

#define DT_MONITOR_I2C(channels_literal, trigger_literal, timeout_literal) \
    &(dt_monitor_spec_t){ "i2c", channels_literal, trigger_literal, timeout_literal }

#define DT_MONITOR_SPI(channels_literal, trigger_literal, timeout_literal) \
    &(dt_monitor_spec_t){ "spi", channels_literal, trigger_literal, timeout_literal }

#endif

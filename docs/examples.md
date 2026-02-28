# Examples

- `examples/uart_loopback`: UART smoke test with `HT_RESET_NONE`
- `examples/i2c_smoke`: I2C smoke test with `HT_RESET_SOFT`
- `examples/spi_transfer`: SPI transfer validation with `HT_RESET_HARD`
- `examples/monitor_spec_uart.json`: host-side monitor spec for Saleae assertion
- `tests/test_plan_smoke.json`: multi-test suite plan with per-test reset policy

Each example emits `HT_EVENT` records that the host runner parses.

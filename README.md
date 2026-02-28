# dog-test: Embedded HIL/IT/ST Test Framework

A toolchain-generic embedded test framework focused on ARM (`arm-none-eabi`) with host orchestration via Python and `pyOCD`.

## Implemented MVP features

1. Macro-based assert checks for embedded C/C++ tests
2. Test runner with IDE integration (VS Code tasks + CTest entry)
3. Build, flash, execute flow on target (`cmake` + backend + serial parser)
4. Saleae Logic Python API integration for UART/I2C/SPI capture and decode
5. Per-test reset mode support (`none`, `soft`, `hard`)
6. Optional test-plan mode to run each test once and reset before the next
7. Future Rust support path via protocol-compatible host runner

## Project layout

- `framework/include/hiltest/`: assert macros, test case types, monitor abstractions
- `framework/src/`: test registry and target runner
- `tools/hil_runner.py`: host-side orchestrator
- `tools/backends/`: flash/reset adapters (`pyOCD` + OpenOCD)
- `tools/backends/lang/`: language adapters (Rust placeholder)
- `tools/logic/`: Saleae capture and protocol assertion modules
- `examples/`: UART/I2C/SPI sample tests
- `.vscode/tasks.json`: configure/build/test task integration

## Quick start

### 1) Install dependencies

```bash
python3 -m pip install -r requirements.txt
```

### 2) Configure + build

```bash
cmake --preset host-debug
cmake --build --preset host-debug
```

### 3) Run sample via host runner

```bash
python3 tools/hil_runner.py \
  --firmware build/host-debug/examples/uart_loopback/uart_loopback \
  --test-name uart_loopback \
  --serial-port /dev/ttyACM0 \
  --reset-mode none
```

### 3b) Non-hardware smoke run (IDE/CI wiring check)

```bash
python3 tools/hil_runner.py \
  --firmware build/host-debug/examples/uart_loopback/uart_loopback \
  --skip-flash \
  --skip-reset \
  --skip-monitor \
  --skip-target-exec
```

### 4) Use monitor spec

```bash
python3 tools/hil_runner.py \
  --firmware build/host-debug/examples/uart_loopback/uart_loopback \
  --serial-port /dev/ttyACM0 \
  --monitor-spec '{"protocol":"uart","channels":"ch0=tx,ch1=rx","trigger":"start-on-byte:0x55","timeout_ms":1000,"expect":{"protocol":"uart","min_items":1}}'
```

### 5) Backend selection

`pyOCD` (default):

```bash
python3 tools/hil_runner.py --backend pyocd --firmware build/host-debug/examples/uart_loopback/uart_loopback
```

OpenOCD:

```bash
python3 tools/hil_runner.py \
  --backend openocd \
  --openocd-interface interface/cmsis-dap.cfg \
  --openocd-target target/stm32f4x.cfg \
  --firmware build/host-debug/examples/uart_loopback/uart_loopback
```

### 6) Run test plan once with reset between tests

```bash
python3 tools/hil_runner.py \
  --test-plan tests/test_plan_smoke.json \
  --reset-between-tests
```

## Writing test cases

```c
#include "hiltest/assert.h"
#include "hiltest/test_case.h"

static const ht_monitor_spec_t my_monitor = {
    "uart", "ch0=tx,ch1=rx", "start-on-byte:0x55", 1000
};

HT_TEST("my_case", HT_RESET_SOFT, &my_monitor, my_case) {
    int expected = 42;
    int observed = 42;
    HT_ASSERT_EQ(expected, observed);
    return HT_TEST_PASSED;
}
```

## VS Code integration

Use tasks:
- `cmake:configure-host`
- `cmake:build-host`
- `ctest:host`

`ctest:host` currently runs in non-hardware smoke mode to validate integration without requiring target hardware attached.

## Notes

- `tools/logic/` captures with Saleae Automation API and decodes table-export rows for UART/I2C/SPI.
- For hardware-less validation, use `--skip-flash --skip-reset --skip-monitor --skip-target-exec`.
- Rust integration is planned by implementing the same `HT_EVENT` protocol from Rust test binaries.

See:
- [Architecture](docs-site/content/en/4.reference/1.architecture.md)
- [HIL Workflow](docs-site/content/en/4.reference/3.hil-workflow.md)
- [Examples](docs-site/content/en/4.reference/2.examples.md)
- [Rust Roadmap](docs-site/content/en/4.reference/4.rust-roadmap.md)

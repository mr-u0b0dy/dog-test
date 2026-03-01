# Contributing to dog-test

Thank you for your interest in improving dog-test! This guide covers the
development workflow, coding conventions, and how to submit changes.

## Development Setup

### Prerequisites

- **CMake ≥ 3.22** and **Ninja** (build system)
- **GCC** or **Clang** (host tests) — optionally `arm-none-eabi-gcc` for
  cross-compilation
- **Python ≥ 3.10** with `pip`

### First-time setup

```bash
# Clone the repository
git clone https://github.com/mr-u0b0dy/dog-test.git
cd dog-test

# Install Python dependencies (dev extras include linting & coverage)
pip install -e ".[dev]"

# Configure and build the host preset
cmake --preset host-debug
cmake --build --preset host-debug
```

### Running the test suite

```bash
# C tests (via CTest)
ctest --preset host-debug

# Python tests
python -m pytest tests/python/ -v
```

## Making Changes

1. **Create a feature branch** from `main`.
2. **Write tests** for any new feature or bug fix — both C unit tests in
   `tests/host/test_framework.c` and Python tests in `tests/python/`.
3. **Follow the coding style** (see below).
4. **Run the full test suite** before pushing.
5. **Open a Pull Request** with a clear description of the change.

## Coding Style

### C

- C11 standard, `-Wall -Wextra -Wpedantic` clean.
- Use `HT_` prefix for all public macros and `ht_` for functions.
- Include the SPDX license header at the top of every new file:
  ```c
  /* SPDX-License-Identifier: Apache-2.0 */
  /* Copyright 2026 dog-test contributors */
  ```

### Python

- Target Python 3.10+.
- Use type hints on all public functions.
- Format with **ruff** (`ruff format`) and lint with **ruff** (`ruff check`).
- Include the SPDX license header at the top of every new file:
  ```python
  # SPDX-License-Identifier: Apache-2.0
  # Copyright 2026 dog-test contributors
  ```

### Commit Messages

Follow the [Conventional Commits](https://www.conventionalcommits.org/) style:

```
feat: add HT_ASSERT_ARRAY_EQ macro
fix: correct float comparison tolerance in runner
docs: update Python API reference
test: add backend mock tests
refactor: extract backend registry
```

## Reporting Issues

- Use GitHub Issues to report bugs or request features.
- Include a minimal reproduction and your toolchain versions.

## License

By contributing, you agree that your contributions will be licensed under the
Apache License 2.0.

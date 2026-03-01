---
seo:
  title: dog-test — Hardware-in-the-Loop Testing for Embedded Systems
  description: An open-source HIL testing framework for embedded C/C++ targets.
    Combines an embedded assertion library, Python orchestration, and Saleae logic
    analyser integration for automated hardware verification.
---

::u-page-hero
#title
Hardware-in-the-Loop Testing for Embedded Systems

#description
dog-test is an open-source framework for writing, running, and automating integration tests
on real embedded hardware. Write assertions in C, orchestrate from Python, capture signals
with Saleae — all from a single test plan.

#links
  :::u-button
  ---
  color: neutral
  size: xl
  to: /en/getting-started/introduction
  trailing-icon: i-lucide-arrow-right
  ---
  Get started
  :::

  :::u-button
  ---
  color: neutral
  icon: simple-icons-github
  size: xl
  to: https://github.com/mr-u0b0dy/dog-test
  variant: outline
  ---
  View on GitHub
  :::
::

::u-page-section
#title
Built for embedded verification

#features
  :::u-page-feature
  ---
  icon: i-lucide-bug
  ---
  #title
  Embedded [C Assertion Library]{.text-primary}

  #description
  Fatal and non-fatal assertions, typed comparisons (u32, float, memory), string matching, custom messages, and skip support — all in a single `assert.h` header.
  :::

  :::u-page-feature
  ---
  icon: i-lucide-terminal
  ---
  #title
  Python [Host Orchestrator]{.text-primary}

  #description
  Flash, reset, capture serial output, and evaluate results with `dt_runner.py`. Supports pyOCD and OpenOCD backends, test plans, retry for flaky tests, and JUnit XML export.
  :::

  :::u-page-feature
  ---
  icon: i-lucide-activity
  ---
  #title
  Saleae [Logic Analyser]{.text-primary} Integration

  #description
  Capture UART, I2C, and SPI signals during test execution and assert on decoded protocol data — frame counts, content matching, and protocol validation.
  :::

  :::u-page-feature
  ---
  icon: i-lucide-layers
  ---
  #title
  CMake [Preset-Based]{.text-primary} Build System

  #description
  Host and ARM cross-compilation presets, CTest integration with labels and timeouts, and a clean separation between framework library, examples, and tests.
  :::

  :::u-page-feature
  ---
  icon: i-lucide-tags
  ---
  #title
  Test [Tags & Filtering]{.text-primary}

  #description
  Tag tests with `DT_TEST_T` and filter by name or tag at runtime. Run "all UART tests" or "all smoke tests" without naming conventions.
  :::

  :::u-page-feature
  ---
  icon: i-lucide-bot
  ---
  #title
  MCP & [LLM Integration]{.text-primary}

  #description
  Documentation site exposes MCP tools and `llms.txt` endpoints for AI-assisted development and automated documentation queries.
  :::
::

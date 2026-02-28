#!/usr/bin/env python3
"""HIL embedded test runner – orchestrates flash, reset, serial capture, and
optional Saleae logic-analyser monitoring for dog-test integration tests."""
from __future__ import annotations

import argparse
import json
import logging
import os
import subprocess
import sys
import time
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Optional

try:
    import serial  # type: ignore[import-untyped]
except ImportError:
    serial = None  # type: ignore[assignment]

try:
    from saleae import automation as _saleae_automation  # noqa: F401
except ImportError:
    _saleae_automation = None  # type: ignore[assignment]

# Ensure the project root is importable so 'tools.*' packages resolve whether
# this module is run as a script or imported from the project root.
_PROJECT_ROOT = Path(__file__).resolve().parents[1]
if str(_PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(_PROJECT_ROOT))

from tools.backends.flash.openocd_backend import OpenOcdFlashBackend
from tools.backends.flash.pyocd_backend import PyOcdFlashBackend
from tools.backends.reset.openocd_reset import OpenOcdResetBackend
from tools.backends.reset.pyocd_reset import PyOcdResetBackend
from tools.logic.assertions import evaluate_monitor_expectations
from tools.logic.saleae_adapter import SaleaeLogicAdapter
from tools.specs.monitor_contract import MonitorRequest

log = logging.getLogger("hil_runner")


# ── Data models ────────────────────────────────────────────────────────

@dataclass
class TestExecutionConfig:
    name: str
    firmware: Path
    board_id: Optional[str]
    serial_port: Optional[str]
    baudrate: int
    reset_mode: str
    backend: str
    openocd_interface: Optional[str]
    openocd_target: Optional[str]
    monitor: Optional[MonitorRequest]


@dataclass
class TestPlanItem:
    name: str
    firmware: Path
    reset_mode: str
    monitor: Optional[MonitorRequest]


@dataclass
class TestResult:
    """Result of a single test execution."""
    name: str
    status: str  # "passed", "failed", "skipped", "error"
    duration_s: float = 0.0
    message: str = ""
    attempt: int = 1
    serial_output: str = ""


# ── Helpers ────────────────────────────────────────────────────────────

def _run_cmd(command: list[str], cwd: Optional[Path] = None) -> subprocess.CompletedProcess[bytes]:
    """Run a command, capturing output and raising on failure."""
    result = subprocess.run(command, cwd=cwd, capture_output=True)
    if result.returncode != 0:
        stderr_text = result.stderr.decode(errors="replace").strip()
        stdout_text = result.stdout.decode(errors="replace").strip()
        detail = stderr_text or stdout_text or "(no output)"
        raise RuntimeError(
            f"command failed (exit {result.returncode}): {' '.join(command)}\n{detail}"
        )
    return result


def parse_ht_event_line(line: str) -> dict[str, str]:
    """Parse an HT_EVENT key=value line into a dict.

    Handles quoted values (e.g. key="value with spaces") safely.
    """
    parts: dict[str, str] = {}
    rest = line
    # strip the "HT_EVENT <type>" prefix
    tokens = rest.split(None, 2)
    if len(tokens) < 2:
        return parts
    rest = tokens[2] if len(tokens) > 2 else ""

    while rest:
        rest = rest.lstrip()
        if not rest:
            break
        eq = rest.find("=")
        if eq < 0:
            break
        key = rest[:eq]
        rest = rest[eq + 1:]

        if rest.startswith('"'):
            # quoted value – find unescaped closing quote
            rest = rest[1:]
            value_parts: list[str] = []
            while rest:
                bslash = rest.find("\\")
                quote = rest.find('"')
                if quote < 0:
                    value_parts.append(rest)
                    rest = ""
                    break
                if bslash >= 0 and bslash < quote:
                    value_parts.append(rest[:bslash])
                    if bslash + 1 < len(rest):
                        value_parts.append(rest[bslash + 1])
                    rest = rest[bslash + 2:]
                else:
                    value_parts.append(rest[:quote])
                    rest = rest[quote + 1:]
                    break
            parts[key] = "".join(value_parts)
        else:
            # unquoted value – read until next space
            sp = rest.find(" ")
            if sp < 0:
                parts[key] = rest
                rest = ""
            else:
                parts[key] = rest[:sp]
                rest = rest[sp:]
    return parts


# ── Build helpers ──────────────────────────────────────────────────────

def configure_build(preset: str) -> None:
    _run_cmd(["cmake", "--preset", preset])


def build_target(preset: str, target: Optional[str]) -> None:
    cmd = ["cmake", "--build", "--preset", preset]
    if target:
        cmd.extend(["--target", target])
    _run_cmd(cmd)


# ── Flash / Reset / Serial / Monitor ──────────────────────────────────

def flash_firmware(config: TestExecutionConfig) -> None:
    if config.backend == "pyocd":
        backend = PyOcdFlashBackend(config.board_id)
    elif config.backend == "openocd":
        backend = OpenOcdFlashBackend(config.openocd_interface, config.openocd_target)
    else:
        raise ValueError(f"unsupported backend: {config.backend}")
    backend.flash(config.firmware)


def apply_reset(config: TestExecutionConfig) -> None:
    if config.reset_mode == "none":
        return
    if config.backend == "pyocd":
        reset_backend = PyOcdResetBackend(config.board_id)
    elif config.backend == "openocd":
        reset_backend = OpenOcdResetBackend(config.openocd_interface, config.openocd_target)
    else:
        raise ValueError(f"unsupported backend: {config.backend}")

    if config.reset_mode == "soft":
        reset_backend.soft_reset()
    elif config.reset_mode == "hard":
        reset_backend.hard_reset()
    else:
        raise ValueError(f"unsupported reset mode: {config.reset_mode}")


def run_target_test(config: TestExecutionConfig) -> tuple[int, list[str]]:
    if not config.serial_port:
        log.warning("no serial port configured; target execution output not collected")
        return (0, [])

    if serial is None:
        raise RuntimeError("pyserial is required for target execution (pip install pyserial)")

    overall_timeout = max(30.0, (config.monitor.timeout_ms / 1000.0 * 3) if config.monitor else 30.0)
    deadline = time.monotonic() + overall_timeout

    with serial.Serial(config.serial_port, config.baudrate, timeout=1.0) as ser:
        lines: list[str] = []
        while time.monotonic() < deadline:
            raw = ser.readline()
            if not raw:
                continue
            try:
                line = raw.decode(errors="replace").strip()
            except Exception:
                continue
            if not line:
                continue
            log.info(line)
            lines.append(line)
            if line.startswith("HT_EVENT summary"):
                break
        else:
            log.error("serial read timed out after %.0fs without summary event", overall_timeout)
            return (1, lines)

    failed = 0
    for line in lines:
        if line.startswith("HT_EVENT summary"):
            try:
                parts = parse_ht_event_line(line)
                failed = int(parts.get("failed", "1"))
            except (ValueError, KeyError) as exc:
                log.error("failed to parse summary line %r: %s", line, exc)
                failed = 1
            break
    return (failed, lines)


def run_monitor(config: TestExecutionConfig) -> None:
    if config.monitor is None:
        return

    if _saleae_automation is None:
        raise RuntimeError("saleae-automation package is required for monitoring (pip install saleae)")

    adapter = SaleaeLogicAdapter.from_environment()
    capture = adapter.capture_protocol(
        protocol=config.monitor.protocol,
        channels=config.monitor.channels,
        trigger=config.monitor.trigger,
        duration_ms=config.monitor.timeout_ms,
    )
    evaluate_monitor_expectations(capture, config.monitor.expect)


# ── Single test execution (with retry) ────────────────────────────────

def execute_single_test(
    config: TestExecutionConfig,
    args: argparse.Namespace,
    retries: int = 0,
) -> TestResult:
    """Execute a single test, optionally retrying on failure."""
    last_result = TestResult(name=config.name, status="error")

    for attempt in range(1, retries + 2):  # retries=0 means 1 attempt
        t0 = time.monotonic()
        try:
            if not args.skip_flash:
                flash_firmware(config)
            if not args.skip_reset:
                apply_reset(config)
            if not args.skip_monitor:
                run_monitor(config)
            if args.skip_target_exec:
                failed = 0
                serial_lines: list[str] = []
            else:
                failed, serial_lines = run_target_test(config)
            status = "failed" if failed else "passed"
        except Exception as exc:
            status = "error"
            failed = 1
            serial_lines = []
            log.error("test %s attempt %d error: %s", config.name, attempt, exc)

        elapsed = time.monotonic() - t0
        last_result = TestResult(
            name=config.name,
            status=status,
            duration_s=elapsed,
            message="" if status == "passed" else f"attempt {attempt}",
            attempt=attempt,
            serial_output="\n".join(serial_lines),
        )

        if status == "passed":
            if attempt > 1:
                log.info("test %s passed on retry %d", config.name, attempt)
            break
        if attempt <= retries:
            log.warning("test %s failed (attempt %d/%d), retrying…", config.name, attempt, retries + 1)
            time.sleep(0.5)

    return last_result


# ── Test plan ──────────────────────────────────────────────────────────

def load_test_plan(path: str) -> list[TestPlanItem]:
    plan_data = json.loads(Path(path).read_text(encoding="utf-8"))
    plan_dir = Path(path).parent
    items = plan_data.get("tests", [])
    loaded: list[TestPlanItem] = []
    for item in items:
        monitor = None
        monitor_value = item.get("monitor")
        if monitor_value:
            if isinstance(monitor_value, str):
                # Treat as file path (relative to the plan file directory)
                spec_path = plan_dir / monitor_value
                monitor_data = json.loads(spec_path.read_text(encoding="utf-8"))
                monitor = MonitorRequest.from_dict(monitor_data)
            else:
                monitor = MonitorRequest.from_dict(monitor_value)
        loaded.append(
            TestPlanItem(
                name=str(item.get("name", "unnamed_test")),
                firmware=Path(str(item["firmware"])),
                reset_mode=str(item.get("reset_mode", "none")),
                monitor=monitor,
            )
        )
    return loaded


def _validate_firmware_paths(items: list[TestPlanItem]) -> None:
    """Fail-fast validation: ensure all firmware paths exist before starting."""
    missing = [item.firmware for item in items if not item.firmware.exists()]
    if missing:
        formatted = "\n  ".join(str(p) for p in missing)
        raise FileNotFoundError(f"firmware files not found:\n  {formatted}")


def run_test_plan(args: argparse.Namespace) -> tuple[int, list[TestResult]]:
    test_plan = load_test_plan(args.test_plan)
    _validate_firmware_paths(test_plan)

    results: list[TestResult] = []
    total_failed = 0

    for index, item in enumerate(test_plan):
        log.info("HT_HOST start name=%s", item.name)
        config = TestExecutionConfig(
            name=item.name,
            firmware=item.firmware,
            board_id=args.board_id,
            serial_port=args.serial_port,
            baudrate=args.baudrate,
            reset_mode=item.reset_mode,
            backend=args.backend,
            openocd_interface=args.openocd_interface,
            openocd_target=args.openocd_target,
            monitor=item.monitor,
        )
        result = execute_single_test(config, args, retries=args.retries)
        results.append(result)
        failed = 0 if result.status == "passed" else 1
        total_failed += failed
        log.info("HT_HOST done name=%s status=%s (%.2fs)", item.name, result.status, result.duration_s)

        if args.reset_between_tests and index < len(test_plan) - 1:
            if args.skip_reset:
                continue
            inter_test_reset = TestExecutionConfig(
                name=item.name,
                firmware=item.firmware,
                board_id=args.board_id,
                serial_port=args.serial_port,
                baudrate=args.baudrate,
                reset_mode=item.reset_mode,
                backend=args.backend,
                openocd_interface=args.openocd_interface,
                openocd_target=args.openocd_target,
                monitor=None,
            )
            apply_reset(inter_test_reset)
            time.sleep(0.1)

    log.info("HT_HOST summary total=%d failed=%d", len(test_plan), total_failed)
    return (0 if total_failed == 0 else 1, results)


# ── JUnit XML output ──────────────────────────────────────────────────

def write_junit_xml(results: list[TestResult], path: str) -> None:
    """Write test results as JUnit XML for CI integration."""
    testsuite = ET.Element("testsuite", {
        "name": "hil_tests",
        "tests": str(len(results)),
        "failures": str(sum(1 for r in results if r.status == "failed")),
        "errors": str(sum(1 for r in results if r.status == "error")),
        "skipped": str(sum(1 for r in results if r.status == "skipped")),
        "time": f"{sum(r.duration_s for r in results):.3f}",
    })

    for r in results:
        tc = ET.SubElement(testsuite, "testcase", {
            "name": r.name,
            "classname": "hil",
            "time": f"{r.duration_s:.3f}",
        })
        if r.status == "failed":
            ET.SubElement(tc, "failure", {"message": r.message or "test failed"})
        elif r.status == "error":
            ET.SubElement(tc, "error", {"message": r.message or "test error"})
        elif r.status == "skipped":
            ET.SubElement(tc, "skipped", {"message": r.message or ""})

        if r.serial_output:
            sysout = ET.SubElement(tc, "system-out")
            sysout.text = r.serial_output

    tree = ET.ElementTree(testsuite)
    ET.indent(tree, space="  ")
    tree.write(path, encoding="unicode", xml_declaration=True)
    log.info("JUnit XML written to %s", path)


# ── CLI ────────────────────────────────────────────────────────────────

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="HIL embedded test runner")
    parser.add_argument("--configure-preset", default=None)
    parser.add_argument("--build-preset", default=None)
    parser.add_argument("--target", default=None)
    parser.add_argument("--firmware", default=None)
    parser.add_argument("--board-id", default=os.getenv("HIL_BOARD_ID"))
    parser.add_argument("--serial-port", default=os.getenv("HIL_SERIAL_PORT"))
    parser.add_argument("--baudrate", type=int, default=int(os.getenv("HIL_BAUDRATE", "115200")))
    parser.add_argument("--test-name", default="embedded_suite")
    parser.add_argument("--reset-mode", choices=["none", "soft", "hard"], default="none")
    parser.add_argument("--backend", choices=["pyocd", "openocd"], default="pyocd")
    parser.add_argument("--openocd-interface", default=os.getenv("HIL_OPENOCD_INTERFACE"))
    parser.add_argument("--openocd-target", default=os.getenv("HIL_OPENOCD_TARGET"))
    parser.add_argument("--monitor-spec", default=None, help="JSON monitor spec")
    parser.add_argument("--test-plan", default=None, help="JSON file with list of tests")
    parser.add_argument("--tag", default=None, help="run only tests matching this tag (target-side filtering)")
    parser.add_argument("--filter", default=None, help="run only tests whose name contains this string (target-side filtering)")
    parser.add_argument("--reset-between-tests", action="store_true")
    parser.add_argument("--skip-flash", action="store_true")
    parser.add_argument("--skip-reset", action="store_true")
    parser.add_argument("--skip-target-exec", action="store_true")
    parser.add_argument("--skip-monitor", action="store_true")
    parser.add_argument("--retries", type=int, default=0, help="number of retries for flaky tests")
    parser.add_argument("--junit-xml", default=None, help="write JUnit XML results to this path")
    parser.add_argument("-v", "--verbose", action="store_true", help="enable debug logging")
    parser.add_argument("-q", "--quiet", action="store_true", help="suppress info logging")
    return parser.parse_args()


def _setup_logging(args: argparse.Namespace) -> None:
    if args.verbose:
        level = logging.DEBUG
    elif args.quiet:
        level = logging.WARNING
    else:
        level = logging.INFO
    logging.basicConfig(
        level=level,
        format="%(asctime)s [%(levelname)s] %(name)s: %(message)s",
        datefmt="%H:%M:%S",
    )


def main() -> int:
    args = parse_args()
    _setup_logging(args)

    if args.configure_preset:
        configure_build(args.configure_preset)

    if args.build_preset:
        build_target(args.build_preset, args.target)

    monitor = None
    if args.monitor_spec:
        monitor = MonitorRequest.from_dict(json.loads(args.monitor_spec))

    results: list[TestResult] = []

    if args.test_plan:
        rc, results = run_test_plan(args)
    else:
        if not args.firmware:
            raise ValueError("--firmware is required when --test-plan is not provided")

        config = TestExecutionConfig(
            name=args.test_name,
            firmware=Path(args.firmware),
            board_id=args.board_id,
            serial_port=args.serial_port,
            baudrate=args.baudrate,
            reset_mode=args.reset_mode,
            backend=args.backend,
            openocd_interface=args.openocd_interface,
            openocd_target=args.openocd_target,
            monitor=monitor,
        )

        result = execute_single_test(config, args, retries=args.retries)
        results.append(result)
        rc = 0 if result.status == "passed" else 1

    if args.junit_xml and results:
        write_junit_xml(results, args.junit_xml)

    return rc


if __name__ == "__main__":
    sys.exit(main())

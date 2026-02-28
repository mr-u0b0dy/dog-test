#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Optional

PROJECT_ROOT = Path(__file__).resolve().parents[1]
if str(PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(PROJECT_ROOT))

from tools.backends.flash.openocd_backend import OpenOcdFlashBackend
from tools.backends.flash.pyocd_backend import PyOcdFlashBackend
from tools.backends.reset.openocd_reset import OpenOcdResetBackend
from tools.backends.reset.pyocd_reset import PyOcdResetBackend
from tools.logic.assertions import evaluate_monitor_expectations
from tools.logic.saleae_adapter import SaleaeLogicAdapter
from tools.specs.monitor_contract import MonitorRequest


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


def _run_cmd(command: list[str], cwd: Optional[Path] = None) -> None:
    result = subprocess.run(command, cwd=cwd, check=False)
    if result.returncode != 0:
        raise RuntimeError(f"command failed: {' '.join(command)}")


def configure_build(preset: str) -> None:
    _run_cmd(["cmake", "--preset", preset])


def build_target(preset: str, target: Optional[str]) -> None:
    cmd = ["cmake", "--build", "--preset", preset]
    if target:
        cmd.extend(["--target", target])
    _run_cmd(cmd)


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
        backend = PyOcdResetBackend(config.board_id)
    elif config.backend == "openocd":
        backend = OpenOcdResetBackend(config.openocd_interface, config.openocd_target)
    else:
        raise ValueError(f"unsupported backend: {config.backend}")

    if config.reset_mode == "soft":
        backend.soft_reset()
    elif config.reset_mode == "hard":
        backend.hard_reset()
    else:
        raise ValueError(f"unsupported reset mode: {config.reset_mode}")


def run_target_test(config: TestExecutionConfig) -> int:
    if not config.serial_port:
        print("warning: no serial port configured; target execution output not collected")
        return 0

    import serial

    with serial.Serial(config.serial_port, config.baudrate, timeout=1.0) as ser:
        lines = []
        while True:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode(errors="replace").strip()
            if line:
                print(line)
                lines.append(line)
            if line.startswith("HT_EVENT summary"):
                break

    failed = 0
    for line in lines:
        if line.startswith("HT_EVENT summary"):
            parts = dict(part.split("=") for part in line.split() if "=" in part)
            failed = int(parts.get("failed", "1"))
            break
    return failed


def run_monitor(config: TestExecutionConfig) -> None:
    if config.monitor is None:
        return

    adapter = SaleaeLogicAdapter.from_environment()
    capture = adapter.capture_protocol(
        protocol=config.monitor.protocol,
        channels=config.monitor.channels,
        trigger=config.monitor.trigger,
        duration_ms=config.monitor.timeout_ms,
    )
    evaluate_monitor_expectations(capture, config.monitor.expect)


def execute_single_test(config: TestExecutionConfig, args: argparse.Namespace) -> int:
    if not args.skip_flash:
        flash_firmware(config)
    if not args.skip_reset:
        apply_reset(config)
    if not args.skip_monitor:
        run_monitor(config)
    failed = 0 if args.skip_target_exec else run_target_test(config)
    return failed


def load_test_plan(path: str) -> list[TestPlanItem]:
    plan_data = json.loads(Path(path).read_text(encoding="utf-8"))
    items = plan_data.get("tests", [])
    loaded: list[TestPlanItem] = []
    for item in items:
        monitor = None
        if item.get("monitor"):
            monitor = MonitorRequest.from_dict(item["monitor"])
        loaded.append(
            TestPlanItem(
                name=str(item.get("name", "unnamed_test")),
                firmware=Path(str(item["firmware"])),
                reset_mode=str(item.get("reset_mode", "none")),
                monitor=monitor,
            )
        )
    return loaded


def run_test_plan(args: argparse.Namespace) -> int:
    test_plan = load_test_plan(args.test_plan)
    total_failed = 0

    for index, item in enumerate(test_plan):
        print(f"HT_HOST start name={item.name}")
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
        failed = execute_single_test(config, args)
        total_failed += failed
        print(f"HT_HOST done name={item.name} failed={failed}")

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

    print(f"HT_HOST summary total={len(test_plan)} failed={total_failed}")
    return 0 if total_failed == 0 else 1


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
    parser.add_argument("--reset-between-tests", action="store_true")
    parser.add_argument("--skip-flash", action="store_true")
    parser.add_argument("--skip-reset", action="store_true")
    parser.add_argument("--skip-target-exec", action="store_true")
    parser.add_argument("--skip-monitor", action="store_true")
    return parser.parse_args()


def main() -> int:
    args = parse_args()

    if args.configure_preset:
        configure_build(args.configure_preset)

    if args.build_preset:
        build_target(args.build_preset, args.target)

    monitor = None
    if args.monitor_spec:
        monitor = MonitorRequest.from_dict(json.loads(args.monitor_spec))

    if args.test_plan:
        return run_test_plan(args)

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

    failed = execute_single_test(config, args)
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())

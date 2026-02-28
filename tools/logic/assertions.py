# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
from __future__ import annotations

from tools.logic.decoders import decode_by_protocol
from tools.logic.saleae_adapter import LogicCapture


def evaluate_monitor_expectations(capture: LogicCapture, expect: dict) -> None:
    decoded = decode_by_protocol(capture)

    required_protocol = expect.get("protocol")
    if required_protocol and required_protocol.lower() != capture.protocol.lower():
        raise AssertionError(
            f"protocol expectation mismatch: expected={required_protocol} actual={capture.protocol}"
        )

    min_items = int(expect.get("min_items", 0))
    count = 0
    if "frames" in decoded:
        count = len(decoded["frames"])
    elif "transactions" in decoded:
        count = len(decoded["transactions"])

    if count < min_items:
        raise AssertionError(f"monitor expectation failed: count={count} < min_items={min_items}")

    contains = expect.get("contains")
    if contains:
        entries = decoded.get("frames") or decoded.get("transactions") or []
        haystack = "\n".join(str(item) for item in entries)
        if str(contains) not in haystack:
            raise AssertionError(f"monitor expectation failed: '{contains}' not found")

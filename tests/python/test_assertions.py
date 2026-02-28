"""Unit tests for tools.logic.assertions"""
from __future__ import annotations

import pytest

from tools.logic.assertions import evaluate_monitor_expectations
from tools.logic.saleae_adapter import LogicCapture


def _make_capture(protocol: str, rows: list[dict]) -> LogicCapture:
    return LogicCapture(
        protocol=protocol,
        channels="ch0=a",
        trigger="",
        raw={"rows": rows},
    )


class TestEvaluateMonitorExpectations:
    def test_protocol_match(self):
        cap = _make_capture("uart", [])
        # Should pass — protocol matches, no other expectations
        evaluate_monitor_expectations(cap, {"protocol": "uart"})

    def test_protocol_mismatch(self):
        cap = _make_capture("uart", [])
        with pytest.raises(AssertionError, match="protocol expectation mismatch"):
            evaluate_monitor_expectations(cap, {"protocol": "spi"})

    def test_min_items_pass(self):
        rows = [{"Time": "0", "Data": "x"}, {"Time": "1", "Data": "y"}]
        cap = _make_capture("uart", rows)
        evaluate_monitor_expectations(cap, {"min_items": 2})

    def test_min_items_fail(self):
        cap = _make_capture("uart", [{"Time": "0", "Data": "x"}])
        with pytest.raises(AssertionError, match="min_items"):
            evaluate_monitor_expectations(cap, {"min_items": 5})

    def test_contains_pass(self):
        rows = [{"Time": "0", "Data": "hello"}]
        cap = _make_capture("uart", rows)
        evaluate_monitor_expectations(cap, {"contains": "hello"})

    def test_contains_fail(self):
        rows = [{"Time": "0", "Data": "hello"}]
        cap = _make_capture("uart", rows)
        with pytest.raises(AssertionError, match="not found"):
            evaluate_monitor_expectations(cap, {"contains": "goodbye"})

    def test_empty_expectations(self):
        cap = _make_capture("uart", [])
        evaluate_monitor_expectations(cap, {})

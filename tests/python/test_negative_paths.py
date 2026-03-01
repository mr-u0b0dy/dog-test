# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
"""Negative-path and edge-case tests for Python tooling."""
from __future__ import annotations

import json
from pathlib import Path

import pytest

from tools.dt_runner import load_test_plan, parse_dt_event_line
from tools.logic.decoders import decode_by_protocol
from tools.logic.saleae_adapter import LogicCapture
from tools.specs.monitor_contract import MonitorRequest

# ── MonitorRequest validation ──────────────────────────────────────────


class TestMonitorRequestValidation:
    def test_valid_expect_keys(self):
        req = MonitorRequest.from_dict({
            "protocol": "uart",
            "expect": {"protocol": "uart", "min_items": 1, "contains": "hello"},
        })
        assert req.expect["min_items"] == 1

    def test_unknown_expect_key_raises(self):
        with pytest.raises(ValueError, match="unknown keys in monitor expect"):
            MonitorRequest.from_dict({
                "protocol": "uart",
                "expect": {"min_item": 1},  # typo: should be min_items
            })

    def test_empty_expect_is_valid(self):
        req = MonitorRequest.from_dict({"protocol": "spi", "expect": {}})
        assert req.expect == {}

    def test_multiple_unknown_keys(self):
        with pytest.raises(ValueError, match="unknown keys"):
            MonitorRequest.from_dict({
                "protocol": "uart",
                "expect": {"proto": "uart", "minimum": 5},
            })


# ── Test plan error paths ─────────────────────────────────────────────


class TestLoadTestPlanErrors:
    def test_malformed_json_raises(self, tmp_path: Path):
        bad_file = tmp_path / "bad.json"
        bad_file.write_text("{not valid json", encoding="utf-8")
        with pytest.raises(json.JSONDecodeError):
            load_test_plan(str(bad_file))

    def test_missing_file_raises(self, tmp_path: Path):
        with pytest.raises(FileNotFoundError):
            load_test_plan(str(tmp_path / "nonexistent.json"))

    def test_missing_firmware_key_raises(self, tmp_path: Path):
        plan = {"tests": [{"name": "no_firmware"}]}
        plan_file = tmp_path / "plan.json"
        plan_file.write_text(json.dumps(plan), encoding="utf-8")
        with pytest.raises(KeyError):
            load_test_plan(str(plan_file))

    def test_plan_with_bad_monitor_spec(self, tmp_path: Path):
        plan = {
            "tests": [
                {
                    "name": "bad_mon",
                    "firmware": "/dev/null",
                    "monitor": {
                        "protocol": "uart",
                        "expect": {"invalid_key": True},
                    },
                }
            ]
        }
        plan_file = tmp_path / "plan.json"
        plan_file.write_text(json.dumps(plan), encoding="utf-8")
        with pytest.raises(ValueError, match="unknown keys"):
            load_test_plan(str(plan_file))


# ── parse_dt_event_line edge cases ────────────────────────────────────


class TestParseEdgeCases:
    def test_completely_empty_string(self):
        result = parse_dt_event_line("")
        assert result == {}

    def test_only_event_type(self):
        result = parse_dt_event_line("DT_EVENT boot")
        assert result == {}

    def test_malformed_no_equals(self):
        result = parse_dt_event_line("DT_EVENT summary no_equals_here")
        assert result == {}

    def test_unterminated_quote(self):
        result = parse_dt_event_line('DT_EVENT fail name="unterminated')
        assert result.get("name") == "unterminated"


# ── Decoder edge cases ────────────────────────────────────────────────


class TestDecoderEdgeCases:
    def test_unsupported_protocol_raises(self):
        capture = LogicCapture(protocol="can", channels="", trigger="", raw={})
        with pytest.raises(ValueError, match="unsupported protocol"):
            decode_by_protocol(capture)

    def test_empty_rows_uart(self):
        capture = LogicCapture(protocol="uart", channels="", trigger="", raw={"rows": []})
        result = decode_by_protocol(capture)
        assert result["frames"] == []

    def test_empty_rows_i2c(self):
        capture = LogicCapture(protocol="i2c", channels="", trigger="", raw={"rows": []})
        result = decode_by_protocol(capture)
        assert result["transactions"] == []

    def test_non_dict_raw(self):
        capture = LogicCapture(protocol="uart", channels="", trigger="", raw="not a dict")
        result = decode_by_protocol(capture)
        assert result["frames"] == []

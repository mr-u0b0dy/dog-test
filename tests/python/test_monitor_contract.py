"""Unit tests for tools.specs.monitor_contract"""
from __future__ import annotations

from tools.specs.monitor_contract import MonitorRequest


class TestMonitorRequest:
    def test_from_dict_complete(self):
        data = {
            "protocol": "uart",
            "channels": "ch0=tx,ch1=rx",
            "trigger": "start-on-byte:0x55",
            "timeout_ms": 2000,
            "expect": {"min_items": 3},
        }
        req = MonitorRequest.from_dict(data)
        assert req.protocol == "uart"
        assert req.channels == "ch0=tx,ch1=rx"
        assert req.trigger == "start-on-byte:0x55"
        assert req.timeout_ms == 2000
        assert req.expect == {"min_items": 3}

    def test_from_dict_defaults(self):
        req = MonitorRequest.from_dict({})
        assert req.protocol == ""
        assert req.channels == ""
        assert req.trigger == ""
        assert req.timeout_ms == 1000
        assert req.expect == {}

    def test_from_dict_partial(self):
        req = MonitorRequest.from_dict({"protocol": "spi", "timeout_ms": 500})
        assert req.protocol == "spi"
        assert req.timeout_ms == 500
        assert req.channels == ""

    def test_from_dict_type_coercion(self):
        req = MonitorRequest.from_dict({"timeout_ms": "3000"})
        assert req.timeout_ms == 3000

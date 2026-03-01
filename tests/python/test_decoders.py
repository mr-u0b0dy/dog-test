"""Unit tests for tools.logic.decoders"""
from __future__ import annotations

import pytest

from tools.logic.decoders import decode_by_protocol, decode_i2c, decode_spi, decode_uart
from tools.logic.saleae_adapter import LogicCapture


def _make_capture(protocol: str, rows: list[dict]) -> LogicCapture:
    return LogicCapture(
        protocol=protocol,
        channels="ch0=a",
        trigger="",
        raw={"rows": rows},
    )


class TestDecodeUart:
    def test_empty(self):
        cap = _make_capture("uart", [])
        result = decode_uart(cap)
        assert result["frames"] == []

    def test_basic_frames(self):
        rows = [
            {"Time": "0.001", "Data": "0x55", "Type": "data"},
            {"Time": "0.002", "Data": "0xAA", "Type": "data"},
        ]
        cap = _make_capture("uart", rows)
        result = decode_uart(cap)
        assert len(result["frames"]) == 2
        assert result["frames"][0]["value"] == "0x55"
        assert result["frames"][1]["time"] == "0.002"

    def test_missing_fields_use_defaults(self):
        rows = [{"Time": "0.001"}]
        cap = _make_capture("uart", rows)
        result = decode_uart(cap)
        assert result["frames"][0]["value"] == ""
        assert result["frames"][0]["type"] == "data"


class TestDecodeI2C:
    def test_transactions(self):
        rows = [
            {"Time": "0.001", "Address": "0x48", "Data": "0x10", "Read/Write": "W"},
        ]
        cap = _make_capture("i2c", rows)
        result = decode_i2c(cap)
        assert len(result["transactions"]) == 1
        assert result["transactions"][0]["address"] == "0x48"
        assert result["transactions"][0]["rw"] == "W"


class TestDecodeSPI:
    def test_transactions(self):
        rows = [
            {"Time": "0.001", "MOSI": "0xFF", "MISO": "0x00"},
        ]
        cap = _make_capture("spi", rows)
        result = decode_spi(cap)
        assert len(result["transactions"]) == 1
        assert result["transactions"][0]["mosi"] == "0xFF"
        assert result["transactions"][0]["miso"] == "0x00"


class TestDecodeByProtocol:
    def test_uart_dispatch(self):
        cap = _make_capture("uart", [{"Time": "0", "Data": "x"}])
        result = decode_by_protocol(cap)
        assert "frames" in result

    def test_i2c_dispatch(self):
        cap = _make_capture("i2c", [])
        result = decode_by_protocol(cap)
        assert "transactions" in result

    def test_spi_dispatch(self):
        cap = _make_capture("spi", [])
        result = decode_by_protocol(cap)
        assert "transactions" in result

    def test_unsupported_raises(self):
        cap = _make_capture("can", [])
        with pytest.raises(ValueError):
            decode_by_protocol(cap)

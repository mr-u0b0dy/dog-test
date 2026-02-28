# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
from __future__ import annotations

from tools.logic.saleae_adapter import LogicCapture


def decode_uart(capture: LogicCapture) -> dict:
    rows = capture.raw.get("rows", []) if isinstance(capture.raw, dict) else []
    frames = []
    for row in rows:
        frames.append(
            {
                "time": row.get("Time", ""),
                "value": row.get("Data", row.get("Value", "")),
                "type": row.get("Type", "data"),
            }
        )
    return {"frames": frames, "meta": capture.raw}


def decode_i2c(capture: LogicCapture) -> dict:
    rows = capture.raw.get("rows", []) if isinstance(capture.raw, dict) else []
    transactions = []
    for row in rows:
        transactions.append(
            {
                "time": row.get("Time", ""),
                "address": row.get("Address", ""),
                "data": row.get("Data", row.get("Value", "")),
                "rw": row.get("Read/Write", ""),
            }
        )
    return {"transactions": transactions, "meta": capture.raw}


def decode_spi(capture: LogicCapture) -> dict:
    rows = capture.raw.get("rows", []) if isinstance(capture.raw, dict) else []
    transactions = []
    for row in rows:
        transactions.append(
            {
                "time": row.get("Time", ""),
                "mosi": row.get("MOSI", row.get("Data", "")),
                "miso": row.get("MISO", ""),
            }
        )
    return {"transactions": transactions, "meta": capture.raw}


def decode_by_protocol(capture: LogicCapture) -> dict:
    protocol = capture.protocol.lower()
    if protocol == "uart":
        return decode_uart(capture)
    if protocol == "i2c":
        return decode_i2c(capture)
    if protocol == "spi":
        return decode_spi(capture)
    raise ValueError(f"unsupported protocol: {capture.protocol}")

# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
from __future__ import annotations

import csv
import os
import tempfile
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Any


@dataclass
class LogicCapture:
    protocol: str
    channels: str
    trigger: str
    raw: Any


def _parse_channel_numbers(channels: str) -> list[int]:
    values: list[int] = []
    for token in channels.split(","):
        token = token.strip()
        if not token:
            continue
        if token.startswith("ch") and "=" in token:
            left, _right = token.split("=", 1)
            values.append(int(left.replace("ch", "")))
        elif token.isdigit():
            values.append(int(token))
    return sorted(set(values))


class SaleaeLogicAdapter:
    def __init__(self, host: str, port: int) -> None:
        self.host = host
        self.port = port

    @classmethod
    def from_environment(cls) -> SaleaeLogicAdapter:
        host = os.getenv("SALEAE_HOST", "127.0.0.1")
        port = int(os.getenv("SALEAE_PORT", "10430"))
        return cls(host, port)

    def _analyzer_type(self, protocol: str) -> str:
        value = protocol.lower()
        if value == "uart":
            return "Async Serial"
        if value == "i2c":
            return "I2C"
        if value == "spi":
            return "SPI"
        raise ValueError(f"unsupported Saleae protocol: {protocol}")

    def _analyzer_settings(self, protocol: str, channels: list[int]) -> dict[str, Any]:
        value = protocol.lower()
        if value == "uart":
            return {"Input Channel": channels[0] if channels else 0}
        if value == "i2c":
            return {
                "SCL": channels[0] if len(channels) > 0 else 0,
                "SDA": channels[1] if len(channels) > 1 else 1,
            }
        if value == "spi":
            return {
                "Clock": channels[0] if len(channels) > 0 else 0,
                "MOSI": channels[1] if len(channels) > 1 else 1,
                "MISO": channels[2] if len(channels) > 2 else 2,
                "Enable": channels[3] if len(channels) > 3 else 3,
            }
        raise ValueError(f"unsupported Saleae protocol: {protocol}")

    def _read_csv_rows(self, file_path: Path) -> list[dict[str, str]]:
        if not file_path.exists():
            return []
        with file_path.open("r", encoding="utf-8", newline="") as handle:
            return list(csv.DictReader(handle))

    def capture_protocol(
        self,
        protocol: str,
        channels: str,
        trigger: str,
        duration_ms: int,
    ) -> LogicCapture:
        try:
            from saleae import automation
        except ImportError as exc:
            raise RuntimeError("saleae automation package is required") from exc

        digital_channels = _parse_channel_numbers(channels)

        with automation.Manager.connect(self.host, self.port) as manager:
            capture = manager.start_capture(device_id=None)
            analyzer = capture.add_analyzer(
                self._analyzer_type(protocol),
                settings=self._analyzer_settings(protocol, digital_channels),
            )

            time.sleep(max(0.05, duration_ms / 1000.0))
            capture.stop()

            rows: list[dict[str, str]] = []
            with tempfile.TemporaryDirectory() as temp_dir:
                csv_path = Path(temp_dir) / f"{protocol}_capture.csv"
                try:
                    capture.export_data_table(filepath=str(csv_path), analyzers=[analyzer])
                    rows = self._read_csv_rows(csv_path)
                except Exception:
                    import logging as _log
                    _log.getLogger(__name__).error(
                        "Saleae CSV export failed for protocol=%s", protocol, exc_info=True
                    )
                    raise

            exported = {
                "duration_ms": duration_ms,
                "channels": channels,
                "trigger": trigger,
                "protocol": protocol,
                "rows": rows,
            }

        return LogicCapture(protocol=protocol, channels=channels, trigger=trigger, raw=exported)

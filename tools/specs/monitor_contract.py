from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass
class MonitorRequest:
    protocol: str
    channels: str
    trigger: str
    timeout_ms: int
    expect: dict[str, Any]

    @classmethod
    def from_dict(cls, data: dict[str, Any]) -> "MonitorRequest":
        return cls(
            protocol=str(data.get("protocol", "")),
            channels=str(data.get("channels", "")),
            trigger=str(data.get("trigger", "")),
            timeout_ms=int(data.get("timeout_ms", 1000)),
            expect=dict(data.get("expect", {})),
        )

# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
from __future__ import annotations

import logging
from dataclasses import dataclass
from typing import Any

log = logging.getLogger(__name__)

_VALID_EXPECT_KEYS = frozenset({"protocol", "min_items", "contains"})


@dataclass
class MonitorRequest:
    protocol: str
    channels: str
    trigger: str
    timeout_ms: int
    expect: dict[str, Any]

    def validate(self) -> None:
        """Validate the expect schema, raising ValueError on unknown keys."""
        unknown = set(self.expect) - _VALID_EXPECT_KEYS
        if unknown:
            raise ValueError(
                f"unknown keys in monitor expect: {sorted(unknown)}; "
                f"allowed: {sorted(_VALID_EXPECT_KEYS)}"
            )

    @classmethod
    def from_dict(cls, data: dict[str, Any]) -> "MonitorRequest":
        instance = cls(
            protocol=str(data.get("protocol", "")),
            channels=str(data.get("channels", "")),
            trigger=str(data.get("trigger", "")),
            timeout_ms=int(data.get("timeout_ms", 1000)),
            expect=dict(data.get("expect", {})),
        )
        instance.validate()
        return instance

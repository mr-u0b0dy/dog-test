# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass
class RustTestBinary:
    path: Path
    target_triple: str


class RustAdapter:
    def build(self, crate_dir: Path, target_triple: str) -> RustTestBinary:
        raise NotImplementedError("Rust integration is planned in a future phase")

    def event_protocol_name(self) -> str:
        return "DT_EVENT"

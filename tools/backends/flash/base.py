# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
from __future__ import annotations

from abc import ABC, abstractmethod
from pathlib import Path


class FlashBackend(ABC):
    @abstractmethod
    def flash(self, firmware_path: Path) -> None:
        raise NotImplementedError

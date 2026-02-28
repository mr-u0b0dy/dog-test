# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
from __future__ import annotations

from abc import ABC, abstractmethod


class ResetBackend(ABC):
    @abstractmethod
    def soft_reset(self) -> None:
        raise NotImplementedError

    @abstractmethod
    def hard_reset(self) -> None:
        raise NotImplementedError

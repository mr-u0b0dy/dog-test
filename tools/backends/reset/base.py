from __future__ import annotations

from abc import ABC, abstractmethod


class ResetBackend(ABC):
    @abstractmethod
    def soft_reset(self) -> None:
        raise NotImplementedError

    @abstractmethod
    def hard_reset(self) -> None:
        raise NotImplementedError

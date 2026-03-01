# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
"""Flash backend registry — maps backend names to classes."""
from __future__ import annotations

from tools.backends.flash.base import FlashBackend
from tools.backends.flash.openocd_backend import OpenOcdFlashBackend
from tools.backends.flash.pyocd_backend import PyOcdFlashBackend

# ── Registry ───────────────────────────────────────────────────────────
_FLASH_BACKENDS: dict[str, type[FlashBackend]] = {
    "pyocd": PyOcdFlashBackend,
    "openocd": OpenOcdFlashBackend,
}


def register_flash_backend(name: str, cls: type[FlashBackend]) -> None:
    """Register a custom flash backend (e.g. for J-Link or probe-rs)."""
    _FLASH_BACKENDS[name] = cls


def get_flash_backend(name: str) -> type[FlashBackend]:
    """Look up a flash backend by name, raising ValueError if unknown."""
    try:
        return _FLASH_BACKENDS[name]
    except KeyError:
        available = ", ".join(sorted(_FLASH_BACKENDS))
        raise ValueError(f"unknown flash backend '{name}'; available: {available}") from None


__all__ = [
    "FlashBackend",
    "OpenOcdFlashBackend",
    "PyOcdFlashBackend",
    "register_flash_backend",
    "get_flash_backend",
]

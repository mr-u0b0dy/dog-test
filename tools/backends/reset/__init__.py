# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
"""Reset backend registry — maps backend names to classes."""
from __future__ import annotations

from tools.backends.reset.base import ResetBackend
from tools.backends.reset.openocd_reset import OpenOcdResetBackend
from tools.backends.reset.pyocd_reset import PyOcdResetBackend

# ── Registry ───────────────────────────────────────────────────────────
_RESET_BACKENDS: dict[str, type[ResetBackend]] = {
    "pyocd": PyOcdResetBackend,
    "openocd": OpenOcdResetBackend,
}


def register_reset_backend(name: str, cls: type[ResetBackend]) -> None:
    """Register a custom reset backend (e.g. for J-Link or probe-rs)."""
    _RESET_BACKENDS[name] = cls


def get_reset_backend(name: str) -> type[ResetBackend]:
    """Look up a reset backend by name, raising ValueError if unknown."""
    try:
        return _RESET_BACKENDS[name]
    except KeyError:
        available = ", ".join(sorted(_RESET_BACKENDS))
        raise ValueError(f"unknown reset backend '{name}'; available: {available}") from None


__all__ = [
    "ResetBackend",
    "OpenOcdResetBackend",
    "PyOcdResetBackend",
    "register_reset_backend",
    "get_reset_backend",
]

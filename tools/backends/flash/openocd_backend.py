# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
from __future__ import annotations

import subprocess
from pathlib import Path

from tools.backends.flash.base import FlashBackend


class OpenOcdFlashBackend(FlashBackend):
    def __init__(self, interface_cfg: str | None, target_cfg: str | None) -> None:
        self.interface_cfg = interface_cfg
        self.target_cfg = target_cfg

    def flash(self, firmware_path: Path) -> None:
        if not self.interface_cfg or not self.target_cfg:
            raise RuntimeError("openocd backend requires --openocd-interface and --openocd-target")

        cmd = [
            "openocd",
            "-f",
            self.interface_cfg,
            "-f",
            self.target_cfg,
            "-c",
            f"program {firmware_path} verify reset exit",
        ]
        result = subprocess.run(cmd, capture_output=True)
        if result.returncode != 0:
            detail = (
                result.stderr.decode(errors="replace").strip()
                or result.stdout.decode(errors="replace").strip()
            )
            raise RuntimeError(f"OpenOCD flash failed (exit {result.returncode}): {detail}")

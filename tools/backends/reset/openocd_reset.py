# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
from __future__ import annotations

import subprocess

from tools.backends.reset.base import ResetBackend


class OpenOcdResetBackend(ResetBackend):
    def __init__(self, interface_cfg: str | None, target_cfg: str | None) -> None:
        self.interface_cfg = interface_cfg
        self.target_cfg = target_cfg

    def _run(self, command: str) -> None:
        if not self.interface_cfg or not self.target_cfg:
            raise RuntimeError("openocd reset requires --openocd-interface and --openocd-target")

        cmd = [
            "openocd",
            "-f",
            self.interface_cfg,
            "-f",
            self.target_cfg,
            "-c",
            f"init; {command}; shutdown",
        ]
        result = subprocess.run(cmd, capture_output=True)
        if result.returncode != 0:
            detail = (
                result.stderr.decode(errors="replace").strip()
                or result.stdout.decode(errors="replace").strip()
            )
            raise RuntimeError(
                f"OpenOCD reset command failed: {command} "
                f"(exit {result.returncode}): {detail}"
            )

    def soft_reset(self) -> None:
        self._run("reset run")

    def hard_reset(self) -> None:
        self._run("reset halt; reset run")

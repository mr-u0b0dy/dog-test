from __future__ import annotations

import subprocess
from typing import Optional

from tools.backends.reset.base import ResetBackend


class OpenOcdResetBackend(ResetBackend):
    def __init__(self, interface_cfg: Optional[str], target_cfg: Optional[str]) -> None:
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
        result = subprocess.run(cmd, check=False)
        if result.returncode != 0:
            raise RuntimeError(f"OpenOCD reset command failed: {command}")

    def soft_reset(self) -> None:
        self._run("reset run")

    def hard_reset(self) -> None:
        self._run("reset halt; reset run")

# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
from __future__ import annotations

import subprocess

from tools.backends.reset.base import ResetBackend


class PyOcdResetBackend(ResetBackend):
    def __init__(self, board_id: str | None) -> None:
        self.board_id = board_id

    def _run(self, command: str) -> None:
        cmd = ["pyocd", "cmd", "-c", command]
        if self.board_id:
            cmd.extend(["-u", self.board_id])
        result = subprocess.run(cmd, capture_output=True)
        if result.returncode != 0:
            detail = (
                result.stderr.decode(errors="replace").strip()
                or result.stdout.decode(errors="replace").strip()
            )
            raise RuntimeError(
                f"pyOCD reset command failed: {command} "
                f"(exit {result.returncode}): {detail}"
            )

    def soft_reset(self) -> None:
        self._run("reset")

    def hard_reset(self) -> None:
        self._run("reset halt; reset")

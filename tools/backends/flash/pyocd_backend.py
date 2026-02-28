from __future__ import annotations

import subprocess
from pathlib import Path
from typing import Optional

from tools.backends.flash.base import FlashBackend


class PyOcdFlashBackend(FlashBackend):
    def __init__(self, board_id: Optional[str]) -> None:
        self.board_id = board_id

    def flash(self, firmware_path: Path) -> None:
        cmd = ["pyocd", "flash", str(firmware_path)]
        if self.board_id:
            cmd.extend(["-u", self.board_id])
        result = subprocess.run(cmd, capture_output=True)
        if result.returncode != 0:
            detail = result.stderr.decode(errors="replace").strip() or result.stdout.decode(errors="replace").strip()
            raise RuntimeError(f"pyOCD flash failed (exit {result.returncode}): {detail}")

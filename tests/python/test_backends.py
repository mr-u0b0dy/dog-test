# SPDX-License-Identifier: Apache-2.0
# Copyright 2026 dog-test contributors
"""Mock tests for flash and reset backends."""
from __future__ import annotations

from pathlib import Path
from unittest.mock import MagicMock, patch

import pytest

from tools.backends.flash.pyocd_backend import PyOcdFlashBackend
from tools.backends.flash.openocd_backend import OpenOcdFlashBackend
from tools.backends.reset.pyocd_reset import PyOcdResetBackend
from tools.backends.reset.openocd_reset import OpenOcdResetBackend
from tools.backends.flash import get_flash_backend, register_flash_backend
from tools.backends.reset import get_reset_backend, register_reset_backend


# ── Flash backend tests ───────────────────────────────────────────────


class TestPyOcdFlashBackend:
    @patch("tools.backends.flash.pyocd_backend.subprocess.run")
    def test_flash_success(self, mock_run: MagicMock):
        mock_run.return_value = MagicMock(returncode=0)
        backend = PyOcdFlashBackend(board_id=None)
        backend.flash(Path("/tmp/firmware.elf"))
        mock_run.assert_called_once()
        cmd = mock_run.call_args[0][0]
        assert "pyocd" in cmd
        assert "flash" in cmd
        assert "/tmp/firmware.elf" in cmd

    @patch("tools.backends.flash.pyocd_backend.subprocess.run")
    def test_flash_with_board_id(self, mock_run: MagicMock):
        mock_run.return_value = MagicMock(returncode=0)
        backend = PyOcdFlashBackend(board_id="ABC123")
        backend.flash(Path("/tmp/firmware.elf"))
        cmd = mock_run.call_args[0][0]
        assert "-u" in cmd
        assert "ABC123" in cmd

    @patch("tools.backends.flash.pyocd_backend.subprocess.run")
    def test_flash_failure_raises(self, mock_run: MagicMock):
        mock_run.return_value = MagicMock(
            returncode=1,
            stderr=b"flash error",
            stdout=b"",
        )
        backend = PyOcdFlashBackend(board_id=None)
        with pytest.raises(RuntimeError, match="pyOCD flash failed"):
            backend.flash(Path("/tmp/firmware.elf"))


class TestOpenOcdFlashBackend:
    @patch("tools.backends.flash.openocd_backend.subprocess.run")
    def test_flash_success(self, mock_run: MagicMock):
        mock_run.return_value = MagicMock(returncode=0)
        backend = OpenOcdFlashBackend("interface/cmsis-dap.cfg", "target/stm32f4x.cfg")
        backend.flash(Path("/tmp/firmware.elf"))
        mock_run.assert_called_once()
        cmd = mock_run.call_args[0][0]
        assert "openocd" in cmd

    def test_flash_missing_config_raises(self):
        backend = OpenOcdFlashBackend(None, None)
        with pytest.raises(RuntimeError, match="openocd backend requires"):
            backend.flash(Path("/tmp/firmware.elf"))

    @patch("tools.backends.flash.openocd_backend.subprocess.run")
    def test_flash_failure_raises(self, mock_run: MagicMock):
        mock_run.return_value = MagicMock(
            returncode=1,
            stderr=b"openocd error",
            stdout=b"",
        )
        backend = OpenOcdFlashBackend("interface/cmsis-dap.cfg", "target/stm32f4x.cfg")
        with pytest.raises(RuntimeError, match="OpenOCD flash failed"):
            backend.flash(Path("/tmp/firmware.elf"))


# ── Reset backend tests ───────────────────────────────────────────────


class TestPyOcdResetBackend:
    @patch("tools.backends.reset.pyocd_reset.subprocess.run")
    def test_soft_reset(self, mock_run: MagicMock):
        mock_run.return_value = MagicMock(returncode=0)
        backend = PyOcdResetBackend(board_id=None)
        backend.soft_reset()
        mock_run.assert_called_once()

    @patch("tools.backends.reset.pyocd_reset.subprocess.run")
    def test_hard_reset(self, mock_run: MagicMock):
        mock_run.return_value = MagicMock(returncode=0)
        backend = PyOcdResetBackend(board_id="XYZ")
        backend.hard_reset()
        cmd = mock_run.call_args[0][0]
        assert "-u" in cmd
        assert "XYZ" in cmd

    @patch("tools.backends.reset.pyocd_reset.subprocess.run")
    def test_reset_failure_raises(self, mock_run: MagicMock):
        mock_run.return_value = MagicMock(
            returncode=1,
            stderr=b"reset error",
            stdout=b"",
        )
        backend = PyOcdResetBackend(board_id=None)
        with pytest.raises(RuntimeError, match="pyOCD reset command failed"):
            backend.soft_reset()


class TestOpenOcdResetBackend:
    @patch("tools.backends.reset.openocd_reset.subprocess.run")
    def test_soft_reset(self, mock_run: MagicMock):
        mock_run.return_value = MagicMock(returncode=0)
        backend = OpenOcdResetBackend("interface/cmsis-dap.cfg", "target/stm32f4x.cfg")
        backend.soft_reset()
        mock_run.assert_called_once()

    def test_reset_missing_config_raises(self):
        backend = OpenOcdResetBackend(None, None)
        with pytest.raises(RuntimeError, match="openocd reset requires"):
            backend.soft_reset()


# ── Backend registry tests ────────────────────────────────────────────


class TestBackendRegistry:
    def test_get_flash_backend_pyocd(self):
        cls = get_flash_backend("pyocd")
        assert cls is PyOcdFlashBackend

    def test_get_flash_backend_openocd(self):
        cls = get_flash_backend("openocd")
        assert cls is OpenOcdFlashBackend

    def test_get_flash_backend_unknown_raises(self):
        with pytest.raises(ValueError, match="unknown flash backend"):
            get_flash_backend("jlink")

    def test_register_custom_flash_backend(self):
        from tools.backends.flash.base import FlashBackend

        class CustomFlash(FlashBackend):
            def flash(self, firmware_path):
                pass

        register_flash_backend("custom", CustomFlash)
        assert get_flash_backend("custom") is CustomFlash

    def test_get_reset_backend_pyocd(self):
        cls = get_reset_backend("pyocd")
        assert cls is PyOcdResetBackend

    def test_get_reset_backend_unknown_raises(self):
        with pytest.raises(ValueError, match="unknown reset backend"):
            get_reset_backend("probe-rs")

    def test_register_custom_reset_backend(self):
        from tools.backends.reset.base import ResetBackend

        class CustomReset(ResetBackend):
            def soft_reset(self):
                pass

            def hard_reset(self):
                pass

        register_reset_backend("custom", CustomReset)
        assert get_reset_backend("custom") is CustomReset

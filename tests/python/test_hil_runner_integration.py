"""Integration tests for hil_runner helpers: load_test_plan, write_junit_xml,
and extended parse_ht_event_line coverage."""
from __future__ import annotations

import json
from pathlib import Path
from xml.etree import ElementTree as ET

from tools.hil_runner import TestResult, load_test_plan, parse_ht_event_line, write_junit_xml

# ── load_test_plan tests ──────────────────────────────────────────────


class TestLoadTestPlan:
    def _write_plan(self, plan: dict, tmpdir: Path) -> str:
        path = tmpdir / "plan.json"
        path.write_text(json.dumps(plan), encoding="utf-8")
        return str(path)

    def test_basic_plan(self, tmp_path: Path):
        plan = {
            "tests": [
                {"name": "t1", "firmware": "/dev/null", "reset_mode": "soft"},
                {"name": "t2", "firmware": "/dev/null"},
            ]
        }
        path = self._write_plan(plan, tmp_path)
        items = load_test_plan(path)
        assert len(items) == 2
        assert items[0].name == "t1"
        assert items[0].reset_mode == "soft"
        assert items[0].firmware == Path("/dev/null")
        assert items[1].name == "t2"
        assert items[1].reset_mode == "none"
        assert items[1].monitor is None

    def test_plan_with_monitor(self, tmp_path: Path):
        plan = {
            "tests": [
                {
                    "name": "mon",
                    "firmware": "/dev/null",
                    "monitor": {
                        "protocol": "uart",
                        "channels": "ch0=tx",
                        "trigger": "",
                        "timeout_ms": 2000,
                        "expect": {"min_items": 1},
                    },
                }
            ]
        }
        path = self._write_plan(plan, tmp_path)
        items = load_test_plan(path)
        assert items[0].monitor is not None
        assert items[0].monitor.protocol == "uart"
        assert items[0].monitor.timeout_ms == 2000

    def test_plan_empty_tests(self, tmp_path: Path):
        plan = {"tests": []}
        path = self._write_plan(plan, tmp_path)
        items = load_test_plan(path)
        assert items == []

    def test_plan_missing_name_defaults(self, tmp_path: Path):
        plan = {"tests": [{"firmware": "/dev/null"}]}
        path = self._write_plan(plan, tmp_path)
        items = load_test_plan(path)
        assert items[0].name == "unnamed_test"


# ── write_junit_xml tests ─────────────────────────────────────────────


class TestWriteJunitXml:
    def test_basic_output(self, tmp_path: Path):
        results = [
            TestResult(name="t1", status="passed", duration_s=0.5),
            TestResult(name="t2", status="failed", duration_s=1.0, message="boom"),
            TestResult(name="t3", status="skipped", duration_s=0.0, message="no hw"),
            TestResult(name="t4", status="error", duration_s=0.1, message="exc"),
        ]
        xml_path = str(tmp_path / "results.xml")
        write_junit_xml(results, xml_path)

        tree = ET.parse(xml_path)
        root = tree.getroot()
        assert root.tag == "testsuite"
        assert root.attrib["tests"] == "4"
        assert root.attrib["failures"] == "1"
        assert root.attrib["errors"] == "1"
        assert root.attrib["skipped"] == "1"

        cases = root.findall("testcase")
        assert len(cases) == 4
        assert cases[0].attrib["name"] == "t1"
        assert cases[1].find("failure") is not None
        assert cases[1].find("failure").attrib["message"] == "boom"
        assert cases[2].find("skipped") is not None
        assert cases[3].find("error") is not None

    def test_all_passed(self, tmp_path: Path):
        results = [TestResult(name="ok", status="passed", duration_s=0.01)]
        xml_path = str(tmp_path / "pass.xml")
        write_junit_xml(results, xml_path)
        tree = ET.parse(xml_path)
        root = tree.getroot()
        assert root.attrib["failures"] == "0"
        assert root.attrib["errors"] == "0"

    def test_empty_results(self, tmp_path: Path):
        xml_path = str(tmp_path / "empty.xml")
        write_junit_xml([], xml_path)
        tree = ET.parse(xml_path)
        root = tree.getroot()
        assert root.attrib["tests"] == "0"


# ── Extended parse_ht_event_line coverage ─────────────────────────────


class TestParseHtEventLineExtended:
    def test_fail_event(self):
        line = (
            'HT_EVENT fail name=my_test reason=assert file=main.c line=17 '
            'expr="x == y" msg="expected x == y" expected=1 actual=2'
        )
        result = parse_ht_event_line(line)
        assert result["name"] == "my_test"
        assert result["reason"] == "assert"
        assert result["file"] == "main.c"
        assert result["line"] == "17"
        assert result["expr"] == "x == y"
        assert result["expected"] == "1"
        assert result["actual"] == "2"

    def test_skip_event(self):
        line = 'HT_EVENT skip name=cond_test reason="hardware not connected"'
        result = parse_ht_event_line(line)
        assert result["name"] == "cond_test"
        assert result["reason"] == "hardware not connected"

    def test_start_event_with_tags(self):
        line = 'HT_EVENT start name=tagged_uart reset=1 monitor=none tags="uart,smoke"'
        result = parse_ht_event_line(line)
        assert result["name"] == "tagged_uart"
        assert result["reset"] == "1"
        assert result["monitor"] == "none"
        assert result["tags"] == "uart,smoke"

    def test_start_event_without_tags(self):
        line = "HT_EVENT start name=basic reset=0 monitor=none"
        result = parse_ht_event_line(line)
        assert result["name"] == "basic"
        assert "tags" not in result

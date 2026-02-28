"""Unit tests for HT_EVENT line parsing logic in hil_runner."""
from __future__ import annotations

from tools.hil_runner import parse_ht_event_line


class TestParseHtEventLine:
    def test_simple_summary(self):
        line = "HT_EVENT summary total=3 passed=2 failed=1 skipped=0"
        result = parse_ht_event_line(line)
        assert result["total"] == "3"
        assert result["passed"] == "2"
        assert result["failed"] == "1"
        assert result["skipped"] == "0"

    def test_pass_event(self):
        line = "HT_EVENT pass name=my_test"
        result = parse_ht_event_line(line)
        assert result["name"] == "my_test"

    def test_quoted_values(self):
        line = 'HT_EVENT fail name=my_test msg="expected 1 == 2"'
        result = parse_ht_event_line(line)
        assert result["name"] == "my_test"
        assert result["msg"] == "expected 1 == 2"

    def test_escaped_quote_in_value(self):
        line = 'HT_EVENT fail name=t msg="has \\"quotes\\" inside"'
        result = parse_ht_event_line(line)
        assert result["msg"] == 'has "quotes" inside'

    def test_empty_line(self):
        result = parse_ht_event_line("")
        assert result == {}

    def test_minimal_event(self):
        result = parse_ht_event_line("HT_EVENT boot")
        assert result == {}

    def test_numeric_values(self):
        line = "HT_EVENT fail name=t line=42 expected=100 actual=200"
        result = parse_ht_event_line(line)
        assert result["line"] == "42"
        assert result["expected"] == "100"
        assert result["actual"] == "200"

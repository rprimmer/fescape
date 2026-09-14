"""CLI regression tests; no third-party dependencies."""
import itertools
import os
from pathlib import Path
import random
import subprocess
import sys
import tempfile
import unittest

BINARY = str(Path(sys.argv.pop(1)).resolve())


def expected(data, repeats=False, octal=False, newline=False):
    result = bytearray()
    for value, group in itertools.groupby(data):
        count = sum(1 for _ in group)
        if 32 <= value <= 126 or (value == 10 and not newline):
            result.extend(bytes([value]) * count)
        else:
            code = (f"<{value:03o}>" if octal else f"<0x{value:02x}>").encode()
            if repeats:
                result.extend(code)
                if count > 1:
                    result.extend(f"[{count}]".encode())
            else:
                result.extend(code * count)
    return bytes(result)


class FilterTests(unittest.TestCase):
    def run_filter(self, *args, data=b"", **kwargs):
        return subprocess.run([BINARY, *args], input=data, capture_output=True, **kwargs)

    def assert_output(self, data, output, *args):
        result = self.run_filter(*args, data=data)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stderr, b"")
        self.assertEqual(result.stdout, output)

    def test_empty_and_printable(self):
        self.assert_output(b"", b"")
        self.assert_output(b"hello ~\n", b"hello ~\n")

    def test_all_bytes_and_option_combinations(self):
        rng = random.Random(2026)
        data = bytes(range(256)) + b"\n\nA\0\0\xff\xff\n\n"
        data += bytes(rng.randrange(256) for _ in range(4096))
        for repeats, octal, newline in itertools.product([False, True], repeat=3):
            args = [flag for flag, enabled in zip(
                ["-r", "-o", "-n"], [repeats, octal, newline]) if enabled]
            with self.subTest(args=args):
                self.assert_output(data, expected(data, repeats, octal, newline), *args)

    def test_newline_runs_at_eof(self):
        self.assert_output(b"\n\n\n", b"\n\n\n", "-r")
        self.assert_output(b"\n\n\n", b"<0x0a>[3]", "-rn")
        self.assert_output(b"\n\n\n", b"<012>[3]", "-rno")
        self.assert_output(b"\n\nA", b"\n\nA", "-r")

    def test_long_run(self):
        self.assert_output(b"\0" * 100000, b"<0x00>[100000]", "-r")

    def test_long_options(self):
        self.assert_output(b"\0\0\n", b"<000>[2]<012>",
                           "--repeats", "--octal", "--newline")

    def test_files_and_stdin_in_order(self):
        with tempfile.TemporaryDirectory() as directory:
            first, last = Path(directory) / "first", Path(directory) / "last"
            first.write_bytes(b"A\0")
            last.write_bytes(b"B\n")
            result = self.run_filter(str(first), "-", str(last), data=b"middle\t")
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stdout, b"A<0x00>middle<0x09>B\n")
            self.assertEqual(self.run_filter(str(first)).stdout,
                             self.run_filter(data=first.read_bytes()).stdout)

    def test_empty_file_and_repeat_boundaries(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "input"
            path.write_bytes(b"")
            self.assert_output(b"", b"", str(path))
            path.write_bytes(b"\0\0")
            self.assert_output(b"", b"<0x00>[2]<0x00>[2]", "-r", str(path), str(path))

    def test_dash_filename_and_repeated_stdin(self):
        with tempfile.TemporaryDirectory() as directory:
            (Path(directory) / "-file").write_bytes(b"ok")
            result = self.run_filter("--", "-file", cwd=directory)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stdout, b"ok")
        self.assert_output(b"once", b"once", "-", "-")

    def test_missing_file_reports_failure_and_continues(self):
        with tempfile.TemporaryDirectory() as directory:
            missing = str(Path(directory) / "missing")
            result = self.run_filter(missing, "-", data=b"still processed")
            self.assertNotEqual(result.returncode, 0)
            self.assertIn(missing.encode(), result.stderr)
            self.assertEqual(result.stdout, b"still processed")

    def test_read_error(self):
        with tempfile.TemporaryDirectory() as directory:
            result = self.run_filter(directory)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn(directory.encode(), result.stderr)

    def test_buffered_output_failure(self):
        # A read-only descriptor reliably rejects the final buffered write.
        with open(os.devnull, "rb") as output:
            result = subprocess.run([BINARY], input=b"small buffered write",
                                    stdout=output, stderr=subprocess.PIPE)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn(b"standard output", result.stderr)

    def test_immediate_output_failure(self):
        with open(os.devnull, "rb") as output:
            result = subprocess.run([BINARY], input=b"\0" * 100000,
                                    stdout=output, stderr=subprocess.PIPE)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn(b"standard output", result.stderr)

    def test_help_and_invalid_option(self):
        for option in ["-h", "--help"]:
            result = self.run_filter(option)
            self.assertEqual(result.returncode, 0)
            self.assertIn(b"Usage:", result.stdout)
            self.assertEqual(result.stderr, b"")
        for option in ["-z", "--unknown", "--octal=yes"]:
            result = self.run_filter(option)
            self.assertNotEqual(result.returncode, 0)
            self.assertEqual(result.stdout, b"")
            self.assertIn(b"invalid option", result.stderr)


if __name__ == "__main__":
    unittest.main()

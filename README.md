# fescape

[fescape](https://github.com/rprimmer/fescape) displays binary input safely in a
terminal by replacing non-printable bytes with hexadecimal or octal codes.
Input and output are streamed, so it also works in pipelines with bounded memory.

```sh
make
printf 'hello\tworld\n' | ./build/fescape
# hello<0x09>world
printf '\000\000\000\n' | ./build/fescape -rn
# <0x00>[3]<0x0a>
```

## Usage

```text
fescape [OPTIONS] [FILE ...]
```

| Option | Effect |
| --- | --- |
| `-h`, `--help` | Show help and exit |
| `-r`, `--repeats` | Collapse repeated escaped bytes into `<code>[count]` |
| `-n`, `--newline` | Escape LF bytes; otherwise preserve every newline |
| `-o`, `--octal` | Use `<000>` through `<377>` instead of `<0x00>` through `<0xff>` |

Provide options before filenames. With no files, input comes from stdin. A `-`
operand reads stdin at that position; `--` ends option parsing for filenames
beginning with a dash. Multiple files are processed in order without inserted
separators. Repeat counts include the first byte and reset for each input operand.
Printable ASCII bytes (`0x20`–`0x7e`) pass through unchanged; all other bytes are
escaped except LF by default. UTF-8 is processed as individual bytes, not Unicode
characters. Output does not depend on the locale.

```sh
./build/fescape -ro binary.dat
./build/fescape file1 - file2 < another-file
./build/fescape -- -filename
```

Missing or unreadable inputs produce diagnostics on stderr and processing
continues with subsequent inputs. Read, write, and final output-flush failures
return a nonzero exit status. Success returns zero.

## Build and install

A C11 compiler and GNU Make are required. The CLI uses `getopt_long`, available
on macOS and Linux. Python 3 is required only for tests. Doxygen and Pandoc are
optional documentation tools.

| Command | Result |
| --- | --- |
| `make` or `make release` | Optimized executable at `build/fescape` |
| `make debug` | Separate unoptimized executable at `build/debug/fescape` |
| `make test` | Run CLI regression tests |
| `make manpage` | Manual at `build/man/fescape.1` |
| `make docs` | API documentation at `build/docs/html/index.html` |
| `make markdown` | README as `build/readme.html` |
| `make install` | Install executable and manual under `/usr/local` |
| `make clean` | Remove compiler intermediates, retaining executables and docs |
| `make distclean` | Remove the generated `build/` directory |

Compiler and linker settings accept standard overrides: `CC`, `CPPFLAGS`,
`CFLAGS`, `LDFLAGS`, and `LDLIBS`. Debug builds append `-O0 -g3` to `CFLAGS`.
Header dependencies are generated automatically. Release and debug builds can
run in parallel; documentation is independent of compilation.

Install to a writable prefix, or stage a package without changing the system:

```sh
make install PREFIX="$HOME/.local"
make install DESTDIR=/tmp/fescape-package PREFIX=/usr/local
```

Add the chosen prefix's `bin` directory to `PATH` as needed. The manual installs
under `share/man/man1` beneath the same prefix.

## Source layout

- [src/main.c](src/main.c): option parsing, stream ownership, and diagnostics.
- [src/fescape.c](src/fescape.c): byte conversion and repeat compression.
- [src/fescape.h](src/fescape.h): filter options and return-value contract.
- [tests/test_fescape.py](tests/test_fescape.py): byte-level and CLI regression tests.
- [man/fescape.1.in](man/fescape.1.in): maintained manual source.
- [makefile](makefile): compilation, testing, documentation, and staged installation.

## Refactor notes

The September 2026 refactor removes unused general-purpose system utilities and
macOS-specific program-name handling. It fixes missing repeat counts for escaped
newlines at EOF, preserves unescaped blank lines with `-r`, detects read and
buffered write failures, and continues after an input file fails.

Named files previously gained an extra newline; they now produce the same bytes
as stdin. This also means an empty file produces no output. Generated binaries,
manuals, and documentation now belong under `build/` rather than in source
control. `make markdown` generates HTML; the previous PDF documentation exports
are no longer build prerequisites or maintained deliverables.

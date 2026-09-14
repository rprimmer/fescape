.DEFAULT_GOAL := all

CC ?= cc
CPPFLAGS ?=
CFLAGS ?= -O2 -g
LDFLAGS ?=
LDLIBS ?=
PYTHON ?= python3
PREFIX ?= /usr/local
DESTDIR ?=
DOXYGEN ?= doxygen
PANDOC ?= pandoc

WARNINGS := -std=c11 -Wall -Wextra -Wpedantic
SOURCES := src/main.c src/fescape.c
OBJECTS := $(SOURCES:src/%.c=build/%.o)
DEBUG_OBJECTS := $(SOURCES:src/%.c=build/debug/%.o)

.PHONY: all release debug test docs markdown manpage install clean distclean help
all: build/fescape
release: all
debug: build/debug/fescape

build/fescape: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

build/debug/fescape: $(DEBUG_OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

build/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNINGS) -MMD -MP -c $< -o $@

build/debug/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNINGS) -O0 -g3 -MMD -MP -c $< -o $@

-include $(OBJECTS:.o=.d) $(DEBUG_OBJECTS:.o=.d)

test: build/fescape
	$(PYTHON) tests/test_fescape.py $(CURDIR)/build/fescape

manpage: build/man/fescape.1
build/man/fescape.1: man/fescape.1.in
	@mkdir -p $(@D)
	cp $< $@

docs:
	@mkdir -p build/docs
	$(DOXYGEN) Doxyfile

markdown: build/readme.html
build/readme.html: README.md
	@mkdir -p $(@D)
	$(PANDOC) --standalone $< -o $@

install: all manpage
	install -d "$(DESTDIR)$(PREFIX)/bin" "$(DESTDIR)$(PREFIX)/share/man/man1"
	install -m 755 build/fescape "$(DESTDIR)$(PREFIX)/bin/fescape"
	install -m 644 build/man/fescape.1 "$(DESTDIR)$(PREFIX)/share/man/man1/fescape.1"

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) $(DEBUG_OBJECTS) $(DEBUG_OBJECTS:.o=.d)

distclean:
	rm -rf build

help:
	@echo 'make / release  Build build/fescape'
	@echo 'make debug      Build build/debug/fescape'
	@echo 'make test       Run regression tests (Python 3)'
	@echo 'make manpage    Copy the manual into build/man/'
	@echo 'make docs       Generate HTML API documentation (Doxygen)'
	@echo 'make markdown   Generate build/readme.html (Pandoc)'
	@echo 'make install    Install executable and manual; supports PREFIX and DESTDIR'
	@echo 'make clean      Remove compiler intermediates, retaining executables and docs'
	@echo 'make distclean  Remove all generated build output'

ifndef MAKE_VERSION
$(error This makefile expects GNU Make.)
endif

PROGRAM 		:= fescape
INSTALL_DIR		:= /usr/local/bin/
INSTALL_ARGS	:= -m 755
CC      		:= clang
CFLAGS			:= -g -Wall -Wextra -DDEBUG $(INCLUDES)
LDFLAGS			?=
INCLUDES		?=
LIBS			?=
SRCDIR     		:= src
OBJDIR     		:= obj
DOCDIR  		:= docs
MANDIR			:= man
BINDIR  		:= bin
BIN     		:= $(BINDIR)/$(PROGRAM)
SRCS    		= $(wildcard $(SRCDIR)/*.c)
OBJS    		= $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
MANPAGE 		:= $(PROGRAM).1
PANDOC			:= $(shell command -v pandoc)
INSTALL			:= $(shell command -v install)
DOXYGEN 		:= $(shell command -v doxygen)

all: $(BINDIR) $(BIN)

$(BINDIR):
	mkdir -p $(SRCDIR) $(OBJDIR) $(BINDIR) $(DOCDIR) $(MANDIR)

$(BIN): $(OBJS)| $(OBJDIR) $(BINDIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: release markdown docs clean install manpage help

help:
	@echo "make bin --> create project directory structure"
	@echo "make --> create debug version"
	@echo "make release --> create final version for release"
	@echo "make markdown --> convert markdown files"
	@echo "make manpage --> create manpage"
	@echo "make docs --> create API doc"
	@echo "make install --> install executable"
	@echo "make clean --> removed unneeded files"
	@echo "make help --> display this help text"

release: CFLAGS := -Wall -Wextra -O2 -DNDEBUG $(INCLUDES)
release: clean markdown docs manpage $(BIN)
	@echo "Release build complete."

markdown: README.md
ifndef PANDOC
	$(error "pandoc(1) not found.")
endif
	@$(PANDOC) README.md -o readme.pdf

docs:
ifndef DOXYGEN
	$(error "doxygen(1) not found.")
endif
	$(DOXYGEN) Doxyfile > makefile.out 2>&1
	$(MAKE) -C $(DOCDIR)/latex >> makefile.out 2>&1
	ln -sf docs/html/index.html $(PROGRAM)-apidoc.html
	cp docs/latex/refman.pdf ./$(PROGRAM)-apidoc.pdf

manpage:
	sed 's/@MAN_DATE@/$(shell date "+%B %d, %Y")/' $(MANDIR)/$(PROGRAM).1.in > $(MANDIR)/$(MANPAGE)

clean:
	@if [ -z "$(BINDIR)" ] || [ -z "$(OBJDIR)" ]; then \
		echo "Error: BINDIR or OBJDIR is unset or empty; clean operation aborted to prevent potential harm."; \
		exit 1; \
	fi
	@echo "Removing objects and binaries..."
	@find ./$(BINDIR) -type f -exec rm -f {} +
	@find ./$(OBJDIR) -type f -exec rm -f {} +
	@rm -rf *.dSYM
	@rm -f readme.pdf $(PROGRAM)-apidoc.*
	@echo "Clean operation completed safely."

install:
ifndef INSTALL
	$(error "install(1) not found.")
endif
	$(INSTALL) $(INSTALL_ARGS) $(BINDIR)/$(PROGRAM) $(INSTALL_DIR)
	@echo "Installation successful. $(INSTALL_DIR)$(PROGRAM) can be used"

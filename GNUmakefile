# make bin -> create project directory structre
# make release -> make final version for release
# make markdown -> convert markdown files
# make docs --> create API doc
# make install --> install executable

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
SRC     		:= src
OBJ     		:= obj
BINDIR  		:= bin
DOCDIR  		:= docs
MANDIR			:= man
SRCS    		= $(wildcard $(SRC)/*.c)
OBJS    		= $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
BIN     		= $(BINDIR)/$(PROGRAM)
MANPAGE 		= $(PROGRAM).1
PANDOC			:= $(shell command -v pandoc)
INSTALL			:= $(shell command -v install)
DOXYGEN 		:= /Applications/Doxygen.app/Contents/Resources/doxygen

all: $(BINDIR) $(BIN)

$(BINDIR):
	mkdir -p $(SRC) $(OBJ) $(BINDIR) $(DOCDIR)

$(BIN): $(OBJS) $(OBJ) $(BINDIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.c $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: release markdown docs clean install manpage

release: CFLAGS=-Wall -Wextra -O2 -DNDEBUG $(INCLUDES)
release: clean markdown docs manpage $(BIN)
	@echo "Release build complete."

markdown: README.md
ifndef PANDOC
	$(error "pandoc(1) nof found.")
endif
	@$(PANDOC) README.md -o readme.pdf

$(DOCDIR):
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
	$(RM) -rf $(BINDIR)/* $(OBJ)/* *.dSYM readme.pdf $(PROGRAM)-apidoc.*

install:
ifndef INSTALL
	$(error "install(1) not found.")
endif
	$(INSTALL) $(INSTALL_ARGS) $(PROGRAM) $(INSTALL_DIR)
	@echo "Installation successful, $(INSTALL_DIR)/$(PROGRAM) can be used"

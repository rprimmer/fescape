# make bin -> create project directory structre
# make release -> make final version for release
# make markdown -> convert markdown files
# make docs --> create API doc

PROGRAM = fescape
CC      = clang
CFLAGS  = -g -Wall -Wextra -DDEBUG
LDFLAGS =
SRC     = src
OBJ     = obj
BINDIR  = bin
DOCDIR  = docs
SRCS    = $(wildcard $(SRC)/*.c)
OBJS    = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
BIN     = $(BINDIR)/$(PROGRAM)
MANPAGE = $(PROGRAM).1
PANDOC  = pandoc
DOXYGEN = /Applications/Doxygen.app/Contents/Resources/doxygen

all: $(BINDIR) $(BIN)

$(BINDIR):
	mkdir -p $(SRC) $(OBJ) $(BINDIR) $(DOCDIR)

$(BIN): $(OBJS) $(OBJ) $(BINDIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.c $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: release markdown docs clean

release: CFLAGS=-Wall -Wextra -O2 -DNDEBUG
release: clean markdown docs $(BIN)
	@echo "Release build complete."

markdown: README.md
	$(PANDOC) README.md -o readme.pdf
	# $(PANDOC) $(PROGRAM)-design-spec.md -o $(PROGRAM)-design-spec.pdf

docs: $(DOCDIR)
	$(DOXYGEN) Doxyfile
	cp docs/latex/refman.pdf ./$(PROGRAM)-apidoc.pdf

clean:
	$(RM) -rf $(BINDIR)/* $(OBJ)/* *.dSYM readme.pdf $(PROGRAM)-design-spec.pdf $(PROGRAM)-apidoc.pdf

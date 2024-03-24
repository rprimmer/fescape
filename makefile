# make bin -> create project directory structre
# make release -> make final version for release
# make markdown -> convert markdown files

PROGRAM = 	fescape
CC  =		clang
CFLAGS =	-g -Wall -Wextra -DDEBUG
LDFLAGS =
SRC = 		src
OBJ = 		obj
SRCS =		$(wildcard $(SRC)/*.c)
OBJS = 		$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
BINDIR = 	bin
DOCDIR = 	docs
BIN = 		$(BINDIR)/$(PROGRAM)
MANPAGE =	$(PROGRAM).1

all: $(BIN)

$(BINDIR):
	mkdir -p $(SRC) $(OBJ) $(BINDIR) $(DOCDIR)

$(BIN): $(OBJS) $(OBJ)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.c $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

release: CFLAGS=-Wall -Wextra -O2 -DNDEBUG
release: clean
release: markdown
release: $(BIN)

markdown: README.md
	pandoc README.md -o readme.pdf
#	pandoc $(PROGRAM)-design-spec.md -o $(PROGRAM)-design-spec.pdf

clean:
	$(RM) -rf $(BINDIR)/* $(OBJ)/* *.dSYM readme.pdf $(PROGRAM)-design.spec.pdf

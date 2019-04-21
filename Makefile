# Makefile

CPPFLAGS = -MMD
CC = gcc -fsanitize=address
CFLAGS = -Wall -Wextra -Wformat=2 -pedantic -std=c99 -O0 -g3
LDFLAGS =
LDLIBS = -lm

SRC = ls.c arg.c scanner.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: ls

ls: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}   # remove object files
	${RM} ${DEP}   # remove dependency files
	${RM} ls       # remove main program

-include ${DEP}

# END

CC      = gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -lgdi32
GAME_DIR = .

########################Project files########################

# Here you add the names of header-source file pairs. E.g. gameControl adds gameControl.h and gameControl.c
_GAME_SOURCE_HEADER_PAIRS = game math room render assets \

# Here on the other hand you add just the headers.
_GAME_JUST_HEADERS = state.h

########################Project files########################

# Prepends paths with game folder
GAME_SOURCE_HEADER_PAIRS = $(patsubst %,$(GAME_DIR)/%,$(_GAME_SOURCE_HEADER_PAIRS))
GAME_JUST_HEADERS = $(patsubst %,$(GAME_DIR)/%,$(_GAME_JUST_HEADERS))

# Adds .h and .o
GAME_HEADERS = $(GAME_SOURCE_HEADER_PAIRS:=.h) $(GAME_JUST_HEADERS)
GAME_OBJECTS = $(GAME_SOURCE_HEADER_PAIRS:=.o) main.o glad.o libglfw3.a

%.o: %.c $(GAME_HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)


.SUFFIXES: .c .o

.PHONY: all clean

game: $(GAME_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	find . -type f -name '*.o' -delete

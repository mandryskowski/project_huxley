CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -pedantic
EMULATE_DIR = emulator
ASSEMBLE_DIR = assembler

########################Project files########################

# Here you add the names of header-source file pairs. E.g. emulateControl adds emulateControl.h and emulateControl.c
_EMULATE_SOURCE_HEADER_PAIRS = emulateControl \
	instructions/branchInstruction instructions/immediateInstruction instructions/registerInstruction instructions/sdtInstruction \
	util/bitwiseInstruction util/emulateUtility util/outputFileGenerator

# Here on the other hand you add just the headers.
_EMULATE_JUST_HEADERS = util/state.h

# Similarly for assemble.
_ASSEMBLE_SOURCE_HEADER_PAIRS = assembleControl \
	instructions/assembleBranch instructions/assembleDPI instructions/assembleSDT instructions/assembleSpecial \
	util/assembleUtility

_ASSEMBLE_JUST_HEADERS =

########################Project files########################

# Prepends paths with emulate/assemble folder
EMULATE_SOURCE_HEADER_PAIRS = $(patsubst %,$(EMULATE_DIR)/%,$(_EMULATE_SOURCE_HEADER_PAIRS))
EMULATE_JUST_HEADERS = $(patsubst %,$(EMULATE_DIR)/%,$(_EMULATE_JUST_HEADERS))

ASSEMBLE_SOURCE_HEADER_PAIRS = $(patsubst %,$(ASSEMBLE_DIR)/%,$(_ASSEMBLE_SOURCE_HEADER_PAIRS))
ASSEMBLE_JUST_HEADERS = $(patsubst %,$(ASSEMBLE_DIR)/%,$(_ASSEMBLE_JUST_HEADERS))

# Adds .h and .o
EMULATE_HEADERS = $(EMULATE_SOURCE_HEADER_PAIRS:=.h) $(EMULATE_JUST_HEADERS)
EMULATE_OBJECTS = $(EMULATE_SOURCE_HEADER_PAIRS:=.o) emulator/emulate.o

ASSEMBLE_HEADERS = $(ASSEMBLE_SOURCE_HEADER_PAIRS:=.h) $(ASSEMBLE_JUST_HEADERS)
ASSEMBLE_OBJECTS = $(ASSEMBLE_SOURCE_HEADER_PAIRS:=.o) assembler/assemble.o

$(EMULATE_DIR)/%.o: %.c $(EMULATE_HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ASSEMBLE_DIR)/%.o: %.c $(ASSEMBLE_HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)


.SUFFIXES: .c .o

.PHONY: all clean

emulate: $(EMULATE_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

assemble: $(ASSEMBLE_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

all: assemble emulate
clean:
	find . -type f -name '*.o' -delete

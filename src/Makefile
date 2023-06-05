CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -pedantic
EMULATE_DIR = emulator
ASSEMBLE_DIR = assembler

_EMULATE_SOURCE_HEADER_PAIRS = emulateControl \
	instructions/branchInstruction instructions/immediateInstruction instructions/registerInstruction instructions/sdtInstruction \
	util/bitwiseInstruction util/emulateUtility util/outputFileGenerator
_EMULATE_JUST_HEADERS = util/state.h

_EMULATE_HEADERS = $(_EMULATE_SOURCE_HEADER_PAIRS:=.h) $(_EMULATE_JUST_HEADERS)
EMULATE_HEADERS = $(patsubst %,$(EMULATE_DIR)/%,$(_EMULATE_HEADERS))

_EMULATE_OBJECTS = $(_EMULATE_SOURCE_HEADER_PAIRS:=.o) emulate.o
EMULATE_OBJECTS = $(patsubst %,$(EMULATE_DIR)/%,$(_EMULATE_OBJECTS))

_ASSEMBLE_SOURCE_HEADER_PAIRS = assembleControl label \
	instructions/assembleBranch instructions/assembleDPI instructions/assembleSDT instructions/assembleSpecial \
	util/assembleUtility
_ASSEMBLE_JUST_HEADERS = 

_ASSEMBLE_HEADERS = $(_ASSEMBLE_SOURCE_HEADER_PAIRS:=.h) $(_ASSEMBLE_JUST_HEADERS)
ASSEMBLE_HEADERS = $(patsubst %,$(ASSEMBLE_DIR)/%,$(_ASSEMBLE_HEADERS))

_ASSEMBLE_OBJECTS = $(_ASSEMBLE_SOURCE_HEADER_PAIRS:=.o) assemble.o
ASSEMBLE_OBJECTS = $(patsubst %,$(ASSEMBLE_DIR)/%,$(_ASSEMBLE_OBJECTS))

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

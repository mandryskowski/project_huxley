CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

emulateObjects = emulator/emulate.o emulator/emulateControl.o \
	emulator/util/bitwiseInstruction.o emulator/util/emulateUtility.o emulator/util/outputFileGenerator.o \
	emulator/instructions/branchInstruction.o emulator/instructions/immediateInstruction.o emulator/instructions/registerInstruction.o emulator/instructions/sdtInstruction.o

emulator/emulate.o: emulator/emulateControl.h emulator/util/state.h emulator/util/outputFileGenerator.h emulator/instructions/immediateInstruction.h
emulator/emulateControl.o: emulator/emulateControl.h emulator/util/outputFileGenerator.h emulator/util/emulateUtility.h emulator/instructions/immediateInstruction.h emulator/instructions/branchInstruction.h emulator/instructions/registerInstruction.h emulator/instructions/sdtInstruction.h
emulator/util/bitwiseInstruction.o: emulator/util/bitwiseInstruction.h
emulator/util/emulateUtility.o: emulator/util/emulateUtility.h
emulator/util/outputFileGenerator.o: emulator/util/outputFileGenerator.h
emulator/instructions/branchInstruction.o: emulator/instructions/branchInstruction.h emulator/util/state.h emulator/util/emulateUtility.h
emulator/instructions/immediateInstruction.o: emulator/instructions/immediateInstruction.h emulator/util/emulateUtility.h
emulator/instructions/registerInstruction.o: emulator/instructions/immediateInstruction.h emulator/instructions/registerInstruction.h emulator/util/bitwiseInstruction.h emulator/util/emulateUtility.h
emulator/instructions/sdtInstruction.o:	 emulator/util/emulateUtility.h emulator/instructions/sdtInstruction.h


#emulateObjects = emulator/emulate.o emulator/emulateControl.o \
#	emulator/util/bitwiseInstruction.o emulator/util/emulateUtility.o emulator/util/outputFileGenerator.o \
#	emulator/instructions/branchInstruction.o emulator/instructions/immediateInstruction.o emulator/instructions/registerInstruction.o emulator/instructions/sdtInstruction.o
#
#emulator/emulate.o: emulator/emulateControl.h emulator/util/state.h emulator/util/outputFileGenerator.h emulator/instructions/immediateInstruction.h
#emulator/emulateControl.o: emulator/emulateControl.h emulator/util/outputFileGenerator.h emulator/util/emulateUtility.h emulator/instructions/immediateInstruction.h emulator/instructions/branchInstruction.h emulator/instructions/registerInstruction.h emulator/instructions/sdtInstruction.h
#emulator/util/bitwiseInstruction.o: emulator/util/bitwiseInstruction.h
#emulator/util/emulateUtility.o: emulator/util/emulateUtility.h
#emulator/util/outputFileGenerator.o: emulator/util/outputFileGenerator.h
#emulator/instructions/branchInstruction.o: emulator/instructions/branchInstruction.h emulator/util/state.h emulator/util/emulateUtility.h
#emulator/instructions/immediateInstruction.o: emulator/instructions/immediateInstruction.h emulator/util/emulateUtility.h
#emulator/instructions/registerInstruction.o: emulator/instructions/immediateInstruction.h emulator/instructions/registerInstruction.h emulator/util/bitwiseInstruction.h emulator/util/emulateUtility.h
#emulator/instructions/sdtInstruction.o:	 emulator/util/emulateUtility.h emulator/instructions/sdtInstruction.h




.SUFFIXES: .c .o

.PHONY: all clean

emulate: $(emulateObjects)
	cc -o emulate $(emulateObjects)

assemble: $(assembleObjects)
	cc -o assemble $(assembleObjects)

all: assemble emulate

clean:
	$(RM) *.o assemble emulate
	

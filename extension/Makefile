CC      = gcc
LINK_CC = gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -lm
LDFLAGS=
CXXFLAGS  ?= -std=c++11 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -lm -fno-exceptions
GAME_DIR = .
GAME_OBJECTS = 

########################OS Dependent#########################

ifeq ($(OS),Windows_NT)
# I tried really hard to make this extension link on Windows using gcc instead of g++.
# Unfortunately there is a complicated bunch of issues that prevented me from doing so.
# Imgui with glfw backend gets confused when compiling them as a dynamic library (which is done for linux and macOS).
# So we had to resort to building them as a static library. Unfortunately one of the files in the Imgui wrapper for C (cimgui) used the C++ operator new
# which gives an undefined reference error. So we link with g++, but the object files are compiled with gcc.
# Note that we cannot ignore undefined reference errors with MinGW gcc (the linker flag is recognised but ignored) even if we ignore the fact that it is not a great practice.
    LINK_CC   = g++
    CFLAGS += -D WIN32 -Llib/windows -lglfw3 -lm -lcimgui -lgdi32 -lopengl32 -lopenal32 -lws2_32
    LDFLAGS += "-Wl,-rpath,lib/windows/"
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CFLAGS += -D IA32
        endif
    endif

else
    UNAME_S := $(shell uname -s)
        
    ifeq ($(UNAME_S),Linux)
        CFLAGS += -D LINUX -Llib/linux -lcimgui -lm -lopenal
        LDFLAGS="-Wl,-rpath,lib/linux/"
    endif
    ifeq ($(UNAME_S),Darwin)
        CFLAGS += -D OSX -Llib/macos -lcimgui
        LDFLAGS="-Wl,-rpath,lib/macos/"
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CFLAGS += -D ARM
    endif
endif

clean:
	find . -type f -name "*.o" -delete



########################OS Dependent#########################


########################Project files########################

# Here you add the names of header-source file pairs. E.g. gameControl adds gameControl.h and gameControl.c
_GAME_SOURCE_HEADER_PAIRS = game game_math room render audio assets entity movement gui pathfind util level pqueue animation haskell tinywav/tinywav predefinedRooms/room_generator predefinedRooms/monster_spawner predefinedRooms/structure_builder\

# Here on the other hand you add just the headers.
_GAME_JUST_HEADERS = state.h

########################Project files########################

# Prepends paths with game folder
GAME_SOURCE_HEADER_PAIRS = $(patsubst %,$(GAME_DIR)/%,$(_GAME_SOURCE_HEADER_PAIRS))
GAME_JUST_HEADERS = $(patsubst %,$(GAME_DIR)/%,$(_GAME_JUST_HEADERS))

# Adds .h and .o
GAME_HEADERS = $(GAME_SOURCE_HEADER_PAIRS:=.h) $(GAME_JUST_HEADERS)
GAME_OBJECTS += $(GAME_SOURCE_HEADER_PAIRS:=.o) main.o glad.o
#lib/linux/libcimgui.a
#lib/linux/cimgui.a lib/linux/cimgui.o lib/linux/imgui.o lib/linux/imgui_draw.o lib/linux/imgui_tables.o lib/linux/imgui_widgets.o

%.o: %.c $(GAME_HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)


.SUFFIXES: .c .o

.PHONY: all clean

game: $(GAME_OBJECTS)
	$(LINK_CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

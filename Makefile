# ---------------------------------------
# CONFIGURATION BEGIN
# ---------------------------------------

# Entry point for the program and target name
C_SRCS = src/main.c src/csv_parser.c
CPP_SRCS = src/simulation.cpp src/main_memory.cpp src/cache.cpp src/level.cpp src/cache_set.cpp src/cache_line.cpp

# Object files
C_OBJS = $(C_SRCS:.c=.o)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)

# Target name
TARGET := project

# SystemC path (one level above the project)
SCPATH = ../systemc

# Compiler and linker flags

CFLAGS := -std=c11 -I$(SCPATH)/include -Iinclude
CXXFLAGS := -std=c++14 -I$(SCPATH)/include -Iinclude
LDFLAGS := -L$(SCPATH)/lib -lsystemc -lm

# ---------------------------------------
# CONFIGURATION END
# ---------------------------------------

# Determine if clang or gcc is available
CXX := $(shell command -v g++ || command -v clang++)
ifeq ($(strip $(CXX)),)
    $(error Neither clang++ nor g++ is available. Exiting.)
endif

CC := $(shell command -v gcc || command -v clang)
ifeq ($(strip $(CC)),)
    $(error Neither clang nor gcc is available. Exiting.)
endif

# Add rpath except for MacOS
UNAME_S := $(shell uname -s)

ifneq ($(UNAME_S), Darwin)
    LDFLAGS += -Wl,-rpath=$(SCPATH)/lib
endif

# Default to release build for both app and library
all: debug

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug build
debug: CXXFLAGS += -g
		CFLAGS += -g
debug: $(TARGET)

# Release build
release: CXXFLAGS += -O2
release: $(TARGET)

# Rule to link object files to executable
$(TARGET): $(C_OBJS) $(CPP_OBJS)
	$(CXX) $(C_OBJS) $(CPP_OBJS) $(LDFLAGS) -o $(TARGET)

# Clean up
clean:
	rm -f $(TARGET)
	rm -rf $(C_OBJS) $(CPP_OBJS)

.PHONY: all debug release clean

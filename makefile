# Compiler flags
WARN := -Wall -Wextra -Wpedantic -Wshadow -Wcast-align -Wconversion -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wdouble-promotion -Wformat=2
OPT += -pthread
CFLAGS := $(WARN) $(OPT)

# Executables and dependencies
EXE := test ydotool ydotoold
test_DEP := uinput.o test.o
ydotool_DEP := ydotool.o click.o key.o mouse.o type.o uinput.o
ydotoold_DEP := ydotoold.o uinput.o

# Default to building the executables
.PHONY: default
default: $(EXE)

# Compile a C source file
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile a C++ source file
%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test application
test: $(test_DEP)
	$(CC) $(CFLAGS) $^ -o $@

# Main application
ydotool: $(ydotool_DEP)
	$(CC) $(CFLAGS) $^ -o $@

# Daemon application
ydotoold: $(ydotoold_DEP)
	$(CC) $(CFLAGS) $^ -o $@

# Install built binaries
.PHONY: install
install:
	mkdir -p /usr/local/bin
	mv ydotool /usr/local/bin
	mv ydotoold /usr/local/bin

# Remove build files
.PHONY: clean
clean:
	$(RM) $(EXE) *.o

# Compiler flags
WARN := -Wall -Wextra -Wpedantic -Wshadow -Wcast-align -Wconversion -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wdouble-promotion
OPT += -pthread
# Auto-dependency generation (Part 1)
# See: http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
DEPFLAGS = -MT $@ -MMD -MP -MF dep/$*.d
CFLAGS = $(DEPFLAGS) $(WARN) $(OPT)

# Executables
EXE := test ydotool ydotoold

# Secondary expansion for expanding dependency variable lists in generic linking rule
.SECONDEXPANSION:

# Executable dependencies
test_DEP := uinput.o test.o
ydotool_DEP := ydotool.o uinput.o
ydotoold_DEP := ydotoold.o uinput.o

# Default to building the executables
.PHONY: default
default: $(EXE)

# Generic compilation rule
%.o : %.c dep/%.d | dep
	$(CC) $(CFLAGS) -c $< -o $@

# Generic linking rule
$(EXE): %: $$(%_DEP)
	$(CC) $(CFLAGS) $^ -o $@

# Make dependency directory if it doesn't exist
dep:
	@mkdir -p $@

# Auto-dependency generation (Part 2)
# See: http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
SRCS := $(wildcard *.c)
DEPFILES := $(SRCS:%.c=dep/%.d)
$(DEPFILES):
include $(wildcard $(DEPFILES))

# Install built binaries
.PHONY: install
install:
	mkdir -p /usr/local/bin
	cp ydotool /usr/local/bin
	cp ydotoold /usr/local/bin

# Remove build files
.PHONY: clean
clean:
	$(RM) -r $(EXE) *.o ./dep ./doc

# Perform a static analysis check
.PHONY: cppcheck
cppcheck:
	@cppcheck --enable=all --force -q --suppress=missingIncludeSystem .

# Generate doxygen documentation
.PHONY: doxygen
doxygen:
	@doxygen

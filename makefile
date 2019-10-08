WARN := -Wall -Wextra -Wpedantic -Weffc++

INC += -Iinc

OPT += -pthread

LIB += -levdevPlus
LIB += -luInputPlus
LIB += -lboost_program_options
LIB += -ldl

CXXFLAGS := $(WARN) $(INC) $(OPT)

EXE := ydotool ydotoold

ydotool_DEP := ydotool.o click.o key.o mouse.o recorder.o type.o
ydotoold_DEP := ydotoold.o

.PHONY: default
default: $(EXE)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

ydotool: $(ydotool_DEP)
	$(CXX) $(CXXFLAGS) $^ $(LIB) -o $@

ydotoold: $(ydotoold_DEP)
	$(CXX) $(CXXFLAGS) $^ $(LIB) -o $@

.PHONY: clean
clean:
	$(RM) $(EXE) *.o

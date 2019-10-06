WARN := -Wall -Wextra -Wpedantic -Weffc++

INC += -Iclient
INC += -Ilibrary
INC += -Itools
INC += -I.

OPT += -pthread

LIB += -levdevPlus
LIB += -luInputPlus
LIB += -lboost_program_options
LIB += -ldl

CXXFLAGS := $(WARN) $(INC) $(OPT)

ydotool: client/ydotool.cpp library/instance.cpp library/tool.cpp library/utils.cpp tools/click/click.cpp tools/key/key.cpp tools/mouse/mouse.cpp tools/recorder/recorder.cpp tools/type/type.cpp
	$(CXX) $(CXXFLAGS) $^ $(LIB) -o $@


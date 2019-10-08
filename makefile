SRC_DIR := src
INC_DIR := inc
OBJ_DIR := obj
BIN_DIR := bin

RM := rm -rf

WARN := -Wall -Wextra -Wpedantic -Weffc++

INC += -Iinc

OPT += -pthread

LIB += -levdevPlus
LIB += -luInputPlus
LIB += -lboost_program_options
LIB += -ldl

CXXFLAGS := $(WARN) $(INC) $(OPT)

ydotool_DEP := $(patsubst %,$(OBJ_DIR)/%.o, \
	ydotool \
	click \
	key \
	mouse \
	recorder \
	type \
	)

ydotoold_DEP := $(patsubst %,$(OBJ_DIR)/%.o, \
	ydotoold \
	)

.PHONY: default
default: create_dirs $(BIN_DIR)/ydotool $(BIN_DIR)/ydotoold

.PHONY: create_dirs
create_dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/ydotool: $(ydotool_DEP)
	$(CXX) $(CXXFLAGS) $^ $(LIB) -o $@

$(BIN_DIR)/ydotoold: $(ydotoold_DEP)
	$(CXX) $(CXXFLAGS) $^ $(LIB) -o $@

.PHONY: clean
clean:
	$(RM) $(OBJ_DIR)/* $(BIN_DIR)/*

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

ydotool_DEPS := $(patsubst %,$(OBJ_DIR)/%.o, \
	ydotool \
	instance \
	tool \
	utils \
	click \
	key \
	mouse \
	recorder \
	type \
	)

.PHONY: default
default: $(OBJ_DIR) $(BIN_DIR) $(BIN_DIR)/ydotool

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(BIN_DIR):
	@mkdir $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/ydotool: $(ydotool_DEPS)
	$(CXX) $(CXXFLAGS) $^ $(LIB) -o $@

.PHONY: clean
clean:
	$(RM) $(OBJ_DIR) $(BIN_DIR)

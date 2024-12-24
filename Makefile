CXX := clang++
CXX_FLAGS := -std=c++17 -Werror -Wall -Wextra -Wconversion -gdwarf-4 -g -fsanitize=address
LD_FLAGS := -L/usr/lib -lstdc++ -lm
OPENGL := -framework OpenGL
BUILD := build
OBJ_DIR := $(BUILD)/obj
APP_DIR := $(BUILD)/app
TARGET := engine
INCLUDE := -Iinclude/
SFML_INCLUDE := -I /opt/homebrew/Cellar/sfml/**/include/
SFML_LIB := -L /opt/homebrew/Cellar/sfml/**/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
SRC := $(wildcard src/*.cpp) \
	$(wildcard src/**/*.cpp) \
	$(wildcard vendor/*.cpp)

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

.PHONY: all
all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(INCLUDE) -c $< -MMD -o $@ $(SFML_INCLUDE)

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -o $(APP_DIR)/$(TARGET) $^ $(SFML_INCLUDE) $(LD_FLAGS) $(SFML_LIB) $(OPENGL)

-include $(DEPENDENCIES)

.PHONY: build
build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

.PHONY: run
run: all
	./$(APP_DIR)/$(TARGET)

.PHONY: debug
debug: all
	lldb ./$(APP_DIR)/$(TARGET)

.PHONY: check-leaks
check-leaks: all
	leaks -atExit -- ./$(APP_DIR)/$(TARGET)

.PHONY: loc
loc:
	find src/ \( -name "*.cpp" -o -name "*.hpp" \) -type f -exec wc -l {} +

.PHONY: clean
clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

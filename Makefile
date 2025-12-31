CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRC_DIR  = src
CMD_DIR  = src/commands
BUILD    = build
TARGET   = $(BUILD)/MiniFileExplorer

SOURCES  = \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/MiniFileExplorer.cpp \
    $(SRC_DIR)/FileSystem.cpp \
    $(SRC_DIR)/Utils.cpp \
    $(CMD_DIR)/Commands.cpp

OBJECTS  = $(SOURCES:%.cpp=$(BUILD)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD)

run: all
	./$(TARGET)

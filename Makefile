CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRC_DIR  = src
CMD_DIR  = src/commands

TARGET   = MiniFileExplorer

SOURCES  = \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/MiniFileExplorer.cpp \
    $(SRC_DIR)/FileSystem.cpp \
    $(SRC_DIR)/Utils.cpp \
    $(CMD_DIR)/Commands.cpp

OBJECTS  = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: all
	./$(TARGET)

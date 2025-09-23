CXX := g++
CXXFLAGS := -Wall -Wextra -O2 -std=c++17

SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)
BIN := bin/wb_app

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f src/*.o $(BIN)

.PHONY: all clean

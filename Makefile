CXX      = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17
LDFLAGS  =
# add -lm because GLUT bitmap helpers may pull math
LDLIBS = -lglfw -lGL -lglut -lm -ldl -lpthread -lX11 -lXrandr -lXi

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
BIN = bin/wb_app

all: $(BIN)

bin:
	@mkdir -p bin

$(BIN): $(OBJ) | bin
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(OBJ) $(BIN)
CXX = g++

CXXFLAGS = -std=c++17 -Wall -O2 -I/home/shvetank/Downloads/glm-master


LIBS = -lglfw -lGLEW -lGL

TARGET = project1

SRC = proj.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)


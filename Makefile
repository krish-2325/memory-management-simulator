CXX=g++
CXXFLAGS= -std=c++11 -Wall
TARGET = memsim
SRC= src/main.cpp
all:
	$(CXX) $(CXXFLAGS) $(SRC) -Iinclude -o $(TARGET)
clean:
	rm -f $(TARGET)
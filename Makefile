# Makefile for gs2png

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Iinclude
LDFLAGS =

TARGET = gs2png
SOURCES = src/main.cpp src/gsdump.cpp src/gsswizzle.cpp
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

# Dependencies
src/main.o: src/main.cpp include/gsdump.h include/gsswizzle.h include/stb_image_write.h
src/gsdump.o: src/gsdump.cpp include/gsdump.h include/types.h
src/gsswizzle.o: src/gsswizzle.cpp include/gsswizzle.h include/types.h

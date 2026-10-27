CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

TARGET = lab_tests.exe
SOURCES = main.cpp tests.cpp
OBJECTS = main.o tests.o

HEADERS = UnqPtr.h ShrdPtr.h MemorySpan.h MsPtr.h tests.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

main.o: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c main.cpp

tests.o: tests.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c tests.cpp

clean:
	rm -f *.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
CC = gcc
CXX = g++
FLAGS = -Wall -ggdb3 -O0

OBJS = md4c.o main.o

TARGET = mp

$(TARGET): $(OBJS)
	$(CXX) $(FLAGS) $(OBJS) -o $(TARGET)
	rm -f $(OBJS)

md4c.o: md4c.c
	$(CC) $(FLAGS) -c md4c.c -o md4c.o

main.o: main.cpp
	$(CXX) $(FLAGS) -c main.cpp -o main.o

clean:
	rm -f $(TARGET)

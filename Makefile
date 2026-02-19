CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -g

TARGET = mysh

OBJS = main.o parser.o executor.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c parser.h executor.h
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

executor.o: executor.c executor.h parser.h
	$(CC) $(CFLAGS) -c executor.c

clean:
	rm -f $(TARGET) *.o

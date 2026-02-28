#Compiler and Compiler flags 
CC = gcc

CFLAGS = -Wall -Wextra -std=gnu11 -g

# Name of the output executable
TARGET = mysh 

# Objects list
OBJS = main.o parser.o executor.o builtins.o

# Builds the shell 
all: $(TARGET)

# Build rule for the shell executable 
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c parser.h executor.h
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

executor.o: executor.c executor.h parser.h
	$(CC) $(CFLAGS) -c executor.c

builtins.o: builtins.c builtins.h
	$(CC) $(CFLAGS) -c builtins.c

# Removes generated files
clean:
	rm -f $(TARGET) *.o

# Removes all .txt files in the directory
clean-txt:
	rm -f *.txt

# To compile and run the shell, type "make" in the terminal,
# then execute "./mysh" to start the shell.
# To clean up the directory, type "make clean".

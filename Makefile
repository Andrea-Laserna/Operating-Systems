#Compiler and Compiler flags 
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 

# Name of the output executable
TARGET = mysh 

# Builds the shell 
all: $(TARGET)

# Build rule for the shell executable 
$(TARGET): main.c parser.c executor.c builtins.c
	$(CC) $(CFLAGS) main.c parser.c executor.c builtins.c -o $(TARGET)

# Removes generated files  
clean:
	rm -f $(TARGET)


# To compile and run the shell, type "make" in the terminal,
# then execute "./mysh" to start the shell.
# To clean up the directory, type "make clean".
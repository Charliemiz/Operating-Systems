# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall

# Libraries to link
LIBS = -lpthread -lm

# Source file
SRC = prog1.c

# Output executable name
TARGET = prog1

# Default target to compile the program
all: $(TARGET)

# Compile the program
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean target to remove the executable
clean:
	rm -f $(TARGET)

# Phony target to prevent confusion with files named 'clean' or 'all'
.PHONY: clean all

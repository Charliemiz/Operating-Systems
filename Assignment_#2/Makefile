CC = gcc
CFLAGS = -Wall -O3 -lrt -lpthread
TARGET = program1

all: $(TARGET)

$(TARGET): program1.c
	$(CC) $(CFLAGS) -o $(TARGET) program1.c

run: $(TARGET)
	./$(TARGET) sequence.txt subsequence.txt 2

clean:
	rm -f $(TARGET)

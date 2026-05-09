CC     = gcc
CFLAGS = -Wall -Wextra -g -I./include
SRCS   = src/main.c src/hash_table.c src/graph.c
TARGET = logistics

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) logistics.exe

run: all
	./$(TARGET)

.PHONY: all clean run
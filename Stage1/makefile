# Sujay Prasad, 20530 - Operating Systems Project 1
# CSC1021/simpleshell

CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -g
TARGET=bin/simpleshell
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)

all: simpleshell

simpleshell: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean simpleshell

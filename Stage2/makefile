# Sujay Prasad, 20530 - Operating Systems Project 1
# CSC1021/simpleshell

CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -g
TARGET=bin/simpleshell
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)

all: simpleshell

simpleshell: $(OBJS)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

src/%.o: src/%.c src/simpleshell.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean simpleshell

# Name: Sujay Prasad
# Student Number: 20530
# I acknowledge the DCU Academic Integrity Policy.

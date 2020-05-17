CC = gcc
CFLAGS = -Wall

SRC = $(shell find ./src -name '*.c')

all: foxc

foxc:
	mkdir -p bin
	${CC} ${CFLAGS} -o bin/foxc ${SRC}

clean:
	rm -rf bin

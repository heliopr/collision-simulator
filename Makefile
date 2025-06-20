CC = gcc
LIBS = -I/usr/local/include $(shell pkg-config --static --libs glfw3) -I./include/
CFLAGS = -Wall -O0

SRC=$(wildcard src/*.c)

all: clean build

run: build
	./build

build: $(SRC)
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@

clean:
	rm -f build
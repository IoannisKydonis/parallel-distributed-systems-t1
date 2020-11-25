CC=gcc-9
CFLAGS=-O3

default: all

all: main

main: main.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c

clean:
	rm -f main

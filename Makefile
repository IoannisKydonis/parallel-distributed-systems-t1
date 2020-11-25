CC=gcc-9
CFLAGS=-O3

default: all

all: main

main: main.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c

clean:
	rm -f main

test:
	./main ./data/s12.mtx
	./main ./data/bcsstm07.mtx
	./main ./data/G1.mtx

CC=gcc-9
CFLAGS=-O3

default: all

all: v2 v3_seq v3_omp v4_seq v4_omp v4_pt

v2: v2.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c

v3_seq: v3_seq.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c

v3_omp: v3_omp.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c -fopenmp

#v3_cilk: v3_cilk.c
#	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c -fcilkplus -lpthread

v4_seq: v4_seq.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c

v4_omp: v4_omp.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c -fopenmp

#v4_cilk: v4_cilk.c
#	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c -fcilkplus

v4_pt: v4_pt.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c -lpthread

clean:
	rm -f main

test:
	./main ./data/s12.mtx
	./main ./data/bcsstm07.mtx
	./main ./data/G1.mtx

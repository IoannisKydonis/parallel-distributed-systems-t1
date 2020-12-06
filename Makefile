CC=gcc
#CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CILKCC=/mnt/apps/prebuilt/OpenCilk/9.0.1/build/bin/clang
CFLAGS=-O3

default: all

all: v2 v3_seq v3_omp v3_cilk v4_seq v4_omp v4_cilk v4_pt

v2: v2.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c

v3_seq: v3_seq.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c controller.c

v3_omp: v3_omp.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c controller.c -fopenmp

v3_cilk: v3_cilk.c
	$(CILKCC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c controller.c -fcilkplus -lpthread

v4_seq: v4_seq.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c controller.c

v4_omp: v4_omp.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c controller.c -fopenmp

v4_cilk: v4_cilk.c
	$(CILKCC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c controller.c -fcilkplus -lpthread

v4_pt: v4_pt.c
	$(CC) $(CFLAGS) -o $@ $^ readmtx.c mmio.c coo2csc.c timer.c arrayutils.c controller.c -lpthread

clean:
	rm -f v2 v3_seq v3_omp v3_cilk v4_seq v4_omp v4_cilk v4_pt *.txt

test:
	./v3_omp ./data/belgium_osm.mtx
	./v3_omp ./data/com-Youtube.mtx
	./v3_omp ./data/mycielskian13.mtx
	./v3_omp ./data/dblp-2010.mtx
	./v3_omp ./data/NACA0015.mtx

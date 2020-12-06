#!/bin/bash
#SBATCH --time=00:10:00
#SBATCH --partition=batch
#SBATCH --cpus-per-task=20

module rm gcc
module rm OpenCilk/9.0.1

make clean

module load gcc
make v3_omp v4_omp v4_pt
module purge
module load OpenCilk/9.0.1
make v3_cilk v4_cilk

rm -r output_parallel
mkdir output_parallel
echo "" > ./output_parallel/v3_omp.txt
echo "" > ./output_parallel/v3_cilk.txt
echo "" > ./output_parallel/v4_omp.txt
echo "" > ./output_parallel/v4_cilk.txt
echo "" > ./output_parallel/v4_pt.txt

for file in ./data/*.mtx; do
	for i in 2 4 5 10 15 20; do
	  echo "V3-OMP with "$file" with "$i" workers:" >> ./output_parallel/v3_omp.txt
	  export OMP_NUM_THREADS=$i
		./v3_omp $file >> ./output_parallel/v3_omp.txt
	done
done

for file in ./data/*.mtx; do
	for i in 2 4 5 10 15 20; do
    echo "V3-CILK with "$file" with "$i" workers:" >> ./output_parallel/v3_cilk.txt
		export CILK_NWORKERS=$i
		./v3_cilk $file >> ./output_parallel/v3_cilk.txt
	done
done

for file in ./data/*.mtx; do
	for i in 2 4 5 10 15 20; do
	  echo "V4-OMP with "$file" with "$i" workers:" >> ./output_parallel/v4_omp.txt
	  export OMP_NUM_THREADS=$i
		./v4_omp $file >> ./output_parallel/v4_omp.txt
	done
done

for file in ./data/*.mtx; do
	for i in 2 4 5 10 15 20; do
    echo "V4-CILK with "$file" with "$i" workers:" >> ./output_parallel/v4_cilk.txt
		export CILK_NWORKERS=$i
		./v4_cilk $file >> ./output_parallel/v4_cilk.txt
	done
done

for file in ./data/*.mtx; do
	for i in 2 4 5 10 15 20; do
	  echo "V4-PT with "$file" with "$i" workers:" >> ./output_parallel/v4_pt.txt
		./v4_pt $file $i >> ./output_parallel/v4_pt.txt
	done
done
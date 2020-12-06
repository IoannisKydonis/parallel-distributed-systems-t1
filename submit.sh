#!/bin/bash
#SBATCH --time=00:05:00
#SBATCH --partition=batch
#SBATCH --cpus-per-task=1

module rm gcc
module rm OpenCilk/9.0.1

module load gcc

make clean
make v3_seq v4_seq

rm -r output
mkdir output
echo "" > ./output/v3_seq.txt
echo "" > ./output/v4_seq.txt

for i in {1..5}; do
  for file in ./data/*.mtx; do
    ./v3_seq $file >> ./output/v3_seq.txt
  done
done

for i in {1..5}; do
  for file in ./data/*.mtx; do
    ./v4_seq $file >> ./output/v4_seq.txt
  done
done

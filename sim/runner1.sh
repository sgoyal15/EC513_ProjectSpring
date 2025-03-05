#!/bin/sh -l
#$ -l h_rt=48:00:00
#$ -N bwvbench1
#$ -pe omp 4

source /projectnb/ec513/materials/HW6/spec-2017/sourceme

build/X86/gem5.opt \
	configs/example/gem5_library/x86-spec-cpu2017-benchmarks1.py \
	--image ../disk-image/spec-2017/spec-2017-image/spec-2017 \
	--partition 1 \
	--benchmark 503.bwaves_r \
	--size test


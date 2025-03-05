#!/bin/sh -l
#$ -l h_rt=24:00:00
#$ -N bwvbench4
#$ -pe omp 1

source /projectnb/ec513/materials/HW6/spec-2017/sourceme

build/X86/gem5.opt \
	configs/spec2017.py \
	--image ../disk-image/spec-2017/spec-2017-image/spec-2017 \
	--partition 1 \
	--benchmark 503.bwaves_r \
	--size test



#!/bin/sh -l
#$ -l h_rt=32:00:00
#$ -N omnetpp_bench
#$ -pe omp 2
#$ -m ea

source /projectnb/ec513/materials/HW6/spec-2017/sourceme

bench="520.omnetpp_r"
#l2mem=("1" "2" "4")
l2mem=("2")

for j in "${l2mem[@]}"; do
	build/X86/gem5.opt \
		configs/spec2017_"$j".py \
		--image ../disk-image/spec-2017/spec-2017-image/spec-2017 \
		--partition 1 \
		--benchmark "${bench}" \
		--size test
	mv /projectnb/ec513/students/agwoska/HW6/spec-2017/gem5/m5out/stats.txt /projectnb/ec513/students/agwoska/HW6/spec-2017/stats/"${bench}"_"$j".stats.txt
done

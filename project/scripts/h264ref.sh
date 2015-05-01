#!/bin/sh

declare -a filenames=("Default" "All_2way" "All_FA_L2Big" "L1_small" "L2_Big" "All_4way" "L1_2way" "L1_small_4way" "All_FA" "L1_8way" "L2_4way")
#echo ${filenames[*]}
trace="h264ref"
for filename in "${filenames[@]}"
do
	echo "$filename"
	zcat /scratch/arp/ecen4593-sp15/traces-long/h264ref.gz | ./memsim "$filename" "$trace"
done

#!/bin/sh

declare -a filenames=("Memchunk16" "Memchunk32" "Memchunk64")
#echo ${filenames[*]}
trace="sjeng"
for filename in "${filenames[@]}"
do
	echo "$filename"
	zcat /scratch/arp/ecen4593-sp15/traces-long/sjeng.gz | ./memsim "$filename" "$trace"
done

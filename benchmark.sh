#!/usr/bin/env bash

#set -x

#The number of blocks and block size are powers of 2, e.g.: 2^7, 2^8,...
#SYSTEMS=(randomreadbench randomreadbenchf randomwritebench randomwritebenchf)

#SYSTEMS=(randomreadbenchd randomreadbenchfd randomwritebenchd randomwritebenchfd)
SYSTEMS=(randomreadbenchfd randomwritebenchd randomwritebenchfd)

BSIZES=(13)
BCAP=(4)
#NBLOCKS=(10 12 14 16 18 20)
NBLOCKS=(10 12 14 16)

NOPS=1000000

# Max execution time. Timeout after the specified time.
# 1020s = 20min = 5min ramp-up + 15min execution time
MAX_EXEC_TIME=2100
#MAX_EXEC_TIME=10
COOL_DOWN=120
NRUNS=5


RESULTS_PATH="results"

clean_caches()
{
    sudo sh -c 'echo 1 > /proc/sys/vm/drop_caches'        
    sudo sh -c 'echo 2 > /proc/sys/vm/drop_caches'
    sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'        
}

function run_test {
    local system=$1
    local nblocks=$((2**($2+0)))
    local bsize=$((2**($3+0)))
    local bcap=$4


    mkdir -p path
    for run in $(seq 0 $NRUNS);
    do
        local file="${system}_${nblocks}_${bsize}_${bcap}_${run}.txt"
        ./src/$system $nblocks $bsize $bcap $NOPS > "$RESULTS_PATH/$file" &
        pid=$!
        sleep $MAX_EXEC_TIME
        local stats=$(./stats.py -i "$RESULTS_PATH/$file")
        printf "%20s %6d %4d %d %d" "$system" "$nblocks" "$bsize" "$bcap" "$run" >> results/stats.csv
        echo "$stats" >> results/stats.csv
        kill $pid
	clean_caches
        sleep $COOL_DOWN
    done

}

touch $RESULTS_PATH/stats.csv

nblocks_size="${#NBLOCKS[@]}"
nblocks_size=$((nblocks_size - 1))

for system in ${SYSTEMS[@]};
do
    for i in $(seq 0 $nblocks_size);
    do
        bsizes_size="${#BSIZES[@]}"
        bsizes_size=$((bsizes_size - 1))

        for j in $(seq 0 $bsizes_size);
        do
            bcap_size="${#BCAP[@]}"
            bcap_size=$((bcap_size - 1))

            for z in $(seq 0 $bcap_size);
            do
                nblocks=${NBLOCKS[i]}
                bsize=${BSIZES[j]}
                bcap=${BCAP[z]}

                echo "$(date) - Run $system with $nblocks blocks of size $bsize and Z $bcap"
                run_test $system $nblocks $bsize $bcap

            done
        done
    done
done



#!/usr/bin/env bash

#set -x

#The number of blocks and block size are powers of 2, e.g.: 2^7, 2^8,...
#SYSTEMS=(randomreadbench randomreadbenchf randomwritebench randomwritebenchf)

SYSTEMS=(randomreadbenchd randomreadbenchfd randomwritebenchd randomwritebenchfd)
#SYSTEMS=(randomreadbench)
NBLOCKS=(12)
BSIZES=(13)
BCAP=(4)
#NBLOCKS=(10 12 14 16 18 20)
#BSIZES=(10 11 12)
#BCAP=(1 2 3 4)

NOPS=1000000

# Max execution time. Timeout after the specified time.
# 1020s = 20min = 5min ramp-up + 15min execution time
#MAX_EXEC_TIME=1800
MAX_EXEC_TIME=10
COOL_DOWN=120
NRUNS=1


RESULTS_PATH="results"


function run_test {
    local system=$1
    local nblocks=$((2**($2+0)))
    local bsize=$((2**($3+0)))
    local bcap=$4

    #echo "f input is $system $nblocks $bsize"
    #echo "file is $file"

    mkdir -p path
    for run in $(seq 0 $NRUNS);
    do
        local file="${system}_${nblocks}_${bsize}_${bcap}_${run}.txt"
        ./src/$system $nblocks $bsize $bcap $NOPS > "$RESULTS_PATH/$file" &
        pid=$!
        sleep $MAX_EXEC_TIME
        local stats=$(./stats.py -i "$RESULTS_PATH/$file")
        echo "$system, $nblocks, $bsize, $bcap, $run, $stats" >> results/stats.csv
    kill $pid

    done
    #cp "src/$system" .
    #sleep $COOL_DOWN
}

#mkdir -p "$RESULTS_PATH/dstat"
touch "RESULTS_PATH/stats.csv"

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


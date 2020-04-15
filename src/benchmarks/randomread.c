#include "oram/oram.h"
#include "oram/orandom.h"
#include "oram/plblock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

char *gen_random(const int len) {
    int i, rval = 0;
    char *s = (char *) malloc(sizeof(char) * len);

    for(i = 0; i < len; i++){
        rval = getRandomInt();
        memcpy(s, &rand, sizeof(char));
    }

    return s;
}


int test(size_t nblocks, 
         size_t blockSize,
         size_t bucketCapcity, 
         size_t nreads) {

    int result = 0;
    size_t wOffset = 0;
    size_t blockWriteOffset = 0;
    clock_t start, end;
    double cpu_time_used;

    AMStash *stash;
    AMPMap *pmap;
    AMOFile *ofile;
    ORAMState state;

    stash = stashCreate();
    pmap = pmapCreate();
    ofile = ofileCreate();

    Amgr amgr;
    amgr.am_stash = stash;
    amgr.am_pmap = pmap;
    amgr.am_ofile = ofile;

    int string_size = 0;
    int index = 0;
    char *data, *value = NULL;

    state = init_oram("teste", nblocks, blockSize, bucketCapcity, &amgr, NULL);

    for (index = 0; index < nblocks; index++) {
        value = gen_random(blockSize);
        write_oram(value, blockSize, index, state, NULL);
        free(value);
    }

    for(index = 0; index < nreads; index++){

        wOffset = (getRandomInt() % nblocks);

        start = clock();
        result = read_oram(&data, wOffset, state, NULL);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("%lf\n", cpu_time_used);

        free(data);
    }

   
    close_oram(state, NULL);
    return 0;
}

int main(int argc, char *argv[]) {
    /* default values*/
    int result;
    size_t nblocks, blockSize, bucketCapacity, nwrites;

    if(argc < 4){
        printf("The benchmark expects 4 inputs: nblocks nblockSize bucketCapacity nWrites\n");
        exit(1);
    }
    
    nblocks         = atoi(argv[1]);
    blockSize       = atoi(argv[2]);
    bucketCapacity  = atoi(argv[3]);
    nwrites         = atoi(argv[4]);
    
    result = test(nblocks, blockSize, bucketCapacity, nwrites);

    return result;
}


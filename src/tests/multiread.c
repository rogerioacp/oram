

#include "oram.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {

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

    size_t fileSize = 100;// file with 100 bytes;
    size_t blockSize = 20;// block size of 20 bytes;
    size_t bucketCapcity = 1; // 1 bucket per tree node;
    size_t result = 0;
    size_t nblocks = fileSize/blockSize;
    int index = 0;
    void *data = NULL;
    state = init("teste", fileSize, blockSize, bucketCapcity, &amgr);

    for(index = 0; index < nblocks; index++){
        printf("Going to read block offset %d\n", index);
        result = read(&data, index, state);
       if (data != NULL || result != 0){
            return 1;
        }
    }
 
    return 0;
}
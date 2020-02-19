#include "oram/oram.h"
#include "oram/plblock.h"

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

    size_t nblocks = 100;// file with 100 bytes;
    size_t blockSize = 20;// block size of 20 bytes;
    size_t bucketCapcity = 4; // 1 bucket per tree node;

    int result = 0;
    char *data = NULL;

    
    state = init_oram("teste", nblocks, blockSize, bucketCapcity, &amgr, NULL);
    result = read_oram(&data, 0, state, NULL); 
    close_oram(state, NULL);
    
    if (result == DUMMY_BLOCK){
        return 0;
    }
    
    return 1;
}


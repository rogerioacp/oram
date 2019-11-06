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

    size_t fileSize = 100;// file with 100 bytes;
    size_t blockSize = 20;// block size of 20 bytes;
    size_t bucketCapcity = 1; // 1 bucket per tree node;
    int result = 0;
    size_t nblocks = fileSize / blockSize;
    int index = 0;
    char *data = NULL;
    state = init_oram("teste", fileSize, blockSize, bucketCapcity, &amgr, NULL);

    for (index = 0; index < nblocks; index++) {
        //printf("Going to read block offset %d\n", index);
        result = read_oram(&data, index, state, NULL);
        if (result != DUMMY_BLOCK) {
            close_oram(state, NULL);
            return 1;
        }
    }
    close_oram(state, NULL);
    return 0;
}

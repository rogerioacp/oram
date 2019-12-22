#include "oram/oram.h"
#include "oram/plblock.h"

#ifdef TEST_PATHORAM
#include "oram/pathoram.h"
#include "oram/pmapdefs/pdeforam.h"
#elif TEST_FORESTORAM
#include "oram/forestoram.h"
#include "oram/pmapdefs/fdeforam.h"
#endif

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {

    AMStash *stash;
    AMPMap  *pmap;
    AMOFile *ofile;
    ORAM    *oram;

    stash = stashCreate();
#ifdef TEST_PATHORAM
    pmap = pmapCreate();
#elif TEST_FORESTORAM
    pmap = fpmapCreate();
#endif
    ofile = ofileCreate();

    Amgr amgr;
    amgr.am_stash = stash;
    amgr.am_pmap = pmap;
    amgr.am_ofile = ofile;

    size_t nBlocks = 15;// file with 100 bytes;
    size_t bSize = 20;// block size of 20 bytes;
    size_t bCapacity = 1; // 1 bucket per tree node;

    int result = 0;
    char *data = NULL;
    printf("Init oram\n");
    
#ifdef TEST_PATHORAM
    oram = init_PathORAM("teste", nBlocks, bSize, bCapacity, &amgr, NULL);
#elif TEST_FORESTORAM
    oram = init_ForestORAM("teste", nBlocks, bSize, bCapacity, 1, &amgr, NULL);
#endif
    
    printf("Going to read\n");
    
    result = oram->read(&data, 0, oram, NULL);
    oram->close(oram, NULL);

    if (result == DUMMY_BLOCK){
        return 0;
    }

    return 1;
}


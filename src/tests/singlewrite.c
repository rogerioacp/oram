#include "oram/oram.h"

#ifdef TEST_PATHORAM
#include "oram/pathoram.h"
#elif TEST_FORESTORAM
#include "oram/forestoram.h"
#endif

#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]) {

    AMStash *stash;
    AMPMap *pmap;
    AMOFile *ofile;
    ORAM* oram;

    stash = stashCreate();
    pmap = pmapCreate();
    ofile = ofileCreate();

    Amgr amgr;
    amgr.am_stash = stash;
    amgr.am_pmap = pmap;
    amgr.am_ofile = ofile;

    size_t nBlocks = 15;// file with 100 bytes;
    size_t bSize = 20;// block size of 20 bytes;
    size_t bCapacity = 1; // 1 bucket per tree node;

    size_t result = 0;
    char *data = NULL;

#ifdef TEST_PATHORAM
    oram = init_PathORAM("teste", nBlocks, bSize, bCapacity, &amgr, NULL);
#elif TEST_FORESTORAM
    oram = init_ForestORAM("teste", nBlocks, bSize, bCapacity, 1, &amgr, NULL);
#endif

    char *teste = "HELLO!";
    size_t s_size = sizeof(char) * strlen(teste);
    result = oram->write(teste, sizeof(char) * strlen(teste), 0, oram, NULL);
    oram->close(oram, NULL)
        ;
    if(result == strlen(teste)){
        return 0;
    }

    return 1;
}


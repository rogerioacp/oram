#include "oram/oram.h"
#include "oram/orandom.h"
#include "oram/plblock.h"

#ifdef TEST_PATHORAM
#include "oram/pathoram.h"
#include "oram/pmapdefs/pdeforam.h"
#elif TEST_FORESTORAM
#include "oram/forestoram.h"
#include "oram/pmapdefs/fdeforam.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *gen_random(const int len) {
    int i = 0;
    char *s = (char *) malloc(sizeof(char) * len);

    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for (i = 0; i < len - 1; ++i) {
        s[i] = alphanum[getRandomInt() % (sizeof(alphanum) - 1)];
    }

    s[len - 1] = '\0';
    return s;
}


int test(size_t nBlocks, size_t bSize, size_t bCapacity, size_t nwrites) {

    int result = 0;
    size_t wOffset = 0;
    size_t blockWriteOffset = 0;

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

    int string_size = 0;
    int index = 0;
    char *data = NULL;
    int nPartitions = 0;
    //printf("Going to init\n");
    //malloc input is size in bytes. sizeof gives size in bytes.
    char **strings = (char **) malloc(sizeof(char *) * nBlocks);
    for (index = 0; index < nBlocks; index++) {
        strings[index] = NULL;
    }
#ifdef TEST_PATHORAM
    oram = init_PathORAM("teste", nBlocks, bSize, bCapacity, &amgr, NULL);
#elif TEST_FORESTORAM
    nPartitions = ceil(log2(nBlocks)); 
    oram = init_ForestORAM("teste", nBlocks, bSize, bCapacity, 
                           nPartitions, &amgr, NULL);
#endif
    //printf("Going to write strings\n");

    for (index = 0; index < nwrites; index++) {
        //printf("Writing loop index %d\n", index);
        string_size = bSize / sizeof(char) - 1;
        /**
          * The string should have at least 1 char or else its too small
          * to actually test something.
          */
        //assert(string_size>1);

        wOffset = (getRandomInt() % nBlocks);
        /* array already stores a malloced string which is being overwritten
         * and if not freed the reference is lost and memory leaked.
         */
        if (strings[wOffset] != NULL) {
            free(strings[wOffset]);
        }
        strings[wOffset] = gen_random(string_size + 1);
        blockWriteOffset = sizeof(char) * strlen(strings[wOffset]) + 1;
        //printf("Generated string %s\n", strings[wOffset]);
        //printf("going to write to oram offset %zu the string %s\n", wOffset, strings[wOffset]);
        oram->write(strings[wOffset], blockWriteOffset, wOffset, oram, NULL);
    }

    for (index = 0; index < nBlocks; index++) {
        //printf("Going to read from oram offset %d\n",index);
        result = oram->read(&data, index, oram, NULL);
        //printf("read from oram offset %d the value %s and compares to %s \n", index, data, strings[index]);

        if ((result != DUMMY_BLOCK && result != strlen(data) + 1) || (result != DUMMY_BLOCK && strcmp(data, strings[index]) != 0)) {
            oram->close(oram, NULL);
            return 1;
        }
        free(strings[index]);
        free(data);
    }

    oram->close(oram, NULL);
    free(strings);
    return 0;
}

int main(int argc, char *argv[]) {
    size_t nBlocks = 500;
    size_t bSize = 20; // bytes
    size_t bCapacity = 1; // nBlocks
    size_t nwrites = 500;

    int n_loops = 1000;
    int i;
    int result = 0;
    for (i = 0; i < n_loops; i++) {
        result |= test(nBlocks, bSize, bCapacity, nwrites);
    }
    return result;
}


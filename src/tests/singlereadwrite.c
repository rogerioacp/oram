#include "oram/oram.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

    size_t fileSize = 300;// file with 100 bytes;
    size_t blockSize = 20;// block size of 20 bytes;
    size_t bucketCapcity = 1; // 1 bucket per tree node;
    size_t result = 0;
    char *data = NULL;

    state = init_oram("teste", fileSize, blockSize, bucketCapcity, &amgr, NULL);
    char *teste = "HELLO!";
    size_t s_size = sizeof(char) * strlen(teste) + 1;
    result = write_oram(teste, s_size, 0, state, NULL);
    result = read_oram(&data, 0, state, NULL);
    result = strcmp(teste, data);
    free(data);
    close_oram(state, NULL);
    return result;
}
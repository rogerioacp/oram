#include "oram.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* gen_random(const int len) {

    char* s = (char*) malloc(sizeof(char)*len);

    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[arc4random() % (sizeof(alphanum) - 1)];
    }

    s[len] = '\0';
    return s;
}
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
    int string_size = 0;
    int index = 0;
    void *data = NULL;
    printf("Going to init\n");
    char **strings = (char**) malloc(sizeof(char*)*nblocks);
    state = init("teste", fileSize, blockSize, bucketCapcity, &amgr);
    printf("Going to write strings\n");
    for(index = 0; index < nblocks; index++){
        string_size = (arc4random()% 10)+1;
        strings[index] = gen_random(string_size+1);
        result = write(strings[index], sizeof(char) * strlen(strings[index])+1, index, state);
    }

    for(index = 0; index < nblocks; index++){
        result = read(&data, index, state);
         if(result != strlen(data)+1|| strcmp(data, strings[index]) != 0){
            return 1;
        } 
    }

    return 0;
}
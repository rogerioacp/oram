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


int test(size_t fileSize, size_t blockSize, size_t bucketCapcity, size_t nwrites){

    size_t result = 0;
    size_t nblocks = fileSize/blockSize;
    size_t wOffset = 0;
    size_t blockWriteOffset = 0;

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
    int readi = 0;
    void *data = NULL;
    //printf("Going to init\n");
    //malloc input is size in bytes. sizeof gives size in bytes.
    char **strings = (char**) malloc(sizeof(char*)*nblocks);
    for(index = 0; index <  nblocks; index++){
        strings[index] = NULL;
    }

    state = init("teste", fileSize, blockSize, bucketCapcity, &amgr);
    //printf("Going to write strings\n");

     for(index = 0; index < nwrites; index++){
        //printf("Writing loop index %d\n", index);
        string_size = blockSize/sizeof(char)-1;
        /**
          * The string should have at least 1 char or else its too small
          * to actually test something.
          */
        //assert(string_size>1);

        wOffset = (arc4random()%nblocks);
        /* array already stores a malloced string which is being overwritten
         * and if not freed the reference is lost and memory leaked.
         */
        if(strings[wOffset] != NULL){
            free(strings[wOffset]);
        }
        strings[wOffset] = gen_random(string_size+1);
        blockWriteOffset = sizeof(char) * strlen(strings[wOffset])+1;

        printf("going to write to oram offset %zu the string %s\n", wOffset, strings[wOffset]);
        write(strings[wOffset], blockWriteOffset, wOffset, state);
        
        for(readi = 0; readi < nblocks; readi++){

           result = read(&data, readi, state);
            printf("read from oram offset %d the value %s and compares to %s \n", readi, data, strings[readi]);

            if((result != 0 && result != strlen(data)+1) ||  (result != 0 && strcmp(data, strings[readi]) != 0)){
                return 1;
            }
        } 
    }

   
    close(state);
    return 0;
}

int main(int argc, char *argv[]) {
    size_t fileSize = 10000; //bytes
    size_t blockSize = 20; // bytes
    size_t bucketCapcity = 1; // nblocks
    size_t nwrites = 500;

    int n_loops = 20;
    int i;
    int result = 0;
    for(i=0; i < n_loops; i++){
        result |= test(fileSize, blockSize, bucketCapcity, nwrites);
    }  
    return result;
}
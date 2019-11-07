/*-------------------------------------------------------------------------
 *
 * ofile.c
 *        Simulation of a file in memory.
 *
 *
 * This code should only be used for tests. The idea of this file is to
 * provide an example and have an in-memory implementation of the methods
 * the ORAM requires to write blocks to a file. This code is used for testing
 * the implementation without having to actually write to a file.
 * 
 * Copyright (c) 2018-2019, HASLab
 *
 * IDENTIFICATION
 *        backend/ofile/ofile.c
 *
 *-------------------------------------------------------------------------
 */

#include "oram/ofile.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


PLBArray file;
unsigned int gnblocks;

static void fileInit(const char *filename, unsigned int nblocks, unsigned int blocksize, void* appData);

static void fileRead(PLBArray  blocks, const char *fileName, BNArray ob_blkno, unsigned int nblocks ,void* appData);

static void fileWrite(PLBArray blocks, const char *fileName, BNArray ob_blkno, unsigned int nblocks, void* appData);

static void fileClose(const char *filename, void* appData);


void fileInit(const char *filename, unsigned int nblocks, unsigned int blocksize, void* appData) {
    int offset;
    file = (PLBArray) malloc(sizeof(PLBlock) * nblocks);
    gnblocks = nblocks;

    for (offset = 0; offset < nblocks; offset++) {
        file[offset] = (PLBlock) malloc(sizeof(struct PLBlock));
        file[offset]->blkno = -1;
        file[offset]->size = blocksize;
        file[offset]->block = (void *) malloc(blocksize);
        // blocks of blocksize bytes
        memset(file[offset]->block, 0, blocksize);
    }

}

void fileRead(PLBArray blocks, const char *fileName, BNArray ob_blknos, unsigned int nblocks,  void* appData) {

    int offset;

    for(offset = 0; offset < nblocks; offset++){
        PLBlock block = blocks[offset];
        BlockNumber ob_blkno = ob_blknos[offset];

        block->blkno = file[ob_blkno]->blkno;
        block->size = file[ob_blkno]->size;
        block->block = malloc(file[ob_blkno]->size);
        memcpy(block->block, file[ob_blkno]->block, file[ob_blkno]->size);

    }
}

void fileWrite(PLBArray blocks, const char *fileName, BNArray ob_blknos, unsigned int nblocks, void* appData) {
    int offset; 

    for(offset = nblocks-1; offset >= 0; offset--){
        PLBlock block = blocks[offset];
        BlockNumber ob_blkno = ob_blknos[offset];

        file[ob_blkno]->blkno = block->blkno;
        file[ob_blkno]->size = block->size;
        memcpy(file[ob_blkno]->block, block->block, block->size);
    }

}


void fileClose(const char * filename, void* appData){
    int i;

    for(i=0; i < gnblocks; i++){
        free(file[i]->block);
        free(file[i]);
    }
    free(file);
}

AMOFile *ofileCreate(void) {
    AMOFile *file = (AMOFile *) malloc(sizeof(AMOFile));
    file->ofileinit = &fileInit;
    file->ofileread = &fileRead;
    file->ofilewrite = &fileWrite;
    file->ofileclose = &fileClose;
    return file;
}

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


PLBList file;
unsigned int gnblocks;

static void fileInit(const char *filename, unsigned int nblocks, unsigned int blocksize, void* appData);

static void fileRead(PLBlock block, const char *fileName, const BlockNumber ob_blkno, void* appData);

static void fileWrite(const PLBlock block, const char *fileName, const BlockNumber ob_blkno, void* appData);

static void fileClose(const char *filename, void* appData);


void fileInit(const char *filename, unsigned int nblocks, unsigned int blocksize, void* appData) {
    int offset;
    file = (PLBList) malloc(sizeof(PLBlock) * nblocks);
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

void fileRead(PLBlock block, const char *fileName, const BlockNumber ob_blkno, void* appData) {
    block->blkno = file[ob_blkno]->blkno;
    block->size = file[ob_blkno]->size;
    block->block = malloc(file[ob_blkno]->size);
    //printf("Going to read block number %d\n", ob_blkno);
    memcpy(block->block, file[ob_blkno]->block, file[ob_blkno]->size);
}

void fileWrite(const PLBlock block, const char *fileName, const BlockNumber ob_blkno, void* appData) {
    file[ob_blkno]->blkno = block->blkno;
    file[ob_blkno]->size = block->size;
    //printf("Going to write block number %d\n", ob_blkno);
    memcpy(file[ob_blkno]->block, block->block, block->size);
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
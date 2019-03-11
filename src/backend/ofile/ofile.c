
#include "ofile.h"
#include <stdio.h>
#include <string.h>

PLBList file;
size_t gnblocks;

static void fileInit(const char *filename, size_t nblocks, size_t blocksize);

static void fileRead(PLBlock block, const char *fileName, const BlockNumber ob_blkno);

static void fileWrite(const PLBlock block, const char *fileName, const BlockNumber ob_blkno);

static void fileClose(const char *filename);


void fileInit(const char *filename, size_t nblocks, size_t blocksize) {
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

void fileRead(PLBlock block, const char *fileName, const BlockNumber ob_blkno) {
    block->blkno = file[ob_blkno]->blkno;
    block->size = file[ob_blkno]->size;
    block->block = malloc(file[ob_blkno]->size);
    memcpy(block->block, file[ob_blkno]->block, file[ob_blkno]->size);
}

void fileWrite(const PLBlock block, const char *fileName, const BlockNumber ob_blkno) {
    file[ob_blkno]->blkno = block->blkno;
    file[ob_blkno]->size = block->size;
    memcpy(file[ob_blkno]->block, block->block, block->size);
}


void fileClose(const char * filename){
    int i;

    for(i=0; i < gnblocks; i++){
        free(file[i]->block);
        free(file[i]);
    }
    free(file);
}

AMOFile *ofileCreate() {
    AMOFile *file = (AMOFile *) malloc(sizeof(AMOFile));
    file->ofileinit = &fileInit;
    file->ofileread = &fileRead;
    file->ofilewrite = &fileWrite;
    file->ofileclose = &fileClose;
    return file;
}
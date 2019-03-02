
#include "ofile.h"
#include <stdio.h>
#include <string.h>

PLBList file;

static void fileInit(const char *filename, size_t nblocks, size_t blocksize);

static void fileRead(PLBlock block, const char *fileName, const BlockNumber ob_blkno);

static void fileWrite(const PLBlock block, const char *fileName, const BlockNumber ob_blkno);


void fileInit(const char *filename, size_t nblocks, size_t blocksize) {
    int offset;
    file = (PLBList) malloc(sizeof(PLBlock) * nblocks);

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
   // memset(block->block, 0, block->size);
    memcpy(block->block, file[ob_blkno]->block, file[ob_blkno]->size);
}

void fileWrite(const PLBlock block, const char *fileName, const BlockNumber ob_blkno) {
    file[ob_blkno]->blkno = block->blkno;
    file[ob_blkno]->size = block->size;
    //printf("Going to write block to file %s  with size %d\n", block->block, block->size);
    memcpy(file[ob_blkno]->block, block->block, block->size);


}

AMOFile *ofileCreate() {
    AMOFile *file = (AMOFile *) malloc(sizeof(AMOFile));
    file->ofileinit = &fileInit;
    file->ofileread = &fileRead;
    file->ofilewrite = &fileWrite;
    return file;
}
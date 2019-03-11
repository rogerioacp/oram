
#include "block.h"
#include "logger.h"

#include <errno.h>
#include <string.h>

//Assumes that the block already comes allocated from the client.
PLBlock createBlock(int blkno, int size, void *data) {
    int save_errno = 0;

    PLBlock block = createEmptyBlock();

    block->blkno = blkno;
    block->size = size;

    save_errno = errno;
    errno = 0;
    block->block = (void *) malloc(size);

    if (block->block == NULL && errno == ENOMEM) {
        logger(OUT_OF_MEMORY);
        errno = save_errno;
        abort();
    }
    memcpy(block->block, data, size);
    errno = save_errno;

    return block;
}


PLBlock createEmptyBlock() {
    int save_errno = 0;

    save_errno = errno;
    errno = 0;

    PLBlock block = (PLBlock) malloc(sizeof(struct PLBlock));

    if (block == NULL && errno == ENOMEM) {
        logger(OUT_OF_MEMORY);
        errno = save_errno;
        abort();
    }

    block->blkno = DUMMY_BLOCK;
    block->size = -1;
    block->block = NULL;
    errno = save_errno;
    return block;
}

PLBlock createRandomBlock(size_t size) {
    int save_errno = 0;
    PLBlock block = createEmptyBlock();

    save_errno = errno;
    errno = 0;

    block->block = (void *) malloc(size);

    if (block->block == NULL && errno == ENOMEM) {
        logger(OUT_OF_MEMORY);
        errno = save_errno;
        abort();
    }

    memset(block->block, 0, size);
    block->size = size;
    errno = save_errno;
    return block;
}

void freeBlock(PLBlock block) {
    free(block->block);
    free(block);
}


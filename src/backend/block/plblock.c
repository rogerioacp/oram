/*-------------------------------------------------------------------------
 *
 * block.c
 *      Methods to allocate and free oram blocks.
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 * IDENTIFICATION
 *        backend/block/block.c
 *
 *-------------------------------------------------------------------------
 */

#include "oram/plblock.h"
#include "oram/logger.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>

PLBlock		dummyBlock = NULL;

/* Assumes that the block already comes allocated from the client. */
PLBlock
createBlock(int blkno, int size, void *data)
{
	int			save_errno = 0;

	PLBlock		block = createEmptyBlock();

	block->blkno = blkno;
	block->size = size;

	save_errno = errno;
	errno = 0;
	block->block = (void *) malloc(size);

	if (block->block == NULL && errno == ENOMEM)
	{
		logger(OUT_OF_MEMORY, "Out of memory createBlock");
		errno = save_errno;
		abort();
	}
	memcpy(block->block, data, size);
	errno = save_errno;

	return block;
}


PLBlock
createEmptyBlock(void)
{
	int			save_errno = 0;

	save_errno = errno;
	errno = 0;

	PLBlock		block = (PLBlock) malloc(sizeof(struct PLBlock));

	if (block == NULL && errno == ENOMEM)
	{
		logger(OUT_OF_MEMORY, "Out of memory createEmptyBlock");
		errno = save_errno;
		abort();
	}

	block->blkno = DUMMY_BLOCK;
	block->size = -1;
	block->block = NULL;
    memset(block->location, 0, sizeof(unsigned int)*2);
	errno = save_errno;
	return block;
}

PLBlock
createRandomBlock(unsigned int size, unsigned int lsize)
{
	int			save_errno = 0;
	PLBlock		block = createEmptyBlock();

	save_errno = errno;
	errno = 0;

	block->block = (void *) malloc(size);
    
	if (block->block == NULL && errno == ENOMEM)
	{
		logger(OUT_OF_MEMORY, "Out of memory createRandomBlock");
		errno = save_errno;
		abort();
	}

	memset(block->block, 0, size);
    block->location[0] = block->location[1] = 0;
	block->size = size;
	errno = save_errno;
	return block;
}

PLBlock
createDummyBlock(unsigned int size, unsigned int lsize)
{

	if (dummyBlock == NULL)
	{
		dummyBlock = createRandomBlock(size, lsize);
	}

	return dummyBlock;
}

/*
void
setLocation(PLBlock block, Location location, unsigned int size){

    block->lsize = size;
    block->location = (Location) malloc(block->lsize);

    memcpy(block->location, location, block->lsize);
    //block->location->leaf = location->leaf;
    //logger(DEBUG, "location is %d\n", block->location->leaf); 
}*/


void
freeBlock(PLBlock block)
{
    free(block->block);
	free(block);
}

void 
freeDummyBlock(){
    if(dummyBlock != NULL){
        if(dummyBlock->block !=NULL){
            free(dummyBlock->block);
        }
        free(dummyBlock);
    }
    dummyBlock = NULL;
}

/*-------------------------------------------------------------------------
 *
 * block.h
 *	  Common block definitions for any component working with blocks.
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef BLOCK_H
#define BLOCK_H

#define DUMMY_BLOCK -1

#include <stdlib.h>

typedef size_t BlockNumber;

typedef struct PLBlock
{
	int			blkno; // blkno is -1 if its dummy block.
	int			size;
	void	   *block;
}		   *PLBlock;

typedef PLBlock *PLBList;

#endif   /* BLOCK_H */
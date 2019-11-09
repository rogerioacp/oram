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

#ifndef PLBLOCK_H
#define PLBLOCK_H

#define DUMMY_BLOCK -1

typedef unsigned int BlockNumber;

typedef struct PLBlock
{
	int			blkno;
			  //blkno is - 1 if its dummy block.
				int size;
	void	   *block;
}		   *PLBlock;

typedef PLBlock *PLBList;

PLBlock		createBlock(int blkno, int size, void *block);

PLBlock		createEmptyBlock(void);

PLBlock		createRandomBlock(unsigned int size);

PLBlock		createDummyBlock(unsigned int size);

void		freeBlock(PLBlock block);


#endif							/* PLBLOCK_H */

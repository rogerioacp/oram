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

typedef size_t BlockNumber;

typedef struct PLBlock{
	int blkno; //Original File BlockNumber, -1 if dummy block.
	void* block;
} *PLBlock; //Plaintext block

typedef PLBlock* PLBList;


#endif 		/*BLOCK_H*/
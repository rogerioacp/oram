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

#include "oram/common.h"
#include "oram/pmap.h"


/* For each paintext block we keep track of its current location 
 * (e.g.: tree leaf in pathoram).
 * This location is necessary for constructions without an internal
 * position map to enable the eviction algorithm to know the correct
 * location (e.g.: tree Leaf in path oram) of a block.
 * If the construction has a pmap, than the lsize can be set to 0 and
 * the location is ignored.
 *
 * As the struct location depends on the ORAM construction, we keep the
 * possible locaitons in an inte array. Currently, the array supports at most
 * two values (leaf and partition). In case of a pathoram construction,
 * the second position is ignored. While not the most elegant solution,
 * it simplifies the integration with other projects.
 **/
typedef struct PLBlock
{
    
    /* blkno is - 1 if its dummy block */
	int			    blkno; 
    int             size;
    unsigned int    location[2];
	void*           block;

}		   *PLBlock;

typedef PLBlock *PLBList;

PLBlock		createBlock(int blkno, int size, void *block);

PLBlock		createEmptyBlock(void);

PLBlock		createRandomBlock(unsigned int size, unsigned int lsize);

PLBlock		createDummyBlock(unsigned int size, unsigned int lsize);

void        setLocation(PLBlock block, Location location, unsigned int size);

void		freeBlock(PLBlock block);

void        freeDummyBlock();

#endif							/* PLBLOCK_H */

/*-------------------------------------------------------------------------
 *
 * pathoram.h
 *	  prototypes for pathoram.h
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 * pathoram.c
 *
 *-------------------------------------------------------------------------
 */
#ifndef PATHORAM_H
#define PATHORAM_H

#include "oram/oram.h"


/**
 * Function that initializes and returns the state of an ORAM algorithm.
 * args:
 * size_t fileSize - Expected size of the protected file.
 * size_t blockSize - Size of each block in the protected file.
 * size_t bucketCapcity - Number of buckets in an ORAM Tree node.
 * Amgr amgr - Access manager functions to store data.
 *
 */

ORAM   init_PathORAM(const char *file, 
                 unsigned int nblocks, 
                 unsigned int blockSize, 
                 unsigned int bucketCapacity,
                 ORAM *state, voidid *appData);


#endif                  /*PATHORAM_H*/

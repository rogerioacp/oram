/*-------------------------------------------------------------------------
 *
 * foram.h
 *	  prototypes for foram.c.
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 * foram.h is an extension of the ORAM interface.
 * 
 * Besides providing the default primitives to read and write obliviously from
 * a file, it also provides two additional primitives that separates the 
 * process of reading blocks obliviously and evicting the accessed blocks.
 *
 * This API extension is necessary enable some systems to execute background
 * tasks that shuffle and evict the blocks while the main process is doing 
 * some additional processing.
 *
 *-------------------------------------------------------------------------
 */
#ifndef FORAM_H
#define FORAM_H


#include "oram/oram.h"


int read_foram(char **ptr, BlockNumber blkno, ORAMState state, void* appData);

int evict_foram(char* data, unsigned int blksize, BlockNumber blkno,ORAMState state, void* appData);


#endif     /* FORAM_H */    
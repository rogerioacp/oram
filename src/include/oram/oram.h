/*-------------------------------------------------------------------------
 *
 * oram.h
 *	  prototypes for oram.c.
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 * oram.c
 *
 *-------------------------------------------------------------------------
 */
#ifndef ORAM_H
#define ORAM_H


#include "oram/plblock.h"
#include "oram/stash.h"
#include "oram/pmap.h"
#include "oram/ofile.h"


/***
 * Opaque structure definition of ORAM Internal State.
 * This state is hidden from any client application and depends
 * on the underlying implementation.
 */
typedef struct ORAMState *ORAMState;


/**
 *  Structure used by the ORAM functions to read necessary
 *  blocks to complete a request.
 *
 *  The ORAM algorithms require multiple reads and write on different files
 *  to not only create the oblivious file but also store some additional
 *  metadata.
 *
 * This structure currently contemplates position based ORAM algorithms that
 * have a stash, a positionMap and the underlying oblivious file.
 *
 * The algorithm can at different times read/write to the stash or
 * the position map to update its internal state.
 *
 */
typedef struct Amgr
{
	AMStash    *am_stash;
	AMPMap	   *am_pmap;
	AMOFile    *am_ofile;
} Amgr;


/**
 * Function that initializes and returns the state of an ORAM algorithm.
 * args:
 * size_t fileSize - Expected size of the protected file.
 * size_t blockSize - Size of each block in the protected file.
 * size_t bucketCapcity - Number of buckets in an ORAM Tree node.
 * Amgr amgr - Access manager functions to store data.
 *
 */

ORAMState	init_oram(const char *file, unsigned int nblocks, unsigned int blockSize, unsigned int bucketCapacity, Amgr *amgr, void *appData);

/**
 * ORAM read operation that triggers a sequence of oblivious file reads and
 * writes to find the input BlockNumber blkno. Returns a char* containing the
 * requested block.
 *
 */
int			read_oram(char **ptr, BlockNumber blkno, ORAMState state, void *appdata);

/**
 * ORAM write request that triggers a sequence of oblivious file reads and
 * writes that hides the input block.
 *
 */
int			write_oram(char *data, unsigned int blksize, BlockNumber blkno, ORAMState state, void *appData);


/**
 * Close request that correctly closes all of the ORAM resourceS:
 * - Oblivious File (e.g: File descriptors)
 * - Position Map
 * - Stash
 */
void		close_oram(ORAMState state, void *appData);

#ifdef STASH_COUNT
void        logStashes(ORAMState state);
#endif
void        evictTimers(ORAMState state);

#endif							/* ORAM_H */

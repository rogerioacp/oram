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
 * ORAM read operation that triggers a sequence of oblivious file reads and
 * writes to find the input BlockNumber blkno. Returns a char* containing the
 * requested block.
 *
 */
typedef void (*oram_read) (char **ptr, 
                           BlockNumber blkno, 
                           ORAMState state, void *appdata);


/**
 * ORAM write request that triggers a sequence of oblivious file reads and
 * writes that hides the input block.
 *
 */
typedef void (*oram_write) (char *data, unsigned int blksize, 
                            blocknumber blkno, ORAMState state, void *appdata);

/**
 * Close request that correctly closes all of the ORAM resourceS:
 * - Oblivious File (e.g: File descriptors)
 * - Position Map
 * - Stash
 */

typedef void (*oram_close) (ORAMState state, void *appData);



#ifdef STASH_COUNT
typedef void (*oram_logstashes) (ORAMState state);
#endif


typedef struct ORAM
{

    //internal state
    ORAMState state;

    //input functions
    oram_read   read;
    oram_write  write;
    oram_close  close;

} ORAM;

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


#endif							/* ORAM_H */

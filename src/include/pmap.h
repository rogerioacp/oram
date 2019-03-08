/*-------------------------------------------------------------------------
 *
 * pmap.h
 *	  prototypes for any position map implementation in backend/pmap
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef PMAP_H
#define PMAP_H

#include "block.h"

// nBlocks -> number of original blocks.
typedef void (*pminit_function)(const char *fileName, const size_t nBlocks, const size_t treeHeight);

typedef size_t (*pmget_function)(const char *fileName, const BlockNumber blkno);

typedef void (*pmupdate_function)(const BlockNumber newBlkno, const BlockNumber realBlkno, const char *fileName);

typedef void (*pmclose_function)(const char *fileName);


/*Access manager to position map*/
typedef struct AMPMap {
    pminit_function pminit;
    pmget_function pmget;
    pmupdate_function pmupdate;
    pmclose_function pmclose;
} AMPMap;

AMPMap *pmapCreate();

#endif        /*PMAP_H*/
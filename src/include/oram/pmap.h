/*-------------------------------------------------------------------------
 *
 * pmap.h
 *	  prototypes for any position map implementation in backend/pmap
 *
 *
 * Copyright (c) 2018-2020, HASLab
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef PMAP_H
#define PMAP_H

#include "oram/common.h"

typedef struct PMap *PMap;
typedef struct Location *Location;
typedef struct TreeConfig *TreeConfig;


/*  nBlocks -> number of original blocks. */
typedef PMap (*pminit_function) (const char *fileName, const unsigned int nBlocks, TreeConfig treeCofnig);

typedef Location (*pmget_function) (PMap pmap, const char *fileName, const BlockNumber blkno);

typedef void (*pmupdate_function) (PMap pmap, const char *fileName, const BlockNumber realBlkno);

typedef void (*pmclose_function) (PMap pmap, const char *fileName);

/*Access manager to position map*/
typedef struct AMPMap
{
	pminit_function pminit;
	pmget_function pmget;
	pmupdate_function pmupdate;
	pmclose_function pmclose;
} AMPMap;

AMPMap	   *pmapCreate(void);

#endif							/* OFILE_H*/

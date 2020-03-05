/*-------------------------------------------------------------------------
 *
 * pmap.c
 *      In-memory implementation of a position map.
 *
 * Implementation of a in-memory position map that maps block offsets to
 * oram leaf nodes in an array. This implementation assumes that only a
 * single file is being accessed obliviously and ignores the filename.
 *
 * Copyright (c) 2018-2019, HASLab
 *
 * IDENTIFICATION
 *        backend/pmap/pmap.c
 *
 *-------------------------------------------------------------------------
 */

#include <math.h>
#include <stdlib.h>

#include "oram/pmap.h"
#include "oram/orandom.h"
#include "oram/pmapdefs/pdeforam.h"

struct PMap
{
	struct Location *map;
    int treeHeight;
};


static PMap pmapInit(const char *filename, const unsigned int nblocks, TreeConfig config);

static Location pmapGet(PMap pmap, const char *fileName, const BlockNumber blkno);

static void pmapUpdate(PMap pmap, const char *fileName, const BlockNumber realBlkno);

static void pmapClose(PMap pmap, const char *filename);

PMap
pmapInit(const char *filename, unsigned int nblocks, TreeConfig treeConfig)
{
	int			i;
	BlockNumber r;
	PMap		pmap;

	pmap = (PMap) malloc(sizeof(struct PMap));
	pmap->map = (Location) malloc(sizeof(struct Location) * nblocks);
    pmap->treeHeight = treeConfig->treeHeight;

	for (i = 0; i < nblocks; i++)
	{
		r = (BlockNumber) ((BlockNumber) getRandomInt()) % ((BlockNumber) (pow(2, treeConfig->treeHeight)));
		pmap->map[i].leaf = r;
	}

	return pmap;
}

Location
pmapGet(PMap pmap, const char *fileName, const BlockNumber blkno)
{
	return &pmap->map[blkno];
}


void
pmapUpdate(PMap pmap, const char *fileName, const BlockNumber realBlkno){
    pmap->map[realBlkno].leaf = (BlockNumber) ((BlockNumber) getRandomInt()) % ((BlockNumber) (pow(2, pmap->treeHeight)));

}

void
pmapClose(PMap pmap, const char *filename)
{
	free(pmap->map);
	free(pmap);
}

void pmapSetToken(PMap pmap, const unsigned int* token){
}
AMPMap *
pmapCreate(void)
{
	AMPMap	   *pmap = (AMPMap *) malloc(sizeof(AMPMap));

	pmap->pminit = &pmapInit;
	pmap->pmget = &pmapGet;
	pmap->pmupdate = &pmapUpdate;
	pmap->pmclose = &pmapClose;
    pmap->pmstoken = &pmapSetToken;
	return pmap;
}

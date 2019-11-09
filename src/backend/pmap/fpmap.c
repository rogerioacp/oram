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
#include <stdio.h>
#include <stdlib.h>

#include "oram/pmap.h"
#include "oram/orandom.h"
#include "oram/pmapdefs/foram.h"


struct PMap {
    struct Location *map;
};


static PMap pmapInit(const char *filename, const unsigned int nblocks, TreeConfig config);

static Location pmapGet(PMap pmap, const char *fileName, const BlockNumber blkno);

static void pmapUpdate(PMap pmap, Location newLocation, const BlockNumber realBlkno, const char *fileName);

static void pmapClose(PMap pmap, const char *filename);

PMap pmapInit(const char *filename, unsigned int nblocks, TreeConfig treeConfig) {

    unsigned int treeHeight;
    unsigned int nPartitions;
    int i;
    unsigned int leaf;
    unsigned int partition;

    treeHeight = treeConfig->treeHeight;
    nPartitions = treeConfig->nPartitions;

    PMap pmap;

    pmap = (PMap) malloc(sizeof(struct PMap));
    pmap->map = (Location) malloc(sizeof(struct Location) * nblocks);

    for (i = 0; i < nblocks; i++) {

        leaf = (BlockNumber) ((BlockNumber) getRandomInt()) % ((BlockNumber) (pow(2, treeHeight)));
        partition = (BlockNumber) ((BlockNumber) getRandomInt()  % ((BlockNumber) nPartitions));
        pmap->map[i].partition = partition;
        pmap->map[i].leaf = leaf;
    }

    return pmap;
}

Location pmapGet(PMap pmap, const char *fileName, const BlockNumber blkno) {
    return &pmap->map[blkno];
}

void pmapUpdate(PMap pmap, Location newLocation, const BlockNumber realBlkno, const char *fileName) {
    pmap->map[realBlkno].partition = newLocation->partition;
    pmap->map[realBlkno].leaf = newLocation->leaf;
}


void pmapClose(PMap pmap, const char *filename) {
    free(pmap->map);
    free(pmap);
}

AMPMap *pmapCreate(void) {
    AMPMap *pmap = (AMPMap *) malloc(sizeof(AMPMap));
    pmap->pminit = &pmapInit;
    pmap->pmget = &pmapGet;
    pmap->pmupdate = &pmapUpdate;
    pmap->pmclose = &pmapClose;
    return pmap;
}
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


unsigned int *map;


static void pmapInit(const char *filename, const unsigned int nblocks, const unsigned int treeHeight);

static unsigned int pmapGet(const char *fileName, const BlockNumber blkno);

static void pmapUpdate(const BlockNumber newBlkno, const BlockNumber realBlkno, const char *fileName);

static void pmapClose(const char *filename);

void pmapInit(const char *filename, unsigned int nblocks, unsigned int treeHeight) {
    int i;
    map = (BlockNumber *) malloc(sizeof(BlockNumber) * nblocks);
    BlockNumber r;
    for (i = 0; i < nblocks; i++) {
        r = (BlockNumber) ((BlockNumber) getRandomInt()) % ((BlockNumber) (pow(2, treeHeight)));
        map[i] = r;
    }
}

unsigned int pmapGet(const char *fileName, const BlockNumber blkno) {
    return map[blkno];
}

void pmapUpdate(const BlockNumber newBlkno, const BlockNumber realBlkno, const char *fileName) {
    map[realBlkno] = newBlkno;
}

void pmapClose(const char *filename) {
    free(map);
}

AMPMap *pmapCreate(void) {
    AMPMap *pmap = (AMPMap *) malloc(sizeof(AMPMap));
    pmap->pminit = &pmapInit;
    pmap->pmget = &pmapGet;
    pmap->pmupdate = &pmapUpdate;
    pmap->pmclose = &pmapClose;
    return pmap;
}
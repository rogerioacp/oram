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
#include <errno.h>


#include "oram/pmap.h"
#include "oram/logger.h"
#include "oram/orandom.h"
#include "oram/pmapdefs/fdeforam.h"


struct PMap {
    struct Location *map;
    int treeHeight;
    int nPartitions;
};


static PMap pmapInit(const char *filename, const unsigned int nblocks, TreeConfig config);

static Location pmapGet(PMap pmap, const char *fileName, const BlockNumber blkno);

static void pmapUpdate(PMap pmap, const char *fileName, const BlockNumber realBlkno);

static void pmapClose(PMap pmap, const char *filename);

PMap pmapInit(const char *filename, unsigned int nblocks, TreeConfig treeConfig) {

    unsigned int treeHeight;
    unsigned int nPartitions;
    int i;
    unsigned int leaf;
    unsigned int partition;
    unsigned int save_errno = 0;



    treeHeight = treeConfig->treeHeight;
    nPartitions = treeConfig->nPartitions;

    PMap pmap;
    
    save_errno = errno;
    pmap = (PMap) malloc(sizeof(struct PMap));
    
    if(pmap == NULL && errno == ENOMEM){
        logger(OUT_OF_MEMORY, "Out of memory when allocating possition map\n");
        errno = save_errno;
        abort();
    }

    errno = 0;
    pmap->map = (Location) malloc(sizeof(struct Location) * nblocks);
    
    if(pmap->map == NULL && errno == ENOMEM){
        logger(OUT_OF_MEMORY, "Out of memory when allocation pmap blocks\n");
        errno = save_errno;
        abort();
    }
    
    errno = save_errno;

    for (i = 0; i < nblocks; i++) {

        leaf = (BlockNumber) ((BlockNumber) getRandomInt()) % ((BlockNumber) (pow(2, treeHeight)));
        partition = (BlockNumber) ((BlockNumber) getRandomInt()  % ((BlockNumber) nPartitions));
        
        if( (leaf < 0 || leaf > pow(2, treeHeight)) || (partition > nPartitions) ){
            logger(DEBUG, "pmap leaf %d or partition %d do not match\n");
        }

        pmap->map[i].partition = partition;
        pmap->map[i].leaf = leaf;
    }
    pmap->treeHeight = treeHeight;
    pmap->nPartitions = nPartitions;

    return pmap;
}

Location pmapGet(PMap pmap, const char *fileName, const BlockNumber blkno) {
    return &pmap->map[blkno];
}

void pmapUpdate(PMap pmap, const char *fileName, const BlockNumber realBlkno) {
    pmap->map[realBlkno].partition =  (BlockNumber) ((BlockNumber) getRandomInt()  % ((BlockNumber) pmap->nPartitions));
    pmap->map[realBlkno].leaf = (BlockNumber) ((BlockNumber) getRandomInt()) % ((BlockNumber) (pow(2, pmap->treeHeight)));

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


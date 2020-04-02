/*-------------------------------------------------------------------------
 *
 * Token pmap for path oram
 * Implementation of a pmap that generates the leaf of a pathoram tree
 * from a cryptographic token given as input by a client application.
 * 
 * Copyright (c) 2018-2020, HASLab
 *
 * IDENTIFICATION
 *        backend/pmap/tpmap.c
 *
 *-------------------------------------------------------------------------
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "oram/logger.h"
#include "oram/pmap.h"
#include "oram/orandom.h"
#include "oram/logger.h"
#include "oram/pmapdefs/pdeforam.h"

#include <time.h>


//The token size is 4 integers (128 bits, the size of an AES block)
#define TOKEN_SIZE 4

struct PMap
{

    int treeHeight; 
    unsigned int* token;
    Location loc;
};


static PMap pmapInit(const char *filename, const unsigned int nblocks, TreeConfig config);

static Location pmapGet(PMap pmap, const char *fileName, const BlockNumber blkno);

static void pmapUpdate(PMap pmap, const char *fileName, const BlockNumber realBlkno);

static void pmapClose(PMap pmap, const char *filename);

static void pmapSetToken(PMap pmap, const unsigned int* token);

PMap
pmapInit(const char *filename, unsigned int nblocks, TreeConfig treeConfig)
{
	int			i;
	BlockNumber r;
	PMap		pmap;

	pmap = (PMap) malloc(sizeof(struct PMap));
    pmap->treeHeight = treeConfig->treeHeight;
    pmap->token = (unsigned int*) malloc(TOKEN_SIZE*sizeof(unsigned int));
    pmap->loc = (Location) malloc(sizeof(struct Location));

	return pmap;
}

void pmapSetToken(PMap pmap, const unsigned int* token){
    memcpy(pmap->token, token, TOKEN_SIZE*sizeof(unsigned int));
}

Location
pmapGet(PMap pmap, const char *fileName, const BlockNumber blkno)
{
  
    struct timespec ts_start;
	struct timespec ts_end;
    double elapsedTime;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);

	pmap->loc->leaf = (pmap->token[0] % ((BlockNumber) (pow(2, pmap->treeHeight))));

    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    elapsedTime = (ts_end.tv_nsec-ts_start.tv_nsec);
    
    logger(PROFILE, "PMAP_GET %s %f\n", fileName, elapsedTime);   

    return pmap->loc;

    
}


void
pmapUpdate(PMap pmap, const char *fileName, const BlockNumber realBlkno){
    struct timespec ts_start;
    struct timespec ts_end;
    double elapsedTime;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);

    //Simply shifts the next token to the first posstion
    //Both the path oram and forest oram will issue a new pmapGet request
    //that will return a new leaf location.
    pmap->token[0] = pmap->token[1]; 
  
    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    elapsedTime = (ts_end.tv_nsec-ts_start.tv_nsec);

    logger(PROFILE, "PMAP_UPDATE %s %f\n", fileName, elapsedTime);   

}

void
pmapClose(PMap pmap, const char *filename)
{
    free(pmap->loc);
    free(pmap->token);
	free(pmap);
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

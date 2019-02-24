
#include <math.h>
#include <stdio.h>

#include "pmap.h"


size_t* map;


static void pmapInit(const char *filename, const size_t nblocks, const size_t treeHeight);
static size_t pmapGet(const char* fileName, const BlockNumber blkno);
static void pmapUpdate(const BlockNumber newBlkno, const BlockNumber realBlkno, const char* fileName);


void pmapInit(const char *filename, size_t nblocks, size_t treeHeight)
{
	map = (BlockNumber*) malloc(sizeof(BlockNumber)*nblocks);
	int i;
	BlockNumber r;
	for(i = 0; i < nblocks; i++){
		r =  (BlockNumber) ((BlockNumber)arc4random())%((BlockNumber)(pow(2,treeHeight)));
		map[i] = r;
	}
}

size_t pmapGet(const char* fileName, const BlockNumber blkno)
{
	return map[blkno];
}

void pmapUpdate(const BlockNumber newBlkno, const BlockNumber realBlkno, const char* fileName){
	map[realBlkno] = newBlkno;
}


AMPMap* pmapCreate(){
	AMPMap* pmap = (AMPMap*) malloc(sizeof(AMPMap));
	pmap->pminit = &pmapInit;
	pmap->pmget = &pmapGet;
	pmap->pmupdate = &pmapUpdate;
	return pmap;
}
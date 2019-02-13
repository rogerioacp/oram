
#include "pmap.h"

size_t* map;

void pmapInit(const char *filename, size_t nblocks){
	map = (size_t*) malloc(sizeof(size_t)*nblocks);
}

size_t pmapRead(const char* fileName, const BlockNumber blkno)
{
	return map[blkno];
}

void pmapUpdate(const BlockNumber newBlkno, const BlockNumber realBlkno, const char* fileName){
	map[realBlkno] = newBlkno;
}
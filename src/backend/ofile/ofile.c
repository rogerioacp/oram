
#include "ofile.h"

PLBList file;

void fileInit(const char *filename, size_t nblocks, size_t blocksize){
	file = (PLBList) malloc(sizeof(PLBlock)*nblocks);
}

void fileRead(PLBlock* block, const char *fileName, const BlockNumber ob_blkno)
{
	*block = file[ob_blkno];
}

void fileWrite(const PLBlock block, const char *fileName, const BlockNumber ob_blkno){
	file[ob_blkno] = block;
}
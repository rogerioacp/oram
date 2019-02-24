

#include "oram.h"
#include "stash.h"
#include "pmap.h"
#include "ofile.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]){

	AMStash* stash;
	AMPMap* pmap;
	AMOFile* ofile;
	ORAMState state;

	stash = stashCreate();
	pmap = pmapCreate();
	ofile = ofileCreate();

	Amgr amgr;
	amgr.am_stash = stash;
	amgr.am_pmap = pmap;
	amgr.am_ofile = ofile;

	size_t fileSize = 100;// file with 100 bytes;
	size_t blockSize = 20;// block size of 20 bytes;
	size_t bucketCapcity = 1; // 1 bucket per tree node;
	size_t result = 0;
	void* data = NULL;

	state = init("teste", fileSize, blockSize, bucketCapcity, &amgr);
	char* teste= "HELLO!";
	size_t s_size = sizeof(char)*strlen(teste);
	result = write(teste, sizeof(char)*strlen(teste), 0, state);
	result = read(&data, 0, state);
	printf("read string %s", data);

	return 0;
}
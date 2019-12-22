#include "oram/oram.h"
#include "oram/plblock.h"

#ifdef TEST_PATHORAM
#include "oram/pathoram.h"
#include "oram/pmapdefs/pdeforam.h"
#elif TEST_FORESTORAM
#include "oram/forestoram.h"
#include "oram/pmapdefs/fdeforam.h"
#endif


#include <stdio.h>
#include <string.h>

int
main(int argc, char *argv[])
{

	AMStash    *stash;
	AMPMap	   *pmap;
	AMOFile    *ofile;
	ORAM       *oram;

	stash = stashCreate();
#ifdef TEST_PATHORAM
    pmap = pmapCreate();
#elif TEST_FORESTORAM
    pmap = fpmapCreate();
#endif

	ofile = ofileCreate();

	Amgr		amgr;

	amgr.am_stash = stash;
	amgr.am_pmap = pmap;
	amgr.am_ofile = ofile;

	size_t		nBlocks = 15;
	size_t		bSize = 20; // block size of 20 bytes;
	size_t		bCapacity = 1; // 1 bucker per tree node;


	int			result = 0;
	int			index = 0;
	char	    *data = NULL;

#ifdef TEST_PATHORAM
    oram = init_PathORAM("teste", nBlocks, bSize, bCapacity, &amgr, NULL);
#elif TEST_FORESTORAM
    oram = init_ForestORAM("teste", nBlocks, bSize, bCapacity, 1, &amgr, NULL);
#endif


	for (index = 0; index < nBlocks; index++)
	{
		/* printf("Going to read block offset %d\n", index); */
		result = oram->read(&data, index, oram, NULL);
		if (result != DUMMY_BLOCK)
		{
			oram->close(oram, NULL);
			return 1;
		}
	}
	oram->close(oram, NULL);
	return 0;
}


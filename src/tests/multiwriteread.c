#include "oram/oram.h"
#include "oram/orandom.h"

#ifdef TEST_PATHORAM
#include "oram/pathoram.h"
#include "oram/pmapdefs/pdeforam.h"
#elif TEST_FORESTORAM
#include "oram/forestoram.h"
#include "oram/pmapdefs/fdeforam.h"
#endif



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
gen_random(const int len)
{
	int			i = 0;
	char	   *s = (char *) malloc(sizeof(char) * len);
	static const char alphanum[] =
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz";

	for (i = 0; i < len - 1; ++i)
	{
		s[i] = alphanum[getRandomInt() % (sizeof(alphanum) - 1)];
	}

	s[len - 1] = '\0';
	return s;
}

int
main(int argc, char *argv[])
{

	AMStash    *stash;
	AMPMap	   *pmap;
	AMOFile    *ofile;
	ORAM	   *oram;

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
    size_t		bSize = 20; //block size of 20 bytes;
	size_t		bCapacity = 1; //1 bucket per tree node;
	int			result = 0;
	
    int			string_size = 0;
	int			index = 0;
	char	    *data = NULL;

	/* printf("Going to init\n"); */
	char	  **strings = (char **) malloc(sizeof(char *) * nBlocks);


#ifdef TEST_PATHORAM
    oram = init_PathORAM("teste", nBlocks, bSize, bCapacity, &amgr, NULL);
#elif TEST_FORESTORAM
    oram = init_ForestORAM("teste", nBlocks, bSize, bCapacity, 1, &amgr, NULL);
#endif

	/* printf("Going to write strings\n"); */

	for (index = 0; index < nBlocks; index++)
	{
		string_size = bSize / sizeof(char) - 1;
		string_size = string_size == 0 ? 1 : string_size;
		string_size += 1;
		strings[index] = gen_random(string_size);

		/*
		printf("Going to write on offset %d the string %s\n",index,
               strings[index]);*/

		result = oram->write(strings[index], 
                             sizeof(char) * strlen(strings[index]) + 1, 
                             index, oram, NULL);
	}

	for (index = 0; index < nBlocks; index++)
	{
		//printf("Going to read %d\n",index); 
		result = oram->read(&data, index, oram, NULL);
		//printf("read string %s with result %d\n", (char*) data, result);

		if (result != strlen(data) + 1 || strcmp(data, strings[index]) != 0)
		{
			oram->close(oram, NULL);
			free(data);
			return 1;
		}

		free(strings[index]);
		free(data);
	}

	oram->close(oram, NULL);
	free(strings);
	
    return 0;
}


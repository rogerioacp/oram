#include "oram/oram.h"
#include "oram/orandom.h"

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
	ORAMState	state;

	stash = stashCreate();
	pmap = pmapCreate();
	ofile = ofileCreate();

	Amgr		amgr;

	amgr.am_stash = stash;
	amgr.am_pmap = pmap;
	amgr.am_ofile = ofile;

	size_t		fileSize = 100;

	//file with 100 bytes;
	size_t		blockSize = 20;

	//block size of 20 bytes;
	size_t		bucketCapcity = 1;

	//1 bucket per tree node;
	int			result = 0;
	size_t		nblocks = fileSize / blockSize;
	int			string_size = 0;
	int			index = 0;
	char	   *data = NULL;

	/* printf("Going to init\n"); */
	char	  **strings = (char **) malloc(sizeof(char *) * nblocks);

	state = init_oram("teste", fileSize, blockSize, bucketCapcity, &amgr, NULL);
	/* printf("Going to write strings\n"); */

	for (index = 0; index < nblocks; index++)
	{
		string_size = blockSize / sizeof(char) - 1;
		string_size = string_size == 0 ? 1 : string_size;
		string_size += 1;
		strings[index] = gen_random(string_size);

		/*
		 * printf("Going to write on offset %d the string %s\n",index,
		 * strings[index]);
		 */
		result = write_oram(strings[index], sizeof(char) * strlen(strings[index]) + 1, index, state, NULL);
	}

	for (index = 0; index < nblocks; index++)
	{
		/* printf("Going to read %d\n",index); */
		result = read_oram(&data, index, state, NULL);
		/* printf("read string %s with result %d\n", (char*) data, result); */

		if (result != strlen(data) + 1 || strcmp(data, strings[index]) != 0)
		{
			close_oram(state, NULL);
			free(data);
			return 1;
		}
		free(strings[index]);
		free(data);
	}
	close_oram(state, NULL);
	free(strings);
	return 0;
}


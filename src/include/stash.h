/*-------------------------------------------------------------------------
 *
 * stash.h
 *	  prototypes for any stash implementation in backend/stash
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef STASH_H
#define STASH_H

#include "block.h"


typedef void (*stashinit_function) (const char *filename, const size_t blockSize);
typedef void (*stashget_function) (PLBlock block, const BlockNumber pl_blkno, const char *fileName);
typedef void (*stashadd_function) (const char *fileName, const PLBlock block);
typedef void (*stashremove_function) (const char *filename, const PLBlock block);

typedef void (*stashstartIt_function) (const char *filename);
typedef size_t(*stashnext_function) (const char *filename, PLBlock *block);

typedef void (*stashcloseIt_function) (const char *filename);

/* Access manager to stash */
typedef struct AMStash
{
	/* Stash access functions */
	stashinit_function stashinit;
	stashget_function stashget;
	stashadd_function stashadd;
	stashremove_function stashremove;

	/* Stash iterator functions */
	stashstartIt_function stashstartIt;
	stashnext_function stashnext;
	stashcloseIt_function stashcloseIt;
}			AMStash;


AMStash    *stashCreate();

#endif							/* STASH_H */

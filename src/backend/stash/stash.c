/*-------------------------------------------------------------------------
 *
 * stash.c
 *      In-memory implementation of a stash.
 *
 * Implementation of a in-memory stash that keeps tracks of the blocks read
 * from an oblivious file and the blocks that have to be updated. This
 * implementation depends on the GSList of the glib and assumes that
 * only a single file is being accessed obliviously and ignores the filename.
 *
 * Copyright (c) 2018-2019, HASLab
 *
 * IDENTIFICATION
 *        backend/stash/stash.c
 *
 *-------------------------------------------------------------------------
 */

#include <collectc/list.h>
#include <stdio.h>
#include <string.h>
#include "oram/stash.h"
#include "oram/logger.h"


struct Stash
{
	List	   *list;
	ListIter	iterator;
};


/* non-export function prototypes */
static Stash stashInit(const char *filename, const unsigned int stashSize, const unsigned int blockSize, void *appData);

static void stashAdd(Stash stash, const char *filename, const PLBlock block, void *appData);

static int stashUpdate(Stash stash, const char *filename, const PLBlock block, void *appData);

static void stashGet(Stash stash, PLBlock block, BlockNumber pl_blkno, const char *filename, void *appData);

static void stashRemove(Stash stash, const char *filename, const PLBlock block, void *appData);

int			stashTake(Stash stash, const char *filename, unsigned int blkno, void *appData);


static void stashClose(Stash stash, const char *filename, void *appData);

static void stashStartIt(Stash stash, const char *filename, void *appData);

static unsigned int stashNext(Stash stash, const char *filename, PLBlock *block, void *appData);

static void stashCloseIt(Stash stash, const char *filename, void *appData);

AMStash *
stashCreate(void)
{
	AMStash    *stash = (AMStash *) malloc(sizeof(AMStash));

	stash->stashinit = &stashInit;
	stash->stashget = &stashGet;
	stash->stashadd = &stashAdd;
	stash->stashupdate = &stashUpdate;
	stash->stashremove = &stashRemove;
	stash->stashtake = &stashTake;
	stash->stashclose = &stashClose;

	stash->stashstartIt = &stashStartIt;
	stash->stashnext = &stashNext;
	stash->stashcloseIt = &stashCloseIt;

	return stash;
}

Stash
stashInit(const char *filename, const unsigned int stashSize, const unsigned int blockSize, void *appData)
{
	Stash		stash = (Stash) malloc(sizeof(struct Stash));

	list_new(&(stash->list));
	return stash;
}

void
stashGet(Stash stash, PLBlock block, BlockNumber pl_blkno, const char *filename, void *appData)
{
	ListIter	iter;
	PLBlock		aux = NULL;
	void	   *element;

	/* logger(DEBUG, "Going to get from stash block number %d", pl_blkno); */
	list_iter_init(&iter, stash->list);
	while (list_iter_next(&iter, &element) != CC_ITER_END)
	{
		aux = (PLBlock) element;
		if ((unsigned int) aux->blkno == pl_blkno)
		{
			block->blkno = aux->blkno;
			block->size = aux->size;
			block->block = malloc(aux->size);
			memcpy(block->block, aux->block, aux->size);
			break;
		}
	}
}

void
stashAdd(Stash stash, const char *filename, const PLBlock block, void *appData)
{
	/* logger(DEBUG, "Going to add to stash block number %d", block->blkno); */
	list_add(stash->list, block);
}

int
stashUpdate(Stash stash, const char *filename, const PLBlock block, void *appData)
{

	/*
	 * logger(DEBUG, "Going to update stash with block number %d",
	 * block->blkno);
	 */

	ListIter	iter;
	PLBlock		aux = NULL;
	void	   *element;
	int			found = 0;

	list_iter_init(&iter, stash->list);

	while (list_iter_next(&iter, &element) != CC_ITER_END)
	{
		aux = (PLBlock) element;

		if ((unsigned int) aux->blkno == block->blkno)
		{
			found = 1;
			free(aux->block);
			aux->block = block->block;
			aux->size = block->size;
			free(block);
			break;
		}
	}

	if (!found)
	{
		list_add(stash->list, block);
	}
    return found;
}

void
stashRemove(Stash stash, const char *filename, const PLBlock block, void *appData)
{
	ListIter	iter;
	PLBlock		aux = NULL;
	void	   *element;

	/* logger(DEBUG, "Going to remove stash  block number %d", block->blkno); */

	list_iter_init(&iter, stash->list);

	while (list_iter_next(&iter, &element) != CC_ITER_END)
	{
		aux = (PLBlock) element;

		if ((unsigned int) aux->blkno == block->blkno)
		{
			break;
		}
	}

	list_remove(stash->list, aux, NULL);
}

int
stashTake(Stash stash, const char *filename, unsigned int blkno, void *appData)
{
	void	   *element;
	ListIter	iter;
	PLBlock		aux = NULL;

	list_iter_init(&iter, stash->list);
	int			found = 0;

	while (list_iter_next(&iter, &element) != CC_ITER_END)
	{
		aux = (PLBlock) element;

		if ((unsigned int) aux->blkno == blkno)
		{
			found = 1;
			break;
		}
	}

	if (found)
	{
		list_remove(stash->list, aux, NULL);
		free(aux->block);
		free(aux);
		/* free((*block)->block); */
		/* free(*block); */
		/* *block = aux; */
	}

	return found;
}


void
destroyNotifyPLBlock(void *data)
{
	freeBlock((PLBlock) data);
}

void
stashClose(Stash stash, const char *filename, void *appData)
{
	list_remove_all_cb(stash->list, &destroyNotifyPLBlock);
	list_destroy(stash->list);
	/* list = NULL; */
	free(stash);
}


void
stashStartIt(Stash stash, const char *filename, void *appData)
{
	list_iter_init(&(stash->iterator), stash->list);
}

unsigned int
stashNext(Stash stash, const char *filename, PLBlock *block, void *appData)
{

	void	   *element;

	if (list_iter_next(&stash->iterator, &element) == CC_ITER_END)
	{
		return 0;
	}

	*block = (PLBlock) element;
	return 1;
}

void
stashCloseIt(Stash stash, const char *filename, void *appData)
{
	/* iterator = NULL; */
}

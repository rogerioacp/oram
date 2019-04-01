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

#include "oram/plblock.h"


typedef void (*stashinit_function)(const char *filename, const unsigned int  blockSize);

typedef void (*stashget_function)(PLBlock block, const BlockNumber pl_blkno, const char *fileName);

typedef void (*stashadd_function)(const char *fileName, const PLBlock block);

typedef void (*stashupdate_function)(const char *fileName, const PLBlock block);


typedef void (*stashremove_function)(const char *filename, const PLBlock block);

typedef void (*stashclose_function)(const char *filename);

typedef void (*stashstartIt_function)(const char *filename);

typedef unsigned int (*stashnext_function)(const char *filename, PLBlock *block);

typedef void (*stashcloseIt_function)(const char *filename);


/* Access manager to stash */
typedef struct AMStash {
    /* Stash access functions */
    stashinit_function stashinit;
    stashget_function stashget;
    stashadd_function stashadd;
    stashupdate_function stashupdate;
    stashremove_function stashremove;
    stashclose_function stashclose;

    /* Stash iterator functions */
    stashstartIt_function stashstartIt;
    stashnext_function stashnext;
    stashcloseIt_function stashcloseIt;
} AMStash;


AMStash *stashCreate(void);

#endif                            /* STASH_H */

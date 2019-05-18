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

typedef struct Stash *Stash;


typedef Stash (*stashinit_function)(const char *filename, const unsigned int  blockSize);

typedef void (*stashget_function)(Stash stash, PLBlock block, const BlockNumber pl_blkno, const char *fileName);

typedef void (*stashadd_function)(Stash stash, const char *fileName, const PLBlock block);

typedef void (*stashupdate_function)(Stash stash, const char *fileName, const PLBlock block);


typedef void (*stashremove_function)(Stash stash, const char *filename, const PLBlock block);

typedef void (*stashclose_function)(Stash stash, const char *filename);

typedef void (*stashstartIt_function)(Stash stash, const char *filename);

typedef unsigned int (*stashnext_function)(Stash stash, const char *filename, PLBlock *block);

typedef void (*stashcloseIt_function)(Stash stash,const char *filename);


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

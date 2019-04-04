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

static List *list;

static ListIter iterator;

/* non-export function prototypes */
static void stashInit(const char *filename, const unsigned int blockSize);

static void stashAdd(const char *filename, const PLBlock block);

static void stashUpdate(const char *filename, const PLBlock block);

static void stashGet(PLBlock block, BlockNumber pl_blkno, const char *filename);

static void stashRemove(const char *filename, const PLBlock block);

static void stashClose(const char *filename);

static void stashStartIt(const char *filename);

static unsigned int stashNext(const char *filename, PLBlock *block);

static void stashCloseIt(const char *filename);

AMStash *stashCreate(void) {
    AMStash *stash = (AMStash *) malloc(sizeof(AMStash));
    stash->stashinit = &stashInit;
    stash->stashget = &stashGet;
    stash->stashadd = &stashAdd;
    stash->stashupdate = &stashUpdate;
    stash->stashremove = &stashRemove;
    stash->stashclose = &stashClose;

    stash->stashstartIt = &stashStartIt;
    stash->stashnext = &stashNext;
    stash->stashcloseIt = &stashCloseIt;

    return stash;
}

void stashInit(const char *filename, const unsigned int blockSize) {
    list_new(&list);
}

void stashGet(PLBlock block, BlockNumber pl_blkno, const char *filename) {
    ListIter iter;
    PLBlock aux = NULL;
    void *element;

    list_iter_init(&iter, list);
    while(list_iter_next(&iter, &element) != CC_ITER_END){
        aux = (PLBlock) element;
        if ((unsigned int) aux->blkno == pl_blkno) {
            block->blkno = aux->blkno;
            block->size = aux->size;
            block->block = malloc(aux->size);
            memcpy(block->block, aux->block, aux->size);
            break;
        }
    }
}

void stashAdd(const char *filename, const PLBlock block) {
    list_add(list,block);
}


void stashUpdate(const char *filename, const PLBlock block) {

    ListIter iter;
    PLBlock aux = NULL;
    void *element;
    int found = 0;

    list_iter_init(&iter, list);

    while (list_iter_next(&iter, &element) != CC_ITER_END) {
        aux = (PLBlock) element;

        if ((unsigned int) aux->blkno == block->blkno) {
            found = 1;
            free(aux->block);
            aux->block = block->block;
            aux->size = block->size;
            free(block);
            break;
        }
    }

    if (!found) {
        list_add(list, block);
    }
}

void stashRemove(const char *filename, const PLBlock block) {
    ListIter iter;
    PLBlock aux = NULL;
    void *element;
    void *toRemove;

    list_iter_init(&iter, list);

    while (list_iter_next(&iter, &element) != CC_ITER_END) {
        aux = (PLBlock) element;

        if ((unsigned int) aux->blkno == block->blkno) {
            break;
        }
    }

    list_remove(list, aux, &toRemove);
}


void destroyNotifyPLBlock(void* data) {
    freeBlock((PLBlock) data);
}

void stashClose(const char *filename) {
    list_remove_all_cb(list, &destroyNotifyPLBlock);
    list_destroy(list);
    list = NULL;
}


void stashStartIt(const char *filename) {
    list_iter_init(&iterator, list);
}

unsigned int stashNext(const char *filename, PLBlock *block) {

    void* element;
    if (list_iter_next(&iterator, &element) == CC_ITER_END) {
        return 0;
    }

    *block = (PLBlock) element;
    return 1;
}

void stashCloseIt(const char *filename) {
    //iterator = NULL;
}
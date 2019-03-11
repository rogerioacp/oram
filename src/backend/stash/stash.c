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

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include "stash.h"

GSList *list;

GSList *iterator;

/* non-export function prototypes */
static void stashInit(const char *filename, const size_t blockSize);

static void stashAdd(const char *filename, const PLBlock block);

static void stashUpdate(const char *filename, const PLBlock block);

static void stashGet(PLBlock block, BlockNumber pl_blkno, const char *filename);

static void stashRemove(const char *filename, const PLBlock block);

static void stashClose(const char *filename);

static void stashStartIt(const char *filename);

static size_t stashNext(const char *filename, PLBlock *block);

static void stashCloseIt(const char *filename);

AMStash *stashCreate() {
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

void stashInit(const char *filename, const size_t blockSize) {
    list = NULL;
}

void stashGet(PLBlock block, BlockNumber pl_blkno, const char *filename) {
    GSList *head = list;
    PLBlock aux = NULL;
    while (head != NULL) {
        aux = (PLBlock) head->data;

        if ((size_t) aux->blkno == pl_blkno) {
            block->blkno = aux->blkno;
            block->size = aux->size;
            block->block = malloc(aux->size);
            memcpy(block->block, aux->block, aux->size);
            break;
        }
        head = g_slist_next(head);
    }
}

void stashAdd(const char *filename, const PLBlock block) {
    list = g_slist_append(list, block);
}


void stashUpdate(const char *filename, const PLBlock block) {
    GSList *head = list;
    PLBlock aux = NULL;
    int found = 0;

    while (head != NULL) {
        aux = (PLBlock) head->data;

        if ((size_t) aux->blkno == block->blkno) {
            found = 1;
            free(aux->block);
            aux->block = block->block;
            aux->size = block->size;
            free(block);
            break;
        }
        head = g_slist_next(head);
    }


    if (!found) {
        list = g_slist_append(list, block);
    }
}

void stashRemove(const char *filename, const PLBlock block) {
    GSList *head = list;
    PLBlock aux = NULL;

    while (head != NULL) {
        aux = (PLBlock) head->data;

        if ((size_t) aux->blkno == block->blkno) {
            break;
        }
        head = g_slist_next(head);
    }
    list = g_slist_remove_link(list, head);
    g_slist_free_1(head);
}


void destroyNotifyPLBlock(gpointer data) {
    freeBlock((PLBlock) data);
}

void stashClose(const char *filename) {
    g_slist_free_full(list, &destroyNotifyPLBlock);
    list = NULL;
}


void stashStartIt(const char *filename) {
    iterator = list;
}

size_t stashNext(const char *filename, PLBlock *block) {
    if (iterator == NULL) {
        return 0;
    }
    *block = (PLBlock) iterator->data;
    iterator = iterator->next;
    return 1;
}

void stashCloseIt(const char *filename) {
    iterator = NULL;
}
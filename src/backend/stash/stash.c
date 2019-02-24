#include <glib.h>
#include <stdio.h>
#include "stash.h"

GSList      *list;

GSList      *iterator;

/* non-export function prototypes */
static void stashInit(const char *filename, const size_t blockSize);
static void stashAdd(const char *filename, const PLBlock block);
static void stashGet(PLBlock block, BlockNumber pl_blkno, const char *filename);
static void stashRemove(const char* filename, const PLBlock block);
static void stashStartIt(const char* filename);
static size_t stashNext(const char* filename, PLBlock *block);
static void stashClose(const char* filename);

AMStash* stashCreate(){
	AMStash* stash = (AMStash*) malloc(sizeof(AMStash));
	stash->stashinit = &stashInit;
	stash->stashget = &stashGet;
	stash->stashadd = &stashAdd;
	stash->stashremove = &stashRemove;

	stash->stashstartIt = &stashStartIt;
	stash->stashnext = &stashNext;
	stash->stashcloseIt = &stashClose;

	return stash;
}

void stashInit(const char *filename, const size_t blockSize){
	list = NULL;
}

void stashAdd(const char *filename, const PLBlock block){
	list = g_slist_append(list, block);
}

void stashGet(PLBlock block, BlockNumber pl_blkno, const char *filename){
	GSList* head =  list;
	PLBlock aux = NULL;
	while( head != NULL){
		aux = (PLBlock) head->data;

		if((size_t) aux->blkno == pl_blkno){
			block->blkno = aux->blkno;
			block->size = aux->size;
			block->block = malloc(aux->size);
			memcpy(block->block, aux->block, aux->size);
			break;
		}
		head = g_slist_next(head);
	}
}

void stashRemove(const char* filename, const PLBlock block){
	GSList* head =  list;
	PLBlock aux = NULL;

	while( head != NULL){
		aux = (PLBlock) head->data;

		if((size_t) aux->blkno == block->blkno){
			break;
		}
		head = g_slist_next(head);
	}
	list = g_slist_remove_link(list, head);
}


void stashStartIt(const char* filename){
	iterator = list;
}

size_t stashNext(const char* filename, PLBlock *block){
	if(iterator == NULL){
		return 0;
	}
	*block = (PLBlock) iterator->data;
	iterator = iterator->next;
	return 1;
}

void stashClose(const char* filename){
	iterator = NULL;

}
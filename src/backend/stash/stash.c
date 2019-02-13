#include <glib.h>
#include "stash.h"

GSList      *list;

GSList      *iterator;

void stashInit(const char *filename, const size_t blockSize){
	list = NULL;
}

void stasAdd(const char *filename, const PLBlock block){
	g_slist_append(list, block);
}

void stasGet(PLBlock* block, BlockNumber pl_blkno, const char *filename){
	GSList head =  list;
	PLBlock aux = NULL;

	while( head != NULL){
		aux = (PLBlock) head->data;

		if((size_t) aux->blkno == pl_blkno){
			*block = aux;
			break;
		}
		head = g_slist_next(head);
	}
}

void stashRemove(const char* filename, const PLBlock block){
	GSList head =  list;
	PLBlock aux = NULL;

	while( head != NULL){
		aux = (PLBlock) head->data;

		if((size_t) aux->blkno == pl_blkno){
			break;
		}
		head = g_slist_next(head);
	}
	g_slist_remove(slist, aux);
}


void stashStartIt(const char* filename){
	iterator = list;
}

void stashNext(const char* filename, PLBlock * block){
	*block = g_list_next(iterator);
	iterator = *block;
}

void stashClose(const char* filename){
	*block = NULL;

}
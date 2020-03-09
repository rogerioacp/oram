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

#include <errno.h>
#include <collectc/list.h>
#include <stdio.h>
#include <string.h>
#include "oram/stash.h"
#include "oram/logger.h"


struct Stash
{
	unsigned int it;
    unsigned int size;

    PLBlock* blocks;
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
	
    int i, save_errno = 0;
    Stash stash;
    
    save_errno = errno;
    errno = 0;
    
    stash = (Stash) malloc(sizeof(struct Stash));

    if(stash == NULL && errno == ENOMEM){
        logger(OUT_OF_MEMORY, "Out of memory building stash state");
        errno = save_errno;
        abort();
    }
    //logger(DEBUG, "Initializing stash with size %d\n", stashSize);
    stash->blocks = (PLBlock*) malloc(sizeof(PLBlock)*stashSize);
    
    if(stash->blocks == NULL  && errno == ENOMEM){
        logger(OUT_OF_MEMORY, "Out of memory allocating stash array");
        errno = save_errno;
        abort();
    }
    for(i = 0; i < stashSize; i++){
       

        stash->blocks[i] = (PLBlock) malloc(sizeof(struct PLBlock));

        if(stash->blocks[i] == NULL && errno == ENOMEM){
            logger(OUT_OF_MEMORY, "Out of memory allocating stash array");
            errno = save_errno;
            abort();
        }
        memset(stash->blocks[i], 0, sizeof(struct PLBlock));
        stash->blocks[i]->blkno = DUMMY_BLOCK;
    }

    
    errno = save_errno;
    
    stash->size = stashSize; 

	return stash;
}

void stashPrint(Stash stash){

    int offset;
    PLBlock aux;
    int real = 0; 
    for(offset = 0; offset < stash->size; offset++){
        aux = stash->blocks[offset];

        if(aux->blkno == DUMMY_BLOCK){
            logger(DEBUG, "Stash block at offset %d is dummy\n", offset);
        }else{
            real +=1;
            logger(DEBUG, "Stash block at offset %d has blkno %d and data %s\n", offset, aux->blkno, aux->block);
        }
    }
    logger(DEBUG, "Total number of real blocks in stash is %d\n", real);
}

void
stashGet(Stash stash, PLBlock block, BlockNumber pl_blkno, const char *filename, void *appData)
{
	PLBlock		aux;
	int         offset;


    for(offset = 0; offset < stash->size; offset++){
        aux = stash->blocks[offset];

        if((unsigned int) aux->blkno == pl_blkno){
            block->blkno = aux->blkno;
			block->size = aux->size;
            //block->lsize = aux->lsize;
			block->block = malloc(aux->size);
			memcpy(block->block, aux->block, aux->size);

        }
    }

}

void
stashAdd(Stash stash, const char *filename, const PLBlock block, void *appData)
{

	PLBlock		aux;
	int         offset, inserted;
    inserted = 0;
    for(offset = 0; offset < stash->size; offset++)
    {
        aux = stash->blocks[offset];
        if(!inserted && (unsigned int) aux->blkno == DUMMY_BLOCK){
            memcpy(aux, block, sizeof(struct PLBlock));
            free(block);
            inserted = 1;
            //break;
        }
    }

}

int
stashUpdate(Stash stash, const char *filename, const PLBlock block, void *appData)
{	 

    int     offset, target;
    int     found = 0;
    PLBlock aux;
    

    target = -1;
    
    for(offset = 0; offset < stash->size; offset++){
        
        aux = stash->blocks[offset];
        if(!found && (unsigned int) aux->blkno == block->blkno){
            free(aux->block);
            //free(aux->location);
            target = offset;
            found =1;
            //break;
        }else if(!found && aux->blkno == DUMMY_BLOCK){
            target = offset;
        }
    }

    aux = stash->blocks[target];
    
    if(target == -1){
        logger(DEBUG, "No available space to write or update out of %d", stash->size);
        exit(-1);
    }

    memcpy(aux, block, sizeof(struct PLBlock));
    free(block);
    return found;

}

void
stashRemove(Stash stash, const char *filename, const PLBlock block, void *appData)
{

	PLBlock	aux;
	int     offset;

    for(offset = 0; offset < stash->size; offset++){
        
        aux = stash->blocks[offset];
        
        if((unsigned int) aux->blkno == block->blkno)
        {
           stash->blocks[offset] = (PLBlock) malloc(sizeof(struct PLBlock));
           memset(stash->blocks[offset], 0, sizeof(struct PLBlock));
           stash->blocks[offset]->blkno = DUMMY_BLOCK;
           //break;
        }

    }
}

int
stashTake(Stash stash, const char *filename, unsigned int blkno, void *appData)
{

	PLBlock		aux;
	int         offset, found = 0;

    for(offset = 0; offset < stash->size; offset++){
        
        aux = stash->blocks[offset];
        
        if((unsigned int) aux->blkno == blkno)
        {
            free(aux->block);
            //free(aux->location);
            memset(aux, 0 , sizeof(struct PLBlock));
            aux->blkno = DUMMY_BLOCK;
            found = 1;
            //break;
        }

    }
    return found;
}

void    
stashClose(Stash stash, const char *filename, void *appData)
{


	struct PLBlock		aux;
    int offset;

    for(offset=0; offset < stash->size; offset++){
        //free(stash->blocks[offset]->location);
        free(stash->blocks[offset]->block);
        free(stash->blocks[offset]);
    }

    free(stash->blocks);
    free(stash);
}


void
stashStartIt(Stash stash, const char *filename, void *appData)
{
    stash->it = 0;
}

unsigned int
stashNext(Stash stash, const char *filename, PLBlock *block, void *appData)
{
    int offset;
    int onlyDummys = 1;
    PLBlock aux;
     
    for(offset=stash->it; offset < stash->size; offset++){
        aux = stash->blocks[offset];
        if(aux->blkno != DUMMY_BLOCK){
            onlyDummys = false;
            break;
        }
    }

    stash->it = offset +1;
    if(onlyDummys){
        return 0;
    }else{
        *block = stash->blocks[offset];
        return 1;
    }    
}

void
stashCloseIt(Stash stash, const char *filename, void *appData)
{
    stash->it = stash->size;
}

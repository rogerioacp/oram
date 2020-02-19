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
    logger(DEBUG, "Initializing stash with size %d\n", stashSize);
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
    //int         condition;
    //void	   *element;

    //block->block = malloc(aux->size);
    //char* data = malloc(stash->blockSize);
    //memset (data, 0, stash->blockSize);
    //logger(DEBUG, "StashGet block %d\n", pl_blkno);

    for(offset = 0; offset < stash->size; offset++){
        aux = stash->blocks[offset];
        //condition = (aux->blkno) == pl_blkno;
       // block->blkno = (condition & aux->blkno) | (condition & block->blkno);
       // block->size = (condition & aux->size) | (condition & block->size);
        //memset(data, condition, stash->blockSize);
        //memcpy(block->block,    

        if((unsigned int) aux->blkno == pl_blkno){
            //logger(DEBUG, "Found block in stash offset %d\n", aux->blkno);
            block->blkno = aux->blkno;
			block->size = aux->size;
			block->block = malloc(aux->size);
			memcpy(block->block, aux->block, aux->size);

        }else{
            //logger(DEBUG, "stash offset %d has block %d\n",offset, aux->blkno);
        }
    }

}

void
stashAdd(Stash stash, const char *filename, const PLBlock block, void *appData)
{
	//logger(DEBUG, "Going to add to stash block number %d\n", block->blkno);

	PLBlock		aux;
	int                 offset;

    for(offset = 0; offset < stash->size; offset++)
    {
        aux = stash->blocks[offset];
        if((unsigned int) aux->blkno == DUMMY_BLOCK){
            memcpy(aux, block, sizeof(struct PLBlock));
            free(block);
            //logger(DEBUG, "Going to insert block %d in stash offset %d and string %s \n", aux->blkno, offset, aux->block);
            break;
        }
    }

}

int
stashUpdate(Stash stash, const char *filename, const PLBlock block, void *appData)
{

	
   // logger(DEBUG, "Going to update stash with block number %d and string %s\n", block->blkno, block->block);
	 

    int     offset, target;
    int     found = 0;
    PLBlock aux;
    

    target = -1;
    
    for(offset = 0; offset < stash->size; offset++){
        
        aux = stash->blocks[offset];
        //logger(DEBUG, "looping stash update %d\n", offset);
        if((unsigned int) aux->blkno == block->blkno){
            //logger(DEBUG, "Found existing matching block at offset %d\n", offset);
            free(aux->block);
            target = offset;
            found =1;
            break;
        }else if(aux->blkno == DUMMY_BLOCK){
            //logger(DEBUG, "Selected dummy block  at offset %d\n", offset);
            target = offset;
        }
    }

    aux = stash->blocks[target];
    if(target == -1){
        logger(DEBUG, "No available space to write or update out of %d", stash->size);
        exit(-1);
    }
    //assert(target != NULL);
    memcpy(aux, block, sizeof(struct PLBlock));
    free(block);
    //logger(DEBUG, "updated stash offset %d offset with block blkno %d and string %s\n",  target , aux->blkno, aux->block);
    return found;

}

void
stashRemove(Stash stash, const char *filename, const PLBlock block, void *appData)
{

	PLBlock		aux;
	int                 offset;


    //logger(DEBUG, "Removing block %d from stash\n", block->blkno);
    //free(block->block);
   // memset(block->block, 0, block->size);
   // block->size = 0;
   // block->blkno = DUMMY_BLOCK;

    for(offset = 0; offset < stash->size; offset++){
        
        aux = stash->blocks[offset];
        
        if((unsigned int) aux->blkno == block->blkno)
        {
           //logger(DEBUG, "Removing block %d from stash at offset %d\n", aux->blkno, offset);
           stash->blocks[offset] = (PLBlock) malloc(sizeof(struct PLBlock));
           memset(stash->blocks[offset], 0, sizeof(struct PLBlock));
           stash->blocks[offset]->blkno = DUMMY_BLOCK;
           //free(aux->block);
           //memset(aux, 0, sizeof(struct PLBlock)); 
           //aux->blkno = DUMMY_BLOCK;
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
        free(stash->blocks[offset]->block);
        free(stash->blocks[offset]);
       //stash->blocks[offset]->blkno = DUMMY_BLOCK;
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
        //logger(DEBUG, "Iterating over stash offset %d\n", offset);
        if(aux->blkno != DUMMY_BLOCK){
            //logger(DEBUG, "Found real block during iteration at offset %d\n", offset);
            onlyDummys = false;
            break;
        }
    }

    stash->it = offset +1;
    if(onlyDummys){
        //logger(DEBUG, "Only dummies %d offset %d\n", onlyDummys, offset);
        return 0;
    }else{
        //logger(DEBUG, " found real %d offset %d with data %s\n", onlyDummys, offset, aux->block);
        *block = stash->blocks[offset];//createBlock(aux->blkno, aux->size, aux->block);
        //stash->it = offset + 1;
	    return 1;

    }    
}

void
stashCloseIt(Stash stash, const char *filename, void *appData)
{
    stash->it = stash->size;
}

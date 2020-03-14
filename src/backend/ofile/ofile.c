/*-------------------------------------------------------------------------
 *
 * ofile.c
 *        Simulation of a file in memory.
 *
 *
 * This code should only be used for tests. The idea of this file is to
 * provide an example and have an in-memory implementation of the methods
 * the ORAM requires to write blocks to a file. This code is used for testing
 * the implementation without having to actually write to a file.
 * 
 * Copyright (c) 2018-2019, HASLab
 *
 * IDENTIFICATION
 *        backend/ofile/ofile.c
 *
 *-------------------------------------------------------------------------
 */

#include "oram/ofile.h"
#include "oram/logger.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

struct FileHandler{
    PLBList file;
    unsigned int nblocks;
};

static FileHandler fileInit(const char *filename, unsigned int nblocks, 
                            unsigned int blocksize, unsigned int locationSize,
                            void* appData);

static void fileRead(FileHandler fhandler, PLBlock block, 
                     const char *fileName, const BlockNumber ob_blkno, 
                     void* appData);

static void fileWrite(FileHandler fhandler, const PLBlock block, 
                      const char *fileName, const BlockNumber ob_blkno,
                      void* appData);

static void fileClose(FileHandler fhandler, const char *filename, 
                      void* appData);


FileHandler fileInit(const char *filename, unsigned int nblocks, 
                     unsigned int blocksize, unsigned int locationSize,
                     void* appData) {

    FileHandler handler;
    int offset;
    unsigned int save_errno = 0;

    save_errno = errno;
    errno = 0;

    handler = (FileHandler) malloc(sizeof(struct FileHandler));

    if(handler == NULL && errno == ENOMEM)
    {
        logger(OUT_OF_MEMORY, "Out of memory initializing memory file handler\n");
        errno = save_errno;
        abort();
    }

    

    handler->file = (PLBList) malloc(sizeof(PLBlock) * nblocks);
    
    if(handler->file  == NULL && errno == ENOMEM){

        logger(OUT_OF_MEMORY, "Out of memory initializing memory file\n");
        errno = save_errno;
        abort();
    }

    errno = save_errno;

    handler->nblocks = nblocks;

    for (offset = 0; offset < nblocks; offset++) {
        save_errno = errno;
        errno = 0;

        handler->file[offset] = (PLBlock) malloc(sizeof(struct PLBlock));

        if(handler->file[offset] == NULL && errno == ENOMEM)
        {
            logger(OUT_OF_MEMORY, "Out of memory initializing file block\n");
		    errno = save_errno;
		    abort();
        }

        errno = save_errno;
        
        handler->file[offset]->blkno = -1;
        handler->file[offset]->size = blocksize;

        /*TODO: Add verification code for available size*/
        handler->file[offset]->block = (void *) malloc(blocksize);
                
        memset(handler->file[offset]->block, 0, blocksize);
        handler->file[offset]->location[0] = 0;   
        handler->file[offset]->location[1] = 0;    
    }

    return handler;

}

void
fileRead(FileHandler handler, PLBlock block, const char *fileName,
         const BlockNumber ob_blkno, void* appData) {

    PLBlock cblock = handler->file[ob_blkno];

    block->blkno = cblock->blkno;
    block->size = cblock->size;
    block->location[0] = cblock->location[0];
    block->location[1] = cblock->location[1];
    block->block = malloc(cblock->size);

    memcpy(block->block, cblock->block, cblock->size);
}

void
fileWrite(FileHandler handler, const PLBlock block, const char *fileName, 
          const BlockNumber ob_blkno, void* appData) {

    PLBlock cblock = handler->file[ob_blkno];

    cblock->blkno = block->blkno;
    cblock->size = block->size;
    cblock->location[0] = block->location[0];
    cblock->location[1] = block->location[1];

    memcpy(cblock->block, block->block, block->size);
}


void 
fileClose(FileHandler handler, const char * filename, void* appData){

    int i;

    for(i=0; i < handler->nblocks; i++){
        free(handler->file[i]->block);
        free(handler->file[i]);
    }
    free(handler->file);
    free(handler);
}

AMOFile *ofileCreate(void) {
    AMOFile *file = (AMOFile *) malloc(sizeof(AMOFile));
    file->ofileinit = &fileInit;
    file->ofileread = &fileRead;
    file->ofilewrite = &fileWrite;
    file->ofileclose = &fileClose;
    return file;
}


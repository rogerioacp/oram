/*-------------------------------------------------------------------------
 *
 * ofile.h
 *	  prototypes for any oblivious file implementation in backend/ofile
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef OFILE_H
#define OFILE_H

#include "oram/plblock.h"


typedef struct FileHandler* FileHandler;

typedef FileHandler (*ofileinit_function) (const char *fileName, 
                                           unsigned int totalNodes, 
                                           unsigned int blockSize,
                                           unsigned int locationSize,
                                           void *appData);

typedef void (*ofileread_function) (FileHandler handler, 
                                    PLBlock block, 
                                    const char *fileName, 
                                    const BlockNumber ob_blkno, void *appData);

typedef void (*ofilewrite_function) (FileHandler handler,
                                     const PLBlock block, 
                                     const char *fileName, 
                                     const BlockNumber ob_blkno, void *appData);

typedef void (*ofileclose_function) (FileHandler, 
                                     const char *fileName, void *appData);

typedef struct AMOFile
{
	ofileinit_function ofileinit;
	ofileread_function ofileread;
	ofilewrite_function ofilewrite;
	ofileclose_function ofileclose;
} AMOFile;

AMOFile    *ofileCreate(void);

#endif							/* OFILE_H*/

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

#include "block.h"

typedef int (*ofileinit_function) (const char *fileName, size_t totalNodes, size_t blockSize);
typedef void (*ofileread_function) (PLBlock* block, const char *fileName, const BlockNumber ob_blkno);
typedef size_t (*ofilewrite_function) (const PLBlock block, const char *fileName, const BlockNumber ob_blkno);

//Access manager to oblivious file.
typedef struct AMOFile{
	ofileinit_function ofileinit;
	ofileread_function ofileread;
	ofilewrite_function ofilewrite;
}AMOFile;


#endif  /*OFILE_H*/
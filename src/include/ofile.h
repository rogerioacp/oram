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

typedef void (*ofileinit_function) (const char *fileName, size_t totalNodes, size_t blockSize);
typedef void (*ofileread_function) (PLBlock block, const char *fileName, const BlockNumber ob_blkno);
typedef void (*ofilewrite_function) (const PLBlock block, const char *fileName, const BlockNumber ob_blkno);

typedef struct AMOFile
{
	ofileinit_function	ofileinit;
	ofileread_function	ofileread;
	ofilewrite_function	ofilewrite;
}		AMOFile;

AMOFile* ofileCreate();

#endif  /*OFILE_H*/
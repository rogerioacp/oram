/*-------------------------------------------------------------------------
 *
 * pmapdefs/tpmap.h
 *
 * Interface definition for token position map. This is used to create
 * a cascading ORAM. The client application provides a cryptographic token
 * that is used to generate the random location to access. In case of a 
 * a Path ORAM the token is used to generate the current leaf of a block and
 * its next location. In case of a Forest ORAM its used to generate the leaf
 * and a partition.
 *
 * Copyright (c) 2018-2020, HASLab
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef TPMAP_H
#define TPMAP_H

//Current token size is 128 bits or 16 bytes (the size of an AES BLOCK)
#define TOKEN_SIZE 16


void pmapSetToken(PMap pmap, const unsigned char* token);

#endif							/* PMAP_DEFS_FORAM_H */

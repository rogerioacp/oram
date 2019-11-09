/*-------------------------------------------------------------------------
 *
 * pmapdefs/poram.h
 *	  strut definitions for path ORAM's position map location and tree
 * configuration.
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef PMAP_DEFS_PORAM_H
#define PMAP_DEFS_PORAM_H

struct Location
{
	unsigned int leaf;
};
struct TreeConfig
{
	unsigned int treeHeight;
};
#endif							/* PMAP_DEFS_PORAM_H */

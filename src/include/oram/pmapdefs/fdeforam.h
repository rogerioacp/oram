/*-------------------------------------------------------------------------
 *
 * pmapdefs/poram.h
 *	  strut definitions for forest ORAM's position map location and tree
 * configuration.
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef PMAP_DEFS_FORAM_H
#define PMAP_DEFS_FORAM_H

struct Location
{
	unsigned int partition;
	unsigned int leaf;
};

struct TreeConfig
{
	unsigned int treeHeight;
	unsigned int nPartitions;
};

#endif							/* PMAP_DEFS_FORAM_H */

/*-------------------------------------------------------------------------
 *
 * coram.h
 *
 * Interface definition for a cascading oram. This is used to set the 
 * cryptographic token for a block location.
 * The client application provides a cryptographic token
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

#ifndef CORAM_H
#define CORAM_H


void setToken(ORAMState state, const unsigned int* token);

#endif						

/*-------------------------------------------------------------------------
 *
 * linux_random.c
 *	   Implementation of random library for linux systems.
 * 
 * Copyright (c) 2018-2019, HASLab
 *
 * IDENTIFICATION
 *		  backend/orandom/linux_random.c
 *
 *-------------------------------------------------------------------------
 */

#include "oram/orandom.h"
#include <stdlib.h>
#include <time.h>

unsigned int getRandomInt(void) {
   return random();
}


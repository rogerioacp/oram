/*-------------------------------------------------------------------------
 *
 * bsd_random.c
 *	   Implementation of random library for bsd systems.
 * 
 * Copyright (c) 2018-2019, HASLab
 *
 * IDENTIFICATION
 *		  backend/orandom/bsd_random.c
 *
 *-------------------------------------------------------------------------
 */

#include "oram/orandom.h"
#include <stdlib.h>


unsigned int getRandomInt(void) {
    return arc4random();
}

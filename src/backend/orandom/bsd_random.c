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

#include "orandom.h"
#include <stdlib.h>


unsigned int getRandomInt() {
    return arc4random();
}

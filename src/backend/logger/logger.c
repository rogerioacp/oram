/*-------------------------------------------------------------------------
 *
 * logger.c
 *        Simple log implementation to the stdio.
 *
 *
 * Code to be used by tests and debugging to print error messages for the 
 * stdio. Not ready for production.
 * 
 * Copyright (c) 2018-2019, HASLab
 *
 * IDENTIFICATION
 *        backend/logger/logger.c
 *
 *-------------------------------------------------------------------------
 */

#include "logger.h"

#include <stdio.h>

void logger(int error_code) {
    printf("Error in ORAM with code %d", error_code);
}
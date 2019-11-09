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

#include "oram/logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 300

void logger(int level, const char* message, ...){


    char buf[BUFSIZE];
    memset(buf, 0, BUFSIZE);
    //int written = 0;
    //int result;
    va_list ap;
    va_start(ap, message);
    /*written = vsnprintf(buf, BUFSIZE, message, ap);
    if(written < BUFSIZE){
        buf = realloc(buf, sizeof(char)*written);
        memset(buf, 0, written);
    }*/

    vsnprintf(buf, BUFSIZE, message, ap);
    va_end(ap);
    printf("%d - %s", level, buf);
}


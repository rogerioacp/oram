/*-------------------------------------------------------------------------
 *
 * logger.h
 *	  log definitions for any logging system to be integrated
 *
 *
 * Copyright (c) 2018-2019, HASLab
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef LOGGER_H
#define LOGGER_H
#include <stdarg.h>

#define OUT_OF_MEMORY 1
#define DEBUG 2

#define LBUFSIZE 200

void		logger(int level, const char *message,...);


#endif							/* LOGGER_H */

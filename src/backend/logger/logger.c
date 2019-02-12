#include "logger.h"

#include <stdio.h>

void logger(int error_code){
	printf("Error in ORAM with code %d", error_code);
}
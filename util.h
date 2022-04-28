#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
#include <stdlib.h>
#include <string.h>

void split_str(char* str, const char* delim, char slices[20][20]) {
	int i = 0;
	char *ptr = strtok(str, delim);
	while(ptr != NULL) {
		strcpy(slices[i], ptr);
		ptr = strtok(NULL, delim);
		i++;
	}
}

#endif
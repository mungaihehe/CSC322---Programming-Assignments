#ifndef APP_H_INCLUDED
#define APP_H_INCLUDED
#include <stdlib.h>
#include <string.h>
#include "catalog.h"
#include "util.h"
#include "handler.h"

void app(struct Catalog* catalog) {
	while(1) {
		displayHandlerOptions();
		
		char argstr[100];
		scanf(" %[^\n]", argstr);

		char args[20][20];
		split_str(argstr, ",", args);
		
		int selection = atoi(args[0]);
		
		handler(catalog, selection, args);
	}
}


#endif
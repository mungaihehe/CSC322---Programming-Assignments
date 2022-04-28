#ifndef HANDLER_H_INCLUDED
#define HANDLER_H_INCLUDED
#include <stdlib.h>
#include <string.h>
#include "catalog.h"
#include "util.h"

void displayHandlerOptions() {
	printf("\n-- Online Shopping --\n");
	printf("1. Add Item - args(name, price, stock)\n");
	printf("2. Display Catalog - args()\n");
	printf("3. Search For Item - args(name)\n");
	printf("4. Purchase Item - args(name, payment_amount)\n");
	printf("5. Exit\n");
	printf("\n");
	printf("Select an Action: ");
}

void handler(struct Catalog* catalog, int selection, char args[20][20]) {
	switch(selection) {
		case 1: ; {
			// printf("Name: %s Price: %s Stock: %s\n", args[1], args[2], args[3]);
			struct Item *item = newItem(args[1], atoi(args[2]), atoi(args[3]));
			// displayItem(*item);
			addToCatalog(catalog, item);
			break;
		}
		case 2: ; {
			displayCatalog(*catalog);
			break;
		}
		case 3: ; {
			struct Item* item = searchCatalog(catalog, args[1]);
			displayItem(*item);
			break;
		}
		case 4: ; {
			int change = purchaseItem(catalog, args[1], atoi(args[2]));
			printf("Change: %d\n", change);
			break;
		}
		case 5: ; {
			return;
		}
		default:
			printf("Invalid selection.\n");
			break;
	}
}

#endif
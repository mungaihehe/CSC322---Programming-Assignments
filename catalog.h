#ifndef CATALOG_H_INCLUDED
#define CATALOG_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include "item.h"

struct Catalog {
	struct Item items[20];
	int count;
};

struct Catalog* newCatalog() {
	struct Catalog *catalog = (struct Catalog*) malloc(sizeof(struct Catalog));
	return catalog;
}

void displayCatalog(struct Catalog catalog) {
	int i;
	for(i = 0; i<catalog.count; i++) {
		struct Item item = catalog.items[i];
		printf("\n");
		printf("Catalog item:\t%d\n", i + 1);
		printf("Item Name:\t%s\n", item.name);
		printf("Item Price:\t%s\n", item.price);
		printf("Item Stock:\t%s\n", item.stock);
		printf("\n");
	}
}

struct Item* searchCatalog(struct Catalog catalog, const char* itemName) {
	//returns an item of 0 stock when no item is found
	int i;
	for(i = 0; i<catalog.count; i++) {
		struct Item *item = &catalog.items[i];
		if(strcmp(itemName, item->name) == 0) {
			return item;
		}
	}
	return newItem("", 0, 0);
}

int purchaseItem(struct Catalog *catalog, const char* itemName, int amount) {
	//returns change amount
	struct Item* item = searchCatalog(*catalog, itemName);
	if(item->stock == 0) {
		printf("Item's stock is 0.\n");
		return amount;
	}
	if(item->price > amount) {
		printf("Insufficient amount.\n");
		return amount;
	}
	item->stock--;
	return amount - item->price;
}


#endif
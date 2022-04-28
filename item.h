#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED
#include <stdlib.h>
#include <string.h>

struct Item {
	char name[20];
	int price;
	int stock;
};

struct Item* newItem(const char* name, int price, int stock) {
	struct Item *item = (struct Item*) malloc(sizeof(struct Item));

	strcpy(item->name, name);
	item->price = price;
	item->stock = stock;
	return item;
}

#endif
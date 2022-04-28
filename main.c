#include <stdio.h>
#include "app.h"
#include "catalog.h"

int main(int argc, char *argv[]) {
	struct Catalog* catalog = newCatalog();
	app(catalog);
	return 0;
}
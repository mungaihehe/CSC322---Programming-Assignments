#ifndef SLAVE_H_INCLUDED
#define SLAVE_H_INCLUDED
#include <stdlib.h>
#include <string.h>
#include "catalog.h"
#include "util.h"
#include "handler.h"
#include "cross_platform_sockets.h"
#include <string.h>

struct SlaveArgs
{
	struct Catalog *catalog;
	char request[1024];
	SOCKET socket_listen;
	struct sockaddr_storage client_address
};

int slave(struct SlaveArgs *slaveArgs)
{
	struct Catalog *catalog = slaveArgs->catalog;
	char request[1024];
	strcpy(request, slaveArgs->request);
	SOCKET socket_listen = slaveArgs->socket_listen;
	struct sockaddr_storage client_address = slaveArgs->client_address;

	printf("Remote address is: ");
	char address_buffer[100];
	char service_buffer[100];
	getnameinfo(((struct sockaddr *)&client_address), sizeof(client_address), address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST | NI_NUMERICSERV);
	printf("%s %s\n", address_buffer, service_buffer);

	char args[20][20];
	split_str(request, ",", args);

	int selection = atoi(args[0]);

	char buf[BUFSIZ];
	memset(buf, 0, sizeof(buf));
	fflush(stdout);
	setbuf(stdout, buf);
	int code = handler(catalog, selection, args);
	fflush(stdout);
	setbuf(stdout, NULL);

	int bytes_sent = sendto(socket_listen, buf, (int)strlen(buf), 0, (const struct sockaddr *)&client_address, sizeof(client_address));
	printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(buf));
	return code;
}

#endif
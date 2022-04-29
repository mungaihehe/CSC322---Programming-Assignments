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
	SOCKET socket_listen;
	SOCKET socket_client;
	struct sockaddr_storage client_address;
};

int slave(struct SlaveArgs *slaveArgs)
{
	struct Catalog *catalog = slaveArgs->catalog;
	SOCKET socket_listen = slaveArgs->socket_listen;
	SOCKET socket_client = slaveArgs->socket_client;
	struct sockaddr_storage client_address = slaveArgs->client_address;
	int code;
	while (1)
	{
		printf("Remote address is: ");
		char address_buffer[100];
		char service_buffer[100];
		getnameinfo(((struct sockaddr *)&client_address), sizeof(client_address), address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST | NI_NUMERICSERV);
		printf("%s %s\n", address_buffer, service_buffer);

		char request[1024];
		int bytes_received = recv(socket_client, request, 1024, 0);

		printf("Received %d bytes.\n", bytes_received);
		printf("%.*s", bytes_received, request);

		char args[20][20];
		split_str(request, ",", args);

		int selection = atoi(args[0]);

		char buf[BUFSIZ];
		memset(buf, 0, sizeof(buf));
		fflush(stdout);
		setbuf(stdout, buf);
		code = handler(catalog, selection, args);
		fflush(stdout);
		setbuf(stdout, NULL);
		int bytes_sent = send(socket_client, buf, (int)strlen(buf), 0);
		printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(buf));

		if (code == -1)
			break;
	}
	printf("Closing client connection...\n");
	CLOSESOCKET(socket_client);
	return code;
}

#endif
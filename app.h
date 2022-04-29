#ifndef APP_H_INCLUDED
#define APP_H_INCLUDED
#include <stdlib.h>
#include <string.h>
#include "catalog.h"
#include "util.h"
#include "handler.h"
#include "cross_platform_sockets.h"
#include "slave.h"
#include <process.h>

void app(struct Catalog *catalog)
{
#if defined(_WIN32)
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n");
		return;
	}
#endif

	printf("Configuring local address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	struct addrinfo *bind_address;
	getaddrinfo(0, "8080", &hints, &bind_address);

	printf("Creating socket...\n");
	SOCKET socket_listen;
	socket_listen = socket(bind_address->ai_family,
						   bind_address->ai_socktype, bind_address->ai_protocol);

	if (!ISVALIDSOCKET(socket_listen))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return;
	}

	printf("Binding socket to local address...\n");
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
		return;
	}
	freeaddrinfo(bind_address);
	// no need to call listen() or accept() for UDP

	fd_set master;
	FD_ZERO(&master);
	FD_SET(socket_listen, &master);
	SOCKET max_socket = socket_listen;
	printf("Waiting for connections...\n");

	while (1)
	{

		struct sockaddr_storage client_address;
		socklen_t client_len = sizeof(client_address);
		struct SlaveArgs *slaveArgs = (struct SlaveArgs *)malloc(sizeof(struct SlaveArgs));
		int bytes_received = recvfrom(socket_listen, slaveArgs->request, 1024, 0, (struct sockaddr *)&client_address, &client_len);

		printf("Received %d bytes.\n", bytes_received);
		printf("%.*s", bytes_received, slaveArgs->request);
		slaveArgs->catalog = catalog;
		slaveArgs->client_address = client_address;
		slaveArgs->socket_listen = socket_listen;
		if (_beginthread((void (*)(void *))slave, 16536, (void *)slaveArgs) < 0)
		{
			printf("Error spawning thread.\n");
			break;
		}
	}

	printf("Closing listening socket...\n");
	CLOSESOCKET(socket_listen);
#if defined(_WIN32)
	WSACleanup();
#endif
	printf("Finished.\n");
}

#endif
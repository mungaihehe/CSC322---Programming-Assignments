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
	hints.ai_socktype = SOCK_STREAM;
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

	printf("Listening...\n");
	if (listen(socket_listen, 10) < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
		return;
	}

	while (1)
	{
		printf("Waiting for connections...\n");

		struct sockaddr_storage client_address;
		int client_len = sizeof(client_address);

		SOCKET socket_client = accept(socket_listen, (struct sockaddr *)&client_address, &client_len);
		printf("Client connected...\n");
		struct SlaveArgs *slaveArgs = (struct SlaveArgs *)malloc(sizeof(struct SlaveArgs));
		slaveArgs->catalog = catalog;
		slaveArgs->socket_listen = socket_listen;
		slaveArgs->socket_client = socket_client;
		slaveArgs->client_address = client_address;
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
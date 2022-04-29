#ifndef APP_H_INCLUDED
#define APP_H_INCLUDED
#include <stdlib.h>
#include <string.h>
#include "catalog.h"
#include "util.h"
#include "handler.h"
#include "cross_platform_sockets.h"

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

	while (1)
	{
		printf("Waiting to receive...\n");
		struct sockaddr_storage client_address;
		socklen_t client_len = sizeof(client_address);
		char request[1024];
		int bytes_received = recvfrom(socket_listen, request, 1024, 0, (struct sockaddr *)&client_address, &client_len);

		printf("Received %d bytes.\n", bytes_received);
		printf("%.*s", bytes_received, request);

		printf("Remote address is: ");
		char address_buffer[100];
		char service_buffer[100];
		getnameinfo(((struct sockaddr *)&client_address), client_len, address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST | NI_NUMERICSERV);
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
		if (code == -1)
		{
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
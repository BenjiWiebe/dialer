#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

void send_data(char *data)
{
	WSADATA wsaData;
	SOCKET sock = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int r;

	r = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(r != 0)
	{
		warning("WSAStartup failed with error: %d\n", r);
		exit(1);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	r = getaddrinfo(HOSTNAME, _PORTNO, &hints, &result);
	if(r != 0)
	{
		warning("getaddrinfo failed with error: %d\n", r);
		WSACleanup();
		exit(1);
	}

	for(ptr=result; ptr != NULL ;ptr=ptr->ai_next)
	{
		sock = socket(ptr->ai_family, ptr->ai_socktype,ptr->ai_protocol);
		if(sock == INVALID_SOCKET)
		{
			warning("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			exit(1);
		}

		r = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
		if(r == SOCKET_ERROR)
		{
			closesocket(sock);
			sock = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if(sock == INVALID_SOCKET){
		warning("Unable to connect to server!\n");
		WSACleanup();
		exit(1);
	}

	r = send(sock, data, (int)strlen(data), 0);
	if (r == SOCKET_ERROR)
	{
		warning("send failed with error: %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	r = shutdown(sock, SD_SEND);
	if(r == SOCKET_ERROR)
	{
		warning("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	closesocket(sock);
	WSACleanup();
}

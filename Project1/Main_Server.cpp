#define WIN32_LEAN_AND_MEAN // Strips out rarely used calls

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5150"

int main(int argc, char** argv)
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)
	{
		// Something went wrong, tell the user the error id
		printf("WSAStartup() failed with error: %d\n", iResult);
		return 1;
	}
	else
	{
		printf("WSAStartup() was successful!\n");
	}

	// #1 - Socket
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET acceptSocket = INVALID_SOCKET;

	struct addrinfo *addrResult = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addrResult);
	if (iResult != 0)
	{
		printf("getaddrinfo() failed with error: %d\n", iResult);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("getaddrinfo() was successful!\n");
	}

	// Create a SOCKET for connecting to the server
	listenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);

	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("socket() was successful!\n");
	}

	// #2 - Bind - Setup the TCP listening socket
	iResult = bind(
		listenSocket,
		addrResult->ai_addr,
		(int)addrResult->ai_addrlen
	);

	if (iResult == SOCKET_ERROR)
	{
		printf("bind() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("bind() was successful!\n");
	}

	// We don't need this anymore
	freeaddrinfo(addrResult);

	// #3 - Listen
	iResult = listen(listenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR)
	{
		printf("listen() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("listen() was successful!\n");
	}

	// #4 - Accept (Blocking Call)
	printf("Waiting for client to connect\n");
	acceptSocket = accept(listenSocket, NULL, NULL);
	if (acceptSocket == INVALID_SOCKET)
	{
		printf("accept() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("accept() was successful!\n");
	}

	// No longer need server socket
	closesocket(listenSocket);

	// #5 - recv & send (Blocking Calls)
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	int iSendResult;

	do
	{
		printf("Waiting to receive data from client\n");
		iResult = recv(acceptSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			// We have recieved data successfully
			// iResult is the number of bytes received
			printf("Bytes received:%d\n", iResult);

			// Send data to the client
			iSendResult = send(acceptSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send() failed with error:%d\n", WSAGetLastError());
				closesocket(acceptSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult < 0)
		{
			printf("recv failed with errors:%d\n", WSAGetLastError());
			closesocket(acceptSocket);
			WSACleanup();
			return 1;
		}
		else // iResult == 0
		{
			printf("Connection closing...\n");
		}
	} while (iResult > 0);

	// #6 - Close
	iResult = shutdown(acceptSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(acceptSocket);
		WSACleanup();
		return 1;
	}

	// Cleanup
	closesocket(acceptSocket);
	WSACleanup();

	return 0;
}
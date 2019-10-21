// Main_Client.cpp
// Jenny Moon & Ryan O'Donnell
// Chatroom client for the server

#define WIN32_LEAN_AND_MEAN // Strips out rarely used calls

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include "Buffer_Client.h"
#include "Protocol_Client.h"

SOCKET Connection;
int commandID;
bool run = true;
void ClientThread();

int main(void)
{
	//Winsock Startup
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
	{
		MessageBox(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	// Socket addres info
	SOCKADDR_IN addr;

	int sizeofadr = sizeof(addr);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
	addr.sin_port = htons(1234567);
	addr.sin_family = AF_INET; //IPv4 

	Connection = socket(AF_INET, SOCK_STREAM, NULL); //Creates connection socket
	if (connect(Connection, (SOCKADDR*)&addr, sizeofadr) != 0)
	{
		MessageBox(NULL, "Failed to connect", "Error", MB_OK | MB_ICONERROR);
	}

	std::cout << "Connected!" << std::endl;

	std::cout << "Enter name: ";
	Protocol* nameProtocol = new Protocol();
	nameProtocol->CreateBuffer(256);
	std::string input = "";
	std::getline(std::cin, input);

	nameProtocol->messageBody.name = input.c_str();
	nameProtocol->SendName(*nameProtocol->buffer);

	std::vector<char> packet = nameProtocol->buffer->mBuffer;
	send(Connection, &packet[0], packet.size(), 0);

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL); //Create a thread

	while (true)
	{
		Protocol* messageSendProtocol = new Protocol();

		std::string input = "";
		std::getline(std::cin, input);
		messageSendProtocol->CreateBuffer(256);
		messageSendProtocol->messageHeader.commandId = commandID;

		if (commandID == 2)
		{
			messageSendProtocol->messageBody.roomName = input.c_str();
			messageSendProtocol->JoinRoom(*messageSendProtocol->buffer);
		}
		else if (commandID == 4)
		{
			if (input == "LeaveRoom")
			{
				messageSendProtocol->LeaveRoom(*messageSendProtocol->buffer);
			}
			else
			{
				messageSendProtocol->messageBody.name = nameProtocol->messageBody.name;
				messageSendProtocol->messageBody.message = input.c_str();
				messageSendProtocol->SendMessages(*messageSendProtocol->buffer);
			}
		}

		std::vector<char> packet = messageSendProtocol->buffer->mBuffer;
		send(Connection, &packet[0], packet.size(), 0);
		Sleep(10);
	}

	system("pause");
	return 0;
}

// Handle the client thread and receives messages from the server
void ClientThread()
{
	std::vector<char> packet(512);
	int packLength;
	while (run)
	{
		if ((packLength = recv(Connection, &packet[0], packet.size(), NULL)) < 1) {
			std::cout << "Closing connection" << std::endl;
			closesocket(Connection);
			WSACleanup();
			run = false;
		}
		else
		{
			Protocol* messageProtocol = new Protocol();
			messageProtocol->CreateBuffer(512);

			messageProtocol->buffer->mBuffer = packet;
			messageProtocol->ReadHeader(*messageProtocol->buffer);

			messageProtocol->buffer->ResizeBuffer(messageProtocol->messageHeader.packetLength);
			
			messageProtocol->ReceiveMessage(*messageProtocol->buffer);
			std::cout << messageProtocol->messageBody.message << std::endl;
			commandID = messageProtocol->messageHeader.commandId;

			delete messageProtocol;
		}
	}
}

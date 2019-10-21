// Main_Server.cpp
// Jenny Moon & Ryan O'Donnell
// Non-blocking server using threads

#define WIN32_LEAN_AND_MEAN // Strips out rarely used calls

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <vector>

#include "Protocol_Server.h"

struct Client
{
	SOCKET Connection;
	std::string name = "";
	std::string room = "";
};

Client Clients[100];
std::vector<int> lobby;

int clientsCounter = 0;
void HandleClients(int index);
void SendMessageToClient(SOCKET theConnection, int id, std::string message);
void SendMessageToAllInGroup(std::string groupName, int id, std::string message);
void SendMessageOthersInGroup(int clientIndex, std::string groupName, int id, std::string message);

int main()
{
	//Winsock Startup
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
	{
		MessageBox(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	std::cout << "Starting server" << std::endl;

	// Socket addres info
	SOCKADDR_IN addr;

	int addrlen = sizeof(addr);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
	addr.sin_port = htons(1234567);
	addr.sin_family = AF_INET; //IPv4 

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Creates socket to listen for new connections
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); // Binds the address to the socket
	listen(sListen, SOMAXCONN); //Listen the socket

	//To accept a connection
	SOCKET newConnection; //Socket to hold the client's connection
	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);
		if (newConnection == 0)
		{
			std::cout << "Failed to accept the client's connection." << std::endl;
		}
		else
		{
			std::cout << "New client Connected!" << std::endl;

			Clients[i].Connection = newConnection;
			lobby.push_back(clientsCounter);
			clientsCounter++;
			std::cout << "Number of Connections: " << lobby.size() << std::endl;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)HandleClients, (LPVOID)(i), NULL, NULL); //Create a thread 
		}
	}

	system("pause");
	return 0;
}

void HandleClients(int index)
{
	int packLength;
	bool run = true;

	while (run)
	{
		std::vector<char> packet(512);
		if ((packLength = recv(Clients[index].Connection, &packet[0], packet.size(), NULL)) < 1) 
		{
			closesocket(Clients[index].Connection);
			run = false;
		}
		else
		{
			Protocol* messageProtocol = new Protocol();
			messageProtocol->CreateBuffer(512);

			messageProtocol->buffer->mBuffer = packet;
			messageProtocol->ReadHeader(*messageProtocol->buffer);

			// Create name
			if (messageProtocol->messageHeader.commandId == 1)
			{
				messageProtocol->ReceiveName(*messageProtocol->buffer);
				Clients[index].name = messageProtocol->messageBody.name;
				std::string greet = "Hello [" + messageProtocol->messageBody.name + "]!\nEnter a number (1 - 3) to join a room!\n1 - Prequel Memes, 2 - Chonkers, 3 - Rare puppers";
				SendMessageToClient(Clients[index].Connection, 2, greet);
			}

			// Join the room
			if (messageProtocol->messageHeader.commandId == 2) 
			{				
				std::string setRoom = "\nEnter \'LeaveRoom\' to leave room";
				SendMessageToClient(Clients[index].Connection, 2, setRoom);
				
				messageProtocol->JoinRoom(*messageProtocol->buffer);

				if (messageProtocol->messageBody.roomName == "1")
				{
					Clients[index].room = "Prequelmemes";
					std::string message = "[" + Clients[index].name + "] has joined the room Prequelmemes";
					SendMessageToAllInGroup("Prequelmemes", 4, message);
				}
				else if (messageProtocol->messageBody.roomName == "2")
				{
					Clients[index].room = "Chonkers";
					std::string message = "[" + Clients[index].name + "] has joined the room Chonkers";
					SendMessageToAllInGroup("Chonkers", 4, message);
				}
				else if (messageProtocol->messageBody.roomName == "3")
				{
					Clients[index].room = "Rarepuppers";
					std::string message = "[" + Clients[index].name + "] has joined the room Rarepuppers";
					SendMessageToAllInGroup("Rarepuppers", 4, message);
				}
				else {
					std::string setGroup = "\nWrong group number! Try again!\n1 - Prequelmemes, 2 - Chonkers, 3 - Rarepuppers";
					SendMessageToClient(Clients[index].Connection, 2, setGroup);
				}
			}
			
			// Leave the room
			if (messageProtocol->messageHeader.commandId == 3) 
			{
				std::string message = "[" + Clients[index].name + "] left the room";
				SendMessageToAllInGroup(Clients[index].room, 4, message);
				Clients[index].room = "";

				std::string setRoom = "\nEnter a number(1 - 3) to join a room!\n1 - Prequel Memes, 2 - Chonkers, 3 - Rare puppers";
					SendMessageToClient(Clients[index].Connection, 2, setRoom);
			}

			// Send message
			if (messageProtocol->messageHeader.commandId == 4)
			{
				messageProtocol->ReceiveMessage(*messageProtocol->buffer);
				std::cout << "[" << Clients[index].room << "][" << messageProtocol->messageBody.name << "]: " << messageProtocol->messageBody.message << std::endl;
				std::string message = "[" + messageProtocol->messageBody.name + "]: " + messageProtocol->messageBody.message;
				SendMessageOthersInGroup(index, Clients[index].room, 4, message);
			}

			packLength = 0;
			packet.clear();
			delete messageProtocol;
		}
	}
}

// Sends message to current client
void SendMessageToClient(SOCKET theConnection, int id, std::string message)
{
	Protocol* messageSendProtocol = new Protocol();
	messageSendProtocol->CreateBuffer(512);
	messageSendProtocol->messageHeader.commandId = id;
	messageSendProtocol->messageBody.message = message;
	messageSendProtocol->SendMessages(*messageSendProtocol->buffer, id);

	std::vector<char> packet = messageSendProtocol->buffer->mBuffer;
	send(theConnection, &packet[0], packet.size(), 0);

	delete messageSendProtocol;
}

// Broadcasts to all (Join/Leave)
void SendMessageToAllInGroup(std::string groupName, int id, std::string message)
{
	Protocol* messageSendProtocol = new Protocol();
	messageSendProtocol->CreateBuffer(512);

	messageSendProtocol->messageHeader.commandId = id;
	messageSendProtocol->messageBody.message = message;
	
	messageSendProtocol->SendMessages(*messageSendProtocol->buffer, id);

	std::vector<char> packet = messageSendProtocol->buffer->mBuffer;
	for (int i = 0; i < clientsCounter; i++)
	{
		if (Clients[i].room == groupName)
		{
			send(Clients[i].Connection, &packet[0], packet.size(), 0);
		}
	}
	delete messageSendProtocol;
}

// Messages others in the room 
void SendMessageOthersInGroup(int clientIndex, std::string roomName, int id, std::string message)
{
	Protocol* messageSendProtocol = new Protocol();
	messageSendProtocol->CreateBuffer(512);
	messageSendProtocol->messageHeader.commandId = id;

	messageSendProtocol->messageBody.message = message;
	messageSendProtocol->SendMessages(*messageSendProtocol->buffer, id);

	std::vector<char> packet = messageSendProtocol->buffer->mBuffer;
	for (int i = 0; i < clientsCounter; i++)
	{
		if (clientIndex == i)
		{
			continue;
		}
		if (Clients[i].room == roomName)
		{
			send(Clients[i].Connection, &packet[0], packet.size(), 0);
		}
	}

	delete messageSendProtocol;
}

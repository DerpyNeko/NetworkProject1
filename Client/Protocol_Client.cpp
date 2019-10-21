// Protocol_Client.cpp
// Jenny Moon & Ryan O'Donnell
// Creates and reads the formatting of the packets needed for sending and receiving

#include "Protocol_Client.h"
#include <iostream>

Protocol::Protocol()
{
	this->buffer = NULL;
}

Protocol::~Protocol()
{
	delete this->buffer;
}

void Protocol::CreateBuffer(size_t size)
{
	this->buffer = new Buffer(size);
}

void Protocol::ReadHeader(Buffer &myBuffer)
{
	this->messageHeader.packetLength = myBuffer.ReadInt32LE();
	this->messageHeader.commandId = myBuffer.ReadShort16LE();
	return;
}

// Sends user inputted name to server as a packet
void Protocol::SendName(Buffer &myBuffer)
{
	this->messageHeader.commandId = 1;
	this->messageHeader.packetLength = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.name.length();
	myBuffer.ResizeBuffer(this->messageHeader.packetLength);
	myBuffer.WriteInt32LE(this->messageHeader.packetLength);
	myBuffer.WriteShort16LE(this->messageHeader.commandId);
	myBuffer.WriteInt32LE(this->messageBody.name.length());
	const char *temp = this->messageBody.name.c_str();
	for (int i = 0; temp[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(temp[i]);
	}
}

// Sends server room number that client chose as a packet
void Protocol::JoinRoom(Buffer &myBuffer)
{
	this->messageHeader.commandId = 2;
	this->messageHeader.packetLength = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.roomName.length();
	myBuffer.ResizeBuffer(this->messageHeader.packetLength);
	myBuffer.WriteInt32LE(this->messageHeader.packetLength);
	myBuffer.WriteShort16LE(this->messageHeader.commandId);
	myBuffer.WriteInt32LE(this->messageBody.roomName.length());
	const char *temp = this->messageBody.roomName.c_str();
	for (int i = 0; temp[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(temp[i]);
	}
}

// Sends server the leave room command
void Protocol::LeaveRoom(Buffer &myBuffer)
{
	this->messageHeader.commandId = 3;
	this->messageHeader.packetLength = sizeof(int) + sizeof(short);
	myBuffer.ResizeBuffer(this->messageHeader.packetLength);
	myBuffer.WriteInt32LE(this->messageHeader.packetLength);
	myBuffer.WriteShort16LE(this->messageHeader.commandId);
}

// Receive message from server
void Protocol::ReceiveMessage(Buffer &myBuffer)
{
	int length = myBuffer.ReadInt32LE();
	for (int i = 0; i <= length - 1; i++)
	{
		this->messageBody.message += myBuffer.ReadChar8LE();
	}
}

// Sends client name and message to server as a packet
void Protocol::SendMessages(Buffer &myBuffer)
{
	this->messageHeader.commandId = 4;
	this->messageHeader.packetLength = sizeof(int) + sizeof(short) + sizeof(int) + sizeof(int) + this->messageBody.name.length()+ sizeof(int) + this->messageBody.message.length();

	myBuffer.ResizeBuffer(this->messageHeader.packetLength);
	myBuffer.WriteInt32LE(this->messageHeader.packetLength);
	myBuffer.WriteShort16LE(this->messageHeader.commandId);

	myBuffer.WriteInt32LE(this->messageBody.name.length());
	const  char *tempName = this->messageBody.name.c_str();
	for (int i = 0; tempName[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(tempName[i]);
	}

	myBuffer.WriteInt32LE(this->messageBody.message.length());
	const  char *tempMessage = this->messageBody.message.c_str();
	for (int i = 0; tempMessage[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(tempMessage[i]);
	}
}

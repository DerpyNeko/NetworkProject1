// Protocol_Server.cpp
// Jenny Moon & Ryan O'Donnell
// Creates and reads the formatting of the packets needed for sending and receiving

#include "Protocol_Server.h"
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

void Protocol::ReceiveName(Buffer &myBuffer)
{
	int length = myBuffer.ReadInt32LE();
	for (int i = 0; i <= length - 1; i++)
	{
		this->messageBody.name += myBuffer.ReadChar8LE();
	}
}

void Protocol::JoinRoom(Buffer &myBuffer)
{
	int length = myBuffer.ReadInt32LE();
	for (int i = 0; i <= length - 1; i++)
	{
		this->messageBody.roomName += myBuffer.ReadChar8LE();
	}
}

void Protocol::ReceiveMessage(Buffer &myBuffer)
{
	ReceiveName(myBuffer);

	int length = myBuffer.ReadInt32LE();
	for (int i = 0; i <= length - 1; i++)
	{
		this->messageBody.message += myBuffer.ReadChar8LE();
	}
}

void Protocol::SendMessages(Buffer &myBuffer, int id)
{
	this->messageHeader.commandId = id;
	this->messageHeader.packetLength = sizeof(int) + sizeof(short) + sizeof(int) + this->messageBody.message.length();

	myBuffer.ResizeBuffer(this->messageHeader.packetLength);
	myBuffer.WriteInt32LE(this->messageHeader.packetLength);
	myBuffer.WriteShort16LE(this->messageHeader.commandId);
	myBuffer.WriteInt32LE(this->messageBody.message.length());
	const  char *temp = this->messageBody.message.c_str();
	for (int i = 0; temp[i] != '\0'; i++)
	{
		myBuffer.WriteChar8LE(temp[i]);
	}
}

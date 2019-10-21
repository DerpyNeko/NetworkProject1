// Buffer_Server.cpp
// Jenny Moon & Ryan O'Donnell
// Serialization and deserialization of basic types

#include "Buffer_Server.h"
#include <iostream>

Buffer::Buffer(size_t size) : mWriteIndex(0), mReadIndex(0)
{
	mBuffer.resize(size);
}

// Resizes the buffer to a given size
void Buffer::ResizeBuffer(size_t size)
{
	mBuffer.resize(size);
}

//Serializes the int value to the end of the buffer
void Buffer::WriteInt32LE(int value)
{
	mBuffer[mWriteIndex] = value;
	mBuffer[mWriteIndex + 1] = value >> 8;
	mBuffer[mWriteIndex + 2] = value >> 16;
	mBuffer[mWriteIndex + 3] = value >> 24;

	mWriteIndex += 4;
}

//Deserializes an int value at the end of the buffer
int Buffer::ReadInt32LE(void)
{
	uint32_t value = mBuffer[mReadIndex];
	value |= mBuffer[mReadIndex + 1] << 8;
	value |= mBuffer[mReadIndex + 2] << 16;
	value |= mBuffer[mReadIndex + 3] << 24;

	mReadIndex += 4;

	return value;
}

//Serializes the short value to the end of the buffer
void Buffer::WriteShort16LE(short value)
{
	mBuffer[mWriteIndex] = value;
	mBuffer[mWriteIndex + 1] = value >> 8;

	mWriteIndex += 2;
}

//Deserializes an short value at the end of the buffer
short Buffer::ReadShort16LE(void)
{
	short value = mBuffer[mReadIndex];
	value |= mBuffer[mReadIndex + 1] << 8;
	
	mReadIndex += 2;

	return value;
}

// Serializes the char value to the end of the buffer
void Buffer::WriteChar8LE(char value)
{
	mBuffer[mWriteIndex] = value;
	mWriteIndex += 1;
}

//Deserializes an char value at the end of the buffer
char Buffer::ReadChar8LE(void)
{
	char value = mBuffer[mReadIndex];
	mReadIndex += 1;

	return value;
}

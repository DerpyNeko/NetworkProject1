// Buffer_Client.h
// Jenny Moon & Ryan O'Donnell
// Serialization and deserialization of basic types

#ifndef BUFFER_CLIENT_HG
#define BUFFER_CLIENT_HG

#include <vector>

class Buffer {
public:
	Buffer(size_t size);

	void ResizeBuffer(size_t size);

	void WriteInt32LE(int value);
	int ReadInt32LE(void);

	void WriteShort16LE(short value);
	short ReadShort16LE(void);

	//void WriteChar8LE(uint8_t letter);
	//char ReadChar8LE(void);

	//void WriteStringLE(std::string value);
	//std::string ReadStringLE(void);

private:
	std::vector<uint8_t> mBuffer;

	int mReadIndex;
	int mWriteIndex;
};

#endif

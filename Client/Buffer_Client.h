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

	std::vector<char> GetBuffer();
	void SetBuffer(std::vector<char> buffer);

	void WriteInt32LE(int value);
	int ReadInt32LE(void);

	void WriteShort16LE(short value);
	short ReadShort16LE(void);

	void WriteChar8LE(char letter);
	char ReadChar8LE(void);

private:
	std::vector<char> mBuffer;

	int mReadIndex;
	int mWriteIndex;
};

#endif

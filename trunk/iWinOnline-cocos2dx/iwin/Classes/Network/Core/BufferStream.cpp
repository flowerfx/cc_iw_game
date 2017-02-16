#include "BufferStream.h"
#include "RKBaseLog/debug.h"
using namespace RKUtils;
#define BUFFER_SIZE 2048

BufferStream::BufferStream()
{
	size = 0;
	index = 0;
	data = nullptr;
}

BufferStream::~BufferStream() {
	if (data)
	{
		delete data;
	}
	data = nullptr;
}

void BufferStream::pushData(ubyte* bytes, int len)
{
	ubyte* tmp = nullptr;
	if (data == nullptr)
	{
		data = new ubyte[len];
	}
	else
	{
		tmp = new ubyte[size+len];
		memcpy(tmp, data, size);
		CC_SAFE_DELETE_ARRAY(data);
		data = tmp;
	}
	memcpy(data + size, bytes, len);
	size += len;
}
int BufferStream::readMessSize()
{
	int i = 0;
	int len = 4;
	ubyte* out = new ubyte[len];
	memcpy(out, data, len);
	if (!out)
	{
		return 0;
	}
	int n = len >> 1;
	for (int i = 0; i<n; ++i) {
		out[i] ^= out[len - i - 1];
		out[len - i - 1] ^= out[i];
		out[i] ^= out[len - i - 1];
	}
	memcpy(&i, out, 4);
	CC_SAFE_DELETE_ARRAY(out);
	return i;
}
bool BufferStream::isFullMess()
{
	return (readMessSize() > 4) && (readMessSize() <= size);
}
int BufferStream::readBytes(ubyte*& out, int len)
{
	if (out == nullptr) {
		out = new ubyte[len];
	}
	memcpy(out, data, len);
	memcpy(data, data+len, size - len);
	size-= len;
	return len;
}
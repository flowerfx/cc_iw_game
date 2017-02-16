#ifndef __BUFFER_STREAM_H__
#define __BUFFER_STREAM_H__

#include <string>
#include "Common/Common.h"
#include "NetworkMacros.h"
using namespace std;

class BufferStream{
private:
	ubyte* data;
	int size;
	int index;

	
public:
	BufferStream();
	~BufferStream();
	void pushData(ubyte* data, int size);
	bool isFullMess();
	int readBytes(ubyte*& out, int len);
	int readMessSize();
};

#endif
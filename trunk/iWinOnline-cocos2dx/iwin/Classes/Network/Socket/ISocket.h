#ifndef __IWIN__SOCKETPROTOCOL__
#define __IWIN__SOCKETPROTOCOL__

#include "cocos2d.h"
#if defined WINDOWS || defined _WINDOWS
#include <winsock2.h>
//#include "pthread.h"
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include <iostream>
#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "../Core/IMessagehandler.h"
#include "SocketConfig.h"
class IMessagehandler;
class ISocket {
public:
	ISocket() : _delegate(0) {}
	virtual ~ISocket() {}
    
	virtual bool connect(const char* ip, unsigned short int port) = 0;
	virtual int read(char* buf, int count) = 0;
	virtual int send(const char* buf, int count) = 0;
	virtual void close() = 0;
    
	void setDelegate(IMessagehandler* pDelegate) { 
		_delegate = pDelegate;
		_delegate->retain();
	}
	
protected:
	IMessagehandler*		_delegate;
	int sock;
	struct sockaddr_in addr_in;

};

#endif

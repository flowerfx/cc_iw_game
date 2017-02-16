#ifndef __IWIN_SCOKET__H__
#define __IWIN_SCOKET__H__

#include <iostream>
#include "ISocket.h"
class Socket : public ISocket {
    
public:
	Socket();
	~Socket();
	std::function<void()> funtion_connected;
	bool connect(const char* ip, unsigned short int port);
	int read(char* buf, int count);
	int send(const char* buf, int count);
	void close();
	void onConnected();
	bool isConnected() { return _bConnected; }
	std::mutex m_mutex;
private:
    
private:
	bool _bConnected;
	//pthread_mutex_t m_mutex;
};

#endif 

#include "Socket.h"
#include <string>
#include <cstdint>
#if defined WINDOWS || defined _WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include "DisplayView.h"

using namespace std;
std::mutex lock_thread;

Socket::Socket() : _bConnected(false)
{
	_bConnected = false;
	_delegate = nullptr;
}

Socket::~Socket()
{
	if (_bConnected)
		this->close();
	CC_SAFE_DELETE(_delegate);

}

bool Socket::connect(const char* ip, unsigned short int port)
{
#if defined WINDOWS || defined _WINDOWS
	WORD wVersion = 0x0202;
	WSADATA wsaData;
	int iResult = WSAStartup(wVersion, &wsaData);
	if (iResult != 0)
	{
		return false;
	}
#endif

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		//Cannot create socket
		if (_delegate)
			_delegate->onConnectFail();
		return false;
	}

	memset(&addr_in, 0, sizeof(addr_in));
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = inet_addr(ip);
	addr_in.sin_port = htons(port);

#define	CHECK_CONNECT_TIME_OUT	1
#if ( (!(defined WINDOWS || defined _WINDOWS)) && CHECK_CONNECT_TIME_OUT)
	//try to connect with time out
	long arg;
	int valopt;
	socklen_t lon;
	fd_set testset;
	struct timeval tv;
	arg = fcntl(sock, F_GETFL, NULL);
	if (arg < 0)
	{
		PWARN1("failed when get fcntl ", arg);
		if (_delegate)_delegate->onConnectFail();
		return false;
	}
	arg |= O_NONBLOCK;
	if (fcntl(sock, F_SETFL, arg) < 0)
	{
		PWARN1("failed when set fcntl NB", arg);
		if (_delegate)
			_delegate->onConnectFail();
		return false;
	}
#endif

	int status = ::connect(sock, (struct sockaddr*)&addr_in, sizeof(addr_in));
	PWARN1("connect return %d", status);
	if (status == 0)
	{
		//Connected to..;
#if ( (!(defined WINDOWS || defined _WINDOWS)) && CHECK_CONNECT_TIME_OUT)
		//NONBLOCK that mean error .
		PWARN1("nonblock connect return %d so like error", status);
		if (_delegate)
			_delegate->onConnectFail();
		return false;
#endif
	}
	else
	{
		//Just select with time out
#if ( (!(defined WINDOWS || defined _WINDOWS)) && CHECK_CONNECT_TIME_OUT)

		bool bconnected = false;
		if (status < 0)//NONBLOCK SO SHOULD RETURN IMMEDIALY WITH < 0
		{
			if (errno == EINPROGRESS) //not yet??!!
			{
				tv.tv_sec = 5;//5s
				tv.tv_usec = 0;
				PWARN1("EINPROGRESS select to check seconds");
				FD_ZERO(&testset);
				FD_SET(sock, &testset);
				do
				{
					int sel_res = select(sock + 1, NULL, &testset, NULL, &tv);
					PWARN1("EINPROGRESS select return %d seconds", sel_res);
					if (sel_res < 0)//for sure we get error except INTR!!!
					{
						if (errno == EINTR)//OK intr so try again .
						{
							PWARN1("EINPROGRESS EINTR when select so try select again");
							continue;
						}
					}
					else if (sel_res > 0)//check if we can write sock .
					{
						lon = sizeof(int);
						if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0)//error when get info from file id .
						{
							PWARN1("EINPROGRESS failed when SO_ERROR getsockopt");
						}
						else
						{
							if (valopt)
							{
								//error
								PWARN1("EINPROGRESS getsockopt return an error %d", valopt);
							}
							else
							{
								//ok ok just connected within 5s .
								bconnected = true;
								// Set to blocking mode again...
								arg = fcntl(sock, F_GETFL, NULL);
								arg &= (~O_NONBLOCK);
								if (fcntl(sock, F_SETFL, arg) < 0)
								{
									CCLOG("failed when reset fcntl B so shutdown and reconnect", arg);
									close();
									if (_delegate)
										_delegate->onConnectFail();
									return false;
								}
								//OK that mean we established .
								break;
							}
						}
					}
					else //time out so just like error .
					{
						PWARN1("EINPROGRESS select time out");
					}
					break;
				} while (1);
			}
		}
		//status > 0 that mean error .
		if (!bconnected)
		{
#endif
			//Error: cannot connect to server.
			close();
			if (_delegate)
				_delegate->onConnectFail();
			return false;
#if ( (!(defined WINDOWS || defined _WINDOWS)) && CHECK_CONNECT_TIME_OUT)
		}
#endif
	}

	//Connected
	//_bConnected = true;
	if (_delegate)
	{
		//_delegate->onConnectOk();
		onConnected();
	}
	return true;
}

int Socket::read(char* pBuf, int nBufferSize)
{

	//memset(pBuf, 0, nBufferSize);
	int result;
	int offset = 0;
	struct timeval tv_sleep;
	tv_sleep.tv_sec = 0;
	tv_sleep.tv_usec = 10;
	do
	{

		result = ::recv(sock, pBuf + offset, nBufferSize - offset, 0);
		if ((result > 0) && (offset < nBufferSize))
		{
			offset += result;
#if (WINDOWS)
			Sleep(10);
#else
			select(NULL, NULL, NULL, NULL, &tv_sleep);
#endif
		}

	} while ((offset < nBufferSize) && (result > 0));

	if (result > 0)
	{
		return result;
	}
	//doing it job .
	if (_bConnected)
	{
        /*
		cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([this] {
			if (_delegate) {
				_delegate->onDisConnect();
			}
		});
         */
        if (_delegate) {
            _delegate->onDisConnect();
        }
        _bConnected = false;
        
	}


	return result;


	/*
	 int result = ::recv(sock, pBuf, nBufferSize, 0);
	 if (_bConnected && result <= 0)
	 {
	 _bConnected = false;
	 if (_delegate) {
	 m_mutex.lock();
	 _delegate->onDisConnect();
	 m_mutex.unlock();
	 }
	 }
	 */

}

int Socket::send(const char* pBuf, int nSize)
{
	int offset = 0;
	int status = 0;
	struct timeval tv_sleep;
	tv_sleep.tv_sec = 0;
	tv_sleep.tv_usec = 10;
	do
	{
		status = ::send(sock, pBuf + offset, nSize - offset, 0);
		if ((status > 0) && (offset < nSize))
		{
			offset += status;
#if defined  WINDOWS || defined _WINDOWS
			Sleep(10);
#else
			select(NULL, NULL, NULL, NULL, &tv_sleep);
#endif
		}

	} while ((offset < nSize) && (status > 0));

	if (status > 0)
	{
		return status;
	}
	//doing it job .
	_bConnected = false;
	if (_bConnected)
	{
		_bConnected = false;
		if (_delegate) {
			m_mutex.lock();
			_delegate->onDisConnect();
			m_mutex.unlock();
		}
	}


	return status;
}

void Socket::close()
{
	if (!_bConnected)
		return;
	_bConnected = false;
	//CC_SAFE_RELEASE(_delegate);
#if defined WINDOWS || defined _WINDOWS
	::closesocket(sock);
#else
	::shutdown(sock, SHUT_RDWR);
	::close(sock);
#endif

}

void Socket::onConnected()
{
	_bConnected = true;
	DataWriter* data = new DataWriter();
	data->writeByte(12);
	char* bytes = new char;
	bytes[0] = 12;
	send(bytes, data->getSize());
	if (funtion_connected)
	{
		funtion_connected();
	}
}

#include "SocketConnect.h"
#include <string>
#include <cstdint>
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32||defined WP8 || defined OS_W8 || defined OS_W10
#include <io.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include "../Global/GlobalMessageHandler.h"
#include "../Core/Message.h"
#include "cocos2d.h"

using namespace std;
void* RunHandlerMsg(void* data);
void* RunSendMsg(void* data);
void* sendMsg(void* data);
bool SocketConnect::is_run_handler = false;
bool SocketConnect::is_run_send = false;
SocketConnect::SocketConnect()
{
    p_socket = nullptr;
    p_msg_andler = new  GlobalMessageHandler();
    
    is_run_send = true;
    is_run_handler = true;
    std::thread thread1(RunHandlerMsg, this);
    thread1.detach();
    std::thread thread2(RunSendMsg, this);
    thread2.detach();
    p_buff_stream = nullptr;
	p_socket = new Socket();
	_tea = nullptr;
    
}

SocketConnect::~SocketConnect()
{
    if (p_socket)
    {
        delete p_socket;
    }
    if (p_buff_stream)
    {
        delete p_buff_stream;
    }
    p_buff_stream = nullptr;
    is_run_send = false;
    is_run_handler = false;
    
}

void SocketConnect::connect(std::string ip, unsigned short port)
{
    if (p_socket)
    {
        p_socket->close();
       // delete p_socket;
        //p_socket = nullptr;
        //p_msg_andler = new  GlobalMessageHandler();
		//p_msg_andler->retain();
    }
   
    p_socket->setDelegate(p_msg_andler);
    p_list_msg_send.clear();
    p_socket->funtion_connected = [this]() {
        //std::thread thread1(SocketUpdate, this);
        //thread1.detach();
        
    };
    p_socket->connect(ip.c_str(), port);
    
}

void SocketConnect::sendMessage(iwincore::Message* msg)
{ 
    p_list_msg_send.push_back(msg);
}

void SocketConnect::sendMessage(std::string str)
{
    CCLOG("%s", str.c_str());
    iwincore::Message* msg = new iwincore::Message(2800);
    msg->putString(str);
    sendMessage(msg);
}

void SocketConnect::sendMessage()
{
    
    if (p_list_msg_send.size()<=0|| p_socket == nullptr || !p_socket->isConnected())
        return;
    iwincore::Message* msg = p_list_msg_send[0];
    int encryptLen = 0;
    int len = msg->getBufferSize();
    char* buffer_test = (char*)msg->getBufferBinary();
    unsigned char* encryptData = nullptr;
    if (_tea && msg->isEncrypted())
    {
        encryptData = (unsigned char*)_tea->Encrypt(buffer_test, len, encryptLen);
    }
    else {
        encryptData = (unsigned char*)buffer_test;//new ubyte[2];
        encryptLen = len;
       // encryptData[0] = 1;
    }
    DataWriter* buffer = new DataWriter();
    // Write message length = data length + 1 byte x
    buffer->writeInt32(encryptLen + 1);
    buffer->writeBool(msg->isEncrypted() ? 1 : 0);
    buffer->writeBytes(encryptData, encryptLen);
    unsigned char* data = nullptr;
    int size = buffer->getSize();
    buffer->_getBytes(data);
    p_socket->send((char*)data, size);
    CC_SAFE_DELETE_ARRAY(data);
    CC_SAFE_DELETE_ARRAY(encryptData);
    //fix leak mem
    CC_SAFE_DELETE(msg);
    CC_SAFE_DELETE(buffer);
	if (p_list_msg_send.size() > 0)
	{
		p_list_msg_send.erase(p_list_msg_send.begin() + 0);
	}
}

bool SocketConnect::getConnected()
{
    return p_socket && p_socket->isConnected();
}

void SocketConnect::startThreadLoop()
{
    is_run_send = true;
    is_run_handler = true;
    /*pthread_create(&s_mutex_send, NULL, sendMsg, NULL);
     pthread_create(&s_mutex_handler, NULL, handlerMsg, NULL);*/
    
}

std::mutex m_mutex;

void SocketConnect::HandlerMessage()
{
    /*
     char* orgBuf = new char[SOCKET_BUFFER_RECIVE_SIZE];
     int readLen = p_socket->read(orgBuf, SOCKET_BUFFER_RECIVE_SIZE);
     if (readLen > 0)
     {
     if(!p_buff_stream)
     {
     p_buff_stream = new BufferStream();
     }
     p_buff_stream->pushData((unsigned char*)orgBuf, readLen);
     if (!p_buff_stream->isFullMess())
     {
     return;
     }
     int lenght = p_buff_stream->readMessSize() + 4;
     ubyte* bytes = nullptr;
     p_buff_stream->readBytes(bytes, lenght);
     DataReader* dataview = new DataReader(bytes, lenght);
     
     
     //DataReader* dataview = new DataReader((unsigned char*)orgBuf, readLen);
     int dataLen = dataview->readInt32();
     bool isEncrypted = dataview->readBool();
     unsigned char* msgContent = nullptr;
     int contentLen = dataLen - 1;
     iwincore::Message* msg;
     dataview->readBytes(msgContent, contentLen);
     if (isEncrypted)
     {
     int decryptLen;
     char* decryptContent = _tea->Decrypt((char*)msgContent, contentLen, decryptLen);
     msg = new iwincore::Message((unsigned char*)decryptContent, decryptLen);
     }
     else
     {
     msg = new iwincore::Message(msgContent, contentLen);
     }
     CC_SAFE_DELETE(dataview);
     if (msg->getCommandId() == 500) {
     unsigned char* keyTea = msg->readBuffer(32);
     _tea = new TEA(keyTea, 32);
     if (p_msg_andler != nullptr)
     {
     p_msg_andler->onConnectOk();
     }
     delete msg;
     }
     else
     {
     if (p_msg_andler != nullptr)
     {
     cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([this,msg] {
					p_msg_andler->onMessage(msg);
					delete msg;
     });
     }
     }
     
     }*/
    
    char* orgBuf = new char[LENGHT_CONTENT];
    int lenPos = 0;
    int msgLen = 0;
    int sock_status = 1;
    if (msgLen == 0)
    {
        while ((sock_status = p_socket->read(orgBuf + lenPos, LENGHT_CONTENT)) > 0)
        {
            DataReader* dataviewL = new DataReader((unsigned char*)orgBuf, LENGHT_CONTENT);
            msgLen = dataviewL->readInt32();
            delete dataviewL;
            if (msgLen > LENGHT_CONTENT)
            {
                delete orgBuf;
                lenPos = 0;
                break;
            }
        }
        
    }
    if (msgLen > 0)
    {
        char* orgBufData = new char[msgLen];
        int readLen = sock_status = p_socket->read(orgBufData, msgLen);
        if (readLen > 0)
        {
            DataReader* dataview = new DataReader((unsigned char*)orgBufData, msgLen);
            bool isEncrypted = dataview->readBool();
            unsigned char* msgContent = nullptr;
            int contentLen = msgLen - 1;
            iwincore::Message* msg;
            dataview->readBytes(msgContent, contentLen);
            if (isEncrypted)
            {
                int decryptLen;
                char* decryptContent = _tea->Decrypt((char*)msgContent, contentLen, decryptLen);
                msg = new iwincore::Message((unsigned char*)decryptContent, decryptLen);
            }
            else
            {
                msg = new iwincore::Message(msgContent, contentLen);
            }
            CC_SAFE_DELETE(dataview);
            if (msg->getCommandId() == 500) {
                unsigned char* keyTea = msg->readBuffer(32);
                _tea = new TEA(keyTea, 32);
                if (p_msg_andler != nullptr)
                {
                    p_msg_andler->onConnectOk();
                }
                delete msg;
            }
            else
            {
                //m_mutex.lock();
                if (p_msg_andler != nullptr)
                {
					cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([this, msg] {
						p_msg_andler->onMessage(msg);
						delete msg;
					});
                }
                //m_mutex.unlock();
            }
        }
        delete orgBufData;
    }
    msgLen = 0;
    //really
    if (sock_status <= 0)
    {
        
        //break;
    }
    //we may break but we do not care message will be delete at consumer .
    //delete orgBuf;

    
}

void SocketConnect::onClose()
{
    if(p_buff_stream)
    {
        delete p_buff_stream;
    }
	for (iwincore::Message* msg : p_list_msg_send)
	{
		CC_SAFE_DELETE(msg);
	}
	p_list_msg_send.clear();
    p_buff_stream = nullptr;
    p_socket->close();
}

Socket* SocketConnect::getSocket()
{
    return p_socket;
}

void* RunHandlerMsg(void* data)
{
    SocketConnect* connect = static_cast<SocketConnect*>(data);
    while (SocketConnect::is_run_handler)
    {
        if(connect->getConnected())
        {
            //connect->sendMessage();
            connect->HandlerMessage();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
    return 0;
}
void* RunSendMsg(void* data)
{
    SocketConnect* connect = static_cast<SocketConnect*>(data);
    while (SocketConnect::is_run_send)
    {
        if(connect->getConnected())
        {
            connect->sendMessage();
            //connect->HandlerMessage();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return 0;
}



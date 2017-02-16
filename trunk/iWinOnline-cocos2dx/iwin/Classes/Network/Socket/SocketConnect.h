#ifndef __IWIN_SCOKET_CONNECTION__H__
#define __IWIN_SCOKET_CONNECTION__H__
#include <iostream>
#include "Socket.h"
#include "Network/Core/TEA.h"
#include "Network/Core/BufferStream.h"
namespace iwincore {
    class Message;
}

class SocketConnect  {
    
public:
    bool static is_run_send;
    bool static is_run_handler;
    SocketConnect();
    ~SocketConnect();
    void connect(std::string ip, unsigned short port);
    void sendMessage(iwincore::Message* msg);
    void sendMessage(std::string str);
    void sendMessage();
    bool getConnected();
    void  HandlerMessage();
    void  onClose();
    Socket* getSocket();
private:
    Socket* p_socket;
    TEA* _tea;
    IMessagehandler* p_msg_andler;
    BufferStream* p_buff_stream;
    std::vector<iwincore::Message*>	p_list_msg_send;
    std::vector<iwincore::Message*>	p_list_msg_handler;
    
    void  startThreadLoop();
    
private:
    
};

#endif 

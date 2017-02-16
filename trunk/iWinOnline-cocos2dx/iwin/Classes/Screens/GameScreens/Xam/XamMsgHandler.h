#ifndef __XAM_MSG_HANDLER_H__
#define __XAM_MSG_HANDLER_H__
#include "../../../Network/Core/IMessagehandler.h"
#include "../../../Network/Core/Message.h"

class XamMsgHandler :public IMessagehandler
{
private:
	static XamMsgHandler* _instance;
public:
	static XamMsgHandler* getInstance()
	{
		if (_instance == nullptr)
			_instance = new XamMsgHandler();
		return _instance;
	}
	XamMsgHandler();
	virtual ~XamMsgHandler();

	virtual void onMessage(iwincore::Message* msg) override;
	virtual int onSaveMessage(iwincore::Message* msg) override;

	virtual void onConnectOk()
	{}

	virtual void onConnectFail() {}

	virtual void onDisConnect() {}


};

#endif //__XAM_MSG_HANDLER_H__


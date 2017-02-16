#include "XamService.h"
#include "XamMsgHandler.h"
#include "Common/GameController.h"

XamService::XamService()
{

}

XamService::~XamService()
{

}

void XamService::Move(byte roomID, byte boardID, vector<ubyte> cards)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::XAM_COMMAND);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(XamCommand::MOVE);
	m->putByte((byte)cards.size());
	for (int i = 0; i < cards.size(); i++)
	{
		m->putByte((byte)cards[i]);
	}
	GlobalService::sendMessage(m);
}

void XamService::Skip(byte roomID, byte boardID)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::XAM_COMMAND);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(XamCommand::SKIP);
	GlobalService::sendMessage(m);
}

void XamService::forceFinish(byte roomID, byte boardID)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FORCE_FINISH);
	m->putByte(roomID);
	m->putByte(boardID);
	GlobalService::sendMessage(m);
}

void XamService::baoXam(byte roomID, byte boardID)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::XAM_COMMAND);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(XamCommand::XAM);
	m->putInt(GameController::myInfo->IDDB);
	GlobalService::sendMessage(m);
}

void XamService::skipXam(byte roomID, byte boardID)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::XAM_COMMAND);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(XamCommand::SKIP_XAM);
	m->putInt(GameController::myInfo->IDDB);
	GlobalService::sendMessage(m);
}




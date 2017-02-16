#ifndef __XAM_SERVICE_H__
#define __XAM_SERVICE_H__
#include "../../../Network/Global/Globalservice.h"
#include "../../../Network/Core/Message.h"
#include "../../../Common/GameController.h"
#include "../../../Network/IwinProtocol.h"
class XamService
{
	
public:
	enum XamCommand
	{
		BOARD_INFO = 1,
		MOVE = 2,
		XAM = 3,
		SKIP_XAM = 4,
		SKIP = 5,
		FORCE_FINISH = 6,
		CHAN_XAM = 7,
		MOVE_ERROR = 8,
		HAVE_ONE_CARD = 9,
		DEN_XAM = 10
	};
	XamService();
	~XamService();
	static void Move(byte roomID, byte boardID,vector<ubyte> cards);
	static void Skip(byte roomID, byte boardID);
	static void forceFinish(byte roomID, byte boardID);
	
	/**
	* Báo xâm
	*
	* @param roomID
	* @param boardID
	*/
	static void baoXam(byte roomID, byte boardID);

	/**
	* Không muốn báo xâm
	*
	* @param roomID
	* @param boardID
	*/
	static void skipXam(byte roomID, byte boardID);
	
	
};

#endif //__XAM_SERVICE_H__


#include "XamMsgHandler.h"
#include "cocos2d.h"
#include "../../../Network/IwinProtocol.h"
#include "../../../Common/GameController.h"
#include "../../Object/Card.h"
#include "XamScreen.h"
#include "XamService.h"
/**
* Sub command cho game xam
*/

XamMsgHandler* XamMsgHandler::_instance = nullptr;
XamMsgHandler::XamMsgHandler()
{
}

XamMsgHandler::~XamMsgHandler()
{

}

int XamMsgHandler::onSaveMessage(iwincore::Message* msg)
{
	if (GameController::currentGameScreen)
	{
		return GameController::currentGameScreen->PushReturnMsg(msg);
	}
	return 0;
}

void XamMsgHandler::onMessage(iwincore::Message* msg)
{
	CCLOG("TienLenMsgHandler Main command: %d", msg->getCommandId());
	if (XamScreen::getInstance() == nullptr)
	{
		CCLOG("the game is exit so do-not receive msg anymore ! safe to continue");
		return;
	}

	switch (msg->getCommandId())
	{
	case IwinProtocol::RETURN_GAME:
	{
		int maxPlayer = GameController::getMaxPlayerOf(GameController::gameId);
		byte limitStepTime = msg->readByte();
		int userIDMove = msg->readInt();
		byte card_moved_leng = msg->readByte();
		vector<byte> card_moved(card_moved_leng, 0);
		for (int i = 0; i < card_moved_leng; i++)
		{
			card_moved[i] = msg->readByte();
		}
		int next_user_id = msg->readInt();

		byte card_leng = msg->readByte();
		vector<byte> myCard(card_leng, -1);
		for (int i = 0; i < card_leng; i++)
		{
			myCard[i] = msg->readByte();
		}
		vector<int> user_rank(maxPlayer, 0);
		vector<bool> user_skip(maxPlayer, 0);
		for (int i = 0; i < maxPlayer; i++)
		{
			user_rank[i] = -1;
			user_skip[i] = false;
		}

		byte usersLength = msg->readByte();
		for (int i = 0; i < usersLength; i++)
		{
			user_rank[i] = msg->readInt();
			user_skip[i] = msg->readBool();
		}
		byte limitTime = msg->readByte();
		int xamUserId = msg->readInt();
		bool isShowXamDialog = msg->readBool();
		byte xamTimeRemain = -1;
		if (isShowXamDialog)
		{
			xamTimeRemain = msg->readByte();
		}
		XamScreen::getInstance()->OnReconnect(limitStepTime, userIDMove,
			card_moved, next_user_id, myCard, limitTime, user_rank,
			user_skip, xamUserId, xamTimeRemain);
	}
	break;
	case IwinProtocol::SET_MONEY:
	{
		byte roomID = msg->readByte();
		byte boardID = msg->readByte();
		int money = msg->readInt();
		if (XamScreen::getInstance()->GetRoomID() == roomID
			&&  XamScreen::getInstance()->GetBoardID() == boardID) {
			XamScreen::getInstance()->setBetMoney(money);
		}
	}
	break;
	case IwinProtocol::START:
	{
		byte roomID6 = msg->readByte();
		byte boardID6 = msg->readByte();
		byte interval = msg->readByte();
		vector<unsigned char> myCards(10, 0);
		for (int i = 0; i < 10; i++)
		{
			myCards[i] = msg->readByte();
		}
		int whoMoveFirst = msg->readInt();

		XamScreen::getInstance()->Start(whoMoveFirst, interval, myCards);
	}
	break;
	case IwinProtocol::STOP_GAME:
	{
		byte roomID16 = msg->readByte();
		byte boardID16 = msg->readByte();
		try
		{
			int winnerID = msg->readInt();
			byte lengthUser = msg->readByte();
			vector<int> userIds(lengthUser);
			vector<int> penalty(lengthUser);
			vector<vector<byte>> cardsShow(lengthUser);
			for (int i = 0; i < lengthUser; i++)
			{
				userIds[i] = msg->readInt();
				penalty[i] = msg->readInt();
				byte lengthCard = msg->readByte();
				//cardsShow[i] = new byte[lengthCard];
				for (int j = 0; j < lengthCard; j++)
				{
					cardsShow[i, j].push_back(msg->readByte());
				}
			}
			XamScreen::getInstance()->OnStopGame(roomID16, boardID16, winnerID, userIds, penalty,cardsShow);
		}
		catch (exception e)
		{
			//OnStopGame(roomID16, boardID16, -1, null, null, null);
		}
	}
	break;

	case IwinProtocol::XAM_COMMAND:
	{
		short subCommand = msg->readByte();
		switch (subCommand)
		{
		case XamService::XamCommand::BOARD_INFO:
		{
			byte timeRemain = msg->readByte();

			bool isXam = msg->readBool();
			if (isXam)
			{
				XamScreen::getInstance()->ShowDialogConfirmXam(timeRemain, timeRemain);
			}
			else
			{
				XamScreen::getInstance()->OnGetTimeLeft(timeRemain);
			}
		}
		break;
		case XamService::XamCommand::MOVE:
		{
			byte roomID = msg->readByte();
			byte boardID = msg->readByte();
			int whoMove = msg->readInt();
			byte nCard = msg->readByte();
			vector<unsigned char> cards(nCard);
			for (int i = 0; i < nCard; i++)
			{
				cards[i] = msg->readByte();
			}
			int nextMove = msg->readInt();
			XamScreen::getInstance()->onMove(whoMove, cards, nextMove);
		}
		break;
		case XamService::XamCommand::XAM:
		{
			byte roomID1 = msg->readByte();
			byte boardID1 = msg->readByte();
			int whoXam = msg->readInt();
			XamScreen::getInstance()->Xam(whoXam);
		}
		break;
		case XamService::XamCommand::SKIP_XAM:
		{
			byte roomID2 = msg->readByte();
			byte boardID2 = msg->readByte();
			int whoMoveFirst = msg->readInt();
			XamScreen::getInstance()->SkipXam(whoMoveFirst);
		}
		break;
		case XamService::XamCommand::SKIP:
		{
			byte roomID3 = msg->readByte();
			byte boardID3 = msg->readByte();
			int whoSkip = msg->readInt();
			int nextMove1 = msg->readInt();
			bool clearCurrentCards = true;
			clearCurrentCards = msg->readBool();
			XamScreen::getInstance()->OnSkip(whoSkip, nextMove1, clearCurrentCards);
		}
		break;

		case XamService::XamCommand::FORCE_FINISH:
			/*byte roomID4 = msg->readByte();
			byte boardID4 = msg->readByte();
			CardInfo forceFinishCard = new CardInfo();
			forceFinishCard.owner =  msg->readInt();
			forceFinishCard.cards = new byte[10];
			try
			{
				for (int i = 0; i < 10; i++)
				{
					forceFinishCard.cards[i] = msg->readByte();
				}
			}
			catch (Exception e)
			{
				forceFinishCard.cards = null;
			}
			if (getXamScreen() != null
				&& XamScreen::getInstance()->boardID == boardID4
				&& XamScreen::getInstance()->roomID == roomID4)
			{
				XamScreen::getInstance()->ForceFinish(forceFinishCard);
			}*/
			break;
		case XamService::XamCommand::CHAN_XAM:
			/*byte roomID5 = msg->readByte();
			byte boardID5 = msg->readByte();
			int whoChanXam =  msg->readInt();
			XamScreen::getInstance()->chanXam(whoChanXam);*/
			break;
		case XamService::XamCommand::MOVE_ERROR:
			/*byte roomID19 = msg->readByte();
			byte boardID19 = msg->readByte();
			String info = msg.readString();
			XamScreen::getInstance()->MoveError(info);*/
			break;
		case XamService::XamCommand::HAVE_ONE_CARD:
		{
			int idUser = msg->readInt();
			XamScreen::getInstance()->OnHaveOneCard(idUser);
		}
		break;
		case XamService::XamCommand::DEN_XAM:// den bai neu co bai ma ko bat
		{
			int idUser =  msg->readInt();
			int cardZise =  msg->readInt();
			vector<ubyte> lsCardIds;
			for (int i = 0; i < cardZise; i++)
			{
				lsCardIds.push_back(msg->readByte());
			}

			XamScreen::getInstance()->OnDenBai(idUser, lsCardIds);
		}
		break;
		}
	}
	break;
	}
}

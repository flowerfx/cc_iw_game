#include "CoTuongMsgHandler.h"
#include "cocos2d.h"
#include "Network/IwinProtocol.h"
#include "Common/GameController.h"
#include "Screens/Object/Card.h"
#include "CoTuongScreen.h"
#include "CoTuongPlayer.h"
#include "Screens/Object/PlayerGroup.h"
#include "CoTuongPiece.h"

CoTuongMsgHandler* CoTuongMsgHandler::_instance = nullptr;
CoTuongMsgHandler::CoTuongMsgHandler()
{
	_instance = this;
}

CoTuongMsgHandler::~CoTuongMsgHandler()
{

}

int CoTuongMsgHandler::onSaveMessage(iwincore::Message* msg)
{
	if (GameController::currentGameScreen)
	{
		return GameController::currentGameScreen->PushReturnMsg(msg);
	}
	return 0;
}

void CoTuongMsgHandler::onMessage(iwincore::Message* msg)
{

	switch (msg->getCommandId())
	{
	case IwinProtocol::CO_TUONG_COMMAND: // command game binh
	case IwinProtocol::CO_UP_COMMAND: // command game binh
	{
		byte subCommand = msg->readByte();
		CCLOG("TTTTTTTTTTTT Sub command: %d", (int)subCommand);
		switch (subCommand) {
		case 15: // START:
		{
			byte roomID6 = msg->readByte();
			byte boardID6 = msg->readByte();
			int interval = msg->readInt();
			int whoMoveFirst = msg->readInt();
			int ownerQquotaTime = msg->readInt();
			int quotaTime = msg->readInt();
			if (getCoTuongScreen()->GetBoardID() == boardID6 && getCoTuongScreen()->GetRoomID() == roomID6) {
				for (int i = 0; i < getCoTuongScreen()->getNumberPlayer(); i++) {
					PlayerGroup* pg = getCoTuongScreen()->GetPlayerGroupAtIdx(i);
					if (pg->getPlayer()->IDDB == getCoTuongScreen()->GetOwnerID())
					{
						getCoTuongScreen()->setTimeQuota(pg->getPlayer()->IDDB, ownerQquotaTime);
					}
					else
					{
						getCoTuongScreen()->setTimeQuota(pg->getPlayer()->IDDB, quotaTime);
					}
					
				}
				getCoTuongScreen()->onStartGame(whoMoveFirst, interval);
			}
			break;
		}
		case 12:// het 3 lan di lai
		{
			//coTuongScreen().onRemoveButton(msg.readByte());
			getCoTuongScreen()->onRemoveButton(msg->readByte());
		}
		break;
		case 25: // MOVE
		{
			int whoMove = msg->readInt();
			byte xS = msg->readByte();
			byte yS = msg->readByte();
			byte xD = msg->readByte();
			byte yD = msg->readByte();
			int quotaTime = msg->readInt();
			int nextMove = msg->readInt();
			byte newPieceType = msg->readByte();// co tuong thi la -1
			Player* player = getCoTuongScreen()->getPlayer(whoMove);
			if (player != nullptr) {
				((CoTuongPlayer*)(player))->setQuotaTime(quotaTime);
				getCoTuongScreen()->setTimeQuota(player->IDDB, quotaTime);
			}

			CCLOG("%d %d %d %d %d %d %d", whoMove, xS, yS, xD, yD, nextMove, newPieceType);
			getCoTuongScreen()->onMove(whoMove, xS, yS, xD, yD, nextMove, newPieceType);
			break;
		}
		case 26:// UNDO
		{
			getCoTuongScreen()->onOpponentWantUndo();
			break;
		}
		case 30: // WANT_DRAW:
		{
			getCoTuongScreen()->onOpponentWantDraw();
		}
		break;
		case 31: //DENY_DRAW
		case 36: //DENY_LOSE
		{
			getCoTuongScreen()->addInfo_new(LANG_STR("opponent_not_agree").GetString());
		}
		break;
		case 35:
		{
			getCoTuongScreen()->onOpponentWantLose();
		}
		break;
		case 39:
		{
			getCoTuongScreen()->addInfo_new(msg->readString());
		}
		break;
		case 40:
		{
			std::vector<GameResult*> v;
			GameResult* r1 = new GameResult();
			r1->ID = msg->readInt();
			r1->result = msg->readByte();
			r1->d_money = msg->readInt();
			v.push_back(r1);
			GameResult* r2 = new GameResult();
			r2->ID = msg->readInt();
			r2->result = msg->readByte();
			r2->d_money = msg->readInt();

			v.push_back(r2);
			/**
			* 1 cho chieu bi cac truong hop con lai server khong tra ve
			*/
			byte typeFinish = -1;
			// chi nhan command cua co up co chieu bi khi no la co up va
			// kh co hoa
			if ((GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
				|| GameController::getCurrentGameType() == GameType::GAMEID_CHESS) && r1->result != 0) {
					typeFinish = msg->readByte();
					r1->name = msg->readString();
					r2->name = msg->readString();
					r1->d_sumexp = msg->readInt();
					r2->d_sumexp = msg->readInt();
					r1->d_addexp = msg->readInt();
					r2->d_addexp = msg->readInt();
			}
			//Log.log("CoTuongMsg", "typeFinish " + typeFinish);
			//if (HAVE_CHIEU_BI && typeFinish == 0)
			//	coTuongScreen().finishGame(v, typeFinish);
			//else
			//	coTuongScreen().onMatchResult(v);
			getCoTuongScreen()->finishGame(v, typeFinish);
			SAFE_DELETE_VECTOR(v);
		}
		break;
		case 10:
		{
			if (!msg->isEndRead())
			{
				int ownerUserId = -1, currUserID = -1, moveFirst = -1, intervalView = 0, limitStepTime = 0;
				int board[9][10];
				for (int i = 0; i < 9; i++) {
					for (int j = 0; j < 10; j++) {
						board[i][j] = 0;
					}
				}
				moveFirst = msg->readInt();
				currUserID = msg->readInt();
				limitStepTime = msg->readInt();
				intervalView = msg->readInt();
				ownerUserId = msg->readInt();
				int gameRemainingTime = msg->readInt();
				int pieceNumber = msg->readByte();
				for (int i = 0; i < pieceNumber; i++) {
					byte positionX = msg->readByte();
					byte positionY = msg->readByte();
					byte pieceType = msg->readByte();
					byte isMove2 = msg->readByte();// trong co
												  // up = 1 la
												  // quan do
												  // da lat
					if (GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
						|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) {
						if (isMove2 != 1) {
							pieceType = PIECE_FACE;
						}
						if (ownerUserId == moveFirst) {
							board[positionX][positionY] = pieceType;
						}
						else {
							board[positionX][positionY] = (byte)(pieceType + 10);
						}
					}
					else {
						if (ownerUserId == moveFirst) {
							board[positionX][positionY] = pieceType;
						}
						else {
							board[positionX][positionY] = (byte)(pieceType + 10);
						}
					}
				}
				int oppUserId = msg->readInt();
				int gameRemainingTimeOpp = msg->readInt();
				pieceNumber = msg->readByte();
				for (int i = 0; i < pieceNumber; i++) {
					byte positionX = msg->readByte();
					byte positionY = msg->readByte();
					byte pieceType = msg->readByte();
					byte isMove2 = msg->readByte();
					if (GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
						|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) {
						if (isMove2 != 1) {
							pieceType = PIECE_FACE;
						}
						if (oppUserId == moveFirst) {
							board[positionX][positionY] = pieceType;
						}
						else {
							board[positionX][positionY] = (byte)(pieceType + 10);
						}
					}
					else {
						if (oppUserId == moveFirst) {
							board[positionX][positionY] = pieceType;
						}
						else {
							board[positionX][positionY] = (byte)(pieceType + 10);
						}
					}
				}
				int seat;
				for (int i = 0; i < getCoTuongScreen()->getNumberPlayer(); i++) {
					PlayerGroup* pg = getCoTuongScreen()->GetPlayerGroupAtIdx(i);
					if (pg->getPlayer()->IDDB == getCoTuongScreen()->GetOwnerID())
					{
						getCoTuongScreen()->setTimeQuota(pg->getPlayer()->IDDB, gameRemainingTime);
					}
					else
					{
						getCoTuongScreen()->setTimeQuota(pg->getPlayer()->IDDB, gameRemainingTimeOpp);
					}

				}


				getCoTuongScreen()->onView(moveFirst, ownerUserId, currUserID, limitStepTime, intervalView, board);
			}
		}
		break;
		case 51:
		{
			int userEat = msg->readInt();// nguoi an
			int size = msg->readInt();
			//Log.log("size eat", "" + size);
			int count = 0;
			std::vector<char> eatPieces;
			std::vector<char> lostPieces;
			if (size > 0) {
				while (count < size) {
					eatPieces.push_back(msg->readByte());
					count++;
				}
			}
			size = msg->readInt();
			if (size > 0) {
				count = 0;
				while (count < size) {
					lostPieces.push_back(msg->readByte());
					count++;
				}
			}

			getCoTuongScreen()->onEatAndLostPieces(userEat, eatPieces, lostPieces);
		}
		break;
		case 53:
		{
			int length = msg->readShort();
			std::vector<PieceStep> steps;
			for (short i = 0; i < length; i++) {
				PieceStep step;
				step.typePieceBefore = msg->readByte();
				step.typePieceAfter = msg->readByte();
				step.x = msg->readByte();
				step.y = msg->readByte();
				step.xTo = msg->readByte();
				step.yTo = msg->readByte();
				steps.push_back(step);

			}

			getCoTuongScreen()->setCoPhoReconnect(steps);
		}
		break;
		default:
			CCLOG("Co Tuong TTTTTTTTTTTTTTTTTTTTTTTT Handler %d", subCommand);
		}
		}
		break;
		case IwinProtocol::GET_TIME_BAR:
		{
			ubyte timeStart = msg->readByte();
			getCoTuongScreen()->OnShowCountDown(timeStart, .0f);
		}
		break;
		case IwinProtocol::SET_MONEY:
		{
			int roomId = msg->readByte();
			int boardId = msg->readByte();
			int money = msg->readInt();
			if (getCoTuongScreen()->GetRoomID() == roomId 
				&& getCoTuongScreen()->GetBoardID() == boardId)
			{
				getCoTuongScreen()->setBetMoney(money);
			}
		}
		break;
		case IwinProtocol::QUICK_PLAY:
		{
			char roomID = msg->readByte();
			char boardID = msg->readByte();
			int userId = msg->readInt();
			bool isQuickPlay = msg->readBool();
			if (GameController::currentGameScreen->GetRoomID() == roomID
				&& GameController::currentGameScreen->GetBoardID() == boardID)
			{
				getCoTuongScreen()->onSomeOneQuickPlay(userId, isQuickPlay);
			}
		}
		break;
		case IwinProtocol::RETURN_GAME:
		{
			int ownerUserId = -1, currUserID = -1, moveFirst = -1, intervalView = 0, limitStepTime = 0;
			int board[9][10];
			for (int i = 0; i < 9; i++) {
				for (int j = 0; j < 10; j++) {
					board[i][j] = 0;
				}
			}
			moveFirst = msg->readInt();
			currUserID = msg->readInt();
			limitStepTime = msg->readInt();
			intervalView = msg->readInt();
			ownerUserId = msg->readInt();
			int gameRemainingTime = msg->readInt();
			int pieceNumber = msg->readByte();
			for (int i = 0; i < pieceNumber; i++) {
				byte positionX = msg->readByte();
				byte positionY = msg->readByte();
				byte pieceType = msg->readByte();
				byte isMove2 = msg->readByte();// trong co up =
											  // 1 la quan do
											  // da lat
				if (GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
					|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) {
					if (isMove2 != 1) {
						pieceType = PIECE_FACE;
					}
					if (ownerUserId == moveFirst) {
						board[positionX][positionY] = pieceType;
					}
					else {
						board[positionX][positionY] = (byte)(pieceType + 10);
					}
				}
				else {
					if (ownerUserId == moveFirst) {
						board[positionX][positionY] = pieceType;
					}
					else {
						board[positionX][positionY] = (byte)(pieceType + 10);
					}
				}
			}
			int oppUserId = msg->readInt();
			int gameRemainingTimeOpp = msg->readInt();
			pieceNumber = msg->readByte();
			for (int i = 0; i < pieceNumber; i++) {
				byte positionX = msg->readByte();
				byte positionY = msg->readByte();
				byte pieceType = msg->readByte();
				byte isMove2 = msg->readByte();
				if (GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
					|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP) {
					if (isMove2 != 1) {
						pieceType = PIECE_FACE;
					}
					if (oppUserId == moveFirst) {
						board[positionX][positionY] = pieceType;
					}
					else {
						board[positionX][positionY] = (byte)(pieceType + 10);
					}
				}
				else {
					if (oppUserId == moveFirst) {
						board[positionX][positionY] = pieceType;
					}
					else {
						board[positionX][positionY] = (byte)(pieceType + 10);
					}
				}
			}
			int seat;
			for (int i = 0; i < getCoTuongScreen()->getNumberPlayer(); i++) {
				PlayerGroup* pg = getCoTuongScreen()->GetPlayerGroupAtIdx(i);
				if (pg->getPlayer()->IDDB == getCoTuongScreen()->GetOwnerID())
				{
					getCoTuongScreen()->setTimeQuota(pg->getPlayer()->IDDB, gameRemainingTime);
				}
				else
				{
					getCoTuongScreen()->setTimeQuota(pg->getPlayer()->IDDB, gameRemainingTimeOpp);
				}


				getCoTuongScreen()->onReconnect(moveFirst, ownerUserId, currUserID, limitStepTime, intervalView, board);
			}
			break;
		}
		case IwinProtocol::ERROR_MESSAGE:
		{
			std::string message = msg->readString();
			Utils::showDialogNotice(message);
			
		}
		break;
	}
}
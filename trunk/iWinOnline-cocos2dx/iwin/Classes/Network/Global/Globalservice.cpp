#include "Globalservice.h"
#include "Network/IwinCommand.h"
#include "Network/IwinProtocol.h"
#include "Common/GameController.h"


#include "Network/JsonObject/JsonHeader.h"
#include "Network/JsonObject/Quest/QuestCategory.h"
#include "Network/JsonObject/cashoutruby/RequestHistoryCharge.h"
#include "Network/JsonObject/cashoutruby/CancelGetCard.h"
#include "Network/JsonObject/LuckyCircle/NewCircleInfoRequest.h"
#include "Network/JsonObject/LuckyCircle/NewCircleTurnRequest.h"

#include "Network/JsonObject/Facebook/FacebookIdsList.h"

#include "RKBaseLog/Debug.h"

#include "MainGame.h"
#include "../JsonObject/login/LoginWithToken.h"
#include "../Socket/SocketConnect.h"
#include "network/HttpClient.h"
USING_NS_CC;
using namespace std;
using namespace iwinmesage;
using namespace RKUtils;
using namespace Platform;
bool GlobalService::isReconnect = false;
int maxLenghtJSON = 63000;

MobileClient* GlobalService::_coreservice_websocket = NULL;
SocketConnect* GlobalService::_coreservice_socket = NULL;
std::function<void(void)> GlobalService::actionConnectOk = nullptr;
std::string GlobalService::GS_TAG = "Globalservice";

void GlobalService::sendJSONString(JsonMessage* jsonMsg)
{
	PASSERT2(jsonMsg != nullptr, "jsonMsg invaild!");
	sendString(jsonMsg->toJson());
	CC_SAFE_DELETE(jsonMsg);
}

void GlobalService::setClientLanguage(std::string lang)
{
	iwincore::Message* msg = new iwincore::Message(IwinProtocol::SET_SESSION_INFOR);
	msg->putByte(1);
	msg->putString(lang);
	sendMessage(msg);
}

void GlobalService::onSendClientInfo()
{    

    ClientInforObject clientInfo;
    AppInfo* appInfo = APPINFO;
    DeviceInfo* deviceInfo = DEVICEINFO;
    clientInfo.setName(deviceInfo->getDeviceName());
    clientInfo.setOS(deviceInfo->getOS());
    clientInfo.setPlatform(deviceInfo->getOS());
    clientInfo.setOSVersion(deviceInfo->getOSVersion());
    clientInfo.setRooted(deviceInfo->getIsJailBreak() == 1 ? true : false);
    clientInfo.setAppVersion(appInfo->getVersion());
    clientInfo.setBundleID(appInfo->getBundleID());
    clientInfo.setHeight(appInfo->getAppHeight());
    clientInfo.setWidth(appInfo->getAppWidth());
    clientInfo.setCarrier(deviceInfo->getCarrier());
    clientInfo.setLang(appInfo->getLanguage());
    clientInfo.setNetwork(deviceInfo->getNet());
    clientInfo.setChannel(appInfo->getChanel());
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    clientInfo.setAdvertisingID(deviceInfo->getAdvertisingID());
    clientInfo.setMacAddress(deviceInfo->getMacAddress());
    clientInfo.setAndroidID(deviceInfo->getAndroidID());
    clientInfo.setSimSerial(deviceInfo->getSimSeri());
    clientInfo.setFingerPrint(deviceInfo->getFingerPrint());
    clientInfo.setSubscriberID(deviceInfo->getSubscriberID());
    clientInfo.setBluetoothAddress(deviceInfo->getBluetoothAddress());
    clientInfo.setCheckSum(Platform::makeSHA1Hash(clientInfo.toString()));
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    clientInfo.setUDID(deviceInfo->getDeviceUDID());
    clientInfo.setAdvertisingID(deviceInfo->getAdvertisingID());
    clientInfo.setCheckSum(Platform::makeSHA1Hash(clientInfo.toString()));
#else
    clientInfo.setMacAddress(deviceInfo->getMacAddress());
    clientInfo.setCheckSum(Platform::makeSHA1Hash(clientInfo.toString()));
#endif
    
    CCLOG("GlobalService::onSendClientInfo --> %s", clientInfo.toString().c_str());
	JsonMessage* jsonMsg = new JsonMessage(IwinCommand::SET_CLIENT_DEVICE_INFO);
	jsonMsg->setData(clientInfo.toJson());
	sendJSONString(jsonMsg);
}
void GlobalService::sendString(string data)
{
#if USING_WSOCKET
	if (_coreservice_websocket == NULL || !_coreservice_websocket->isConnect)
	{
		return;
	}
#else
	if (_coreservice_socket == NULL || !_coreservice_socket->getConnected())
	{
		return;
	}
#endif
	iwincore::Message* msg = new iwincore::Message(2800);
	msg->putString(data);
	sendMessage(msg);
	CCLOG("send message %s", data.c_str());

}
void GlobalService::sendString(s16 comand, std::string data)
{
	iwincore::Message* msg = new iwincore::Message(comand);
	msg->putString(data);
	sendMessage(msg);
}

void GlobalService::sendMessage(iwincore::Message* msg)
{
#if USING_WSOCKET
	if (_coreservice_websocket == NULL || !_coreservice_websocket->isConnect)
	{
		return;
	}
	_coreservice_websocket->sendMessage(msg);
#else
	if (_coreservice_socket == NULL || !_coreservice_socket->getConnected())
	{
		return;
	}
	_coreservice_socket->sendMessage(msg);
#endif
	//sendMessage(msg);
}

void GlobalService::CloseSocket()
{
#if USING_WSOCKET
	if (_coreservice_websocket)
	{
		CCASSERT(_coreservice_websocket != nullptr, "_coreservice is null");
		return;
	}
	_coreservice_websocket->CloseSocket();
	delete _coreservice_websocket;
	_coreservice_websocket = nullptr;
#else
	_coreservice_socket->onClose();
	/*delete _coreservice_socket;
	_coreservice_socket = nullptr;*/
#endif
}

void GlobalService::connect(const char* ip, const char* port, const std::function<void()> & result_callback)
{
#if USING_WSOCKET
	if (!_coreservice_websocket)
	{
		delete _coreservice_websocket;
		_coreservice_websocket = nullptr;
	}
	_coreservice_websocket = new MobileClient();
	_coreservice_websocket->init(ip, port);
	
#else
	if (!_coreservice_socket)
	{
		/*_coreservice_socket->onClose();
		delete _coreservice_socket;
		_coreservice_socket = nullptr;*/
		_coreservice_socket = new SocketConnect();
	}
	//unsigned short p;
	int p = atoi(port);
	std::stringstream ss(port);
	_coreservice_socket->connect(ip, (unsigned short)p);
	
#endif
	actionConnectOk = result_callback;
}
void GlobalService::sendSingleCommand(s16 command)
{
	iwincore::Message* msg = new iwincore::Message(command);
	sendMessage(msg);
}
void GlobalService::login(std::string username, std::string pass)
{
	iwincore::Message* msg = new iwincore::Message(IwinProtocol::LOGIN);
	msg->putString(username);
	msg->putString(pass);
	msg->putString("5.1.0");
	sendMessage(msg);
}

void GlobalService::getServerListFromWeb(const std::function<void(void * data_call_back)> & action_done, const std::function<void(void)> & action_fail)
{
	network::HttpRequest* request = new (std::nothrow) network::HttpRequest();
	request->setUrl("https://iwin.me/app/svr.txt");
	request->setRequestType(network::HttpRequest::Type::GET);
	const network::ccHttpRequestCallback& call_back = ([action_done, action_fail](network::HttpClient* pSender, network::HttpResponse* pResponse)
	{
		network::HttpResponse* response = pResponse;
		if (!response)
		{
			if (action_fail)
			{
				action_fail();
			}
			//SocialMgr->SetStateDownload(D_FAIL);
			cocos2d::log("No Response");
		}
		else
		{
			int statusCode = (int)response->getResponseCode();

			char statusString[64] = {};
			sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
			cocos2d::log("response code: %d", statusCode);

			if (!response->isSucceed())
			{
				if (action_fail)
				{
					action_fail();
				}
				//SocialMgr->SetStateDownload(D_FAIL);
				cocos2d::log("response failed");
				cocos2d::log("error buffer: %s", response->getErrorBuffer());
			}
			else
			{
				std::vector<char>*buffer = response->getResponseData();

				if (action_done)
				{
					action_done((void*)buffer);
				}
			}
		}
	});
	request->setResponseCallback(call_back);
	network::HttpClient::getInstance()->send(request);
	request->release();
}

void GlobalService::loginWithFacebookToken(std::string type, std::string token, std::string version)
{
   iwincore::Message* msg = new iwincore::Message(IwinProtocol::LOGIN_WITH_ACCESS_TOKEN);
//    msg->putString(type);
    msg->putString(token);
    msg->putString(version);
    sendMessage(msg);
}

void GlobalService::loginWithTokenJson(std::string type, std::string token, std::string version)
{
	LoginWithToken msgToken;
	msgToken.setToken(token);
	msgToken.setType(type);
	msgToken.setVersion(version);
	JsonMessage* msg = new JsonMessage(IwinCommand::LOGIN_WITH_TOKEN);
	msg->setData(msgToken.toJson());
	sendJSONString(msg);
}

/**
* Lay pham vi tien cuoc ma user co the dat.
*/
void GlobalService::getBetRange() {
	JsonMessage* msg = new JsonMessage(IwinCommand::GET_MIN_MAX_BOARD);
	sendJSONString(msg);

}

/**
* This function send only command ID.
*
* @param commandId
*/
void GlobalService::sendSingleCommand(int commandId) {
	iwincore::Message* m = new iwincore::Message(commandId);
	sendMessage(m);
}

void GlobalService::requestRoomList() {
	sendSingleCommand(IwinProtocol::ROOM_LIST);
}

void GlobalService::joinAnyBoard() {
	sendSingleCommand((GameController::gameId != GameType::GAMEID_POKER
		&&GameController::gameId != GameType::GAMEID_POKER_HK
		&&GameController::gameId != GameType::GAMEID_CO_UP
		&&GameController::gameId != GameType::GAMEID_GIAI_DAU_CO_UP
		&&GameController::gameId != GameType::GAMEID_CHESS &&
		GameController::gameId != GameType::GAMEID_LIENG) ? IwinProtocol::JOIN_ANY_BOARD
		: IwinProtocol::AUTO_JOIN_VIEWER);
}

void GlobalService::chatToBoard(ubyte roomID, ubyte boardID, std::string text) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::ON_CHAT_BOARD);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putString(text);
	sendMessage(m);
}

void GlobalService::joinBoard(ubyte roomID, ubyte boardID, std::string pass) {
	iwincore::Message* m = new iwincore::Message(
		(GameController::gameId != GameType::GAMEID_CHESS
			&&GameController::gameId != GameType::GAMEID_CO_UP
			&&GameController::gameId != GameType::GAMEID_GIAI_DAU_CO_UP
			&&GameController::gameId != GameType::GAMEID_POKER
			&&GameController::gameId != GameType::GAMEID_POKER_HK &&
			GameController::gameId != GameType::GAMEID_LIENG) ? IwinProtocol::JOIN_BOARD
		: IwinProtocol::JOIN_VIEWER);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putString(pass);
	sendMessage(m);

}

/**
* dung khi dang ngoi xem chuyen sang ngoi choi (cho cac game co cho ngoi
* xem) co the xai nhu binh thuong co cac game khong co ngoi xem
*
* @param roomID
* @param boardID
* @param pass
*/
void GlobalService::joinBoard_Normal(ubyte roomID, ubyte boardID, std::string pass) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::JOIN_BOARD);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putString(pass);
	sendMessage(m);
}

void GlobalService::leaveBoard(ubyte roomID, ubyte boardID) {
	CCLOG("leaveBoard");
	iwincore::Message* m = new iwincore::Message(IwinProtocol::LEAVE_BOARD);
	m->putByte(roomID);
	m->putByte(boardID);
	sendMessage(m);
}


void GlobalService::ready(ubyte roomID, ubyte boardID, bool isReady) {
	CCLOG(GS_TAG.c_str(), "ready");
	iwincore::Message* m = new iwincore::Message(IwinProtocol::READY);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(isReady);
	sendMessage(m);
}
void GlobalService::quickPlay(ubyte roomID, ubyte boardID) {
	CCLOG(GS_TAG.c_str(), "QuickPlay");
	iwincore::Message* m = new iwincore::Message(IwinProtocol::QUICK_PLAY);
	m->putByte(roomID);
	m->putByte(boardID);
	//m->putInt(userId);
	sendMessage(m);
}

void GlobalService::confirmLeaveGame(ubyte roomID, ubyte boardID, ubyte isConfirm)
{
	iwincore::Message * m = new iwincore::Message(IwinProtocol::CONFIRM_LEAVE_GAME);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(isConfirm);
	sendMessage(m);
}

/**
* Set money trong cac game ma tien cuoc chung.
*
* @param roomID
* @param boardID
* @param money
*/
void GlobalService::setMoney(ubyte roomID, ubyte boardID, int money) {
	// System.out.println("Set money");
	CCLOG("Game Screen", "Set money");
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SET_MONEY);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putInt(money);
	sendMessage(m);
}

/**
* Cai dat password cho ban choi.
*
* @param roomID
* @param boardID
* @param pass
*/
void GlobalService::setPassword(ubyte roomID, ubyte boardID, std::string pass) {
	CCLOG("Set password %s", pass.c_str());
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SET_PASS);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putString(pass);
	sendMessage(m);
}
void GlobalService::requestProfileOf(int id) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::GET_PROFILE);
	m->putInt(id);
	sendMessage(m);
}

void GlobalService::getAllUserAchievement(int id_user)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::USER_DATA);
	m->putByte(6);
	m->putInt(id_user);
	sendMessage(m);
}

void GlobalService::getUserAchievementOfGameID(int id_user, int game_id)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::USER_DATA);
	m->putByte(6);
	m->putByte(game_id);
	m->putInt(id_user);
	sendMessage(m);
}


void GlobalService::UpdateStatus(std::string status)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::UPDATE_STATUS);
	m->putString(status);
	sendMessage(m);
}

void GlobalService::UpdateProfile(User * user)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::UPDATE_PROFILE);

	m->putString(user->fullname);
	m->putByte(user->gender);
	m->putShort(user->yearOfBirth);
	m->putString(user->address);
	m->putString(user->idnumber);
	m->putString(user->phoneNo);
	m->putByte(user->province);
	m->putByte(user->monthOfBirth);
	m->putByte(user->dayOfBirth);
	sendMessage(m);
}
/**
* Kick player trong ban choi.
*
* @param roomID
* @param boardID
* @param kickID
*/
void GlobalService::kick(ubyte roomID, ubyte boardID, int kickID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::KICK);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putInt(kickID);
	sendMessage(m);
}

void GlobalService::startGame(ubyte roomID, ubyte boardID) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::START);
	m->putByte(roomID);
	m->putByte(boardID);
	sendMessage(m);
}

/**
* Dung cho chuc nang moi. khi start game them config time limit
*
* @param roomID
*            current roomID
* @param boardID
*            current boardID
* @param timeLimit
*            timeSetup from board owner
*/
void GlobalService::startGame(ubyte roomID, ubyte boardID, ubyte timeLimit) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::START);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(timeLimit);
	sendMessage(m);
}

void GlobalService::requestBoardList(ubyte id) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::BOARD_LIST);
	m->putByte(id);
	sendMessage(m);
}

/**
* Thuc hien mo vat pham su kien.
*
* @param itemID
*            :ID cua item muon open.
*/
void GlobalService::openEventItem(int itemID) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::ITEMS);
	m->putByte(17);
	m->putInt(itemID);
	sendMessage(m);
}

/**
* send command request a player join board.
*/
void GlobalService::requestInvite(int userID) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::INVITE_PLAYER);
	m->putInt(userID);
	sendMessage(m);
	// m.cleanup();
}

/**
* Yeu cau server tra ve mot danh sach loai type.
*
* @param type
*            : Loai muon tra 1 : Danh sach TOP/RANK. 2 : Danh sach phong
*            cho 3 : Danh sách tỉnh thành.
*/
void GlobalService::getList(int type) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::GET_LIST);
	m->putByte(type);
	sendMessage(m);
	// m.cleanup();
}

void GlobalService::requestQuestList() {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::QUEST);
	m->putByte(0);
	sendMessage(m);
}

void GlobalService::acceptCancelQuest(int questId, bool isCancel) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::QUEST);
	m->putByte(isCancel ? 2 : 1);
	m->putInt(questId);
	sendMessage(m);
}

void GlobalService::giveItem(ubyte roomid, ubyte controllerid, int itemID,
	std::vector<int> toUserIDs) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::ITEMS);
	m->putByte(IwinProtocol::ITEMS_GIVE_ITEM);
	m->putByte(roomid);
	m->putByte(controllerid);
	m->putInt(itemID);
	for (size_t i = 0; i < toUserIDs.size(); i++) {
		if (toUserIDs[i] >= 0) {
			m->putInt(toUserIDs[i]);
		}
	}

	sendMessage(m);
}

void GlobalService::requestListGift(int categoryId, ubyte page) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::ITEMS);
	m->putByte(15);
	m->putInt(categoryId);
	m->putByte(page);
	sendMessage(m);
}

void GlobalService::buzz() {
	sendSingleCommand(IwinProtocol::BUZZ);
}

void GlobalService::requestBgImage(int id) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::GET_IMAGE);
	m->putByte(5); // Item Image
	m->putInt(id);
	sendMessage(m);
}

/**
* * Yêu cầu server xem thông tin của một bàn chơi.
*
* @param boardID
*            - ID của bàn chơi muốn xem.
*/
void GlobalService::viewBoardInfoOf(ubyte roomID, ubyte boardID) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::BOARD_INFO);
	m->putByte(roomID);
	m->putByte(boardID);
	sendMessage(m);
}

/**
* Game lotto
*/
void GlobalService::sendBuyLotto(int number, int money) {
	iwincore::Message*  m = new iwincore::Message(1403);
	m->putInt(number);
	m->putInt(money);
	sendMessage(m);
}

void GlobalService::getGift(int categoryId, ubyte gender, ubyte type) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::ITEMS);
	m->putByte(IwinProtocol::ITEMS_LIST_CATEGORIES);
	m->putInt(categoryId);
	m->putByte(gender);
	m->putByte(type);
	sendMessage(m);
}

/**
* tu choi loi moi khi vao game
*/
void GlobalService::sendDeny_Invited(int userID) {
	iwincore::Message*  m = new iwincore::Message(1302);
	m->putInt(userID);
	sendMessage(m);
}

/**
* Lấy 1 danh sách các user đang nằm trong phòng ch�? có tên đúng và gần
* đúng với tên truy�?n vào.
*
* @param username
*/
void GlobalService::searchInWaittingList(std::string username) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::SEARCH_WAITING_LIST);
	m->putString(username);
	sendMessage(m);
}

/**
* Set max player in any game with roomID and boardID
*
* @param roomID
* @param boardID
* @param max
*/
void GlobalService::setMaxPlayer(ubyte roomID, ubyte boardID, int max) {
	CCLOG("Game Screen", "Set max player: " + max);
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::SET_MAX_PLAYER);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(max);
	sendMessage(m);
}

/**
* commnad cho game bau cua
*
* @param command
*            WANT_DRAW số 24 dùng cho ti�?n cược : byte sub : + 0 : đặt
*            cược [ int item, int money ] (money cuối cùng sau khi nhân lần
*            cược) + 1: hủy cược [ int item]
* @param roomID
* @param boardID
* @param subCommand
*            0 la bet 1 la delbet 2 setbet
* @param itemID
*            = -1 la ko gui byte nay len
* @param money
*            = -1 la ko gui byte nay len
*/
void GlobalService::betBauCua(s16 command, ubyte roomID, ubyte boardID,
	ubyte subCommand, int itemID, int money) {
	iwincore::Message*  m = new iwincore::Message(command);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(subCommand);
	if (itemID != -1) {
		m->putInt(itemID);
	}
	if (money != -1) {
		m->putInt(money);
	}
	sendMessage(m);
}

/**
* phuong thuc gui msg don gian cho cac game dung chung
*
* @param command
* @param roomID
* @param boardID
* @param mo
*/
void GlobalService::setCommandinGametoServer(s16 command, ubyte roomID,
	ubyte boardID, int mo) {
	iwincore::Message*  m = new iwincore::Message(command);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putInt(mo);
	sendMessage(m);
}

/**
* command lat bai cho game Bai Cao
*
* @param roomID
*            id cua phong
* @param boardID
*            id cua ban
* @param cards
*            mang card
*/
void GlobalService::latBaiBaiCao(ubyte roomID, ubyte boardID, std::vector<char> cardsID) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::MOVE);
	m->putByte(roomID);
	m->putByte(boardID);
	for (int i = 0; i < 3; i++) {
		m->putByte(cardsID[i]);
	}
	sendMessage(m);
}

void GlobalService::requestWaitingListInGame() {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::GET_WAITING_LIST);
	sendMessage(m);
}

void GlobalService::playerToViewer(ubyte roomID, ubyte boardID) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::PLAYER_TO_VIEWER);
	m->putByte(roomID);
	m->putByte(boardID);
	sendMessage(m);
}

/**
* xin thua hoac xin hoa trong game co
*
* @param command
*            gui len server
* @param roomID
*            id cua phong
* @param boardID
*            id cua ban
* @param accept
*            yeu cau gui len true la chap nhan nguoc lai la false
*/
void GlobalService::requestAndAnswerLoseAndDraw(s16 command, ubyte roomID,
	ubyte boardID, bool accept) {
	iwincore::Message*  m = new iwincore::Message(command);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(accept);
	sendMessage(m);
}

/**
* dung cho caro va co vua
*
* @param roomID
* @param boardID
* @param x
* @param y
*/
void GlobalService::move(ubyte roomID, ubyte boardID, ubyte x, ubyte y) {
	iwincore::Message*  m = new iwincore::Message(IwinProtocol::MOVE);
	m->putByte(roomID);
	m->putByte(boardID);
	m->putByte(x);
	m->putByte(y);
	sendMessage(m);
}

/**
* chi gui command va room board
*
* @param command
*            gui command single
* @param roomID
*            id cua phong
* @param boardID
*            id cua ban
*/
void GlobalService::requestSimpleCommand(s16 command, ubyte roomID,
	ubyte boardID) {
	iwincore::Message*  m = new iwincore::Message(command);
	m->putByte(roomID);
	m->putByte(boardID);
	sendMessage(m);
}

void GlobalService::getInfo(int subId) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::GET_INFO);
	m->putByte(subId);
	sendMessage(m);
}

void GlobalService::sendHeartBeat() {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::HEART_BEAT);
	sendMessage(m);
}

void GlobalService::getHeartBeatConfiguration() {
	getInfo(23);
}

void GlobalService::getNewGameList()
{
	JsonMessage* jsonMsg = new JsonMessage("GET_GAME_LIST");
	jsonMsg->setData(rapidjson::Document());
	sendJSONString(jsonMsg);
	//dont need this, delete in sendmessage
	//CC_SAFE_DELETE(jsonMsg);
}

void GlobalService::getCountQuestList()
{
	JsonMessage* jsonMsg = new JsonMessage("COUNT_QUEST_LIST");
	jsonMsg->setData(rapidjson::Document());
	sendJSONString(jsonMsg);

	//dont need this, delete in sendmessage
	//CC_SAFE_DELETE(jsonMsg);
}

/**
 * gửi danh sách facebook id đã mời lên.
 *
 * @param ids
 */
//public static void sendInviteFacebookList(final ArrayList<String> ids) {
//    Platform.instance.getFacebookServices().getFacebookMyInfo(new IActionListener() {
//        
//        @Override
//        public void actionPerformed(Object arg0) {
//            if (arg0 != null) {
//                FacebookMyInfo facebookMyInfo = (FacebookMyInfo) arg0;
//                FacebookIdsList d = new FacebookIdsList();
//                d.ids = ids;
//                d.myId = facebookMyInfo.id;
//                d.ac = Platform.instance.getFacebookServices().getAccessToken();
//                JsonMessage m = new JsonMessage(IWINCommand.INVITE_FACEBOOK, d);
//                sendJSONString(m.toJson());
//            }
//        }
//    });
//}

void GlobalService::sendInviteFacebookList(std::vector<std::string> fbFriendListId)
{
    Platform::FBUtils::getFacebookMyInfo([fbFriendListId](bool res, void* data){
        FacebookMyInfo myInfo = *static_cast<FacebookMyInfo*>(data);
        
        FacebookIdsList fbIDlist;
        fbIDlist.setMyId(myInfo.getId());
        fbIDlist.setAc(Platform::FBUtils::getAccessToken());
        fbIDlist.setIds(fbFriendListId);
        
        JsonMessage* jsonMsg = new JsonMessage(IwinCommand::INVITE_FACEBOOK);
        jsonMsg->setData(fbIDlist.toJson());
        sendJSONString(jsonMsg);
    });
}


/**
 *
 * @param accessToken
 */
bool isRequestingFriendFacebook = false;
void GlobalService::requestFriendFromFacebook(std::string accessToken, int page) {
    if (isRequestingFriendFacebook) {
        return;
    }
    RequestFriendFromFacebook  object;
    object.setAccessToken(accessToken);
    object.setPage(page);
    JsonMessage * msg = new JsonMessage(IwinCommand::REQUEST_FRIEND_FROM_FACEBOOK);
    msg->setData(object.toJson());
    
    isRequestingFriendFacebook = true;
    CCLOG("REQUEST_FRIEND_FROM_FACEBOOK %s", msg->toJson().c_str());
    
    sendJSONString(msg);
}

std::string jsonString_Request_Contact_Friends = "";
bool isRequestingFriendContact = false;
void GlobalService::requestFriendFromContact(std::vector<Contact*> contacts)
{
    if (isRequestingFriendContact)
    {
        return;
    }
    JsonMessage * msg = new JsonMessage(IwinCommand::REQUEST_FRIEND_FROM_CONTACT);
    if (jsonString_Request_Contact_Friends.length() == 0)
    {
        if (contacts.size() > 0)
        {
            RequestFriendFromContact object;
            //vn.me.engine.platform.Contact clientContact;
            
            int contactsSize = contacts.size();
            for (int i = 0; i < contactsSize; i++)
            {
                auto clientContact = contacts[i];
                iwinmesage::Contact  jsonContact;
                std::vector<std::string> phones ;
                if (clientContact != nullptr && clientContact->getPhones().size() > 0)
                {
                    for (int a = 0; a < clientContact->getPhones().size(); a++)
                    {
                        phones.push_back(clientContact->getPhones()[a]);
                    }
                }
                if(phones.size() > 0)
                {
                    jsonContact.setPhones(phones);
                }
                ((&object)->getContacts()).push_back(jsonContact);
            }
            msg->setData(object.toJson());
            
            while (true)
            {
                int len = msg->toJson().length();
                if (len > maxLenghtJSON)
                {
                    ((&object)->getContacts()).clear();
                    int lenghtPerOne = len / contactsSize;
                    int size = (int) (((double) maxLenghtJSON / (double) lenghtPerOne) * 0.85 + 0.5);
                    
                    for (int i = 0; i < size; i++)
                    {
                        auto clientContact = contacts[i];
                        iwinmesage::Contact  jsonContact;
                        std::vector<std::string> phones ;
                        if (clientContact != nullptr && clientContact->getPhones().size() > 0)
                        {
                            for (int a = 0; a < clientContact->getPhones().size(); a++)
                            {
                                phones.push_back(clientContact->getPhones()[a]);
                            }
                        }
                        if(phones.size() > 0)
                        {
                            jsonContact.setPhones(phones);
                        }
                        ((&object)->getContacts()).push_back(jsonContact);
                    }
                    msg->setData(object.toJson());
                }
                else
                {
                    break;
                }
            }
        }
        jsonString_Request_Contact_Friends = msg->toJson();
    }
    isRequestingFriendContact = true;
    
    CCLOG("REQUEST_FRIEND_FROM_CONTACT : lenght = %d , %s",  (int)msg->toJson().length(), msg->toJson().c_str());
    sendJSONString(msg);
}

/**
 *
 * @param location
 * @param page
 *            Bat dau tu 0
 */
bool isRequestingFriendLocation = false;
void GlobalService::requestFriendFromLocation(Platform::GPSLocation * location, int page)
{
    if (isRequestingFriendLocation)
    {
        return;
    }
    RequestFriendFromLocation  object;
    Location loca;
    loca.setLatitude(location->latitude);
    loca.setLongitude(location->longitude);
    object.setLocation(loca);
    object.setPage(page);
    JsonMessage * msg = new JsonMessage(IwinCommand::REQUEST_FRIEND_FROM_LOCATION);
    msg->setData(object.toJson());
    
    isRequestingFriendLocation = true;
    CCLOG("REQUEST_FRIEND_FROM_LOCATION",
          "[longitude : %f , latitude : %f ] %s", location->longitude , location->latitude , msg->toJson().c_str());
    
    sendJSONString(msg);

}


/**
 * Check email khi tao nick.
 *
 * @param userName
 */
void GlobalService::checkEmail(std::string email) {
    CCLOG("Global Service", "checkEmail %s" , email.c_str());
    iwincore::Message *  m = new iwincore::Message(IwinProtocol::CHECK_VALIDATE_EMAIL);
    m->putString(email);
    sendMessage(m);
}

/**
 *
 * @param userName
 * @param pass
 */
void GlobalService::checkPass(std::string userName, std::string pass) {
    CCLOG("Global Service", "checkPass: %s , %s" ,userName.c_str() ,pass.c_str());
    iwincore::Message * m = new iwincore::Message(IwinProtocol::VALID_PASS);
    m->putString(userName);
    m->putString(pass);
    sendMessage(m);
}

/***
 *
 * @param requestId
 * @param accessToken
 * @param listFbIdRequest
 */
void GlobalService::requestSendGift(std::string requestId, std::string accessToken, std::vector<std::string> listFbIdRequest) {
    RequestFacebookRequest object ;
    object.setAccessToken(accessToken);
    object.setListFbIdRequest(listFbIdRequest);
    object.setRequestId(requestId);
    JsonMessage * msg = new JsonMessage(IwinCommand::FACEBOOK_REQUEST);
    msg->setData(object.toJson());
    sendJSONString(msg);
}

/**
 * Clear facebook request theo type.
 *
 * @param type
 *            Giong nhu cac type cua class FacebookRequest
 */
void GlobalService::clearFacebookRequest(int type) {
    RequestFacebookRequest object ;
    object.setType(type);
    JsonMessage * msg = new JsonMessage(IwinCommand::FACEBOOK_REQUEST);
    msg->setData(object.toJson());
    sendJSONString(msg);
}

/**
 * Lay danh
 */
void GlobalService::getMyActivity() {
    JsonMessage * msg = new JsonMessage(IwinCommand::FACEBOOK_REQUEST);
    sendJSONString(msg);
}

/**
 *
 * @param ids
 * @param facebookID
 */
void GlobalService::openFacebookGift(std::vector<long long> ids, std::string facebookID) {
    OpenFacebookRequest object;
    object.setIds(ids);
    object.setFbId(facebookID);
    JsonMessage * msg = new JsonMessage(IwinCommand::OPEN_FACEBOOK_REQUEST);
    msg->setData(object.toJson());
    sendJSONString(msg);
}

// feature question and answer when login successfull
void GlobalService::sendAnswerToServer(int commandId, int questionId, std::string answer) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SEND_ANSWER_TO_SERVER);
	// m.putInt(commandId);
	m->putInt(questionId);
	m->putString(answer);
	sendMessage(m);
}

void GlobalService::sendIdQuestionToServer(int commandId, int questionId) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SEND_ID_QUESTION_TO_SERVER);
	// m.putInt(commandId);
	m->putInt(questionId);
	sendMessage(m);
}

void GlobalService::sendRequestToGetQuestion() {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::RESPOND_QUESTION_FROM_SERVER);
	// m.putInt(commandId);
	sendMessage(m);
}

/**
 * Lay danh sach quest.
 */
// public static void requestQuestList() {
//
// GetQuestList object = new GetQuestList();
// object.setContactStatus(Platform.instance.getContactManager()
// .isAllowOpenContact());
// object.setNotificationStatus(Platform.instance
// .isEnablePushNotification());
//
// JsonMessage msg = new JsonMessage(IWINCommand.GET_QUEST_LIST);
// msg.setData(object);
//
// sendJSONString(msg.toJson());
// }

/**
 * Submit 1 list quest.
 */
void GlobalService::requestSubmitQuest(RequestQuest* object)
{
    // Log

    JsonMessage * msg = new JsonMessage(IwinCommand::SUBMIT_QUEST);
    msg->setData(object->toJson());
    sendJSONString(msg);
}

/**
 * Accept 1 list quest.
 */
void GlobalService::requestAcceptQuest(RequestQuest* object)
{
    // Log

    // End log

    JsonMessage * msg = new JsonMessage(IwinCommand::ACCEPT_QUEST);
    msg->setData(object->toJson());
    sendJSONString(msg);
}

/**
 * Nhan duoc user list (trong userlist co access token).
 *
 * @param code
 *            String get from sdklib.
 */
void GlobalService::sdklibListUserByCode(std::string code)
{
//    CCLOG("Global Service", "sdklibListUserByCode: %s" ,code.c_str());
//    SdklibData o ;
//    o.setCode(code);
//    JsonMessage * m = new JsonMessage(IwinCommand::LIST_USER_BY_CODE);
//    m->setData(o.toJson());
//    sendJSONString(m);
}

/**
 * chi nhan duoc user list
 */
void GlobalService::sdklibListUserByAccessToken(std::string token) {
//    SdklibData o ;
//    o.setAccessToken(token);
//    JsonMessage * m = new JsonMessage(IwinCommand::LIST_USER_BY_ACCESS_TOKEN);
//     m->setData(o.toJson());
//    sendJSONString(m);
}

/**
 * Login voi facebook account.
 *
 * @param token
 * @param username
 */
void GlobalService::loginBySdklibAccount(std::string token, std::string username)
{
//    SdklibData o;
//    o.setAccessToken(token);
//    o.setUsernameIwin(username);
//    JsonMessage * m = new JsonMessage(IwinCommand::LOGIN_BY_SDKLIB_ACCOUNT, o);
//     m->setData(o.toJson());
//    sendJSONString(m);
}

/**
 *
 * @param usernameIwin
 * @param token
 */
void GlobalService::createIWinAccFrom(std::string usernameIwin, std::string token) {
//    SdklibData data ;
//    data.setUsernameIwin(usernameIwin);
//    data.setAccessToken(token);
//    JsonMessage * m = new JsonMessage(IwinCommand::CREATE_USER_BY_SDKLIB_ACCOUNT);
//     m->setData(data.toJson());
//    sendJSONString(m);
}

/**
* lấy danh sách bàn chơi (chủ bàn)
*
* @param betMoney
*/
void GlobalService::requestOwnerBoardList(int betMoney) {
	//Gdx.app.log(TAG, "requestOwnerBoardList: " + betMoney);
	GetOwnerBoardListRequest request;
	request.setWin(betMoney);
	JsonMessage* jsonMsg = new JsonMessage("GET_OWNER_BOARD_LIST");
	jsonMsg->setData(request.toJson());
	sendJSONString(jsonMsg);
}

void GlobalService::setGameType(int gameType, std::function<void()> _onSetGameTypeOK)
{
	GlobalLogicHandler::onSetGameTypeOk = _onSetGameTypeOK;
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SET_GAME_TYPE);
	m->putByte(gameType);
	sendMessage(m);
}

void GlobalService::requestRegister(std::string email, std::string nick) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::NEW_REGISTER);
    m->putString(nick);
	m->putString(email);
	sendMessage(m);
}

void GlobalService::requestCreateBoard(int subCommand, int money, ubyte numPlayer, std::string password)
{
	iwincore::Message* m = new iwincore::Message(
		GameController::getCurrentGameType() == GameType::GAMEID_LIENG ? 2706
		: 2702);
	if (GameController::getCurrentGameType() == GameType::GAMEID_LIENG) {
		m->putInt(money);

	}
	else {
		m->putInt(subCommand);

	}
	m->putInt(money);
	m->putByte(numPlayer);
	m->putString(password);

	sendMessage(m);
	//dont need this, delete in sendmessage
	//CC_SAFE_DELETE(m);
}

void GlobalService::getSMSInfoForgotPassword(std::string username)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::GET_SMS_SYNTAX);
	m->putByte(1);
	m->putString(username);
	sendMessage(m);
}

void GlobalService::CheckUserName(std::string user_name)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::CHECK_USERNAME);
	m->putString(user_name);
	sendMessage(m);
}

void GlobalService::CheckEmail(std::string email)
{
    iwincore::Message* m = new iwincore::Message(IwinProtocol::CHECK_VALIDATE_EMAIL);
    m->putString(email);
    sendMessage(m);
}

void GlobalService::GetQuestCategoryList()
{
	JsonMessage * m = new JsonMessage(IwinCommand::GET_QUEST_CATEGORY_LIST);
	sendJSONString(m);
}

void GlobalService::GetQuestByCategoryID(int id)
{
	QuestCategory c ;
	c.setId(id);
	c.setImgUrl("");
	c.setItemNo(0);
	c.setName("");
	JsonMessage * m = new JsonMessage(IwinCommand::GET_QUESTS_BY_CATEGORY_ID);
	m->setData(c.toJson());
	sendJSONString(m);

//	delete c;
//	delete m;


}

//void GlobalService::SendJSONString(std::string json_str)
//{
//	CCLOG("send JSON Msg %s", json_str.c_str());
//	iwincore::Message* m = new iwincore::Message(2800);
//	m->putString(json_str);
//	sendMessage(m);
//}

/**
* Lấy thông tin captcha lần đầu tiên
*/
void GlobalService::getCaptchaFirstTime() {
/*	CaptchaInfo captchaInfo = new CaptchaInfo();
	captchaInfo = null;
	JsonMessage m = new JsonMessage(IWINCommand.GET_CAPTCHA, captchaInfo);
	sendJSONString(m.toJson());*/
	JsonMessage* jsonMsg = new JsonMessage(IwinCommand::GET_CAPTCHA);
	sendJSONString(jsonMsg);
}

/**
* Lấy thông tin khi refresh captcha
* @param id : id của lần gọi trước đó
*/
void GlobalService::getCaptchaRefresh(std::string id) {
	CaptchaInfo captchaInfo;
	captchaInfo.setId(id);
	JsonMessage* jsonMsg = new JsonMessage(IwinCommand::GET_CAPTCHA);
	jsonMsg->setData(captchaInfo.toJson());
	sendJSONString(jsonMsg);
}

/**
* Gửi thông tin kiểm tra Captcha
* @param id : Id của captcha trong CaptchaInfo
* @param answer : câu trả lời ứng với Image
*/
void GlobalService::sendCheckCaptcha(std::string id, std::string answer) {
	CaptchaAnswer captchaAnswer;
	captchaAnswer.setId(id);
	captchaAnswer.setAnswer(answer);
	JsonMessage* jsonMsg = new JsonMessage(IwinCommand::CHECK_CAPTCHA);
	jsonMsg->setData(captchaAnswer.toJson());
	sendJSONString(jsonMsg);
}

/**
* gui thong tin nap win len server
*
* @param charge_by_id
*            : kieu nap
* @param charge_by_subid
*            : loai the
* @param serial
* @param pin
*/
void GlobalService::sendChargeMoney(ubyte charge_by_id, ubyte charge_by_subid, std::string serial, std::string pin, std::string moneyType) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::CHARGE_MONEY);
	m->putByte(charge_by_id);
	m->putByte(charge_by_subid);
	m->putString(serial);
	if (!pin.empty()) {
		m->putString(pin);
	}
	m->putString(moneyType);
	sendMessage(m);

}

void GlobalService::requestPaymentInfo(const char* unit)
{
	Payment payment;
	payment.setUnit(unit);
	JsonMessage* jsonMsg = new JsonMessage(IwinCommand::REQUEST_PAYMENT_INFOR);
	jsonMsg->setData(payment.toJson());
	sendJSONString(jsonMsg);
}

void GlobalService::getLobbyList(ubyte gameID, ubyte lobbyType) {
	RequestLobbyList r;
	r.setGameId(gameID);
	r.setMoneyType(lobbyType);
	JsonMessage* m = new JsonMessage(IwinCommand::GET_LOBBY_LIST);
	m->setData(r.toJson());
	sendJSONString(m);
}

void GlobalService::doPlayNowLobby(int moneyType, int money) {
	DoPlayNow d;
	d.setMoneyType(moneyType);
	d.setMoney(money);
	JsonMessage* m = new JsonMessage(IwinCommand::DO_PLAY_NOW);
	m->setData(d.toJson());
	sendJSONString(m);
}

void GlobalService::getBoardFromLobby(int moneyType, int money) {
	DoPlayNow d;
	d.setMoneyType(moneyType);
	d.setMoney(money);
	JsonMessage* m = new JsonMessage(IwinCommand::GET_BOARD_FROM_LOBBYID);
	m->setData(d.toJson());
	sendJSONString(m);
}

void GlobalService::sendLobbyChat(ubyte gameId, int userId, std::string message) {
	LobbyChatMessage lm;
	lm.setGameId(gameId);
	lm.setUserId(userId);
	lm.setMessage( message);
	JsonMessage* m = new JsonMessage(IwinCommand::SEND_GLOBAL_CHAT);
	m->setData(lm.toJson());
	sendJSONString(m);
}

void GlobalService::getGlobalChatList() {
	JsonMessage* m = new JsonMessage(IwinCommand::GET_GLOBAL_CHAT_LIST);
	sendJSONString(m);
}

/**
* Lay danh sach cao thu
*
* @param page
* @param gameId
*/
void GlobalService::requestStrongest(int page, ubyte gameId) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::STRONGEST_LIST);
	m->putByte(gameId);
	m->putByte(page);
	sendMessage(m);
}

/**
* lay danh sach dai gia
*
* @param page
*/
void GlobalService::requestRichestWin(int page) {

	iwincore::Message* m = new iwincore::Message(IwinProtocol::RICHEST_LIST);
	m->putByte(page);
	sendMessage(m);
}

void GlobalService::requestRichestRuby(int page) {

	iwincore::Message* m = new iwincore::Message(IwinProtocol::RICHEST_RUBY_LIST);
	m->putInt(page);
	sendMessage(m);
}

void GlobalService::changePassword(std::string old_pass, std::string new_pass)
{
	iwincore::Message * m = new iwincore::Message(IwinProtocol::CHANGE_PASSWORD);
	m->putString(old_pass);
	m->putString(new_pass);
	sendMessage(m);
}

/**
* gui trang thai cho phep moi choi game
*/
void GlobalService::sendStaticInviviteToPlayGame(bool canInvite) {
	iwincore::Message *m = new iwincore::Message(IwinProtocol::INVITATION_SETTING);
	m->putBool(canInvite);
	sendMessage(m);
}

void GlobalService::getForgotPassword(ubyte type, std::string userName)
{
	iwincore::Message *m = new iwincore::Message(IwinProtocol::GET_PASSWORD);
	m->putByte(type);
	m->putString(userName);
	sendMessage(m);
}

void GlobalService::getItemImg(int itemId)
{
	iwincore::Message *m = new iwincore::Message(IwinProtocol::GET_IMAGE);// 79
	m->putByte(4); // Item Image
	m->putInt(itemId);
	sendMessage(m);
}

void GlobalService::requestAvatarShop()
{
	sendSingleCommand(IwinProtocol::REQUEST_AVATARLIST);
}

void GlobalService::requestAvatar(int avatar)
{
	CCLOG("Global Service", "CLIENT: REQUEST_AVATAR, id = %d", avatar);
	getItemImg(avatar);
}
/**
* request mua avatar
*
* @param id
*/
void GlobalService::requestBuyAvatar(int id) 
{
	iwincore::Message *m = new iwincore::Message(IwinProtocol::BUY_AVATAR);
	m->putInt(id);
	sendMessage(m);
}

void GlobalService::getAvatarUsing(int userId, ubyte gender)
{
	iwincore::Message *m = new iwincore::Message(IwinProtocol::ITEMS);
	m->putByte(IwinProtocol::ITEMS_GET_USING_AVATAR);
	m->putInt(userId);
	m->putByte(gender);
	sendMessage(m);
}

/**
* gui du lieu tam hinh chup len server lam avatar.
*
* @param imgData
*/

void GlobalService::sendCaptureAvatar(char* imgData, size_t leng)
{
	iwincore::Message *m = new iwincore::Message(IwinProtocol::SEND_CAPTURE_IMG , false);
	m->putInt(leng);
	for (size_t i = 0; i < leng; i++) {
		m->putByte(imgData[i]);
	}
	sendMessage(m);
}

/**
* Lay hinh avatar voi user id.
*/
void GlobalService::getAvatarByUserID(int userid)
{
	// #if DefaultConfiguration
	// @ Gdx.app.log("Global Service", "Get avatar by userID: " + userid);
	// #endif
	iwincore::Message *m = new iwincore::Message(IwinProtocol::GET_AVATAR_BY_USERNAME);
	m->putInt(userid);
	sendMessage(m);
}
/**
* request lay ds avatar de phan trang
*/
void GlobalService::requestAvatarPage(int page) {
	iwincore::Message *m = new iwincore::Message(1200);
	m->putInt(page);
	sendMessage(m);
}

/**
* request lay ds avatar de phan trang theo gioi tinh @type 1 la nam, 2 nu,
* 3 doc
*
* @param page
*/
void GlobalService::requestAvatarPage(int page, int type) {
	// #if DefaultConfiguration
	// @ Gdx.app.log("Global Service", "Lay page: " + page + " type: " +
	// @ // type);
	// #endif
	iwincore::Message *m = new iwincore::Message(1201);
	m->putInt(page);
	m->putInt(type);
	sendMessage(m);
}

/**
* Láº¥y táº¥t cáº£ cÃ¡c avatar mÃ  user Ä‘ang sá»Ÿ há»¯u.
*/
void GlobalService::getMyAvatars() {
	// #if DefaultConfiguration
	// @ Gdx.app.log("Global Service", "Get My avatars.");
	// #endif
	// Message m = new Message(IWINProtocol.GET_BOUGHT_AVATARS);
	iwincore::Message *m = new iwincore::Message(IwinProtocol::GET_BOUGHT_AVATARS_USE_TEXT_TIME);
	sendMessage(m);
}

/**
* Thuc hien doi avatar cho user.
*
* @param avatarID
*/
void GlobalService::changeMyAvatar(int avatarID) {
	// #if DefaultConfiguration
	// @ Gdx.app.log("Global Service", "Thuc hien doi sang avatar: " +
	// @ // avatarID);
	// #endif
	iwincore::Message *m = new iwincore::Message(IwinProtocol::CHANGE_AVATAR);// 1008
	m->putInt(avatarID);
	sendMessage(m);
}

/**
* Xoa 1 avatar cua user dang co.
*
* @param avatarID
*/
void GlobalService::deleteMyAvatar(int avatarID) {
	iwincore::Message *m = new iwincore::Message(IwinProtocol::DELETE_MY_AVATAR);
	m->putInt(avatarID);
	sendMessage(m);
	// #if DefaultConfiguration
	// @ Gdx.app.log("Global Service", "Goi command xoa avatar.");
	// #endif
}

/**
* Mua vật phẩm hoặc avatar cho bạn
*/
void GlobalService::buyForFriend(ubyte type, int itemID, int friendID, std::string friendName)
{
	iwincore::Message *m = new iwincore::Message(IwinProtocol::FRIEND);
	if (type == 0/*Item.ITEM*/) {
		m->putByte(8);
		m->putInt(itemID);
		m->putInt(friendID);
		m->putString(friendName);
	}
	else if (type == 1/*Item.AVATAR*/) {
		m->putByte(9);
		m->putShort(itemID);
		m->putString(friendName);
		m->putInt(friendID);
	}
	sendMessage(m);
}

/**
* gui request lay avatar nhan ve o Global message handler
*/
void GlobalService::requestShopAvatar(int page)
{
	iwincore::Message *m = new iwincore::Message(IwinProtocol::GET_AVATARS);
	m->putInt(page);
	sendMessage(m);
}

/**
* Lay danh sach category
*/
void GlobalService::getAvatarCategory()
{
	JsonMessage * m = new JsonMessage(IwinCommand::GET_AVA_CAT);
	sendJSONString(m);
}

/**
* Lay danh sach avatar theo id
*
* @param catId
*            id cua category can lay avatar
*/
void GlobalService::getAvatarOfCategory(int catId) 
{
	AvatarCat avatarCat ;
	avatarCat.setId(catId);

	JsonMessage * m = new JsonMessage(IwinCommand::GET_AVA);
	m->setData(avatarCat.toJson());
	sendJSONString(m);
}

/**
* Thuc hien mua 1 avatar
*
* @param avatar
*/
void GlobalService::buyAvatar(int id , int64_t money)
{
	BuyingAvatar buyingAvatar;
	buyingAvatar.setId(id);
	buyingAvatar.setMoney(money);
	JsonMessage * m = new JsonMessage(IwinCommand::BUY_AVA);
	m->setData(buyingAvatar.toJson());
	sendJSONString(m);
}

void GlobalService::getIsEnableRuby() {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::ENABLE_RUBY);
	sendMessage(m);
}

void GlobalService::requestListWait()
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::GET_STATISTIC_WAITING_BOARD_USER);
	sendMessage(m);
}

/**
* lay thong tin doi ruby sang the cao
*/
void GlobalService::requestListRubyToVND() {

	JsonMessage* msg = new JsonMessage(IwinCommand::RUBYDT_GET_INFO);
	sendJSONString(msg);
}

/**

* thuc hien doi the
*/
void GlobalService::requestCharge(PaymentInfo*  payment) {
	JsonMessage* msg = new JsonMessage(IwinCommand::RUBYDT_GET_CARD);
	msg->setData(payment->toJson());
	sendJSONString(msg);
}

/**
* lay list lich su
*/
void GlobalService::requestListHistoryCashOut(int page) {
	//DialogUtil.startWaitDialog();
	//Gdx.app.log(TAG, "requestListChargeHistory " + page);
	RequestHistoryCharge obj;
	obj.setPageIndex(page);
	JsonMessage* msg = new JsonMessage(IwinCommand::RUBYDT_GET_HISTORY);
	msg->setData(obj.toJson());
	sendJSONString(msg);
}


void GlobalService::requestCancleCashout(int id) {
	CancelGetCard obj;
	obj.setLogId(id);
	JsonMessage* msg = new JsonMessage(IwinCommand::RUBYDT_CANCEL_GET_CARD);
	msg->setData(obj.toJson());
	sendJSONString(msg);
}

/*
* Gửi mã xác nhận lên cho server
*/
void GlobalService::sendConfirmSMSCode(std::string confirmation_code)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::CONFIRM_SMS_CODE);
	m->putString(confirmation_code);
	sendMessage(m);
}
/**
* Chứng thực số điện thoại
* type = 1 : Verify phone
* @param value : Phone to verify
*/
void GlobalService::requestVerifyPhone(std::string value) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::VERIFY_USER);
	m->putInt(1);
	m->putString(value);
	sendMessage(m);
}

/**
* Chứng thực Email
* type = 0 : Verify email
* @param value : email to verify
*/
void GlobalService::requestVerifyEmail(std::string value) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::VERIFY_USER);
	m->putInt(0);
	m->putString(value);
	sendMessage(m);



}

/**
* nạp để mua item trong shop item
*
* @param shopId
* @param smsContent_send_to_server
* @param smsTo
*/
void GlobalService::chargeSMSWithShop(int shopId, std::string smsContent_send_to_server, std::string smsTo)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SEND_SMS_FINISH);
	m->putString(smsContent_send_to_server);
	m->putString(smsTo);
	m->putInt(shopId);
	sendMessage(m);
}

void GlobalService::transferMoney(std::string transferNick, int transferQuantity)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::TRANSFER_MONEY);
	m->putString(transferNick);
	m->putInt(transferQuantity);
	sendMessage(m);
}

void GlobalService::transferMoneyWithType(std::string transferNick, int transferQuantity, int type)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::TRANSFER_MONEY);
	m->putString(transferNick);
	m->putInt(transferQuantity);
	m->putInt(type);
	sendMessage(m);
}

/**
* Lay danh sach ban be cua chinh minh. Lay danh sach ban be tu 2.4.3
*/
void GlobalService::requestFriendList() {
	CCLOG("Global Service", "Lay danh sach ban be...");
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FRIEND);
	m->putByte(5);
	sendMessage(m);
}

/**
* tìm nick
*
* @param text
*/
void GlobalService::searchNick(std::string nick) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::SEARCH);
	m->putString(nick);
	sendMessage(m);
}

void GlobalService::addFriend(int id, std::string name) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FRIEND);
	m->putByte(1);
	m->putInt(id);
	m->putString(name);
	sendMessage(m);
}

void GlobalService::requestRichFriendList() {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FRIEND);
	m->putByte(16);
	sendMessage(m);
}

void GlobalService::requestStrongFriendList(int gameId) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FRIEND);
	m->putByte(20);
	m->putInt(gameId);
	sendMessage(m);
}

void GlobalService::friendAcceptRequest(int id) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FRIEND);
	m->putByte(2);
	m->putInt(id);
	sendMessage(m);
}

void GlobalService::friendDenyRequest(int id) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FRIEND);
	m->putByte(3);
	m->putInt(id);
	sendMessage(m);
}

void GlobalService::deleteHistory(int historyId) {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FRIEND);
	m->putByte(12);
	m->putInt(historyId);
	sendMessage(m);
}

void GlobalService::sendMessageToFriend(std::string user_name, std::string msg)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FRIEND);
	m->putByte(13);
	m->putString(user_name);
	m->putString(msg);
	sendMessage(m);
}

void GlobalService::chatToStranger(std::string user_name, std::string msg)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::MESSAGE);
	m->putString(user_name);
	m->putString(msg);
	sendMessage(m);
}

void GlobalService::chatto_old(int to_ID, std::string msg)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::CHAT_TO);
	m->putInt(to_ID);
	m->putString(msg);
	sendMessage(m);
}

void GlobalService::chatto_new(int to_ID, std::string msg)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::NEW_CHAT);
	m->putInt(to_ID);
	m->putString(msg);
	sendMessage(m);
}


void GlobalService::Fire(s16 command, ubyte roomId, ubyte boardId, ubyte type)
{
	auto m = new  iwincore::Message(command);
	m->putByte(roomId);
	m->putByte(boardId);
	m->putByte(type);
	sendMessage(m);
}


void GlobalService::sendCommandinGametoServer(const s16 ADD_CARD, int _roomID, int _boardID, int idUser)
{
	iwincore::Message* m = new iwincore::Message(ADD_CARD);
	m->putByte((byte)_roomID);
	m->putByte((byte)_boardID);
	m->putInt(idUser);
	sendMessage(m);
}

/**
* lay danh sach ma minh da gui loi moi ket ban
*/
void GlobalService::requestListInviteFriend() {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FRIEND);
	m->putByte(6);
	sendMessage(m);
}

/**
* lay danh sach moi minh lam ban
*/
void GlobalService::requestListWaitingFriend() {
	iwincore::Message* m = new iwincore::Message(IwinProtocol::FRIEND);
	m->putByte(14);
	sendMessage(m);
}

/**
* tai xiu
*/

void GlobalService::getTaiXiuInfo(std::string name) 
{
	TaiXiuInfoRequest  uO;
	uO.setUserName(name);
	JsonMessage * m = new JsonMessage(GameController::typeMoneyTaiXiu == RUBY_MONEY
		? IwinCommand::TAIXIU_GET_USER_INFO_RUBY : IwinCommand::TAIXIU_GET_USER_INFO);
	m->setData(uO.toJson());
	sendJSONString(m);
}

void GlobalService::taiXiuBet(std::string name, s64  value, ubyte type) 
{
	TaiXiuBetRequest bR;
	bR.setUsername(name);
	bR.setBetWin(value);
	bR.setBetChoice(type);
	JsonMessage * m = new JsonMessage(GameController::typeMoneyTaiXiu == RUBY_MONEY
		? IwinCommand::TAIXIU_BET_RUBY : IwinCommand::TAIXIU_BET);
	m->setData(bR.toJson());
	sendJSONString(m);

}

void GlobalService::getTaiXiuLeaderBoard() {
	JsonMessage* m = new JsonMessage(GameController::typeMoneyTaiXiu == RUBY_MONEY
		? IwinCommand::TAIXIU_GET_TOP_USERS_RUBY : IwinCommand::TAIXIU_GET_TOP_USERS);
	sendJSONString(m);

}

void GlobalService::getTaiXiuMyHistoryList() {
	JsonMessage* m = new JsonMessage(GameController::typeMoneyTaiXiu == RUBY_MONEY
		? IwinCommand::TAIXIU_GET_USER_HISTORY_RUBY : IwinCommand::TAIXIU_GET_USER_HISTORY);
	sendJSONString(m);

}

void GlobalService::getTaiXiuHistory()
{
	JsonMessage* m = new JsonMessage(GameController::typeMoneyTaiXiu == RUBY_MONEY
		? IwinCommand::TAIXIU_GET_HISTORY_RUBY : IwinCommand::TAIXIU_GET_HISTORY);
	sendJSONString(m);
}

void GlobalService::getTaiXiuMatchInfo(int mathID)
{
	TaiXiuMatchRequest request;
	request.setMatchId(mathID);
	JsonMessage* m = new JsonMessage(GameController::typeMoneyTaiXiu == RUBY_MONEY
		? IwinCommand::TAIXIU_GET_MATCH_HISTORY_RUBY : IwinCommand::TAIXIU_GET_MATCH_HISTORY);
	m->setData(request.toJson());
	sendJSONString(m);
}


void GlobalService::sendChargeMoneyForInApp(ubyte charge_by_id, std::string token, std::string moneyType)
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::CHARGE_MONEY);
	m->putByte(charge_by_id);
	m->putString(token);
	m->putString(moneyType);
	sendMessage(m);
}

void GlobalService::sendScreenIdToServer(int idScreen) {

	// lưu lại current screenId và lastScreenId
	GameController::lastScreenId = GameController::currentScreenId;
	GameController::currentScreenId = idScreen;
	iwincore::Message * m = new iwincore::Message(IwinProtocol::SEND_SCREEDID);
	m->putInt(idScreen);
	sendMessage(m);
}

void GlobalService::GetLuckyCircleInfo(int bonusEventId, int screenType)
{
	NewCircleInfoRequest d;
	d.setCircleId(bonusEventId);
	d.setScreenType(screenType);
	JsonMessage* m = new JsonMessage(IwinCommand::GET_LUCKY_CIRCLE_INFO);
	m->setData(d.toJson());
	sendJSONString(m);
}

void GlobalService::RequestTurningLuckyCircle(int circleType)
{
	NewCircleTurnRequest d;
	d.setCircleId(circleType);
	JsonMessage* m = new JsonMessage(IwinCommand::TURNING_LUCKY_CIRCLE);
	m->setData(d.toJson());
	sendJSONString(m);
}

void GlobalService::RequestTurningLuckyCircleWithCaptcha(int circleType, std::string captcha, std::string token)
{
	NewCircleTurnRequest d;
	d.setCircleId(circleType);
	d.setCaptchaChar(captcha);
	d.setToken(token);
	JsonMessage* m = new JsonMessage(IwinCommand::TURNING_LUCKY_CIRCLE);
	m->setData(d.toJson());
	sendJSONString(m);
}

void GlobalService::onUpdate(float dt)
{
#if USING_WSOCKET
	if (_coreservice_websocket == NULL)
	{
		return;
	}
	_coreservice_websocket->onUpdate(dt);
#else
#endif
}


void GlobalService::getInfoTransferMoney()
{
	iwincore::Message* m = new iwincore::Message(IwinProtocol::TRANSFER_INFO);
	sendMessage(m);
}

void GlobalService::sendGetTransactionHistory(int type, int page)
{
	TransactionHistoryRequest d;
	d.setType(type);
	d.setPage(page);
	JsonMessage* m = new JsonMessage(IwinCommand::GET_TRANSACTION_HISTORY);
	m->setData(d.toJson());
	sendJSONString(m);
}


/**
* Send Active OTP to server
*
* @param typeActive
*            Google(1) , Email(2) , Phone(3)
*/
void GlobalService::sendActiveOTP(int typeActive) {
	CCLOG("sendActiveOTP , typeActive = %d ", typeActive);
	iwincore::Message * m = new iwincore::Message(IwinProtocol::ACTIVE_OTP);
	m->putInt(typeActive);
	sendMessage(m);
}

/**
* Send deactive OTP to server
*
* @param typeActive
*            Google(1) , Email(2) , Phone(3)
*/
void GlobalService::sendDeactiveOTP(int typeActive) {
	CCLOG("sendDeactiveOTP , typeActive = %d", typeActive);
	iwincore::Message * m = new iwincore::Message(IwinProtocol::DEACTIVE_OTP);
	m->putInt(typeActive);
	sendMessage(m);
}

/**
* Send Json thông tin lên server sau khi nhận được mã code OTP
*/
void GlobalService::sendAuthorizeOTP(std::string code, int type, int purpose) {
	CCLOG("sendAuthorizeOTP , code = %s , type = %d , purpose = %d", code.c_str(), type, purpose);
	AuthorizeOTP  object;
	object.setCode(code);
	object.setType(type);
	object.setPurpose(purpose);
	JsonMessage * msg = new JsonMessage(IwinCommand::AUTHORIZE_OTP);
	msg->setData(object.toJson());
	sendJSONString(msg);
}

/**
* Send Json thông tin lên server sau khi nhận được mã code OTP
*/
void GlobalService::sendAuthorizeOTP(std::string code, int type, int purpose, bool isTrustDevice) {
	CCLOG("sendAuthorizeOTP , code = %s , type = %d , purpose = %d", code.c_str(), type, purpose);
	AuthorizeOTP object;
	object.setCode(code);
	object.setType(type);
	object.setPurpose(purpose);
	if (isTrustDevice)
		object.setTrustDevice(1);
	JsonMessage * msg = new JsonMessage(IwinCommand::AUTHORIZE_OTP);
	msg->setData(object.toJson());
	sendJSONString(msg);
}

/**
* lấy danh sách OTP của user khi vào màn hình OTP , trả về OTP nào đang
* active, deactive, dùng cái nào mặc định
*/
void GlobalService::requestOTPStatus() {
	CCLOG("requestOTPStatus");
	iwincore::Message * m = new iwincore::Message(IwinProtocol::GET_OTP_STATUS);
	sendMessage(m);
}

/**
* đổi phương thức lấy otp khi login
* @param typeOTP
*/
void GlobalService::requestChangeOTPLoginType(int typeOTP) {
	CCLOG("requestChangeOTPLoginType , typeOTP = %d" , typeOTP);
	iwincore::Message * m = new iwincore::Message(IwinProtocol::CHANGE_OTP_TYPE);
	m->putInt(typeOTP);
	sendMessage(m);
}

/**
* Đổi phương thức OTP mặc định
*
* @param typeOTP
*            : Loại OTP define trong file ActiveOTP.java
*/
void GlobalService::sendChangeDefaultOTP(int typeOTP) {
	CCLOG("sendChangeDefaultOTP , typeOTP = %d", typeOTP);
	iwincore::Message * m = new iwincore::Message(IwinProtocol::CHANGE_DEFAULT_OTP);
	m->putInt(typeOTP);
	sendMessage(m);
}

/**
* Nhấn gửi lại OTP cho Email và số điện thoại
*
* @param typeOTP
*            : loại OTP define trong ActiveOTP.java
*/
void GlobalService::requestResendOTP(int typeOTP) {
	CCLOG("requestResendOTP , typeOTP = %d" , typeOTP);
	iwincore::Message * m = new iwincore::Message(IwinProtocol::RESEND_OTP);
	m->putInt(typeOTP);
	sendMessage(m);
}


/*
*	no hu service
*/

void GlobalService::nohu_getInfoGame()
{
	JsonMessage * msg = new JsonMessage(GameController::typeMoneyNohu == RUBY_MONEY ?
		IwinCommand::NOHU_GET_INFO_RUBY : IwinCommand::NOHU_GET_INFO);
	sendJSONString(msg);
}

void GlobalService::nohu_sendBet(int id, s64 betMoney)
{
	NoHuBetRequest  object;
	object.setJarId(id);
	object.setBetMoney(betMoney);
	object.setMoneyType(GameController::typeMoneyNohu);
	JsonMessage * msg = new JsonMessage(IwinCommand::NOHU_BET);
	msg->setData(object.toJson());
	sendJSONString(msg);
}

void GlobalService::nohu_sendMyHistory(int page)
{
	NoHuHistoryRequest  object;
	object.setPage(page);
	object.setMoneyType(GameController::typeMoneyNohu);
	JsonMessage * msg = new JsonMessage(IwinCommand::NOHU_GET_USER_HISTORY);
	msg->setData(object.toJson());
	sendJSONString(msg);
}

void GlobalService::nohu_sendTopHistory(int page)
{
	NoHuHistoryRequest  object;
	object.setPage(page);
	object.setMoneyType(GameController::typeMoneyNohu);
	JsonMessage * msg = new JsonMessage(IwinCommand::NOHU_GET_LIST_NOHU);
	msg->setData(object.toJson());
	sendJSONString(msg);
}

void GlobalService::nohu_sendClose()
{
	JsonMessage * msg = new JsonMessage(IwinCommand::NOHU_CLOSE);
	sendJSONString(msg);
}
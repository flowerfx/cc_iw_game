#include "../Core/Message.h"
#include "../Core/MobileClient.h"
#include "../Core/IMessagehandler.h"
#include "../Core/JsonMessage.h"
#include "Network/JsonObject/cashoutruby/PaymentInfo.h"
#include "Platform/Platform.h"
#pragma once
class SocketConnect;
namespace iwinmesage
{
    class Contact;
    class Quest;
	class RequestQuest;
}

class GlobalService
{
private:
	static MobileClient* _coreservice_websocket;
	static SocketConnect* _coreservice_socket;
public:

	static bool isReconnect;


	static std::string GS_TAG ;
	static std::function<void(void)> actionConnectOk ;

	static void connect(const char* ip, const char* port, const std::function<void()> & result_callback);
	static void onSendClientInfo();
	static void setClientLanguage(std::string lang);
	static void sendString(std::string text);
	static void sendString(s16 comand,std::string data);
	static void sendMessage(iwincore::Message* msg);
	static void sendSingleCommand(s16 idCommand);
	static void login(std::string username, std::string pass);
    
	static void getServerListFromWeb(const std::function<void(void * data_call_back)> & action_done, const std::function<void(void)> & action_fail);

    static void getInfo(int subId);

	static void sendJSONString(JsonMessage* jsonMsg);
	/**
	* Lay pham vi tien cuoc ma user co the dat.
	*/
	static void getBetRange();

	/**
	* This function send only command ID.
	*
	* @param commandId
	*/
	static void sendSingleCommand(int commandId);

	static void requestRoomList();
	static void requestProfileOf(int id);
	static void UpdateProfile(User * user);
	static void UpdateStatus(std::string status);
	static void joinAnyBoard();

	static void getAllUserAchievement(int id_user);
	static void getUserAchievementOfGameID(int id_user, int game_id);

	static void chatToBoard(ubyte roomID, ubyte boardID, std::string text);

	static void joinBoard(ubyte roomID, ubyte boardID, std::string pass);

	/**
	* dung khi dang ngoi xem chuyen sang ngoi choi (cho cac game co cho ngoi
	* xem) co the xai nhu binh thuong co cac game khong co ngoi xem
	*
	* @param roomID
	* @param boardID
	* @param pass
	*/
	static void joinBoard_Normal(ubyte roomID, ubyte boardID, std::string pass);
	static void leaveBoard(ubyte roomID, ubyte boardID);
	static void ready(ubyte roomID, ubyte boardID, bool isReady);
	static void quickPlay(ubyte roomID, ubyte boardID);
	static void confirmLeaveGame(ubyte roomID, ubyte boardID, ubyte isConfirm);
	/**
	* Set money trong cac game ma tien cuoc chung.
	*
	* @param roomID
	* @param boardID
	* @param money
	*/
	static void setMoney(ubyte roomID, ubyte boardID, int money);
	/**
	* Cai dat password cho ban choi.
	*
	* @param roomID
	* @param boardID
	* @param pass
	*/
	static void setPassword(ubyte roomID, ubyte boardID, std::string pass);

	/**
	* Kick player trong ban choi.
	*
	* @param roomID
	* @param boardID
	* @param kickID
	*/
	static void kick(ubyte roomID, ubyte boardID, int kickID);
	static void startGame(ubyte roomID, ubyte boardID);
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
	static void startGame(ubyte roomID, ubyte boardID, ubyte timeLimit);

	static void requestBoardList(ubyte id);
	/**
	* Thuc hien mo vat pham su kien.
	*
	* @param itemID
	*            :ID cua item muon open.
	*/
	static void openEventItem(int itemID);

	/**
	* send command request a player join board.
	*/
	static void requestInvite(int userID);

	/**
	* Yeu cau server tra ve mot danh sach loai type.
	*
	* @param type
	*            : Loai muon tra 1 : Danh sach TOP/RANK. 2 : Danh sach phong
	*            cho 3 : Danh sách tỉnh thành.
	*/
	static void getList(int type);

	static void requestQuestList();

	static void acceptCancelQuest(int questId, bool isCancel);

	static void giveItem(ubyte roomid, ubyte controllerid, int itemID,
		std::vector<int> toUserIDs);
	static void requestListGift(int categoryId, ubyte page);
	static void buzz();
	static void requestBgImage(int id);

	/**
	* * Yêu cầu server xem thông tin của một bàn chơi.
	*
	* @param boardID
	*            - ID của bàn chơi muốn xem.
	*/
	static void viewBoardInfoOf(ubyte roomID, ubyte boardID);

	/**
	* Game lotto
	*/
	static void sendBuyLotto(int number, int money);
	static void getGift(int categoryId, ubyte gender, ubyte type);

	/**
	* tu choi loi moi khi vao game
	*/
	static void sendDeny_Invited(int userID);

	/**
	* Lấy 1 danh sách các user đang nằm trong phòng ch�? có tên đúng và gần
	* đúng với tên truy�?n vào.
	*
	* @param username
	*/
	static void searchInWaittingList(std::string username);

	/**
	* Set max player in any game with roomID and boardID
	*
	* @param roomID
	* @param boardID
	* @param max
	*/
	static void setMaxPlayer(ubyte roomID, ubyte boardID, int max);

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
	static void betBauCua(s16 command, ubyte roomID, ubyte boardID,
		ubyte subCommand, int itemID, int money);

	/**
	* phuong thuc gui msg don gian cho cac game dung chung
	*
	* @param command
	* @param roomID
	* @param boardID
	* @param mo
	*/
	static void setCommandinGametoServer(s16 command, ubyte roomID,
		ubyte boardID, int mo);

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
	static void latBaiBaiCao(ubyte roomID, ubyte boardID, std::vector<char> cardsID);
	static void requestWaitingListInGame();
	static void playerToViewer(ubyte roomID, ubyte boardID);
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
	static void requestAndAnswerLoseAndDraw(s16 command, ubyte roomID,
		ubyte boardID, bool accept);

	/**
	* dung cho caro va co vua
	*
	* @param roomID
	* @param boardID
	* @param x
	* @param y
	*/
	static void move(ubyte roomID, ubyte boardID, ubyte x, ubyte y);
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
	static void requestSimpleCommand(s16 command, ubyte roomID,
		ubyte boardID);

	static void sendHeartBeat();
	static void getHeartBeatConfiguration();
	static void getNewGameList();
	static void getCountQuestList();
    
    static void sendInviteFacebookList(std::vector<std::string> fbFriendListId);
    
	// feature question and answer when login successfull
	 static void sendAnswerToServer(int commandId, int questionId, std::string answer);

	 static void sendIdQuestionToServer(int commandId, int questionId);

	 static void sendRequestToGetQuestion();
    /**
     *
     * @param accessToken
     */
    static void requestFriendFromFacebook(std::string accessToken, int page) ;
    
    static void requestFriendFromContact(std::vector<iwinmesage::Contact*> contacts);
    /**
     *
     * @param location
     * @param page
     *            Bat dau tu 0
     */
    static void requestFriendFromLocation(Platform::GPSLocation * location, int page);
    /**
     * Check user khi tao nick.
     *
     * @param userName
     */
    static  void checkUsername(std::string userName) ;
    /**
     * Check email khi tao nick.
     *
     * @param userName
     */
    static void checkEmail(std::string email) ;
    /**
     *
     * @param userName
     * @param pass
     */
     static void checkPass(std::string userName, std::string pass);
    /***
     *
     * @param requestId
     * @param accessToken
     * @param listFbIdRequest
     */
    static void requestSendGift(std::string requestId, std::string accessToken, std::vector<std::string> listFbIdRequest);
    /**
     * Clear facebook request theo type.
     *
     * @param type
     *            Giong nhu cac type cua class FacebookRequest
     */
    static void clearFacebookRequest(int type);
    /**
     * Lay danh
     */
    static void getMyActivity();
    /**
     *
     * @param ids
     * @param facebookID
     */
    static void openFacebookGift(std::vector<long long> ids, std::string facebookID) ;
    /**
     * Lay danh sach quest.
     */
    //  static void requestQuestList() ;
    /**
     * Submit 1 list quest.
     */
    static void requestSubmitQuest(RequestQuest* object);
    
    /**
     * Accept 1 list quest.
     */
    static void requestAcceptQuest(RequestQuest* object) ;
    /**
     * Nhan duoc user list (trong userlist co access token).
     *
     * @param code
     *            String get from sdklib.
     */
     static void sdklibListUserByCode(std::string code) ;
    /**
     * chi nhan duoc user list
     */
    static void sdklibListUserByAccessToken(std::string token) ;
    /**
     * Login voi facebook account.
     *
     * @param token
     * @param username
     */
    static void loginBySdklibAccount(std::string token, std::string username);
    
    /**
     *
     * @param usernameIwin
     * @param token
     */
    static void createIWinAccFrom(std::string usernameIwin, std::string token);
	/**
	* lấy danh sách bàn chơi (chủ bàn)
	*
	* @param betMoney
	*/
	static void requestOwnerBoardList(int betMoney);

	/**
	* Set game type.
	*
	* @param gameType
	* @param onSetGameTypeOK
	*            Call khi set game type ok.
	*/
	static void setGameType(int gameType, std::function<void()> _onSetGameTypeOK);


	static void requestRegister(std::string username, std::string password);

	static void requestCreateBoard(int subCommand, int money, ubyte numPlayer, std::string password);

	static void getSMSInfoForgotPassword(std::string username);
    
    static void loginWithFacebookToken(std::string type, std::string token, std::string version);
	static void loginWithTokenJson(std::string type, std::string token, std::string version);
    
	static void CheckUserName(std::string user_name);
    static void CheckEmail(std::string email);
	static void getCaptchaFirstTime();
	static void getCaptchaRefresh(std::string id);
	static void sendCheckCaptcha(std::string id, std::string answer);

	static void requestPaymentInfo(const char* unit);
	static void GetQuestCategoryList();
	static void GetQuestByCategoryID(int id);
	//static void SendJSONString(std::string json_str);
	static void getLobbyList(ubyte gameID, ubyte lobbyType);
	static void doPlayNowLobby(int moneyType, int money);
	static void getBoardFromLobby(int moneyType, int money);
	static void sendLobbyChat(ubyte gameId, int userId, std::string message);
	static void getGlobalChatList();
	static void sendStaticInviviteToPlayGame(bool canInvite);
	//close socket
	static void CloseSocket();


	/**
	* Lay danh sach cao thu
	*
	* @param page
	* @param gameId
	*/
	static void requestStrongest(int page, ubyte gameId);

	/**
	* lay danh sach dai gia
	*
	* @param page
	*/
	static void requestRichestWin(int page);

	static void requestRichestRuby(int page);

	//change password
	static void changePassword(std::string old_pass, std::string new_pass);

	static void getForgotPassword(ubyte type, std::string userName);

	//avatar service
	static void getItemImg(int itemId);
	//get avatar shop from server
	static void requestAvatarShop();
	static void requestAvatar(int avatar);
	/**
	* request mua avatar
	*
	* @param id
	*/
	static void requestBuyAvatar(int id);

	static void getAvatarUsing(int userId, ubyte gender);

	/**
	* gui du lieu tam hinh chup len server lam avatar.
	*
	* @param imgData
	*/
	static void sendCaptureAvatar(char* imgData, size_t leng);

	/**
	* Lay hinh avatar voi user id.
	*/
	static void getAvatarByUserID(int userid);
	/**
	* request lay ds avatar de phan trang
	*/
	static void requestAvatarPage(int page);
	/**
	* request lay ds avatar de phan trang theo gioi tinh @type 1 la nam, 2 nu,
	* 3 doc
	*
	* @param page
	*/
	static void requestAvatarPage(int page, int type);

	/**
	* Láº¥y táº¥t cáº£ cÃ¡c avatar mÃ  user Ä‘ang sá»Ÿ há»¯u.
	*/
	static void getMyAvatars();

	/**
	* Thuc hien doi avatar cho user.
	*
	* @param avatarID
	*/
	static void changeMyAvatar(int avatarID);

	/**
	* Xoa 1 avatar cua user dang co.
	*
	* @param avatarID
	*/
	static void deleteMyAvatar(int avatarID);

	/**
	* Mua vật phẩm hoặc avatar cho bạn
	*/
	static void buyForFriend(ubyte type, int itemID, int friendID, std::string friendName);

	/**
	* gui request lay avatar nhan ve o Global message handler
	*/
	static void requestShopAvatar(int page);

	/**
	* Lay danh sach category
	*/
	static void getAvatarCategory();

	/**
	* Lay danh sach avatar theo id
	*
	* @param catId
	*            id cua category can lay avatar
	*/
	static void getAvatarOfCategory(int catId);

	/**
	* Thuc hien mua 1 avatar
	*
	* @param avatar
	*/
	static void buyAvatar(int id, int64_t money);

	static void getIsEnableRuby();

	static void requestListWait();
	static void requestListRubyToVND();

	static void requestCharge(PaymentInfo* payment);
	static void requestListHistoryCashOut(int page);
	static void requestCancleCashout(int id);

	/**
	* Gửi mã xác nhận lên cho server
	*/
	static void sendConfirmSMSCode(std::string confirmation_code);
	/**
	* Chứng thực số điện thoại
	* type = 1 : Verify phone
	* @param value : Phone to verify
	*/
	static void requestVerifyPhone(std::string value);

	/**
	* Chứng thực Email
	* type = 0 : Verify email
	* @param value : email to verify
	*/
	static void requestVerifyEmail(std::string value);


	/**
	* nạp để mua item trong shop item
	*
	* @param shopId
	* @param smsContent_send_to_server
	* @param smsTo
	*/
	static void chargeSMSWithShop(int shopId, std::string smsContent_send_to_server, std::string smsTo);

	static void transferMoney(std::string transferNick, int transferQuantity);

	static void transferMoneyWithType(std::string transferNick, int transferQuantity, int type);
	/*
	* friend param 
	
	*/

	/**
	* Lay danh sach ban be cua chinh minh. Lay danh sach ban be tu 2.4.3
	*/
	static void requestFriendList();

	/**
	* tìm nick
	*
	* @param text
	*/
	static void searchNick(std::string nick);

	static void addFriend(int id, std::string name);
	static void requestRichFriendList();
	static void requestStrongFriendList(int gameId);
	static void friendAcceptRequest(int id);
	static void friendDenyRequest(int id);

	/*
	* guoi message cho friend
	*/
	static void sendMessageToFriend(std::string user_name, std::string msg);

	/*
	* chat voi nguoi la (khong biet id)
	*
	*/
	static void chatToStranger(std::string user_name, std::string msg);

	/*
	* chat voi nguoi minh biet id (old va new)
	*
	*/

	static void chatto_old(int to_ID, std::string msg);

	static void chatto_new(int to_ID, std::string msg);

	/**
	* lay danh sach ma minh da gui loi moi ket ban
	*/
	static void requestListInviteFriend();
	/**
	* lay danh sach moi minh lam ban
	*/
	static void requestListWaitingFriend();
	static void deleteHistory(int historyId);
	/**
	* rut bai (dung cho xi zach)
	*
	* @param command
	* @param roomId
	* @param boardId
	* @param type
	*/
	static void Fire(s16 command, ubyte roomId, ubyte boardId, ubyte type);
	static void sendCommandinGametoServer(const s16 ADD_CARD, int _roomID, int _boardID, int idUser);

	/*
	*
	* tai xiu global service
	*/

	static void getTaiXiuInfo(std::string name);
	static void taiXiuBet(std::string name, s64 value, ubyte type);
	static void getTaiXiuLeaderBoard();
	static void getTaiXiuMyHistoryList();
	static void getTaiXiuHistory();
	static void getTaiXiuMatchInfo(int mathID);
	
	//
	static void sendChargeMoneyForInApp(ubyte charge_by_id, std::string token, std::string moneyType);
	static void sendChargeMoney(ubyte charge_by_id, ubyte charge_by_subid, std::string serial, std::string bin, std::string moneyType);

	static void sendScreenIdToServer(int idScreen);
	static void GetLuckyCircleInfo(int bonusEventId, int screenType);
	static void RequestTurningLuckyCircle(int circleType);
	static void RequestTurningLuckyCircleWithCaptcha(int circleType, std::string captcha, std::string token); 
	static void onUpdate(float dt);

	//
	
	static void getInfoTransferMoney();
	static void sendGetTransactionHistory(int type, int page);

	/**
	* Send Active OTP to server
	*
	* @param typeActive
	*            Google(1) , Email(2) , Phone(3)
	*/
	 static void sendActiveOTP(int typeActive);

	/**
	* Send deactive OTP to server
	*
	* @param typeActive
	*            Google(1) , Email(2) , Phone(3)
	*/
	static void sendDeactiveOTP(int typeActive);

	/**
	* Send Json thông tin lên server sau khi nhận được mã code OTP
	*/
	static void sendAuthorizeOTP(std::string code, int type, int purpose);

	/**
	* Send Json thông tin lên server sau khi nhận được mã code OTP
	*/
	static void sendAuthorizeOTP(std::string code, int type, int purpose, bool isTrustDevice);

	/**
	* lấy danh sách OTP của user khi vào màn hình OTP , trả về OTP nào đang
	* active, deactive, dùng cái nào mặc định
	*/
	static void requestOTPStatus();

	/**
	* đổi phương thức lấy otp khi login
	* @param typeOTP
	*/
	static void requestChangeOTPLoginType(int typeOTP);

	/**
	* Đổi phương thức OTP mặc định
	*
	* @param typeOTP
	*            : Loại OTP define trong file ActiveOTP.java
	*/
	static void sendChangeDefaultOTP(int typeOTP);

	/**
	* Nhấn gửi lại OTP cho Email và số điện thoại
	*
	* @param typeOTP
	*            : loại OTP define trong ActiveOTP.java
	*/
	static void requestResendOTP(int typeOTP);

	/*
	*	no hu service
	*/

	static void nohu_getInfoGame();

	static void nohu_sendBet(int id, s64 betMoney);

	static void nohu_sendMyHistory(int page);

	static void nohu_sendTopHistory(int page);

	static void nohu_sendClose();
    
    
    
};


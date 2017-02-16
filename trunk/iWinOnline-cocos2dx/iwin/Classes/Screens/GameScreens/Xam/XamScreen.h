/*!
* \class XamScreen
*
* \brief


*
* \author tuandq
* \date May 2016
*/
#ifndef __GAME_SCREEN_XAM_H__
#define __GAME_SCREEN_XAM_H__

#include "Screens/GameScreens/GameScreens.h"
using namespace cocos2d;
class ObjectEntity;
class TienLenPlayer;
class RectProcessTime;
class Card;
#define NUMBER_CARD_TL 10
class XamScreen : public GameScreen
{
private:
	enum BTN_XAM
	{
		BTN_FIRE_CARD = 0,
		BTN_SORT_CARD,
		BTN_SKIP_CARD,
		BTN_COUNT
	};
	enum TYPE_FORCE_FINISH
	{
		//sanh rong
		STRAIGHT_DRAGON = 10,
		//6 doi
		SIX_PAIRS,
		//5 doi thong
		FIVE_PAIRS_CONT,
		//tu quy heo
		FOUR_HEO,
		//4 sam co
		FOUR_THREE_CARDS,
		// 3 doi thong co 3 bich o van dau tien
		FOUR_TRIPLE,
		//tu quy 3 o van dau tien
		THREE_PAIRS_CONT_WITH_3SPADE
	};

	WidgetEntity * p_layout_fire_card;
	WidgetEntity * p_animate_1;
	WidgetEntity * p_animate_2;
	Vec2 _delta_aligne;
	//
	bool _forceMoveSmallest;
	bool _isFirstMatch;

	// Toi luot di cua.
	int _currentPlayer;

	std::vector<ubyte> _selectedCardIDs; // chon nhieu de danh bai nao dung cho se nang len
										// 10px
										// Loai card dang duoc chon trong bai cua minh.

										// Loại card dang duoc hien thi tren ban.
	char	_currentCardsType;
	bool	_autoSkip;

	std::vector<Card*> _list_fire_card;
	std::vector<Card*> _list_last_card;
	std::vector<Card*> _list_end_card;
	struct DenBai
	{
		int idUser;
		std::vector<ubyte> list_id;
		DenBai()
		{
			idUser = -1;
			list_id.clear();
		}
		DenBai(int id, std::vector<ubyte> list)
		{
			idUser = id;
			list_id = list;
		}
		~DenBai()
		{}
	};
	std::vector<DenBai*> _list_den_bai;
	bool IsAutoSelect;

	RectProcessTime* p_rect_process;
	std::vector<Vec2> _list_pos_swap_group;
	void EnableButton(BTN_XAM btn_type, bool value);

	void dealCards();
	void createUI();

	void CleanMyCard(bool cleanup = false);
	void CleanFireCard();
	void CleanLastCard();
	void CleanEndCard();
	void doFireWork();
	void addLastCard(std::vector<Card*> lstFirecard);
	void runCardFire(std::vector<unsigned char> cards, int whoMove);
	void showResult(vector<int> userIds, vector<int> penalty, vector<vector<byte>> cardsShow);
	void clearDenBai();

	std::vector<unsigned char> doAutoSelect(bool state_skip = false);
	std::vector<unsigned char> getcardFire();
	void startPlay();

	bool _load_card_finish;
	int _number_card_remain_after_game;
	std::vector<char> _list_card_create;

	int _myResult;

	float _pos_y_hl;
private:

	WidgetEntity * p_btn_skip;
	WidgetEntity * p_btn_fire;
	WidgetEntity * p_btn_sort;
	WidgetEntity * p_img_xam;
	WidgetEntity * p_dialog_xam;
	WidgetEntity * p_label_waiting_xam;
	WidgetEntity * p_img_one_card;
	TimeBarProcess * p_timebar_dialog_xam;
	//force finish game
	int _typeForceFinish;
	int _id_whoForceFinish;
	std::vector<char> _list_card_finish;
	std::vector<char> _list_card_select_action;
	std::vector<cocos2d::Vec2> _list_pos_img_xam;
	std::vector<cocos2d::Vec2> _list_pos_img_one_card;
	

protected:
	virtual void	HandleAfterInitPlayer() override;
	virtual void    OnShowEffectHL(int start_card_idx, int end_card_idx, bool isShow = true) override;
	/*
	* lay ten loai toi trang
	*/
	RKString getNameForceFinish(int type);

	
public:
	char _interval;
	static XamScreen* getInstance();
	XamScreen();
	virtual ~XamScreen();

	virtual bool InitGame(int game_id) override;
	virtual int Update(float dt) override;
	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML) override;
	virtual void resetSeat() override;
	virtual void resetGame() override;
	virtual void InitPlayerChild(xml::UILayerWidgetDec* xml_dec, xml::BasicDec *  dec, int idx) override;
	virtual void OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player) override;
	//handle hold and drag(throw card)
	virtual void OnTouchMenuEnd(const cocos2d::Point & p) override;
	virtual void OnTouchMenuHold(const cocos2d::Point & p) override;

	virtual void ActionTouchBeginObject(Utility::wObject::wBasicObject * obj) override;
	virtual void ActionTouchMoveObject(Utility::wObject::wBasicObject * obj) override;

	virtual	void onSomeOneJoinBoard(int seat, Player * p) override;
	virtual	void onSomeOneLeaveBoard(int leave, int newOwner) override;
	virtual void ResetReady() override;
	virtual void setSeat(std::vector<Player*> l_players) override;
	virtual void setPlayerSeat(std::vector<Player*> l_players) override;
	virtual void onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney, RKString dec) override;
	virtual void doSomethingWhenIWin()override;
	virtual void doSomethingWhenILose()override;
	virtual void doSomethingWhenIRaw() override;
	void start(int whoMoveFirst, char interval, std::vector<unsigned char> myCards);
	TienLenPlayer* getMe();
	void layoutMyCard();
	bool isClearCurrentCard;
	void onGetTimeLeftAutoStart(char timeStart);
	void hideAllRank();
	void ResetAllSkip();
	void CheckCardSpecial();
	void OnClearCardAndStartNewLoad();
	bool CheckCardAviableFire();
	
	void Start(int whoMoveFirst, byte interval, vector<unsigned char> myCards);
	void OnStopGame(byte roomID16, byte boardID16, int winnerID, vector<int> userIds, vector<int> penalty, vector<vector<byte>> cardsShow);
	void ShowDialogConfirmXam(byte timeRemain, byte timeremain);
	void OnGetTimeLeft(byte timeRemain);
	void Xam(int whoXam);
	void SkipXam(int whoMoveFirst);
	void OnSkip(int whoSkip, int nextMove1, bool clearCurrentCards);
	void OnReconnect(int interval, int playerId, vector<byte> movedCards,
		int next_user_id, vector<byte> myCards, int timeLimit,
		vector<int> userRank, vector<bool> userSkip, int xamUserId, byte timeXamRemain);
	void onMove(int whoMove, vector<unsigned char> cards, int nextMove);
	void OnHaveOneCard(int idUser);
	void OnDenBai(int idUser, vector<ubyte> lsCardIds);
};

#endif //__GAME_SCREEN_XAM_H__


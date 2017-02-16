/*!
* \class XiZachScreen
*
* \brief


*
* \author tuandq
* \date May 2016
*/
#ifndef __GAME_XIZACH_H__
#define __GAME_XIZACH_H__

#include "Screens/GameScreens/GameScreens.h"
#include "UI/AdvanceUI/wLayout.h"

class ObjectEntity;
class SliderBet;
class Cards;
class XiZachPlayer;
class FlopCardUI;
class XiZachScreen : public GameScreen
{
private:

	std::vector<Vec2>			_list_pos_layer_card;
	std::vector<WidgetEntity*>	ls_players_card;
	std::vector<WidgetEntity*>	ls_players_bet_info;

	std::vector<Card*> ls_cards;

	WidgetEntity * p_panel_slider;

	FlopCardUI*		flopCard;

	WidgetEntity*	btn_bet_money;
	WidgetEntity*	btn_flop_card;
	WidgetEntity*	btn_done;
	LayoutWidget* btn_retrieve;
	SliderBet*		p_slider_bet;
	int whoMove ;
	vector<unsigned char> lsCardValue;
	vector<string> _scores;
	bool _isAllowRetrieveCard = false;

	void DealCardSeat(int s, int num = 2);
	void ShowButtonSetBet(bool param1);

	void flipCard(int seat, vector<char> ids);

	int  _myResult;
protected:
	virtual void		  HandleAfterInitPlayer() override;
	void OnShowBetSlider(bool show);
	void SetTextAndAligneResult(RKString text, WidgetEntity * wpanel , bool isMe);
public:
	static XiZachScreen* getInstance();

	int totalMoney;

	XiZachScreen();
	virtual ~XiZachScreen();

	virtual bool InitGame(int game_id) override;
	virtual int Update(float dt)	   override;
	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, Utility::UI_Widget::WidgetEntity * _widget) override;
	virtual void LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)	override;
	virtual void OnTouchMenuBegin(const cocos2d::Point & p)	 override;

	virtual void onSomeOneJoinBoard(int seat, Player * player) override;
	virtual void onSomeOneLeaveBoard(int leave, int newOwner) override;
	virtual void OnFadeInComplete()	 override;
	virtual void resetGame() override;

	virtual void OnTouchMenuEnd(const Vec2 & p) override;
	virtual void ActionTouchBeginObject(Utility::wObject::wBasicObject * obj) override;
	virtual void ActionTouchMoveObject(Utility::wObject::wBasicObject * obj) override;

	virtual void OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player) override;
	virtual void onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney, RKString dec) override;
	virtual void onBetRange(int min, s64 max, s64 maxMoneyRoom) override;
	virtual void doSetMoney(int money) override;

	virtual void doSomethingWhenIWin()override;
	virtual void doSomethingWhenILose()override;

	void createDynamicCardLayout(Utility::UI_Widget::WidgetEntity* layerCard, int numClone, const std::function<void(Utility::UI_Widget::WidgetEntity*)> & func);
	void layoutGame();

	void showLbInfoBet();
	void hideAllCard();
	void hideAllFlare();
	void ShowButtonAction(bool btnQuickPlay,bool btnDone,bool btnFlop,bool btnBet);

	XiZachPlayer* GetMe();

	virtual void onSomeOneQuickPlay(int user, bool isQuickPlay)	override;
	void OnBetMoney(int money, int user);
	void OnStopGame(ubyte numPlayers, vector<vector<int>> cardR);
	void OnStartGame(vector<char> myCards, ubyte interval, int user);
	void SetScores(int user, string des1);
	void OnSetMove(int user, ubyte time);
	void OnSetMoveOwner(int user, vector<char> cardsID);
	void OnReconnect(
		int limit_time, 
		int remain_time, 
		vector<char> myCards, 
		int crMoveID, 
		vector<vector<int>> playerCards, 
		vector<int> playerBetMoneys, 
		int nPlayer, 
		bool isPlaying, 
		bool isOwnerTurn);
	void OnPlaying(ubyte interval, ubyte numPlayers, vector<vector<int>> cards);
	void OnRetrieveCard(int user, ubyte  idCard);
	void ShowButtonRetrieveCard(bool show);
	void OnFlipCard(int id, std::vector<char> cardID);
	void ShowScores(int seat);
	void ShowFubgeCard(int seat,bool isShow);
	void HideAllFudge();
	bool checkNullCard(vector<char> cards);

	void LayoutSliderBet();

	
};

#endif //__GAME_XIZACH_H__


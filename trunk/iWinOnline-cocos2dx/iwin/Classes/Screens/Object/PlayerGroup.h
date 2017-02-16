#ifndef __PLAYER_GROUP_H__
#define __PLAYER_GROUP_H__
//#include "../../Models/Player.h"
#include "Screens/ScreenManager.h"

class GameScreen;
class Achievement;
class Player;
class PlayerGroup 
{
private:
	WidgetEntity *  _widget;
	//weak pointer for target layout player
	WidgetEntity * p_layout_player;

	WidgetEntity * p_layout_timer;
	WidgetEntity * p_panel_info;


	xml::UILayerWidgetDec* xml_icon_dec;
	xml::UILayerWidgetDec* xml_chat_text_dec;
	xml::UILayerWidgetDec* xml_dec_player;

	//weak pointer that will show us that player group into anyscreen!
	GameScreen *	p_current_game_screen;
	//btn user info
	WidgetEntity * p_btn_user_info;
	//layout list btn control
	WidgetEntity * p_layout_user_control;
	struct ChatBoard
	{
		bool isChatBoard;
		std::string text;
		ChatBoard()
		{
			isChatBoard = true;
			text = "";
		}
		ChatBoard(const std::string t , bool val)
		{
			isChatBoard = val;
			text = t;
		}
		virtual ~ChatBoard()
		{
			isChatBoard = true;
			text = "";
		}
	};
	std::vector<ChatBoard>		_stack_text;
private:
	int			   p_current_rank;
	int			   p_current_seat;
	bool		   p_is_owner;
	s64			   p_current_money;
	Vec2		   p_pos_bonus_money;

	bool		   p_is_lock_chat_action;
	void			ShowChat();

public:
	void	SetPosBonus(Vec2 pos);
	void	SetCurrentGameScreen(GameScreen * gr);
	bool	IsOwner();
	int		GetIDDB();
	int		GetSeat();


	PlayerGroup(WidgetEntity * widget);
	PlayerGroup();

	virtual ~PlayerGroup();
	void	Update(float dt);
	void	setReady(bool ready);
	void	setVisible(bool isVisible);
	void	setPosition(Vec2 pos);
	/*
	* return true if player is null or same id, false if not same id
	*/
	void setPlayer(Player* p, int idx = -1);

	void	removePlayer();
	Player* getPlayer();
	void	start(long totalTime, long currentTime);
	void	startTime(int interval, int param2);
	void	stopTime();

	void	setOwner(bool param1);
	void	setMoney(s64 money);
	void	setRankNumber(int rank);
	void	setIsDenyTurn(bool isDeny);
	void	showFlare(bool visible);



	cocos2d::Rect GetRectFront();

	WidgetEntity * getWidget() { return _widget; }

	void SetTextChat(RKString text,bool isChatBoard = true);
	void HideRank();
	void ShowLayerPlayer(bool isShow);
	void RunTurnEffect(CallFunc * call_back = nullptr);

	void SetXMLData(xml::UILayerWidgetDec* xml_dec, int id);
	void SetAvatar();
	void OnSetData(User* data);

	s64 GetCurrentMoney();
	void SetCurrentMoney(s64 value);

	void SetActionMoneyRunTo(s64 toMoney);
	void SetActionAppearBonusMoney(s64 bonus_money);
	void setQuickPlay(bool isQuickPlay);

	void ShowIconWaiting(bool visible);
};
#define _WIDGET(x) x->getWidget()
#define _PLAYER(x) x->getPlayer()
#endif //__PLAYER_GROUP_H__


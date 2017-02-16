#ifndef __SCREEN_LOBBY_H__
#define __SCREEN_LOBBY_H__

#include "Network/JsonObject/Game/BoardOwner.h"
#include "ScreenManager.h"

using namespace iwinmesage;

class LobbyScreen : public BaseScreen
{
private:
	WidgetEntity * p_list_player_lobby;
	WidgetEntity * p_panel_create_room;
	WidgetEntity * p_list_number_player_choose;

	std::vector<BoardOwner> p_list_player_in_lobby;
	cocos2d::Map<std::string , Sprite*>	p_list_avatar_download;

	WidgetEntity * p_layout_right;
	WidgetEntity * p_panel_avatar;

	WidgetEntity * p_slider_choose_bet;
	WidgetEntity * p_slider_bet_create_room;

	s64 p_current_money_bet;
	int p_delta_choose_bet;
	s64 p_min_money;
	s64 p_max_money;

	int p_current_game_id;
	int p_previous_game_id;

	bool		_have_return_from_game_screen;
protected:
	
	void ResetLeaderboard(bool cleanup = true);

	void ShowRichestWin();
	void ShowRichestRuby();
	void ShowStrongest();

	void OnParseLobbyRoom();
public:
	void SetHaveComeFromGame();
	void SetCurrentGameID(int id);
protected:
	void InitTextFieldValue();
	void CleanUpListAvatar();
public:
	LobbyScreen();
	virtual ~LobbyScreen();

	virtual  int	Init()																		override;
	virtual  int	InitComponent()																override;
	virtual  int	Update(float dt)															override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()														override;
	virtual void OnBeginFadeIn()																override;
	virtual void OnFadeInComplete()																override;
	virtual void ClearComponent()																override;
	virtual void ParseUserUI()																	override;
	virtual void OnKeyBack()																	override;
	virtual void PlayMusic()																	override;

	void doRequestRoom(int money);
	void onSetGameTypeSuccessed();
	void onReceiveOwnerBoardList(std::vector<BoardOwner> boards, int win, int lose, int draw, int leave);

	void SetTextTitle(RKString title);
	void SetBoardLevel(WidgetEntity * text, int level);
	void SetNumberPlayerInBoard(WidgetEntity * panel, int number_player, int max_player);

	void SetInfoWidgetList(WidgetEntity * panel, int idx);
};

#endif //__SCREEN_LOBBY_H__


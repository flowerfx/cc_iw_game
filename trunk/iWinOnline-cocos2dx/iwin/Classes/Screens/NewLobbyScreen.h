#ifndef __NEW_SCREEN_LOBBY_H__
#define __NEW_SCREEN_LOBBY_H__
#include "Network/JsonObject/Game/BoardOwner.h"
#include "Network/JsonObject/lobby/LobbyList.h"
#include "Network/JsonObject/lobby/ChatLobbyList.h"
#include "ScreenManager.h"

using namespace iwinmesage;


class IwinListView;
//class EmotionBoard;
class ChatBoard;

class NewLobbyScreen : public BaseScreen
{
public:
	static const int RUBY_TYPE = 2;
	static const int WIN_TYPE = 1;
private:
	
	WidgetEntity*		p_scrollview_bet;

	WidgetEntity*		p_scrollview_chat;
	WidgetEntity*		p_scrollitem_chat;
	WidgetEntity*		p_panel_right_lobby;
	WidgetEntity*		p_panel_right_board;
	WidgetEntity*		p_panel_user_info;
	WidgetEntity*		p_chat_text_field;
	WidgetEntity*		p_list_player_lobby;
	WidgetEntity*		p_money_value;
	WidgetEntity*		p_bet_slider;
	WidgetEntity*		p_img_history_normal;
	WidgetEntity*		p_img_history_active;
	WidgetEntity*		p_panel_history;
	WidgetEntity*		p_list_history;

	IwinListView*		_listview_board;

	WidgetEntity*		p_top_panel;
	WidgetEntity*		p_panel_left;

	ChatBoard*			_chat_board;
	LobbyList*			_cur_lobby_list_data;
	std::vector<BoardOwner>	_cur_list_board;

	bool		_visible_board;
	s64			_max_money_slider;
	s64			_current_bet;
	bool		_isRoomHistory;

	cocos2d::Map<std::string, Sprite*>	p_list_avatar_download;
	cocos2d::Map<int, Sprite*>			p_list_avatar_copho_rival;
	std::vector<ChatLobbyItem*>			p_list_text_chat;

	bool		_is_textfield_chat_focus;
	bool		_have_return_from_game_screen;
public:
	NewLobbyScreen();
	virtual ~NewLobbyScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnBeginFadeIn()															override;
	virtual void OnFadeInComplete()															override;
	virtual void OnBeginFadeOut()															override;
	virtual void ParseUserUI()																override;
	virtual void ClearComponent()															override;
	virtual void SetActionAfterShow(int action)												override;
	virtual void OnKeyBack()																override;
	virtual void PlayMusic()																override;

	void onSetGameTypeSuccessed();

	void updateItemScrollViewBet(WidgetEntity* entity, LobbyItem* item, int idx);

	void updateItemListViewBoard(WidgetEntity* entity, int idx);
	void updateItemHistory(WidgetEntity* entity, int idx);
	void onReceiveLobbyList(LobbyList* lobby_list);
	void onReceiveOwnerBoardList(std::vector<BoardOwner> boards, int win, int lose, int draw, int leave);
	void onReceiveChatList(ChatLobbyList* chatLobbyList);
	void handleChatEmotion(int idx);
	void setTitle(std::string text);
	void SetCurrentGameID(int id);
	void showHistory(bool visible);
	int GetStateNewLobbyMenu();

	void SetPopularBet();
	void SetHaveComeFromGame();

	void SetValueIntoBetUI(s64 money);

private:
	void showListBoard(bool visible);
	void showLobby(int type);
	void updateNoBoardLabel(s64 money);

	int p_current_game_id;
	int p_previous_game_id;
protected:

	void ResetLeaderboard(bool cleanup = true);

	void ShowRichestWin();
	void ShowRichestRuby();
	void ShowStrongest();

	void OnParseLobbyRoom();
	void OnParseListLobbyBet(WidgetEntity * _widget, int idx_lobby);  
    void OnFocusTextFieldChat(bool value);
	void OnShowListBoardGame(bool value);
	void OnReloadBoardGame(int moneyType, int money);

	void OnSetSliderBet();

	void InitTextFieldValue();
};

#endif //__NEW_SCREEN_LOBBY_H__

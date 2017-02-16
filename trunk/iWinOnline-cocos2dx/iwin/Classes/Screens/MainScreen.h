#ifndef __MAIN_SCREEN_H__
#define __MAIN_SCREEN_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;

enum LIST_GAME
{
	GAME_GAMBLE = 0,
	GAME_CHESS,
	GAME_OTHER,
	GAME_HOT,
	GAME_NEW,
	GAME_UPDATE,
	GAME_TOURNAMENT,
	GAME_POPULAR
};

struct ListGame
{
	LIST_GAME type;
	std::vector<int> list_game;

	ListGame()
	{
		list_game.clear();
	}
	ListGame(LIST_GAME t , std::vector<int> list)
	{
		list_game = list;
		type = t;
	}
	virtual ~ListGame()
	{
		list_game.clear();
	}
};

namespace iwinmesage
{
	class GameList;
}
class GameObjectData;
class MainScreen : public BaseScreen
{
private:
	WidgetEntity * p_bottom_menu;
	WidgetEntity * p_panel_setting;
	WidgetEntity * p_panel_list_game;

	//
	WidgetEntity * p_list_btn_game_win;
	WidgetEntity * p_list_btn_game_ruby;

	std::map<LIST_GAME, ListGame> p_list_game_win;
	std::map<LIST_GAME, ListGame> p_list_game_ruby;

	iwinmesage::GameList*		p_gamelist;


	//
	WidgetEntity * p_list_server;
	WidgetEntity * p_panel_choose_server;
	WidgetEntity * p_panel_list_btn_choose_game;
	WidgetEntity * panel_avatar_user;
	WidgetEntity * p_list_lang_choose;

	WidgetEntity * p_panel_list_btn;

	WidgetEntity * p_panel_notice_12;
	WidgetEntity * p_layout_anoun_server;

	WidgetEntity* p_panel_info_game;
	WidgetEntity* p_panel_cd_change_server;

	int p_stage_get_data;
	//handle annouce server appear and run
	RKString	p_announce_text;
	bool		p_annouce_is_running;
	bool		p_force_stop_annouce;
	float		p_speed_move;

	//dynamic generate list game selected
	std::map<RKString, xml::UILayerWidgetDec*> p_list_sample_dec;
	std::map<int, GameObjectData*> p_list_game_id;

	float p_current_time_request_list_wait;
	bool  p_already_parse_list_wait;


	//merge from libgdx
	bool  p_isCallRequestFriend;
	// check have dialog log out
	bool p_have_layout_logout_appear;
	// check have count down change server
	float	p_timer_count_down;
	bool	p_on_count_down;

protected:
	void SetAnnouncementAction();
	void OnShowHLBtnGameAtIdx(int idx);
	void OnParsePlayerNumberToEachGame();
	void OnShowChangeServer();

	void OnShowListChooseLang(bool visible);

	void OnParseGameList();

	void OnShowHLBtnListGame(int idx);
	void OnShowListBtnGame(bool value);
	void InsertGameType(LIST_GAME type, const std::vector<int> & list_game_win, const std::vector<int> & list_game_ruby);
	//handle generate list btn select type game list at top
	void GenerateBtnListGameType(LIST_GAME type , int money_type );
	void ClearListBtnGame();
	//
	RKString GetIDNameGameByType(LIST_GAME type);

	void OnVisiblePanelSetting(bool value, const std::function<void(void)> & call_back);
	void OnVisiblePanelServer(bool value, const std::function<void(void)> & call_back);
    
    void logoutByType(int loginType);

    
public:
	MainScreen();
	virtual ~MainScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnBeginFadeIn()															override;
	virtual void OnFadeInComplete()															override;
	virtual void OnTouchMenuBegin(const cocos2d::Point & p)											override;
	virtual void ParseUserUI()																override;
	virtual void OnReceiveFriendList()														override;
	virtual void SetActionAfterShow(int action)												override;
	virtual void ClearComponent()															override;
	virtual void OnKeyBack()																override;
	virtual void PlayMusic()																override;


	void onReceivedGameList(void* gamelist);

	void LoadXMLListGame(TiXmlDocument * p_objectXML);
	void OnParseCustomUIDec(vector<int> list_game, RKString name ,std::map<RKString, xml::UILayerWidgetDec*> p_list_sample_dec);
	
	void OnReceiveAnnouncementFromServer(RKString text);
	iwinmesage::GameList* GetGameList();

	void onReceiveAvatarCatList(void * data);
	void onReceiveAvatarList(void * data);
	void onBuyingAvatarResult(void * data);
	void onReceiveListWait(void * data1, void* data2, void* data3);

	void forceStopAnnouce(bool value);
	void SetNumWinfree(int number);
	void SetNumUnReadMsg(int number);

	int GetStateMainScreen();

	void ChangeToNewServer();
	bool OnChangingServer();

	cocos2d::Rect GetWorldObjectPosIntutorial(int state, WidgetEntity * &obj);

	void SetBtnInviteNotifySelected(bool value);
	void logout();

	void UpdateServerList();
};

#endif //__MAIN_SCREEN_H__


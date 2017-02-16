#ifndef __SCREEN_LEADERBOARD_H__
#define __SCREEN_LEADERBOARD_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;

struct ListGameDetail;
class Player;
class LeaderboardScreen : public BaseScreen
{
private:

	WidgetEntity * p_panel_list_choose;
	WidgetEntity * p_panel_list_player;

	WidgetEntity * p_list_choose_leaderboard;
	WidgetEntity * p_list_player_leaderboard;
	WidgetEntity * p_list_friend_leaderboard;

	WidgetEntity * p_layout_top_list_choose;
	WidgetEntity * p_layout_top_list_player;

	WidgetEntity *	p_tab_change_btn;

	WidgetEntity * p_panel_user;

	bool			p_is_on_tab_world; //otherwise is tab friend;

	bool			p_already_set_listgamelb;

	std::vector<Player*>				p_temp_list_player_lb;
	std::vector<Player*>				p_temp_list_friend_lb;

	std::vector<Player*>				p_current_list_player_lb;
	std::vector<Player*>				p_current_list_friend_lb;
	cocos2d::Map<int, Sprite*>			p_list_avatar_download;
	cocos2d::Map<int, Sprite*>			p_list_avatar_friend;

	size_t		p_current_page_;

	int			p_current_rank_idx;
	int			p_current_rank_idx_friend;

	int			p_current_stage_lb;
	int			p_current_game_id;

	int			p_id_player;
	int			p_id_friend;
protected:

	void OnParsePlayerToLeaderboard(WidgetEntity * widget, int idx , bool isPlayer = true);
	void CleanUpListAvatar();
	void ResetLeaderboard(int state);

	void ParseLBPlayer();
	void ParseLBFriend();
public:
	LeaderboardScreen();
	virtual ~LeaderboardScreen();

	virtual  int	Init()																			override;
	virtual  int	InitComponent()																	override;
	virtual  int	Update(float dt)																override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)	override;
	virtual void OnDeactiveCurrentMenu()															override;
	virtual void OnBeginFadeIn()																	override;
	virtual void ParseUserUI()																		override;
	virtual void ClearComponent()																	override;
	virtual void OnKeyBack()																		override;
	virtual void onLanguageChanged(LANGUAGE_TYPE lang)												override;

	void CreateListGameLeaderboard(ListGameDetail lg , int idx);

	void OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player);

	void OnReceiveListFriend(int id, ubyte page, std::vector<Player * > list_player);

	int CurrentStateLeaderboard();
};

#endif //__SCREEN_LEADERBOARD_H__


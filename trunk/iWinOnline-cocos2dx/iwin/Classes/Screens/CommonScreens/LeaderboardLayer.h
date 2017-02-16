#ifndef __LEADERBOARD_LAYER_H__
#define __LEADERBOARD_LAYER_H__
#include "Screens/CommonScreens/CommonLayer.h"
using namespace Utility;
using namespace Utility::UI_Widget;
using namespace iwinmesage;

class Player;
class LeaderboardLayer : public CommonLayer
{
private:
	/*
	use : leaderboard layout
	*/
	//
	WidgetEntity * p_lb_layout_leaderboard;
	WidgetEntity * p_lb_layout_leaderboard_player;

	std::vector<Player*>				p_lb_temp_player;
	std::vector<Player*>				p_lb_current_list_player_;
	cocos2d::Map<int, Sprite*>			p_lb_list_avatar_player_download;

	size_t		p_lb_current_page_lb;

	int			p_lb_current_rank_lb_idx;
	int			p_lb_current_stage_lb;
	int			p_lb_previous_state_lb;

	int			p_current_game_id;

protected:
	Player* LB_GetPlayerByIdx(int idx);

	void LB_OnSetPlayerEachLB(
		std::vector<Player * > list_player,
		int & idx_rank,
		WidgetEntity * list_table,
		cocos2d::Map<int, Sprite*> * list_avatar);
	void LB_OnParsePlayerToLeaderboard(WidgetEntity * widget, int idx, bool isPlayer = true);

public:
	LeaderboardLayer();
	virtual ~LeaderboardLayer();

	virtual void InitLayer(BaseScreen * common_screen) override;
	virtual int  Update(float dt) override;
	virtual bool IsVisibleLayer() override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;

	void LB_ResetLeaderboard(bool cleanup = true);
	void LB_OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player);
	void LB_ShowRichestWin();
	void LB_ShowRichestRuby();
	void LB_ShowStrongest(ubyte current_game_id);
	void LB_OnTouchMenuBegin(const cocos2d::Point & p);
	int GetStateLB();


};

#endif //__LEADERBOARD_LAYER_H__


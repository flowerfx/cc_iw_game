#ifndef __MINI_POKER_H__
#define __MINI_POKER_H__

#include "Screens/CommonScreens/CommonLayer.h"
#include "RKString_Code/RKString.h"
using namespace Utility;
using namespace Utility::UI_Widget;

namespace iwinmesage
{
	class NoHuGameInfo;
	class NoHuBetResult;
	class NoHuUserHistoryServer;
	class NoHuListUser;
	class NoHuUser;
	class NoHuUserHistoryDetail;
	class NoHuJar;
}

class IwinListItemCircle;
class MiniPokerLayer : public CommonLayer
{
private:
	WidgetEntity * p_mini_poker_panel;

	WidgetEntity * p_mini_poker_list_player_layout;
	WidgetEntity * p_mini_poker_myhistory_layout;

	WidgetEntity * p_poker_listplayer_listview;
	WidgetEntity * p_poker_myhistory_listview;

	//top layer handle
	WidgetEntity * p_poker_top_layer;
	WidgetEntity * p_title_top_layer;

	iwinmesage::NoHuGameInfo * _nohu_game_info;
	iwinmesage::NoHuBetResult * _nohu_bet_result;
	iwinmesage::NoHuUserHistoryServer * _nohu_my_history;
	iwinmesage::NoHuListUser* _nohu_list_user;

	//handle top list user
	int _page_user;

	//handle my history
	int _page_my_history;

	//handle drag stick
	WidgetEntity * p_stick_sphere;
	WidgetEntity * p_stick_pivot;
	WidgetEntity * p_stick_layout;

	float	_delta_move_stick_tx;
	Vec2	_previous_pos_touch;
	Vec2	_origin_size_stick;

	bool	_auto_bet_nohu;

	//handle center top
	WidgetEntity * p_center_top_layer;
	WidgetEntity * p_light_animate;
	WidgetEntity * p_listview_jars;
	WidgetEntity * p_listview_jars_ruby;
	WidgetEntity * p_text_total_money;

	IwinListItemCircle * p_list_circle_jar;
	IwinListItemCircle * p_list_circle_jar_ruby;

	//handle rotate card
	WidgetEntity * p_rotate_card_layer;
	Map<int, Sprite*> p_list_img_jar;
	Map<int, Sprite*> p_list_img_jar_ruby;
		//happend when rotate card finish;
	s64			   _money_bonus;
	bool		   _have_first_set_rotate_card;

	//handle fly money user
	WidgetEntity * p_fly_money_layout;
	WidgetEntity * p_sample_money_fly;

	//effect no hu
	WidgetEntity * p_effect_nohu_layer;
	WidgetEntity * p_small_effect_nohu_layer;
	WidgetEntity * p_small_particle_nohu;
	WidgetEntity * p_large_effect_nohu_layer;
	WidgetEntity * p_large_particle_nohu;

	int			   _id_win_no_hu;
	bool		   _have_no_hu;
	s64			   _money_receive_no_hu;
protected:

	void OnConvertToIcon(WidgetEntity * icon, WidgetEntity * text, int value_card);
	//top player
	void OnShowListPlayerPokerLayout(bool value);
	void OnParseListPlayerNohu(WidgetEntity * widget, const iwinmesage::NoHuUser & user, int idx);
	//my history
	void OnShowListMyHistoryPokerLayout(bool value);
	void OnParseListHistoryNohu(WidgetEntity * widget, const iwinmesage::NoHuUserHistoryDetail & his, int idx);
	//stick
	void OnDragStick();
	//return true is bet succeed , otherwise is failed
	bool OnBetJarAtIdx(int idx);
	void OnRunActionBetJar();
	//rotate card
	void OnRotateCardFinish();
	void RotateCardAtIdxLayer(int idx , int id_to);
	void OnParseCardDetailToWidget(WidgetEntity * card, int idx);
	void OnParseIcon(WidgetEntity * big_ico, int name, int element);
	//parse and show jar
	//jar win
	void OnParseJarDetail(WidgetEntity * widget, const iwinmesage::NoHuJar & jar, int idx);
	//jar ruby
	void OnParseJarRubyDetail(WidgetEntity * widget, const iwinmesage::NoHuJar & jar, int idx);

	//set win and ruby
	void RefreshUserMoney();

	//change state ruby or win
	void ChangeStateMoneyNohu(int state);

	//run total money
	void RunTotalMoney();

	//fly money
	void OnHandleFlyUserMoney();
	void ClearObjectFly();

	//effect nohu

	void OnRunEffectNoHu(bool isLargeNohu = true);
	const RKString GetStringWinID(int winID);
	void ResetParamNohuEffect();
public:
	MiniPokerLayer();
	virtual ~MiniPokerLayer();

	virtual void InitLayer(BaseScreen * screen)		override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual int Update(float dt) override;

	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;

	void OnShowMiniPokerLayout(bool value);
	void ParseUserUI();
	void SetMoneyBonusNoHu(const s64 & money);

	void onReceivedNoHuGameInfo(void * data);
	void onReceivedNoHuBet(void * data);
	void onReceivedNoHuUserHistory(void * data);
	void onReceivedNoHuListUser(void * data);
};

#endif //__MINI_POKER_H__


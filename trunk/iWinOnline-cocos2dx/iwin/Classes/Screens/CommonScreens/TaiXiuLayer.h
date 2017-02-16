#ifndef __TAIXIU_LAYER_H__
#define __TAIXIU_LAYER_H__
#include "Screens/CommonScreens/CommonLayer.h"
using namespace Utility;
using namespace Utility::UI_Widget;
namespace iwinmesage
{
	class TaiXiuGameInfo;
	class TaiXiuBetResult;
	class TaiXiuUserOrderList;
	class TaiXiuUserOrder;
	class TaiXiuUserHistoryList;
	class TaiXiuUserHistory;
	class TaiXiuUserBet;
	class TaiXiuMatch;
	class TaiXiuMatchList;
}

class TaiXiuLayer : public CommonLayer
{
private:

	WidgetEntity * p_layout_taixiu;

	WidgetEntity * p_layout_tai_xiu_panel;

	WidgetEntity * p_layout_taixiu_leaderboard;
	WidgetEntity * p_taixiu_list_player_leaderboard;

	WidgetEntity * p_layout_taixiu_myhistory;
	WidgetEntity * p_taixiu_list_myhistory;

	WidgetEntity * p_layout_taixiu_match_history;
	WidgetEntity * p_taixiu_list_match_history;

	WidgetEntity * p_layout_taixiu_chart;
	WidgetEntity * p_chart_node;
	WidgetEntity * p_chart_node_e;
	WidgetEntity * p_line_node;
	WidgetEntity * p_list_node_layout;
	WidgetEntity * p_layout_dice_chart_top;
	WidgetEntity * p_layout_dice_chart_bot_dice1;
	WidgetEntity * p_layout_dice_chart_bot_dice2;
	WidgetEntity * p_layout_dice_chart_bot_dice3;

	WidgetEntity * p_main_taixiu_layout;
	WidgetEntity * p_panel_taixiu_bg_top_layout;
	WidgetEntity * p_panel_taixiu_bg_history;

	WidgetEntity * p_layout_taixiu_bet;
	WidgetEntity * p_text_effect_bet_money;
	WidgetEntity * p_text_effect_message_server;

	WidgetEntity * p_taixiu_circle_layout;
	WidgetEntity * p_taixiu_circle_result_layout;
	WidgetEntity * p_taixiu_circle_dice_layout;
	WidgetEntity * p_taixiu_circle_small_result_layout;
	WidgetEntity * p_taixiu_circle_cd_layout;


	iwinmesage::TaiXiuGameInfo *p_taixiu_gameinfo;

	iwinmesage::TaiXiuBetResult *p_taixiu_bet_result;
	iwinmesage::TaiXiuUserOrderList *p_taixiu_userorder_list;
	iwinmesage::TaiXiuUserHistoryList *p_taixiu_history_list;
	iwinmesage::TaiXiuMatch *p_taixiu_match;
	iwinmesage::TaiXiuMatchList *p_taixiu_match_list;

	WidgetEntity * p_taixiu_new_user_bet_layout;
	WidgetEntity * p_taixiu_fly_user_layout;

	//-1 is default 0 is tai 1 is xiu
	int current_stage_bet_taixiu;
	//
	int _cur_match_id;
	int _live_match_id;
	// 0 is detail 1 is total 2 is total near
	int _idx_chart_panel;
	//
	int		p_result_taixiu;
	int		p_previous_result_taixiu;

	bool	p_taixiu_already_have_result;

	//
	bool	p_have_update_new_history;
	bool	p_have_change_tab_taixiu;
protected:

	void ResetAndSendSerVer();
	void ChangeButtonWinRubyTaiXiu();
	void OnCheckTextInputMoney(WidgetEntity * _widget);
	void OnShowPanelBetTaixiu(bool show, CallFunc * call_back = nullptr);
	WidgetEntity * GetSliderBet();
	void SetInputMoneyTextBet(s64 money);
	s64 GetCurrentMoney();
	void BetMoney(int stage);
	void SetNumberBetResult(s64 money, int state);
	void OnShowResultTaiXiu(int result, bool show);
	void OnParseTaiXiuLeaderboard(WidgetEntity* _widget, iwinmesage::TaiXiuUserOrder * user, int idx);
	void OnParseTaiXiuHistory(WidgetEntity* _widget, iwinmesage::TaiXiuUserHistory * his, int idx);
	void OnParseTaiXiuMatchHistory(WidgetEntity* _widget, iwinmesage::TaiXiuUserBet * his, int idx);

	void OnShowResultCirlceWithTimer(bool show, double long total_time, double long current_time);
	void OnShowResultCircle();

	void OnResetTaiXiu();

	void OnInsertNodeChartToList(int i, int j);
	void OnInsertNodeToTopDice(int i);
	void OnInsertNodeToBotDice(int i , int idx_dice);

	void OnShowChartPanelAtIdx(int idx);
	void OnShowChartListTotal();
	void OnShowChartListTotalNear();
	void OnShowChartDetail();

	WidgetEntity * GetNodeChartAtPoint(int i, int j);
	void OnParseChartDice(int idx_dice);
	void OnShowChartDice(int idx_dice, bool visible);
public:
	TaiXiuLayer();
	virtual ~TaiXiuLayer();

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual int  Update(float dt)	override;
	virtual bool IsVisibleLayer()	override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back)		override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back)		override;


	void OnReceivedTaiXiuInfo(iwinmesage::TaiXiuGameInfo * taixiu_info, ubyte type);
	void OnReceivedTaiXiuBetResult(TaiXiuBetResult* taiXiuBetResult);
	void OnReceivedTaiXiuUserOrderList(TaiXiuUserOrderList * taixiuOderList);
	void OnReceivedTaiXiuUserHistoryList(TaiXiuUserHistoryList* taixiuHistoryList);
	void OnReceiveTaiXiuMatch(TaiXiuMatch * data);
	void OnReceiveTaiXiuMatchList(TaiXiuMatchList* data);

	void ParseUserUI();
	int GetStateTaiXiu();

	void OnShowTaiXiuLayoutAtIdx(int idx);

	void OnShowTaiXiuLayout(bool value);
};

#endif //__TAIXIU_LAYER_H__


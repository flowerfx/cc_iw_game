#ifndef __COMMON_SCREEN_H__
#define __COMMON_SCREEN_H__

#include "ScreenManager.h"

using namespace iwinmesage;
using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
class CommonLayer;

enum COMMON_LAYER
{
	LAYER_NONE = 0,
	DIALOG_LAYER,
	ANNOUNCE_LAYER,
	CHANGE_PASS_LAYER,
	PLAYER_INFO_LAYER,
	CONFIRM_CODE_LAYER,
	INVITE_LAYER,
	TAIXIU_LAYER,
	TUTORIAL_LAYER,
	LUCKY_CIRCLE_LAYER,
	LEADERBOARD_LAYER,
	EMOTION_LAYER,
	TOAST_LAYER,
	MONEY_CHANGE_LAYER,
	MINI_BTN_LAYER,
	MINI_POKER_LAYER,
	LOADING_CIRCLE_LAYER,
	LAYER_COUNT
};


namespace iwinmesage
{
	class TaiXiuGameInfo;
	class TaiXiuBetResult;
	class TaiXiuUserOrderList;
	class TaiXiuUserHistoryList;
	class TaiXiuMatch;
	class TaiXiuMatchList;

	class LuckyCircleResultList;
	class LuckyCircleQuestList;
}
class CommonScreen : public BaseScreen
{
private:
	std::map<COMMON_LAYER , CommonLayer*>	p_list_common_layer;
	BaseScreen * p_screen_come_from;
	Factory::FactoryWithIndex<CommonLayer>	_facetory_common_layer;
protected:
	void RegisterCommonLayer();
public:
	CommonScreen();
	virtual ~CommonScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;

	virtual void	OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void	OnDeactiveCurrentMenu()													override;
	virtual void	OnBeginFadeIn()															override;
	virtual void	OnFadeInComplete()														override;
	virtual void	OnTouchMenuBegin(const cocos2d::Point & p)										override;
	virtual void	ParseUserUI()															override;
	virtual void	SetActionAfterShow(int action)											override;

	int				GetStateCommonScr();
	CommonLayer*	GetLayer(COMMON_LAYER layer);
public:
	//show general common layer
	void OnShowLayer(COMMON_LAYER layer);
	//hide
	void OnHideLayer(COMMON_LAYER layer);
	//
	bool IsLayerVisible(COMMON_LAYER layer);

	//annouce server
	void OnReceiveAnnouncementFromServer(RKString text);
	void ForceStopAnnouce(bool value);
	//invite layer
	void OnSetDataInvite(int gameid, int roomid, int boardid, RKString pass, RKString message, int idavatar);

	//dialog layer
	void OnShowDialog(
		const RKString & str_title = "",
		const RKString & str_message = "",
		TYPE_DIALOG type = TYPE_DIALOG::DIALOG_NONE,
		const char * strCB = "",
		const std::function<void(const char * call_back, const char* btn_name)> & func = nullptr,
		const std::function<void(const char * call_back, const char* btn_name)> & func1 = nullptr,
		const std::function<void(const char * call_back, const char* btn_name)> & func2 = nullptr,
		const RKString & title_btn_0 = "",
		const RKString & title_btn_1 = ""
	);
	void OnHideDialog();

	//player info
	void OnShowPlayerInfo(Player * player);
	void OnSetUserPlayer(User * user);
	void OnReceiveAchievement(int id, void * list_achie);

	//leaderboard
	void LB_ResetLeaderboard(bool cleanup = true);
	void LB_OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player);
	void LB_ShowRichestWin();
	void LB_ShowRichestRuby();
	void LB_ShowStrongest(ubyte current_game_id);

	//lucky circle
	void OnRecieveLuckyCircleResultList(LuckyCircleQuestList* lucky_quest_list);
	void OnRecieveLuckyCircleResultItem(LuckyCircleResultList* lucky_result_item);

	//taixiu
	void OnReceivedTaiXiuInfo(iwinmesage::TaiXiuGameInfo * taixiu_info, ubyte type);
	void OnReceivedTaiXiuBetResult(TaiXiuBetResult* taiXiuBetResult);
	void OnReceivedTaiXiuUserOrderList(TaiXiuUserOrderList * taixiuOderList);
	void OnReceivedTaiXiuUserHistoryList(TaiXiuUserHistoryList* taixiuHistoryList);
	void OnReceivedTaiXiuMatch(TaiXiuMatch * taixiuMatch);
	void onReceiveTaiXiuMatchList(TaiXiuMatchList * taixiuMatchList);

	//mini btn
	void OnShowBtnMini(bool show);
	void OnEnableBtnNoHu(bool value);
	cocos2d::Rect GetRectMiniBtn(WidgetEntity * &obj);

	//emotion layer
	void SetCallBackEmotionBoard(const std::function<void(int)> & func);

	void OnShowToast(std::string content);

	//bool have layer show
	bool HaveAnyLayerShow();

	//handle backkey
	virtual void OnKeyBack() override;

	//money change layer
	void OnShowBonusMoney(s64 money_change, bool isBonus, int typeMoney);

	//nohu layer
	void onReceivedNoHuGameInfo(void * data);
	void onReceivedNoHuBet(void * data);
	void onReceivedNoHuUserHistory(void * data);
	void onReceivedNoHuListUser(void * data);
	void onSetMoneyBonusNohu(const s64 & money);
};

#endif //__COMMON_SCREEN_H__


#include "CommonScreen.h"

#include "Network/Global/Globalservice.h"
#include "Utils.h"

#include "Screens/CommonScreens/TaiXiuLayer.h"
#include "Screens/CommonScreens/LuckyCircleScreen.h"
#include "Screens/CommonScreens/LeaderboardLayer.h"
#include "Screens/CommonScreens/DialogLayer.h"
#include "Screens/CommonScreens/PlayerInfoLayer.h"
#include "Screens/CommonScreens/ChangePassLayer.h"
#include "Screens/CommonScreens/InviteLayer.h"
#include "Screens/CommonScreens/AnnouceServerLayer.h"
#include "Screens/CommonScreens/ConfirmCodeLayer.h"
#include "Screens/CommonScreens/TutorialLayer.h"
#include "Screens/CommonScreens/EmoBoardLayer.h"
#include "Screens/CommonScreens/ToastLayer.h"
#include "Screens/CommonScreens/BonusMoneyLayer.h"
#include "Screens/CommonScreens/MiniBtnLayer.h"
#include "Screens/CommonScreens/MiniPokerLayer.h"
#include "Screens/CommonScreens/LoadingCircleLayer.h"


#include "Common/IwinListViewTable.h"
#include "Common/IwinTextfield.h"
CommonScreen::CommonScreen()
{

	p_screen_come_from = nullptr;

	p_list_common_layer.clear();
}

CommonScreen::~CommonScreen()
{
	p_screen_come_from = nullptr;

	for (auto it : p_list_common_layer)
	{
		delete it.second;
	}
	p_list_common_layer.clear();
}

int CommonScreen::Init()
{
	//override the list view table default with the iwinlistview table
	this->_func_action = [&](LayerEntity * layer) {
		layer->OverloadRegisterUIWidget<IwinListViewTable>(xml::WIDGET_LIST_VIEW_TABLE);
		layer->OverloadRegisterUIWidget<IwinTextFieldHandle>(xml::WIDGET_TEXT_FIELD);
		layer->OverloadRegisterUIWidget<IwinEditBoxdHandle>(xml::WIDGET_EDIT_BOX);
	};
	//
	RKString _menu = "common_screen";
	InitMenuWidgetEntity(_menu);
	CallInitComponent(true);
	return 1;
}

void CommonScreen::RegisterCommonLayer()
{
	_facetory_common_layer.RegisterProduct<ChangePassLayer>(COMMON_LAYER::CHANGE_PASS_LAYER);
	_facetory_common_layer.RegisterProduct<AnnouceServerLayer>(COMMON_LAYER::ANNOUNCE_LAYER);
	_facetory_common_layer.RegisterProduct<PlayerInfoLayer>(COMMON_LAYER::PLAYER_INFO_LAYER);
	_facetory_common_layer.RegisterProduct<ConfirmCodeLayer>(COMMON_LAYER::CONFIRM_CODE_LAYER);
	_facetory_common_layer.RegisterProduct<DialogLayer>(COMMON_LAYER::DIALOG_LAYER);
	_facetory_common_layer.RegisterProduct<InviteLayer>(COMMON_LAYER::INVITE_LAYER);
	_facetory_common_layer.RegisterProduct<LuckyCircleScreen>(COMMON_LAYER::LUCKY_CIRCLE_LAYER);
	_facetory_common_layer.RegisterProduct<LeaderboardLayer>(COMMON_LAYER::LEADERBOARD_LAYER);
	_facetory_common_layer.RegisterProduct<TaiXiuLayer>(COMMON_LAYER::TAIXIU_LAYER);
	_facetory_common_layer.RegisterProduct<TutorialLayer>(COMMON_LAYER::TUTORIAL_LAYER);
	_facetory_common_layer.RegisterProduct<EmoBoardLayer>(COMMON_LAYER::EMOTION_LAYER);
	_facetory_common_layer.RegisterProduct<ToastLayer>(COMMON_LAYER::TOAST_LAYER);
	_facetory_common_layer.RegisterProduct<BonusMoneyLayer>(COMMON_LAYER::MONEY_CHANGE_LAYER);
	_facetory_common_layer.RegisterProduct<MiniBtnLayer>(COMMON_LAYER::MINI_BTN_LAYER);
	_facetory_common_layer.RegisterProduct<MiniPokerLayer>(COMMON_LAYER::MINI_POKER_LAYER);
	_facetory_common_layer.RegisterProduct<LoadingCircleLayer>(COMMON_LAYER::LOADING_CIRCLE_LAYER);
}

int	CommonScreen::InitComponent()
{
	//
	p_IdxMenuData = 0;
	RegisterCommonLayer();
	//create and init list layer common
	for (int i = COMMON_LAYER::DIALOG_LAYER; i < COMMON_LAYER::LAYER_COUNT; i++)
	{
		auto val = _facetory_common_layer.CreateNewProduct((COMMON_LAYER)i);
		if (val)
		{
			val->InitLayer(this);
			p_list_common_layer.insert({ (COMMON_LAYER)i , val });
		}
	}
	

	return 1;
}

CommonLayer* CommonScreen::GetLayer(COMMON_LAYER layer)
{
	if (p_list_common_layer.find(layer) != p_list_common_layer.end())
	{
		return p_list_common_layer.at(layer);
	}
	return nullptr;
}

int CommonScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	for (auto it : p_list_common_layer)
	{
		it.second->Update(dt);
	}

	return 1;
}


void CommonScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	for (auto it : p_list_common_layer)
	{
		bool res = it.second->ProcessWidget(name, type_widget, _widget);
		if (res)
		{
			return;
		}
	}

	if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		auto w_tx = static_cast<IWIN_TF*>(_widget);
		w_tx->HandleEventTextField(name);
	}
}

void CommonScreen::OnDeactiveCurrentMenu()
{
	PASSERT2(false, "this is the specific screen that always show on top screen and not list in the list screen UI of screen manager");
	return;

	ScrMgr->CloseCurrentMenu(COMMON_SCREEN);

	p_menu_show_next = MENU_NONE;

}

void CommonScreen::OnBeginFadeIn()
{
}

void CommonScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
}

void CommonScreen::OnTouchMenuBegin(const cocos2d::Point & p)
{
	static_cast<LeaderboardLayer*>(GetLayer(LEADERBOARD_LAYER))->LB_OnTouchMenuBegin(p);

	static_cast<EmoBoardLayer*>(GetLayer(EMOTION_LAYER))->CheckTouchMenuBegin(p);
}


void	CommonScreen::SetActionAfterShow(int action)
{
	this->PushEvent([action](BaseScreen * scr) {
		CommonScreen * c_scr = (CommonScreen*)scr;
		if (action == 3)
		{
			static_cast<TaiXiuLayer*>(c_scr->GetLayer(TAIXIU_LAYER))->OnShowTaiXiuLayout(0);
		}
		if (action == 4)
		{
			static_cast<TaiXiuLayer*>(c_scr->GetLayer(TAIXIU_LAYER))->OnShowTaiXiuLayout(1);
		}
	}, TIME_ACTION* 2);
}

int CommonScreen::GetStateCommonScr()
{
	int stlb = static_cast<LeaderboardLayer*>(GetLayer(LEADERBOARD_LAYER))->GetStateLB();
	if (stlb != -1)
	{
		return stlb;
	}
	else
	{
		int st = static_cast<TaiXiuLayer*>(GetLayer(TAIXIU_LAYER))->GetStateTaiXiu();
		if (st != -1)
		{
			return st;
		}
	}
	return 5;
}


//common layer
void CommonScreen::OnShowLayer(COMMON_LAYER layer)
{
	//confirm code
	//change pass layer
	//tutorial
	//lucky circle
	//emotion board
	GetLayer(layer)->OnShowLayer();
}

void CommonScreen::OnHideLayer(COMMON_LAYER layer)
{
	//emotion board
	//invite layer
	GetLayer(layer)->OnHideLayer();
}

bool CommonScreen::IsLayerVisible(COMMON_LAYER layer)
{
	return GetLayer(layer)->IsVisibleLayer();
}

//announce server
void CommonScreen::OnReceiveAnnouncementFromServer(RKString text)
{
	static_cast<AnnouceServerLayer*>(GetLayer(ANNOUNCE_LAYER))->OnReceiveAnnouncementFromServer(text);
}
void CommonScreen::ForceStopAnnouce(bool value)
{
	static_cast<AnnouceServerLayer*>(GetLayer(ANNOUNCE_LAYER))->forceStopAnnouce(value);
}

//player info
void CommonScreen::OnShowPlayerInfo(Player * player)
{
	return static_cast<PlayerInfoLayer*>(GetLayer(PLAYER_INFO_LAYER))->OnShowPlayerInfo(player);
}

void CommonScreen::OnSetUserPlayer(User * user)
{
	return static_cast<PlayerInfoLayer*>(GetLayer(PLAYER_INFO_LAYER))->OnSetUserPlayer(user);
}

void CommonScreen::OnReceiveAchievement(int id, void * list_achie)
{
	return static_cast<PlayerInfoLayer*>(GetLayer(PLAYER_INFO_LAYER))->OnReceiveAchievement(id , list_achie);
}


//call popup method
void CommonScreen::OnShowDialog(
	const RKString & str_title,
	const RKString & str_message,
	TYPE_DIALOG type,
	const char * strCB,
	const std::function<void(const char *, const char*)> & func,
	const std::function<void(const char *, const char*)> & func1,
	const std::function<void(const char *, const char*)> & func2,
	const RKString & title_btn_0,
	const RKString & title_btn_1
)
{
	return static_cast<DialogLayer*>(GetLayer(DIALOG_LAYER))->OnShowDialog(
		str_title,
		str_message,
		type,
		strCB,
		func,
		func1,
		func2,
		title_btn_0,
		title_btn_1
		);
}


void CommonScreen::OnHideDialog()
{
	return static_cast<DialogLayer*>(GetLayer(DIALOG_LAYER))->OnHideDialog();
}

//lucky circle

void CommonScreen::OnRecieveLuckyCircleResultList(LuckyCircleQuestList* lucky_quest_list)
{
	static_cast<LuckyCircleScreen*>(GetLayer(LUCKY_CIRCLE_LAYER))->OnRecieveLuckyCircleResultList(lucky_quest_list);
}

void CommonScreen::OnRecieveLuckyCircleResultItem(LuckyCircleResultList* lucky_result_item)
{
	static_cast<LuckyCircleScreen*>(GetLayer(LUCKY_CIRCLE_LAYER))->OnRecieveLuckyCircleResultItem(lucky_result_item);
}

//inivte banner here
void CommonScreen::OnSetDataInvite(int gameid, int roomid, int boardid, RKString pass, RKString message, int idavatar)
{
	static_cast<InviteLayer*>(GetLayer(INVITE_LAYER))->OnSetDataInvite(gameid, roomid, boardid, pass, message, idavatar);
}

//leaderboard

void CommonScreen::LB_ResetLeaderboard(bool cleanup)
{
	static_cast<LeaderboardLayer*>(GetLayer(LEADERBOARD_LAYER))->LB_ResetLeaderboard(cleanup);
}

void CommonScreen::LB_OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player)
{
	static_cast<LeaderboardLayer*>(GetLayer(LEADERBOARD_LAYER))->LB_OnReceiveListPlayer(id,page, list_player);
}

void CommonScreen::LB_ShowRichestWin()
{
	static_cast<LeaderboardLayer*>(GetLayer(LEADERBOARD_LAYER))->LB_ShowRichestWin();
}

void CommonScreen::LB_ShowRichestRuby()
{
	static_cast<LeaderboardLayer*>(GetLayer(LEADERBOARD_LAYER))->LB_ShowRichestRuby();
}

void CommonScreen::LB_ShowStrongest(ubyte current_game_id)
{
	static_cast<LeaderboardLayer*>(GetLayer(LEADERBOARD_LAYER))->LB_ShowStrongest(current_game_id);
}

//mini btn
cocos2d::Rect CommonScreen::GetRectMiniBtn(WidgetEntity * &obj)
{
	return static_cast<MiniBtnLayer*>(GetLayer(MINI_BTN_LAYER))->GetPosBtnMini(obj);
}

void CommonScreen::OnShowBtnMini(bool show)
{
	static_cast<MiniBtnLayer*>(GetLayer(MINI_BTN_LAYER))->OnShowMiniBtn(show);
}

void CommonScreen::OnEnableBtnNoHu(bool value)
{
	static_cast<MiniBtnLayer*>(GetLayer(MINI_BTN_LAYER))->OnEnableBtnNoHu(value);
}
//taixiu

void CommonScreen::ParseUserUI()
{
	static_cast<TaiXiuLayer*>(GetLayer(TAIXIU_LAYER))->ParseUserUI();
	static_cast<MiniPokerLayer*>(GetLayer(MINI_POKER_LAYER))->ParseUserUI();
}

void CommonScreen::OnReceivedTaiXiuInfo(iwinmesage::TaiXiuGameInfo * taixiu_info, ubyte type)
{
	static_cast<TaiXiuLayer*>(GetLayer(TAIXIU_LAYER))->OnReceivedTaiXiuInfo(taixiu_info, type);
}

void CommonScreen::OnReceivedTaiXiuBetResult(TaiXiuBetResult* taiXiuBetResult)
{
	static_cast<TaiXiuLayer*>(GetLayer(TAIXIU_LAYER))->OnReceivedTaiXiuBetResult(taiXiuBetResult);
}

void CommonScreen::OnReceivedTaiXiuUserOrderList(TaiXiuUserOrderList * taixiuOderList)
{
	static_cast<TaiXiuLayer*>(GetLayer(TAIXIU_LAYER))->OnReceivedTaiXiuUserOrderList(taixiuOderList);
}

void CommonScreen::OnReceivedTaiXiuUserHistoryList(TaiXiuUserHistoryList* taixiuHistoryList)
{
	static_cast<TaiXiuLayer*>(GetLayer(TAIXIU_LAYER))->OnReceivedTaiXiuUserHistoryList(taixiuHistoryList);
}

void CommonScreen::OnReceivedTaiXiuMatch(TaiXiuMatch * taixiuMatch)
{
	static_cast<TaiXiuLayer*>(GetLayer(TAIXIU_LAYER))->OnReceiveTaiXiuMatch(taixiuMatch);
}
void CommonScreen::onReceiveTaiXiuMatchList(TaiXiuMatchList * taixiuMatchList)
{
	static_cast<TaiXiuLayer*>(GetLayer(TAIXIU_LAYER))->OnReceiveTaiXiuMatchList(taixiuMatchList);
}

void CommonScreen::SetCallBackEmotionBoard(const std::function<void(int)> & func)
{
	static_cast<EmoBoardLayer*>(GetLayer(EMOTION_LAYER))->SetCallBack(func);
}

void CommonScreen::OnShowToast(std::string content)
{
	static_cast<ToastLayer*>(GetLayer(TOAST_LAYER))->OnShowLayer();
	static_cast<ToastLayer*>(GetLayer(TOAST_LAYER))->SetContent(content);
}

bool CommonScreen::HaveAnyLayerShow()
{
	bool result = (IsLayerVisible(DIALOG_LAYER)
		|| IsLayerVisible(LOADING_CIRCLE_LAYER)
		|| IsLayerVisible(TOAST_LAYER)
		|| IsLayerVisible(TUTORIAL_LAYER)
		|| IsLayerVisible(TAIXIU_LAYER)
		|| IsLayerVisible(MINI_POKER_LAYER)
		|| IsLayerVisible(LUCKY_CIRCLE_LAYER)
		|| IsLayerVisible(INVITE_LAYER)
		|| IsLayerVisible(CONFIRM_CODE_LAYER)
		|| IsLayerVisible(PLAYER_INFO_LAYER)
		|| IsLayerVisible(CHANGE_PASS_LAYER)
		|| IsLayerVisible(LEADERBOARD_LAYER)
		|| IsLayerVisible(EMOTION_LAYER)		
		);

	return result;
}

void CommonScreen::OnKeyBack()
{
	if (IsLayerVisible(DIALOG_LAYER))
	{
		OnHideLayer(DIALOG_LAYER);
	}
	else if (IsLayerVisible(TOAST_LAYER))
	{
		OnHideLayer(TOAST_LAYER);
	}
	else if (IsLayerVisible(TUTORIAL_LAYER))
	{
		OnHideLayer(TUTORIAL_LAYER);
	}
	else if (IsLayerVisible(MINI_POKER_LAYER))
	{
		OnHideLayer(MINI_POKER_LAYER);
	}
	else if (IsLayerVisible(TAIXIU_LAYER))
	{
		OnHideLayer(TAIXIU_LAYER);
	}
	else if (IsLayerVisible(LUCKY_CIRCLE_LAYER))
	{
		OnHideLayer(LUCKY_CIRCLE_LAYER);
	}
	else if (IsLayerVisible(INVITE_LAYER))
	{
		OnHideLayer(INVITE_LAYER);
	}
	else if (IsLayerVisible(CONFIRM_CODE_LAYER))
	{
		OnHideLayer(CONFIRM_CODE_LAYER);
	}
	else if (IsLayerVisible(PLAYER_INFO_LAYER))
	{
		OnHideLayer(PLAYER_INFO_LAYER);
	}
	else if (IsLayerVisible(CHANGE_PASS_LAYER))
	{
		OnHideLayer(CHANGE_PASS_LAYER);
	}
	else if (IsLayerVisible(LEADERBOARD_LAYER))
	{
		OnHideLayer(LEADERBOARD_LAYER);
	}
	else if (IsLayerVisible(EMOTION_LAYER))
	{
		OnHideLayer(EMOTION_LAYER);
	}
}

void CommonScreen::OnShowBonusMoney(s64 money_change , bool isBonus, int typeMoney)
{
	static_cast<BonusMoneyLayer*>(GetLayer(MONEY_CHANGE_LAYER))->ShowTextBonusMoney(money_change, isBonus , typeMoney);
}


//nohu layer
void CommonScreen::onReceivedNoHuGameInfo(void * data)
{
	static_cast<MiniPokerLayer*>(GetLayer(MINI_POKER_LAYER))->onReceivedNoHuGameInfo(data);
}
void CommonScreen::onReceivedNoHuBet(void * data)
{
	static_cast<MiniPokerLayer*>(GetLayer(MINI_POKER_LAYER))->onReceivedNoHuBet(data);
}
void CommonScreen::onReceivedNoHuUserHistory(void * data)
{
	static_cast<MiniPokerLayer*>(GetLayer(MINI_POKER_LAYER))->onReceivedNoHuUserHistory(data);
}
void CommonScreen::onReceivedNoHuListUser(void * data)
{
	static_cast<MiniPokerLayer*>(GetLayer(MINI_POKER_LAYER))->onReceivedNoHuListUser(data);
}

void CommonScreen::onSetMoneyBonusNohu(const s64 & money)
{
	static_cast<MiniPokerLayer*>(GetLayer(MINI_POKER_LAYER))->SetMoneyBonusNoHu(money);
}
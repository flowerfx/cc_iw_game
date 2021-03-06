#include "PlayerInfoLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListViewTable.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"
#include "Models/Achievement.h"
PlayerInfoLayer::PlayerInfoLayer()
{
	p_tab_info_player = nullptr;
	p_list_detail_achieve = nullptr;
	p_tab_achieve_player = nullptr;

	p_list_achieve.clear();

	p_current_player = nullptr;
}

PlayerInfoLayer::~PlayerInfoLayer()
{
	for (size_t i = 0; i < p_list_achieve.size(); i++)
	{
		delete p_list_achieve[i];
	}
	p_list_achieve.clear();

	p_current_player = nullptr;
}

void PlayerInfoLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	p_layout_player_info = _base_screen->GetWidgetChildByName("common_screen.layout_info_player.panel_info_player");
	p_tab_info_player = p_layout_player_info->GetWidgetChildByName(".layout_info");
	p_tab_achieve_player = p_layout_player_info->GetWidgetChildByName(".layout_achieve");
	p_list_detail_achieve = p_tab_achieve_player->GetWidgetChildByName(".list_detail_achieve");
	static_cast<ListViewTableWidget*>(p_list_detail_achieve)->InitTable();
}

bool PlayerInfoLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_close_panel")
		{
			OnHideLayer();
			return true;
		}
		else if (name == "btn_add_friend")
		{
			if (p_current_player)
			{
				ScrMgr->OnShowDialog("notice", (LANG_STR("friend_adding")).GetString() + p_current_player->name, DIALOG_ZERO_BUTTON_WAIT);
				GlobalService::addFriend(p_current_player->IDDB, p_current_player->name);
			}
			return true;
		}
		else if (name == "btn_chat_frend")
		{
			OnHideLayer([]() {
				ScrMgr->SwitchToMenu(FRIEND_SCREEN);
			});
			return true;
		}
	}
	return false;
}

bool PlayerInfoLayer::IsVisibleLayer()
{
	return p_layout_player_info->GetParentWidget()->Visible();
}

void PlayerInfoLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_layout_player_info->SetActionCommandWithName("ON_HIDE", CallFunc::create([this , call_back]() {
		this->p_layout_player_info->ForceFinishAction();
		this->p_layout_player_info->GetParentWidget()->Visible(false);
		static_cast<ListViewTableWidget*>(this->p_list_detail_achieve)->ResetListView();
		if (call_back)
		{
			call_back();
		}
	}));
}

void PlayerInfoLayer::OnShowPlayerInfo(Player * player)
{
	p_current_player = player;
	if (p_current_player->IDDB != GameController::myInfo->IDDB)
	{
		ScrMgr->OnShowWaitingDialog();
	}

	auto _panel = p_layout_player_info->GetWidgetChildByName(".layout_info");

	static_cast<TextWidget*>(_panel->GetWidgetChildByName(".txt_acc"))->SetText(player->name, true);
	static_cast<TextWidget*>(_panel->GetWidgetChildByName(".txt_level"))->SetText(player->getLevelDetail(), true);
	static_cast<TextWidget*>(p_layout_player_info->GetWidgetChildByName(".title_"))->SetText(player->name, true);
	p_list_detail_achieve->Visible(false);
	p_list_detail_achieve->GetParentWidget()->GetWidgetChildAtIdx(0)->Visible(true);
	p_list_detail_achieve->GetParentWidget()->GetWidgetChildAtIdx(0)->SetActionCommandWithName("ROTATE");

	if (p_current_player->IDDB != GameController::myInfo->IDDB)
	{
		GlobalService::requestProfileOf(player->IDDB);
		if (GameController::isFriend(player->IDDB))
		{
			_panel->GetWidgetChildByName(".btn_add_friend")->Visible(false);
			_panel->GetWidgetChildByName(".btn_chat_frend")->Visible(true);
		}
		else
		{
			_panel->GetWidgetChildByName(".btn_add_friend")->Visible(true);
			_panel->GetWidgetChildByName(".btn_chat_frend")->Visible(false);
		}
	}
	else
	{
		OnSetUserPlayer(ScrMgr->GetMyUser());
		_panel->GetWidgetChildByName(".btn_add_friend")->Visible(false);
		_panel->GetWidgetChildByName(".btn_chat_frend")->Visible(false);
	}
	GlobalService::getAllUserAchievement(player->IDDB);



}
extern char * list_sx[3];

void PlayerInfoLayer::OnSetUserPlayer(User * user)
{

	ScrMgr->OnHideDialog();

	p_layout_player_info->GetParentWidget()->Visible(true);
	p_layout_player_info->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
		this->p_layout_player_info->ForceFinishAction();
	}));

	auto _panel = p_layout_player_info->GetWidgetChildByName(".layout_info");
	static_cast<TextWidget*>(_panel->GetWidgetChildByName(".ico_purchase_win.title_"))->SetText(Utils::formatNumber_dot(user->money), true);
	static_cast<TextWidget*>(_panel->GetWidgetChildByName(".ico_purchase_ruby.title_"))->SetText(Utils::formatNumber_dot(user->ruby), true);
	static_cast<TextWidget*>(_panel->GetWidgetChildByName(".txt_time"))->SetText(user->playingTime, true);
	if (user->fullname != "")
	{
		static_cast<TextWidget*>(p_layout_player_info->GetWidgetChildByName(".title_"))->SetText(user->fullname, true);
	}

	auto panel_avatar = _panel->GetWidgetChildByName(".panel_avatar");

	GameMgr->HandleAvatarDownload(user->avatarID, user->nickName,
		[this, panel_avatar](void * data, void * str, int tag)
	{
		this->_base_screen->ParseImgToPanel(panel_avatar, data, str, tag);
	});
}

void PlayerInfoLayer::OnParseAchievementIntoGame(WidgetEntity* widget, Achievement * achie, int idx)
{
	PASSERT2(achie != nullptr, "incorrect data");
	//PASSERT2(idx < p_page_view_achieve->GetNumberChildWidget() && idx >= 0, "idx out of list achievement!");

	PASSERT2(widget != nullptr, "widget is nullptr !");

	auto detail = ScrMgr->GetGameDetailByType((GameType)(achie->gameId));

	static_cast<TextWidget*>(widget->GetWidgetChildByName(".top_title.title"))->SetText(detail.name);
	auto panel_layout = widget->GetWidgetChildByName(".layout_");
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".panel_exp_score.title"))->SetText(std::to_string(achie->exp), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_level_number"))->SetText(std::to_string(achie->eloNumber), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_level"))->SetText(achie->elo, true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_win_number"))->SetText(std::to_string(achie->win), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_lose_number"))->SetText(std::to_string(achie->lose), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_draw_number"))->SetText(std::to_string(achie->draw), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_leave_number"))->SetText(std::to_string(achie->givenUp), true);
}

void PlayerInfoLayer::OnReceiveAchievement(int id, void * list_achie)
{
	for (size_t i = 0; i < p_list_achieve.size(); i++)
	{
		delete p_list_achieve[i];
	}
	p_list_achieve.clear();

	p_list_achieve = *((std::vector<Achievement*>*)list_achie);

	size_t number_achieve = p_list_achieve.size();

	auto _list_view_achieve = static_cast<ListViewTableWidget*>(p_list_detail_achieve);

	_list_view_achieve->addEventListener(
		[this, _list_view_achieve](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = _list_view_achieve->GetCurrentIdxTo();
			if ((size_t)current_idx_to >= this->p_list_achieve.size())
			{
				PASSERT2(false, "problem here !");
				current_idx_to = this->p_list_achieve.size() - 1;
			}
			OnParseAchievementIntoGame(_widget, this->p_list_achieve[current_idx_to], current_idx_to);
		}
	});

	_list_view_achieve->SetNumberSizeOfCell(number_achieve);


	p_list_detail_achieve->Visible(true);
	p_list_detail_achieve->GetParentWidget()->GetWidgetChildAtIdx(0)->GetResource()->stopAllActions();
	p_list_detail_achieve->GetParentWidget()->GetWidgetChildAtIdx(0)->Visible(false);
}
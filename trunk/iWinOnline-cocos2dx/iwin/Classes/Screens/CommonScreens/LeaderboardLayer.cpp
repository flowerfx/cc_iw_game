#include "LeaderboardLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wLoadingBar.h"

#include "Network/Global/Globalservice.h"
#include "InputManager.h"
#include "Screens/CommonScreen.h"
#include "Common/IwinListViewTable.h"

LeaderboardLayer::LeaderboardLayer()
{
	p_lb_layout_leaderboard = nullptr;
	p_lb_layout_leaderboard_player = nullptr;

}

LeaderboardLayer::~LeaderboardLayer()
{
	LB_ResetLeaderboard(true);
}

int LeaderboardLayer::Update(float dt)
{
	static_cast<IwinListViewTable*>(p_lb_layout_leaderboard_player)->UpdateReload(dt);
	return 1;
}

void LeaderboardLayer::LB_OnTouchMenuBegin(const cocos2d::Point & p)
{
	if (p_lb_layout_leaderboard->Visible() && !static_cast<CommonScreen*>(_base_screen)->IsLayerVisible(COMMON_LAYER::PLAYER_INFO_LAYER))
	{
		_base_screen->CheckTouchOutSideWidget(p, p_lb_layout_leaderboard,
			[&](void) {
			OnHideLayer();
		});
	}
}

void LeaderboardLayer::InitLayer(BaseScreen * common_screen)
{
	CommonLayer::InitLayer(common_screen);

	p_lb_current_stage_lb = 0; //0 is richest lb, 1 is strongest lb, 2 is richest ruby lb

	p_lb_current_page_lb = 0;

	p_lb_current_rank_lb_idx = 0;

	p_lb_layout_leaderboard = _base_screen->GetWidgetChildByName("common_screen.leaderboard_layer");
	//
	p_lb_layout_leaderboard_player = p_lb_layout_leaderboard->GetWidgetChildByName(".list_player_iwin_lb");

	auto list_player_lb = static_cast<IwinListViewTable*>(p_lb_layout_leaderboard_player);
	list_player_lb->InitWithParam(
		p_lb_layout_leaderboard->GetWidgetChildByName(".layout_pull_update"),
		p_lb_layout_leaderboard->GetWidgetChildByName(".layout_pull_loadmore"),
		[this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			int current_idx_to = list_player_lb->GetCurrentIdxTo();
			LB_OnParsePlayerToLeaderboard(_widget, current_idx_to);
		},
		[this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			size_t current_idx = list_player_lb->GetCurrentIdxSelected();
			if (current_idx >= 0 && current_idx < this->p_lb_current_list_player_.size())
			{
				Player * player = this->p_lb_current_list_player_[current_idx];
				static_cast<CommonScreen*>(_base_screen)->OnShowPlayerInfo(player);

			}
			else
			{
				PASSERT2(false, "have problem here!");
			}
		},
		[this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			//reload list here
			this->LB_ResetLeaderboard(true);
			if (p_lb_current_stage_lb == 0)
			{
				GlobalService::requestRichestWin(this->p_lb_current_page_lb++);
			}
			else if (p_lb_current_stage_lb == 1)
			{
				GlobalService::requestStrongest(this->p_lb_current_page_lb++, this->p_current_game_id);
			}
			else if (p_lb_current_stage_lb == 2)
			{
				GlobalService::requestRichestRuby(this->p_lb_current_page_lb++);
			}
		},
		[this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (p_lb_current_stage_lb == 0)
			{
				GlobalService::requestRichestWin(this->p_lb_current_page_lb++);
			}
			else if (p_lb_current_stage_lb == 1)
			{
				GlobalService::requestStrongest(this->p_lb_current_page_lb++, this->p_current_game_id);
			}
			else if (p_lb_current_stage_lb == 2)
			{
				GlobalService::requestRichestRuby(this->p_lb_current_page_lb++);
			}
		}
	);
}

void LeaderboardLayer::LB_OnSetPlayerEachLB(
	std::vector<Player * > list_player,
	int & idx_rank,
	WidgetEntity * list_table,
	cocos2d::Map<int, Sprite*>* list_avatar)
{
	for (auto p : list_player)
	{
		p_lb_current_list_player_.push_back(p);
		size_t current_rank_idx = (size_t)idx_rank;
		idx_rank++;
		//download avatar in save into the list
		auto idx_player = p;
		{
			int id_avatar = idx_player->userProfile->avatarID;
			GameMgr->HandleAvatarDownload(idx_player->avatar, idx_player->name,
				[&, this, current_rank_idx, id_avatar, list_avatar, list_table, idx_player](void * data, void * str, int tag)
			{
				Image * img = (Image *)data;
				if (img && img->getWidth() <= 0)
				{
					CC_SAFE_DELETE(img);
					return;
				}
				Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
				list_avatar->insert(idx_player->IDDB, sprite_img);


				auto widget_lb = static_cast<ListViewTableWidget*>(list_table);
				if ((size_t)current_rank_idx < widget_lb->GetListWidgetSample().size())
				{
					widget_lb->UpdateCellAtIdx(current_rank_idx);
				}
			});
		}
	}
	size_t current_cell_number = static_cast<ListViewTableWidget*>(list_table)->GetNumberSizeOfCell();
	size_t pre_number_cell = current_cell_number;
	current_cell_number += list_player.size();
	static_cast<ListViewTableWidget*>(list_table)->SetNumberSizeOfCell(current_cell_number);
	if (pre_number_cell >= static_cast<ListViewTableWidget*>(list_table)->GetListWidgetSample().size())
	{
		static_cast<ListViewTableWidget*>(list_table)->JumpToItemIdx(pre_number_cell);
	}

}

Player* LeaderboardLayer::LB_GetPlayerByIdx(int idx)
{
	std::vector<Player*> list_player = p_lb_current_list_player_;

	if (idx < 0 || (size_t)idx >= list_player.size())
	{
		PASSERT2(idx >= 0 && (size_t)idx < list_player.size(), "idx out of range!");
		return nullptr;
	}

	return list_player[idx];
}

void LeaderboardLayer::LB_OnReceiveListPlayer(int id, ubyte page, std::vector<Player * > list_player)
{

	p_lb_temp_player.clear();
	p_lb_temp_player = list_player;

	p_lb_layout_leaderboard_player->Visible(false);
	p_lb_layout_leaderboard->GetWidgetChildAtIdx(5)->Visible(true);
	p_lb_layout_leaderboard->GetWidgetChildAtIdx(5)->SetActionCommandWithName("ROTATE");

	static_cast<IwinListViewTable*>(p_lb_layout_leaderboard_player)->SetHaveReload([this]() {
		this->LB_OnSetPlayerEachLB(p_lb_temp_player,
			p_lb_current_rank_lb_idx,
			p_lb_layout_leaderboard_player,
			&p_lb_list_avatar_player_download);
		this->p_lb_layout_leaderboard_player->Visible(true);
		this->p_lb_layout_leaderboard->GetWidgetChildAtIdx(5)->Visible(false);
	});
}

void LeaderboardLayer::LB_OnParsePlayerToLeaderboard(WidgetEntity * widget, int idx, bool isPlayer)
{
	Player * player = LB_GetPlayerByIdx(idx);

	if (!player)
	{
		PASSERT2(false, "player is null");
		return;
	}

	if (idx % 2 == 0)
	{
		widget->GetWidgetChildByName(".hl_panel")->Visible(false);
	}
	else
	{
		widget->GetWidgetChildByName(".hl_panel")->Visible(true);
	}

	static_cast<TextWidget*>(widget->GetWidgetChildByName(".stt_id"))->SetText(std::to_string(idx + 1) + ".", true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".title_id"))->SetText(player->name, true);
	if (p_lb_current_stage_lb == 0)
	{
		widget->GetWidgetChildByName(".number_win_id")->Visible(true);
		widget->GetWidgetChildByName(".progress_bar_panel")->Visible(false);
		static_cast<TextWidget*>(widget->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatNumber(player->money), true);
	}
	else if (p_lb_current_stage_lb == 1)
	{
		widget->GetWidgetChildByName(".number_win_id")->Visible(false);
		auto process_bar = widget->GetWidgetChildByName(".progress_bar_panel");
		process_bar->Visible(true);
		//avoid progress overlap
		int current_percent = player->getPercent();
		static_cast<LoadingBarWidget*>(process_bar->GetWidgetChildByName(".progress_bar_value"))->SetPercent(current_percent < 10 ? 10 : current_percent);
		static_cast<TextWidget*>(process_bar->GetWidgetChildByName(".title_percent_value"))->SetText(std::to_string(current_percent) + "%", true);
		RKString level = LangMgr->GetString("level");
		static_cast<TextWidget*>(process_bar->GetWidgetChildByName(".title_level"))->SetText(level + " " + std::to_string(player->getLevel()), true);

	}
	else if (p_lb_current_stage_lb == 2)
	{
		widget->GetWidgetChildByName(".number_win_id")->Visible(true);
		widget->GetWidgetChildByName(".progress_bar_panel")->Visible(false);
		static_cast<TextWidget*>(widget->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatNumber(player->ruby), true);
	}

	auto tx = widget->GetWidgetChildByName(".status_id");
	if (player->userProfile->status != "")
	{
		tx->Visible(true);
		static_cast<TextWidget*>(tx)->SetText(player->userProfile->status, true);
	}
	else
	{
		tx->Visible(false);
	}

	auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");

	if (p_lb_list_avatar_player_download.size() > 0
		&& p_lb_list_avatar_player_download.find(player->IDDB) != p_lb_list_avatar_player_download.end())
	{
		this->_base_screen->ParseSpriteToPanel(panel_avatar, p_lb_list_avatar_player_download.at(player->IDDB), 100);
	}
}

void LeaderboardLayer::LB_ResetLeaderboard(bool cleanup)
{
	p_lb_current_page_lb = 0;

	p_lb_current_rank_lb_idx = 0;

	if (cleanup)
	{
		static_cast<IwinListViewTable*>(p_lb_layout_leaderboard_player)->ResetListView();
		p_lb_list_avatar_player_download.clear();
		SAFE_DELETE_VECTOR(p_lb_current_list_player_);
	}
}

void LeaderboardLayer::LB_ShowRichestWin()
{
	if (!p_lb_layout_leaderboard->Visible())
	{

		p_lb_current_stage_lb = 0;
		ScrMgr->SetScreenIDDirty();
		if (p_lb_previous_state_lb != p_lb_current_stage_lb)
		{
			LB_ResetLeaderboard(true);
			GlobalService::requestRichestWin(this->p_lb_current_page_lb++);
		}
		p_lb_previous_state_lb = p_lb_current_stage_lb;

		p_lb_layout_leaderboard->GetWidgetChildAtIdx(4)->GetWidgetChildAtIdx(2)->Visible(false);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(4)->GetWidgetChildAtIdx(4)->Visible(true);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(4)->GetWidgetChildAtIdx(3)->Visible(false);

	//	p_lb_layout_leaderboard_player->Visible(true);

		p_lb_layout_leaderboard->SetActionCommandWithName("ON_SHOW");

	}
}

void LeaderboardLayer::LB_ShowRichestRuby()
{
	if (!p_lb_layout_leaderboard->Visible())
	{

		p_lb_current_stage_lb = 2;
		ScrMgr->SetScreenIDDirty();
		if (p_lb_previous_state_lb != p_lb_current_page_lb)
		{
			LB_ResetLeaderboard(true);
			GlobalService::requestRichestRuby(this->p_lb_current_page_lb++);
		}
		p_lb_previous_state_lb = p_lb_current_stage_lb;

		p_lb_layout_leaderboard->GetWidgetChildAtIdx(4)->GetWidgetChildAtIdx(2)->Visible(false);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(4)->GetWidgetChildAtIdx(4)->Visible(false);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(4)->GetWidgetChildAtIdx(3)->Visible(true);


//		p_lb_layout_leaderboard_player->Visible(true);

		p_lb_layout_leaderboard->SetActionCommandWithName("ON_SHOW");
	}
}

void LeaderboardLayer::LB_ShowStrongest(ubyte current_game_id)
{
	if (!p_lb_layout_leaderboard->Visible())
	{
		p_current_game_id = current_game_id;

		p_lb_current_stage_lb = 1;
		ScrMgr->SetScreenIDDirty();
		if (p_lb_previous_state_lb != p_lb_current_stage_lb)
		{
			LB_ResetLeaderboard(true);
			GlobalService::requestStrongest(this->p_lb_current_page_lb++, current_game_id);
		}
		p_lb_previous_state_lb = p_lb_current_stage_lb;

		p_lb_layout_leaderboard->GetWidgetChildAtIdx(4)->GetWidgetChildAtIdx(2)->Visible(true);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(4)->GetWidgetChildAtIdx(4)->Visible(false);
		p_lb_layout_leaderboard->GetWidgetChildAtIdx(4)->GetWidgetChildAtIdx(3)->Visible(false);

//		p_lb_layout_leaderboard_player->Visible(true);

		p_lb_layout_leaderboard->SetActionCommandWithName("ON_SHOW");

	}
}

int LeaderboardLayer::GetStateLB()
{
	if (p_lb_layout_leaderboard->Visible())
	{
		return p_lb_current_stage_lb;
	}
	return -1;
}

bool LeaderboardLayer::IsVisibleLayer()
{
	return p_lb_layout_leaderboard->Visible();
}

void LeaderboardLayer::OnHideLayer(const std::function<void(void)> & call_back )
{
	p_lb_layout_leaderboard->SetActionCommandWithName("ON_HIDE", call_back ? CallFunc::create([this , call_back]() {
		p_lb_layout_leaderboard->ForceFinishAction();
		if (call_back)
		{
			call_back();
		}
	}) : nullptr);
}
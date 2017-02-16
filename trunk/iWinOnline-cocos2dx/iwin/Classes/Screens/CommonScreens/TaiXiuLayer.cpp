#include "TaiXiuLayer.h"
#include "Network/JsonObject/taixiu/TaiXiuGameInfo.h"
#include "Network/JsonObject/taixiu/TaiXiuBetResult.h"
#include "Network/JsonObject/taixiu/TaiXiuUserOrderList.h"
#include "Network/JsonObject/taixiu/TaiXiuUserHistoryList.h"
#include "Network/JsonObject/taixiu/TaiXiuUserOrder.h"
#include "Network/JsonObject/taixiu/TaiXiuUserHistory.h"
#include "Network/JsonObject/taixiu/TaiXiuMatchList.h"

#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wTimeProcessWidget.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wSlider.h"
#include "UI/AdvanceUI/wTimeDigitalWidget.h"

#include "Common/IwinListViewTable.h"

#include "Network/Global/Globalservice.h"
#include "Services/GuideService.h"
#include "Screens/CommonScreens/MiniBtnLayer.h"
#define DEFAULT_TX_BET_TIME 60
#define DEFAULT_TX_RES_TIME 15

#define COLOR_TAI Color3B(250,2,9)
#define COLOR_XIU Color3B(2,141,213)

#define DRIG_W 24
#define DRIG_H 12


TaiXiuLayer::TaiXiuLayer()
{
	p_have_update_new_history = false;
	p_have_change_tab_taixiu = false;

	p_taixiu_gameinfo = nullptr;
	p_taixiu_bet_result = nullptr;
	p_taixiu_userorder_list = nullptr;
	p_taixiu_history_list = nullptr;

	p_taixiu_match = nullptr;
	p_taixiu_match_list = nullptr;

	_cur_match_id = _live_match_id = -1;

	_idx_chart_panel = 0;
}

TaiXiuLayer::~TaiXiuLayer()
{
	SAFE_DELETE(p_taixiu_match);
	SAFE_DELETE(p_taixiu_match_list);
	SAFE_DELETE(p_taixiu_gameinfo);
	SAFE_DELETE(p_taixiu_bet_result);
	SAFE_DELETE(p_taixiu_userorder_list);
	SAFE_DELETE(p_taixiu_history_list);
}

int TaiXiuLayer::Update(float dt)
{
	if (!IsVisibleLayer())
		return 0;

	static_cast<IwinListViewTable*>(p_taixiu_list_player_leaderboard)->UpdateReload(dt);
	static_cast<IwinListViewTable*>(p_taixiu_list_myhistory)->UpdateReload(dt);
	static_cast<IwinListViewTable*>(p_taixiu_list_match_history)->UpdateReload(dt);

	if (p_have_update_new_history)
	{
		if (p_taixiu_gameinfo)
		{
			auto history = p_taixiu_gameinfo->getHistoryList();

			if (GameController::typeMoneyTaiXiu == BASE_MONEY)
			{
				p_panel_taixiu_bg_history->GetWidgetChildAtIdx(0)->Visible(true);
				p_panel_taixiu_bg_history->GetWidgetChildAtIdx(1)->Visible(false);
				auto panel_tx_history = p_panel_taixiu_bg_history->GetWidgetChildAtIdx(0);
				panel_tx_history->GetWidgetChildAtIdx(0)->SetActionCommandWithName("EFFECT");
				for (size_t i = 1; i <= history.size(); i++)
				{
					panel_tx_history->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(0)->Visible(!history[i - 1]);
					panel_tx_history->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(1)->Visible(history[i - 1]);
				}
				p_main_taixiu_layout->GetWidgetChildByName(".panel_bg")->Visible(true);
				p_main_taixiu_layout->GetWidgetChildByName(".panel_bg_ruby")->Visible(false);
				p_layout_taixiu_bet->GetWidgetChildByName(".panel_bg")->Visible(true);
				p_layout_taixiu_bet->GetWidgetChildByName(".panel_bg_ruby")->Visible(false);
			}
			else
			{
				p_panel_taixiu_bg_history->GetWidgetChildAtIdx(0)->Visible(false);
				p_panel_taixiu_bg_history->GetWidgetChildAtIdx(1)->Visible(true);
				auto panel_tx_history = p_panel_taixiu_bg_history->GetWidgetChildAtIdx(1);
				panel_tx_history->GetWidgetChildAtIdx(0)->SetActionCommandWithName("EFFECT");
				for (size_t i = 1; i <= history.size(); i++)
				{
					panel_tx_history->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(0)->Visible(!history[i - 1]);
					panel_tx_history->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(1)->Visible(history[i - 1]);
				}
				p_main_taixiu_layout->GetWidgetChildByName(".panel_bg")->Visible(false);
				p_main_taixiu_layout->GetWidgetChildByName(".panel_bg_ruby")->Visible(true);
				p_layout_taixiu_bet->GetWidgetChildByName(".panel_bg")->Visible(false);
				p_layout_taixiu_bet->GetWidgetChildByName(".panel_bg_ruby")->Visible(true);
			}


			static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".left_middle_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(p_taixiu_gameinfo->getTotalUserTai()), true);
			static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".left_top_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(p_taixiu_gameinfo->getTotalWinTai()), true);

			static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".right_top_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(p_taixiu_gameinfo->getTotalWinXiu()), true);
			static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".right_middle_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(p_taixiu_gameinfo->getTotalUserXiu()), true);
		}
		p_have_update_new_history = false;
	}

	return 0;
}

void TaiXiuLayer::ParseUserUI()
{
	if (p_layout_tai_xiu_panel->Visible())
	{
		User * user = GetUser;
		if (user == nullptr)
		{
			//PASSERT2(false, "user is null");
			return;
		}

		s64 money = GameController::myInfo->money;
		s64 ruby = GameController::myInfo->ruby;

		static_cast<TextWidget*>(p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".taixiu_mymoney_txt"))->SetText(Utils::formatNumber_dot(money));
		static_cast<TextWidget*>(p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".taixiu_myruby_txt"))->SetText(Utils::formatNumber_dot(ruby));
	}
}

bool TaiXiuLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;

	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "taixiu_btn_leaderboard")
		{
			if (!p_layout_taixiu_leaderboard->GetParentWidget()->Visible())
			{
				GlobalService::getTaiXiuLeaderBoard();
				p_layout_taixiu_leaderboard->GetParentWidget()->Visible(true);
				auto title_bg = p_layout_taixiu_leaderboard->GetWidgetChildByName(".title_bg");
				if (GameController::typeMoneyTaiXiu == BASE_MONEY)
				{
					title_bg->GetWidgetChildByName(".txt2")->Visible(true);
					title_bg->GetWidgetChildByName(".txt3")->Visible(false);
				}
				else
				{
					title_bg->GetWidgetChildByName(".txt2")->Visible(false);
					title_bg->GetWidgetChildByName(".txt3")->Visible(true);
				}
				p_layout_taixiu_leaderboard->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
					this->p_layout_taixiu_leaderboard->ForceFinishAction();

					p_taixiu_list_player_leaderboard->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(false);
					p_taixiu_list_player_leaderboard->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(true);
					p_taixiu_list_player_leaderboard->GetParentWidget()->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");
				}));
			}
			return true;
		}
		else if (name == "btn_close_lb_taixiu")
		{
			if (p_layout_taixiu_leaderboard->GetParentWidget()->Visible())
			{
				static_cast<ListViewTableWidget*>(p_taixiu_list_player_leaderboard)->ResetListView();
				p_layout_taixiu_leaderboard->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
					this->p_layout_taixiu_leaderboard->ForceFinishAction();
					this->p_layout_taixiu_leaderboard->GetParentWidget()->Visible(false);
				}));
			}
			return true;
		}
		
		else if (name == "taixiu_btn_myhistory")
		{
			if (!p_layout_taixiu_myhistory->GetParentWidget()->Visible())
			{
				GlobalService::getTaiXiuMyHistoryList();
				p_layout_taixiu_myhistory->GetParentWidget()->Visible(true);
				p_layout_taixiu_myhistory->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
					this->p_layout_taixiu_myhistory->ForceFinishAction();

					p_taixiu_list_myhistory->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(false);
					p_taixiu_list_myhistory->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(true);
					p_taixiu_list_myhistory->GetParentWidget()->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");
				}));
			}
			return true;
		}
		else if (name == "btn_close_history_taixiu")
		{
			if (p_layout_taixiu_myhistory->GetParentWidget()->Visible())
			{
				static_cast<ListViewTableWidget*>(p_taixiu_list_myhistory)->ResetListView();
				p_layout_taixiu_myhistory->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
					this->p_layout_taixiu_myhistory->ForceFinishAction();
					this->p_layout_taixiu_myhistory->GetParentWidget()->Visible(false);
				}));
			}
			return true;
		}
		
		else if (name == "taixiu_btn_match_history")
		{
			if (!p_layout_taixiu_match_history->GetParentWidget()->Visible())
			{
				GlobalService::getTaiXiuMatchInfo(_live_match_id);
				p_layout_taixiu_match_history->GetParentWidget()->Visible(true);
				p_layout_taixiu_match_history->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
					this->p_layout_taixiu_match_history->ForceFinishAction();

					p_layout_taixiu_match_history->GetWidgetChildByName(".txt0")->Visible(false);
					p_layout_taixiu_match_history->GetWidgetChildByName(".loading_circle")->Visible(true);
					p_layout_taixiu_match_history->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");
				}));
			}
			return true;
		}
		else if (name == "btn_close_match_history_taixiu")
		{
			if (p_layout_taixiu_match_history->GetParentWidget()->Visible())
			{
				static_cast<ListViewTableWidget*>(p_taixiu_list_match_history)->ResetListView();
				p_layout_taixiu_match_history->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
					this->p_layout_taixiu_match_history->ForceFinishAction();
					this->p_layout_taixiu_match_history->GetParentWidget()->Visible(false);
				}));
			}
			return true;
		}
		else if (name == "btn_his_left")
		{
			GlobalService::getTaiXiuMatchInfo(_cur_match_id - 1);
		}
		else if (name == "btn_his_right")
		{
			if (_cur_match_id < _live_match_id)
			{
				GlobalService::getTaiXiuMatchInfo(_cur_match_id + 1);
			}
		}
		else if (name == "btn_cur_phien")
		{
			GlobalService::getTaiXiuMatchInfo(_live_match_id);
		}

		else if (name == "taixiu_btn_showchart")
		{
			if (!p_layout_taixiu_chart->GetParentWidget()->Visible())
			{
				GlobalService::getTaiXiuHistory();
				p_layout_taixiu_chart->GetParentWidget()->Visible(true);
				p_layout_taixiu_chart->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
					this->p_layout_taixiu_chart->ForceFinishAction();
				}));
			}
			return true;
		}
		else if (name == "btn_close_chart_taixiu")
		{
			if (p_layout_taixiu_chart->GetParentWidget()->Visible())
			{
				//static_cast<ListViewTableWidget*>(p_taixiu_list_match_history)->ResetListView();
				p_layout_taixiu_chart->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
					this->p_layout_taixiu_chart->ForceFinishAction();
					this->p_layout_taixiu_chart->GetParentWidget()->Visible(false);
				}));
			}
			return true;
		}
		else if (name == "btn_chart_left")
		{
			OnShowChartPanelAtIdx(_idx_chart_panel - 1);
		}
		else if (name == "btn_chart_right")
		{
			OnShowChartPanelAtIdx(_idx_chart_panel + 1);
		}

		
		else if (name == "left_panel_btn")
		{
			OnShowTaiXiuLayoutAtIdx(BASE_MONEY);
			return true;
		}
		else if (name == "right_panel_btn")
		{
			OnShowTaiXiuLayoutAtIdx(RUBY_MONEY);
			return true;
		}

		else if (name == "btn_choose_tai")
		{
			//if (current_stage_bet_taixiu == -1 || current_stage_bet_taixiu == 0)
			{
				if (!p_layout_taixiu_bet->Visible())
				{
					current_stage_bet_taixiu = 0;
					OnShowPanelBetTaixiu(true);
				}
				else
				{
					if (current_stage_bet_taixiu == -1 || current_stage_bet_taixiu == 0)
					{
						OnShowPanelBetTaixiu(false, CallFunc::create([this]() {
							this->p_layout_taixiu_bet->ForceFinishAction();
							this->current_stage_bet_taixiu = -1;
						}));
					}
					else
					{
						current_stage_bet_taixiu = 0;
						OnShowPanelBetTaixiu(true);
					}
				}
			}
			return true;
		}
		else if (name == "btn_choose_xiu")
		{
			//if (current_stage_bet_taixiu == -1 || current_stage_bet_taixiu == 1)
			{
				if (!p_layout_taixiu_bet->Visible())
				{
					current_stage_bet_taixiu = 1;
					OnShowPanelBetTaixiu(true);
				}
				else
				{
					if (current_stage_bet_taixiu == -1 || current_stage_bet_taixiu == 1)
					{
						OnShowPanelBetTaixiu(false, CallFunc::create([this]() {
							this->p_layout_taixiu_bet->ForceFinishAction();
							this->current_stage_bet_taixiu = -1;
						}));
					}
					else
					{
						current_stage_bet_taixiu = 1;
						OnShowPanelBetTaixiu(true);
					}
				}
			}
			return true;
		}

		else if (name == "full_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			slider->SetPercent(100);
			SetInputMoneyTextBet(GetCurrentMoney());
			return true;
		}
		else if (name == "3quadter_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			slider->SetPercent(75);
			SetInputMoneyTextBet(GetCurrentMoney() * 3 / 4);
			return true;
		}
		else if (name == "half_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			slider->SetPercent(50);
			SetInputMoneyTextBet(GetCurrentMoney() / 2);
			return true;
		}
		else if (name == "quadter_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			slider->SetPercent(25);
			SetInputMoneyTextBet(GetCurrentMoney() / 4);
			return true;
		}
		else if (name == "plus_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			float value = slider->GetPercent();
			int part = slider->GetPart();
			if (value < 100)
			{
				int current_part = ((value / 100.f) * part) + 0.1f;
				current_part++;
				if (current_part >= part)
				{
					current_part = part;
				}
				if (part <= 0)
				{
					slider->SetPercent(0);
					current_part = 0;
				}
				else
				{
					//float percent_each_part = 
					slider->SetPercent(current_part * 100.f / part);
				}

				s64 current_money_bet = (s64)(current_part * 1000.f);
				SetInputMoneyTextBet(GetCurrentMoney() <= 1000 ? GetCurrentMoney() : current_money_bet);
			}
			return true;
		}
		else if (name == "div_bet_btn")
		{
			SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
			float value = slider->GetPercent();
			int part = slider->GetPart();
			if (value > 0)
			{
				int current_part = ((value / 100.f) * part) + 0.1f;
				current_part--;
				if (current_part < 0)
				{
					current_part = 0;
				}
				if (part <= 0)
				{
					slider->SetPercent(0);
					current_part = 0;
				}
				else
				{
					slider->SetPercent(current_part * 100.f / part);
				}
				s64 current_money_bet = (s64)(current_part  * 1000.f);
				SetInputMoneyTextBet(current_money_bet);
			}
			return true;
		}
		else if (name == "taxiu_bet_tai_btn" || name == "taxiu_bet_xiu_btn")
		{
			BetMoney(name == "taxiu_bet_tai_btn" ? 0 : 1);
			OnShowPanelBetTaixiu(false, CallFunc::create([this]() {
				this->p_layout_taixiu_bet->ForceFinishAction();
				this->current_stage_bet_taixiu = -1;
			}));
			return true;
		}
		else if (name == "taixiu_btn_help")
		{
			OPEN_URL(GET_GUIDE->getTaiXiuHelp());
			return true;
		}

		else if (name == "btn_close_taixiu_panel")
		{
			OnShowTaiXiuLayout(false);
			return true;
		}

	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		if (name == "input_tai_txt_DETACH_IME")
		{
			//current_stage_bet_taixiu = -1;
			OnCheckTextInputMoney(_widget);
			return true;
		}
		else if (name == "input_tai_txt_ATTACH_IME")
		{
			current_stage_bet_taixiu = 0;
			OnShowPanelBetTaixiu(true);
			return true;
		}

		else if (name == "input_xiu_txt_DETACH_IME")
		{
			//current_stage_bet_taixiu = -1;
			OnCheckTextInputMoney(_widget);
			return true;
		}
		else if (name == "input_xiu_txt_ATTACH_IME")
		{
			current_stage_bet_taixiu = 1;
			OnShowPanelBetTaixiu(true);
			return true;
		}

	}
	else if (type_widget == UI_TYPE::UI_SLIDER)
	{
		if (name == "slider_bet_win_PERCENTAGE_CHANGED" || name == "slider_bet_ruby_PERCENTAGE_CHANGED")
		{
			_base_screen->PushEvent([this , _widget](BaseScreen * scr) {
				float value = static_cast<SliderWidget*>(_widget)->GetPercent();
				int part = static_cast<SliderWidget*>(_widget)->GetPart();
				int current_part = ((value / 100.f) * part) + 0.1f;
				s64 current_money_bet = (s64)(current_part * 1000.f);
				SetInputMoneyTextBet(current_money_bet);
			}, 0.1f);
			return true;
		}
	}
	else if (type_widget == UI_TYPE::UI_CHECK_BOX)
	{
		if (name == "btn_check_dice1_SELECTED")
		{
			OnShowChartDice(0, true);
			return true;
		}
		else if (name == "btn_check_dice1_UNSELECTED")
		{
			OnShowChartDice(0, false);
			return true;
		}
		else if (name == "btn_check_dice2_SELECTED")
		{
			OnShowChartDice(1, true);
			return true;
		}
		else if (name == "btn_check_dice2_UNSELECTED")
		{
			OnShowChartDice(1, false);
			return true;
		}
		else if (name == "btn_check_dice3_SELECTED")
		{
			OnShowChartDice(2, true);
			return true;
		}
		else if (name == "btn_check_dice3_UNSELECTED")
		{
			OnShowChartDice(2, false);
			return true;
		}
	}

	return false;
}

void TaiXiuLayer::InitLayer(BaseScreen* screen)
{
	CommonLayer::InitLayer(screen);

	p_layout_taixiu = _base_screen->GetWidgetChildByName("common_screen.taixiu_layout");

	p_layout_tai_xiu_panel = p_layout_taixiu->GetWidgetChildByName(".tai_xiu_layout");
	p_layout_taixiu_leaderboard = p_layout_tai_xiu_panel->GetWidgetChildByName(".taixiu_leaderboard_layout.taixiu_leaderboard_layout");
	p_taixiu_list_player_leaderboard = p_layout_taixiu_leaderboard->GetWidgetChildByName(".taixiu_list_player_leaderboard");
	auto tx_list_player_lb = static_cast<IwinListViewTable*>(p_taixiu_list_player_leaderboard);
	tx_list_player_lb->InitWithParam(
		nullptr,
		nullptr,
		[this, tx_list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = tx_list_player_lb->GetCurrentIdxTo();
				if ((size_t)current_idx_to >= this->p_taixiu_userorder_list->getUserOrderList().size())
				{
					PASSERT2(false, "problem here !");
					current_idx_to = this->p_taixiu_userorder_list->getUserOrderList().size() - 1;
				}
				OnParseTaiXiuLeaderboard(_widget, this->p_taixiu_userorder_list->getUserOrderList()[current_idx_to], current_idx_to);
			}
		},nullptr,nullptr,nullptr);


	p_layout_taixiu_myhistory = p_layout_tai_xiu_panel->GetWidgetChildByName(".taixiu_myhistory_layout.taixiu_myhistory_layout");
	p_taixiu_list_myhistory = p_layout_taixiu_myhistory->GetWidgetChildByName(".taixiu_list_myhistory");
	auto tx_list_myhistory = static_cast<IwinListViewTable*>(p_taixiu_list_myhistory);
	tx_list_myhistory->InitWithParam(
		nullptr,
		nullptr,
		[this, tx_list_myhistory](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = tx_list_myhistory->GetCurrentIdxTo();
				if ((size_t)current_idx_to >= this->p_taixiu_history_list->getUserHistoryList().size())
				{
					PASSERT2(false, "problem here !");
					current_idx_to = this->p_taixiu_history_list->getUserHistoryList().size() - 1;
				}
				OnParseTaiXiuHistory(_widget, this->p_taixiu_history_list->getUserHistoryList()[current_idx_to], current_idx_to);
			}
		},nullptr,nullptr,nullptr);

	p_layout_taixiu_match_history = p_layout_tai_xiu_panel->GetWidgetChildByName(".taixiu_matchhistory_layout.taixiu_matchhistory_layout");
	p_taixiu_list_match_history = p_layout_taixiu_match_history->GetWidgetChildByName(".taixiu_list_history");
	auto tx_list_match_history = static_cast<IwinListViewTable*>(p_taixiu_list_match_history);
	tx_list_match_history->InitWithParam(
		nullptr,
		nullptr,
		[this, tx_list_match_history](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = tx_list_match_history->GetCurrentIdxTo();
			if ((size_t)current_idx_to >= this->p_taixiu_match->getUserBets().size())
			{
				//PASSERT2(false, "problem here !");
				current_idx_to = this->p_taixiu_match->getUserBets().size() - 1;
			}
			OnParseTaiXiuMatchHistory(_widget, &(this->p_taixiu_match->getUserBets()[current_idx_to]), current_idx_to);
		}
	}, nullptr, nullptr, nullptr);

	p_layout_taixiu_chart = p_layout_tai_xiu_panel->GetWidgetChildByName(".taixiu_chart_layout.taixiu_chart_layout");
	p_chart_node = p_layout_taixiu_chart->GetWidgetChildByName(".node");
	p_chart_node_e = p_layout_taixiu_chart->GetWidgetChildByName(".node_e");
	p_line_node = p_layout_taixiu_chart->GetWidgetChildByName(".line");
	p_list_node_layout = p_layout_taixiu_chart->GetWidgetChildByName(".detail_chart_layout.table_bg.list_node");
	
	for (int i = 0; i < DRIG_W; i++)
	{
		if (i % 2 != 0)
		{
			for (int j = DRIG_H - 1; j >= 0; j--)
			{
				OnInsertNodeChartToList(i, j);
			}
		}
		else
		{
			for (int j = 0; j < DRIG_H; j++)
			{
				OnInsertNodeChartToList(i, j);
			}
		}
	}

	p_layout_dice_chart_top = p_layout_taixiu_chart->GetWidgetChildByName(".dice_chart_layout.dice_chart_top_layout.dice_chart_top_detail_layout");
	p_layout_dice_chart_bot_dice1 = p_layout_taixiu_chart->GetWidgetChildByName(".dice_chart_layout.dice_chart_bot_layout.dice_chart_bot_dice1_layout");
	p_layout_dice_chart_bot_dice2 = p_layout_taixiu_chart->GetWidgetChildByName(".dice_chart_layout.dice_chart_bot_layout.dice_chart_bot_dice2_layout");
	p_layout_dice_chart_bot_dice3 = p_layout_taixiu_chart->GetWidgetChildByName(".dice_chart_layout.dice_chart_bot_layout.dice_chart_bot_dice3_layout");

	for (int i = 0; i <= DRIG_W; i++)
	{
		OnInsertNodeToTopDice(i);

		OnInsertNodeToBotDice(i, 0);
		OnInsertNodeToBotDice(i, 1);
		OnInsertNodeToBotDice(i, 2);

	}


	p_layout_taixiu_bet = p_layout_tai_xiu_panel->GetWidgetChildByName(".tai_xiu_panel.taixiu_bet_layout");

	GameController::typeMoneyTaiXiu = BASE_MONEY;
	current_stage_bet_taixiu = -1;
	p_result_taixiu = p_previous_result_taixiu = 0;

	p_taixiu_already_have_result = false;

	p_main_taixiu_layout = p_layout_tai_xiu_panel->GetWidgetChildByName(".tai_xiu_panel");
	p_panel_taixiu_bg_history = p_main_taixiu_layout->GetWidgetChildByName(".bg_history");
	p_panel_taixiu_bg_top_layout = p_main_taixiu_layout->GetWidgetChildByName(".tai_xiu_top_layout");

	p_taixiu_new_user_bet_layout = p_layout_tai_xiu_panel->GetWidgetChildByName(".taixiu_user_bet_win");
	p_taixiu_fly_user_layout = p_layout_tai_xiu_panel->GetWidgetChildByName(".layout_fly_user");
	//create 20 star history

	auto size_bg_his = p_panel_taixiu_bg_history->GetSize();
	//iwin
	float part_each_slide = size_bg_his.x / 20;
	for (int i = 1; i < 20; i++)
	{
		auto star = p_panel_taixiu_bg_history->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(1);
		auto clone_star = star->Clone();
		clone_star->SetPosition(Vec2((part_each_slide * i) + (part_each_slide / 2), size_bg_his.y / 2));

		p_panel_taixiu_bg_history->GetWidgetChildAtIdx(0)->AddChildWidget(clone_star);
		//p_panel_taixiu_bg_history->GetWidgetChildAtIdx(0)->ResizeAndPosChildWidget(clone_star);
	}
	//ruby
	for (int i = 1; i < 20; i++)
	{
		auto star = p_panel_taixiu_bg_history->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(1);
		auto clone_star = star->Clone();
		clone_star->SetPosition(Vec2((part_each_slide * i) + (part_each_slide / 2), size_bg_his.y / 2));

		p_panel_taixiu_bg_history->GetWidgetChildAtIdx(1)->AddChildWidget(clone_star);
		//p_panel_taixiu_bg_history->GetWidgetChildAtIdx(1)->ResizeAndPosChildWidget(clone_star);
	}

	//circle

	p_taixiu_circle_layout = p_main_taixiu_layout->GetWidgetChildByName(".tai_xiu_circle");
	p_taixiu_circle_result_layout = p_taixiu_circle_layout->GetWidgetChildByName(".circle_result");
	p_taixiu_circle_dice_layout = p_taixiu_circle_result_layout->GetWidgetChildByName(".result_dice");
	p_taixiu_circle_small_result_layout = p_taixiu_circle_result_layout->GetWidgetChildByName(".result_taixiu");
	p_taixiu_circle_cd_layout = p_taixiu_circle_layout->GetWidgetChildByName(".circle_with_cd");
}


bool TaiXiuLayer::IsVisibleLayer()
{
	return p_layout_tai_xiu_panel->Visible();
}

void TaiXiuLayer::OnShowTaiXiuLayout(bool value)
{
	if (value)
	{
		p_layout_tai_xiu_panel->Visible(true);
		p_main_taixiu_layout->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
			this->p_main_taixiu_layout->ForceFinishAction();
			this->p_main_taixiu_layout->GetWidgetChildByName(".logo_taixiu")->SetActionCommandWithName("ON_SHOW");
		}));

		ScrMgr->SetScreenIDDirty();
		_base_screen->onMyInfoChanged();

		p_have_change_tab_taixiu = true;
		GlobalService::getTaiXiuInfo(GameController::myInfo->name);
		p_taixiu_already_have_result = false;
	}
	else
	{
		current_stage_bet_taixiu = -1;
		p_layout_taixiu_bet->Visible(false);

		p_layout_tai_xiu_panel->Visible(false);
		p_main_taixiu_layout->Visible(false);
		this->p_main_taixiu_layout->GetWidgetChildByName(".logo_taixiu")->SetActionCommandWithName("ON_HIDE");

		//reset text field tai and xiu

		SetInputMoneyTextBet(1000);
	}
}

 void TaiXiuLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	 OnShowTaiXiuLayout(true);
}
 void TaiXiuLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	 OnShowTaiXiuLayout(false);
}

void TaiXiuLayer::OnResetTaiXiu()
{
	static_cast<MiniBtnLayer*>(GetCommonScr->GetLayer(MINI_BTN_LAYER))->ResetTimer(true, DEFAULT_TX_BET_TIME, DEFAULT_TX_BET_TIME);
	//
	OnShowResultCirlceWithTimer(false, DEFAULT_TX_BET_TIME, DEFAULT_TX_BET_TIME);
	//
	p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_tai")->Visible(false);
	p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_xiu")->Visible(false);
	p_taixiu_circle_layout->GetWidgetChildByName(".circle_time_process")->Visible(false);
	p_taixiu_circle_small_result_layout->Visible(false);
	OnShowResultTaiXiu(0, false);
	p_taixiu_already_have_result = false;

	auto wi = p_main_taixiu_layout->GetWidgetChildByName(".left_input_bg")->GetWidgetChildAtIdx(1);
	static_cast<TextWidget*>(wi)->SetText("0", false);
	wi = p_main_taixiu_layout->GetWidgetChildByName(".right_input_bg")->GetWidgetChildAtIdx(1);
	static_cast<TextWidget*>(wi)->SetText("0", false);

	p_have_update_new_history = true;

	p_taixiu_fly_user_layout->ClearChild();


	static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".left_top_bg")->GetWidgetChildAtIdx(1))->SetText("0" , true);
	static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".right_top_bg")->GetWidgetChildAtIdx(1))->SetText("0", true);
	static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".left_middle_bg")->GetWidgetChildAtIdx(1))->SetText("0", true);
	static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".right_middle_bg")->GetWidgetChildAtIdx(1))->SetText("0", true);

	//reset text field tai and xiu
	SetInputMoneyTextBet(1000);

}

void TaiXiuLayer::OnReceivedTaiXiuInfo(iwinmesage::TaiXiuGameInfo * taixiu_info, ubyte type)
{
	SAFE_DELETE(p_taixiu_gameinfo);
	p_taixiu_gameinfo = taixiu_info;

	_live_match_id = p_taixiu_gameinfo->getMatchId();
	if (_cur_match_id < 0)
	{
		_cur_match_id = _live_match_id;
	}
	if (p_layout_taixiu_match_history->GetParentWidget()->Visible() && _cur_match_id == _live_match_id)
	{
		GlobalService::getTaiXiuMatchInfo(_cur_match_id);
	}


	
	if (p_layout_tai_xiu_panel->Visible())
	{
		if (p_have_change_tab_taixiu)
		{
			p_have_update_new_history = true;
			p_have_change_tab_taixiu = false;
		}

		
		if (p_taixiu_gameinfo->getDice1() > 0 &&
			p_taixiu_gameinfo->getDice2() > 0 &&
			p_taixiu_gameinfo->getDice3() > 0)
		{
			if (!p_taixiu_already_have_result)
			{
				p_have_update_new_history = true;
				p_taixiu_already_have_result = true;
				//
				int total_number_dice = p_taixiu_gameinfo->getDice1() + p_taixiu_gameinfo->getDice2() + p_taixiu_gameinfo->getDice3();
				//show result 
				static_cast<MiniBtnLayer*>(GetCommonScr->GetLayer(MINI_BTN_LAYER))->ResetTimer(false, -1, -1);

				float total_time = p_taixiu_gameinfo->getTimeShowResultTotal() > 0 ? p_taixiu_gameinfo->getTimeShowResultTotal() : DEFAULT_TX_RES_TIME;
				float remain_time = p_taixiu_gameinfo->getTimeShowResultRemain() > 0 ? p_taixiu_gameinfo->getTimeShowResultRemain() / 1000.f : DEFAULT_TX_RES_TIME;

				p_result_taixiu = p_taixiu_gameinfo->getResult();
				OnShowResultCirlceWithTimer(true, total_time, remain_time);

				if (p_result_taixiu == 0) // tai
				{
					p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(0)->Visible(true);
					p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(1)->Visible(false);
					static_cast<TextWidget*>(p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(2))->SetText(std::to_string(total_number_dice), true);
				}
				else //xiu
				{
					p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(0)->Visible(false);
					p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(1)->Visible(true);
					static_cast<TextWidget*>(p_taixiu_circle_small_result_layout->GetWidgetChildAtIdx(2))->SetText(std::to_string(total_number_dice), true);
				}
				if (remain_time < total_time * 2 / 3)
				{
					OnShowResultCircle();
					auto dice1 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(0));
					dice1->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice1, this]() {
						dice1->ForceFinishAction();
						dice1->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice1() + 5);
					}));
					auto dice2 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(1));
					dice2->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice2, this]() {
						dice2->ForceFinishAction();
						dice2->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice2() + 5);
					}));
					auto dice3 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(2));
					dice3->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice3, this]() {
						dice3->ForceFinishAction();
						dice3->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice3() + 5);
					}));
				}
				else
				{
					auto dice1 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(0));
					dice1->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice1, this]() {
						dice1->ForceFinishAction();
						dice1->RunAnim(CallFunc::create([dice1, this]() {
							dice1->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice1() + 5);
						}));
					}));
					auto dice2 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(1));
					dice2->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice2, this]() {
						dice2->ForceFinishAction();
						dice2->RunAnim(CallFunc::create([dice2, this]() {
							dice2->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice2() + 5);
						}));
					}));
					auto dice3 = static_cast<AnimWidget*>(p_taixiu_circle_dice_layout->GetWidgetChildAtIdx(2));
					dice3->SetActionCommandWithName("ON_SHOW", CallFunc::create([dice3, this]() {
						dice3->ForceFinishAction();
						dice3->RunAnim(CallFunc::create([dice3, this]() {
							this->OnShowResultCircle();
							dice3->ShowSpriteAtFrameIdx(p_taixiu_gameinfo->getDice3() + 5);
						}));
					}));
				}

				p_previous_result_taixiu = p_result_taixiu;
			}
		}
		else
		{
			OnShowResultTaiXiu(0, false);

			OnShowResultCirlceWithTimer(false,
				p_taixiu_gameinfo->getTimeBetTotal() > 0 ? p_taixiu_gameinfo->getTimeBetTotal() : DEFAULT_TX_BET_TIME,
				p_taixiu_gameinfo->getTimeBetRemain() >= 0 ? p_taixiu_gameinfo->getTimeBetRemain() / 1000.f : DEFAULT_TX_BET_TIME);

			static_cast<MiniBtnLayer*>(GetCommonScr->GetLayer(MINI_BTN_LAYER))->ResetTimer(true,
				p_taixiu_gameinfo->getTimeBetTotal() > 0 ? p_taixiu_gameinfo->getTimeBetTotal() : DEFAULT_TX_BET_TIME,
				p_taixiu_gameinfo->getTimeBetRemain() >= 0 ? p_taixiu_gameinfo->getTimeBetRemain() / 1000.f : DEFAULT_TX_BET_TIME);
		}

		if (p_taixiu_gameinfo->getTotalWinTai() == 0 &&
			p_taixiu_gameinfo->getTotalWinXiu() == 0 &&
			p_taixiu_gameinfo->getTotalUserTai() == 0 &&
			p_taixiu_gameinfo->getTotalUserXiu() == 0)
		{
			OnResetTaiXiu();
		}
	}
	else
	{
		s64 timebet_remain = p_taixiu_gameinfo->getTimeBetRemain();
		s64 timeresult_remain = p_taixiu_gameinfo->getTimeShowResultRemain();
		s64 timebet_total = p_taixiu_gameinfo->getTimeBetTotal();
		s64 timeresult_total = p_taixiu_gameinfo->getTimeShowResultTotal();

		static_cast<MiniBtnLayer*>(GetCommonScr->GetLayer(MINI_BTN_LAYER))->SetTimer(
			timebet_remain, 
			timeresult_remain, 
			timebet_total, 
			timeresult_total , 
			DEFAULT_TX_BET_TIME,
			DEFAULT_TX_RES_TIME);
	}

	if (p_taixiu_gameinfo->getNewUserBet().getUsername() != "" && p_layout_tai_xiu_panel->Visible())
	{
		int betchoice = p_taixiu_gameinfo->getNewUserBet().getBetChoice();

		auto tx_user = p_taixiu_new_user_bet_layout->Clone();

		p_taixiu_fly_user_layout->InsertChildWidget(tx_user);

		float ratio = GetGameSize().width / _base_screen->GetDesignSize().x;
		float delta_move = (50.f * ratio);
		float div = 100 * ratio;
		tx_user->SetVisible(true);
		tx_user->SetPosition(Vec2
		(
			(betchoice == 0 ? GetRandom(div, GetGameSize().width / 2 ): GetRandom(GetGameSize().width / 2 + div, GetGameSize().width)),
			GetRandom(0 , p_main_taixiu_layout->GetPosition_BottomLeft().y - (div / 2))
		));


		auto pos_coin_tai = p_main_taixiu_layout->GetWidgetChildByName(".left_top_bg")->GetWidgetChildAtIdx(2)->GetPosition_Middle();
		auto pos_coin_xiu = p_main_taixiu_layout->GetWidgetChildByName(".right_top_bg")->GetWidgetChildAtIdx(2)->GetPosition_Middle();

		pos_coin_tai = p_main_taixiu_layout->GetWidgetChildByName(".left_top_bg")->GetResource()->convertToWorldSpace(pos_coin_tai);
		pos_coin_xiu = p_main_taixiu_layout->GetWidgetChildByName(".right_top_bg")->GetResource()->convertToWorldSpace(pos_coin_xiu);

		auto wicon = tx_user->GetWidgetChildByName(".icon");

		wicon->GetWidgetChildAtIdx(0)->Visible(GameController::typeMoneyTaiXiu == BASE_MONEY);
		wicon->GetWidgetChildAtIdx(1)->Visible(GameController::typeMoneyTaiXiu != BASE_MONEY);
		wicon->SetActionCommandWithName("SHOW");

		auto lay_out_txt = tx_user->GetWidgetChildByName(".layout_txt");
		lay_out_txt->SetActionCommandWithName("SHOW");
		static_cast<TextWidget*>(lay_out_txt->GetWidgetChildAtIdx(0))->SetText(p_taixiu_gameinfo->getNewUserBet().getUsername(), true);
		static_cast<TextWidget*>(lay_out_txt->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(p_taixiu_gameinfo->getNewUserBet().getBetMoney()), true);
		if (betchoice == 0)
		{
			lay_out_txt->GetWidgetChildAtIdx(1)->SetColor(COLOR_TAI);
		}
		else
		{
			lay_out_txt->GetWidgetChildAtIdx(1)->SetColor(COLOR_XIU);
		}

		tx_user->GetResource()->runAction(Sequence::create(
			MoveTo::create(1.f, Vec2(tx_user->GetPosition().x, tx_user->GetPosition().y + delta_move)) , 
			betchoice == 0 ? MoveTo::create(0.5f, pos_coin_tai) : MoveTo::create(0.4f, pos_coin_xiu),
			DelayTime::create(0.2f),
			CallFunc::create([tx_user , betchoice , this]()
		{			
			if (betchoice == 0)
			{
				static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".left_middle_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(p_taixiu_gameinfo->getTotalUserTai()), true);
				static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".left_top_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(p_taixiu_gameinfo->getTotalWinTai()), true);
				p_main_taixiu_layout->GetWidgetChildByName(".left_top_bg")->GetWidgetChildAtIdx(3)->SetActionCommandWithName("RUN_EFFECT");
			}
			else
			{
				static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".right_top_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(p_taixiu_gameinfo->getTotalWinXiu()), true);
				static_cast<TextWidget*>(p_main_taixiu_layout->GetWidgetChildByName(".right_middle_bg")->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(p_taixiu_gameinfo->getTotalUserXiu()) , true);
				p_main_taixiu_layout->GetWidgetChildByName(".right_top_bg")->GetWidgetChildAtIdx(3)->SetActionCommandWithName("RUN_EFFECT");
			}
			tx_user->Visible(false); 		
		}),nullptr));
	}
}

void TaiXiuLayer::OnReceivedTaiXiuBetResult(TaiXiuBetResult* taiXiuBetResult)
{
	SAFE_DELETE(p_taixiu_bet_result);
	p_taixiu_bet_result = taiXiuBetResult;

	if (p_taixiu_bet_result->getBetWin() == 0)
	{
		ScrMgr->OnShowOKDialog(p_taixiu_bet_result->getMessage());
		return;
	}

	SetNumberBetResult(p_taixiu_bet_result->getBetWin(), p_taixiu_bet_result->getBetChoice());
}

void TaiXiuLayer::OnReceivedTaiXiuUserOrderList(TaiXiuUserOrderList * taixiuOderList)
{
	SAFE_DELETE(p_taixiu_userorder_list);
	p_taixiu_userorder_list = taixiuOderList;

	p_taixiu_list_player_leaderboard->Visible(false);

	/*p_taixiu_list_player_leaderboard->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(false);
	p_taixiu_list_player_leaderboard->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(true);
	p_taixiu_list_player_leaderboard->GetParentWidget()->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");*/

	static_cast<IwinListViewTable*>(p_taixiu_list_player_leaderboard)->SetHaveReload([this]() {
		p_taixiu_list_player_leaderboard->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(false);
		if (p_taixiu_userorder_list->getUserOrderList().size() > 0)
		{
			p_taixiu_list_player_leaderboard->Visible(true);
			p_taixiu_list_player_leaderboard->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(false);
			static_cast<ListViewTableWidget*>(p_taixiu_list_player_leaderboard)->SetNumberSizeOfCell(p_taixiu_userorder_list->getUserOrderList().size() , true);
		}
		else
		{
			p_taixiu_list_player_leaderboard->Visible(false);
			p_taixiu_list_player_leaderboard->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(true);
		}
	});
}

void TaiXiuLayer::OnReceivedTaiXiuUserHistoryList(TaiXiuUserHistoryList* taixiuHistoryList)
{
	SAFE_DELETE(p_taixiu_history_list);
	p_taixiu_history_list = taixiuHistoryList;

	p_taixiu_list_myhistory->Visible(false);

	/*p_taixiu_list_myhistory->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(false);
	p_taixiu_list_myhistory->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(true);
	p_taixiu_list_myhistory->GetParentWidget()->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");*/

	static_cast<IwinListViewTable*>(p_taixiu_list_myhistory)->SetHaveReload([this]() {
		p_taixiu_list_myhistory->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(false);

		if (p_taixiu_history_list->getUserHistoryList().size() > 0)
		{
			p_taixiu_list_myhistory->Visible(true);
			p_taixiu_list_myhistory->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(false);
			static_cast<ListViewTableWidget*>(p_taixiu_list_myhistory)->SetNumberSizeOfCell(p_taixiu_history_list->getUserHistoryList().size() , true);
		}
		else
		{
			p_taixiu_list_myhistory->Visible(false);
			p_taixiu_list_myhistory->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(true);
		}
		
	});
}

void TaiXiuLayer::OnReceiveTaiXiuMatch(TaiXiuMatch * data)
{
	SAFE_DELETE(p_taixiu_match);
	p_taixiu_match = data;

	//p_taixiu_list_match_history->Visible(false);

	s64 total_tai = 0;
	s64 total_xiu = 0;
	s64 total_total = 0;
	_cur_match_id = p_taixiu_match->getId();
	for (auto m : p_taixiu_match->getUserBets())
	{
		if (m.getBetChoice() == 0) // tai
		{
			total_tai += m.getBetMoney();
		}
		else
		{
			total_xiu += m.getBetMoney();
		}
		total_total += m.getBetMoney();
	}

	static_cast<TextWidget*>(p_layout_taixiu_match_history->GetWidgetChildByName(".layout_tai_bottom.txt0"))->SetText(Utils::formatNumber_dot(total_tai) , true);
	static_cast<TextWidget*>(p_layout_taixiu_match_history->GetWidgetChildByName(".layout_xiu_bottom.txt0"))->SetText(Utils::formatNumber_dot(total_xiu), true);
	static_cast<TextWidget*>(p_layout_taixiu_match_history->GetWidgetChildByName(".layout_total_bottom.txt0"))->SetText(Utils::formatNumber_dot(total_total), true);

	static_cast<TextWidget*>(p_layout_taixiu_match_history->GetWidgetChildByName(".title_bg_top.txt0"))->SetText(StringUtils::format(LANG_STR("tx_phien_id").GetString() , _cur_match_id), true);
	if (_cur_match_id != _live_match_id)
	{
		static_cast<TextWidget*>(p_layout_taixiu_match_history->GetWidgetChildByName(".title_bg_top.txt1"))->SetText(
			StringUtils::format((p_taixiu_match->getResult() == 0 ? LANG_STR("tx_tai_id").GetString() : LANG_STR("tx_xiu_id").GetString()),
				p_taixiu_match->getDice1() + p_taixiu_match->getDice2() + p_taixiu_match->getDice3(), p_taixiu_match->getDice1(), p_taixiu_match->getDice2(), p_taixiu_match->getDice3()), true);
		static_cast<TextWidget*>(p_layout_taixiu_match_history->GetWidgetChildByName(".title_bg_top.btn_cur_phien"))->Visible(true);
	}
	else
	{
		static_cast<TextWidget*>(p_layout_taixiu_match_history->GetWidgetChildByName(".title_bg_top.txt1"))->SetText("on_going");
		static_cast<TextWidget*>(p_layout_taixiu_match_history->GetWidgetChildByName(".title_bg_top.btn_cur_phien"))->Visible(false);
	}



	static_cast<IwinListViewTable*>(p_taixiu_list_match_history)->SetHaveReload([this]()
	{
		p_taixiu_list_match_history->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(false);

		if (p_taixiu_match->getUserBets().size() > 0)
		{
			//p_taixiu_match->ReserveBetList();
		//	p_taixiu_match->getUserBets().reserve();
			p_taixiu_list_match_history->Visible(true);
			p_taixiu_list_match_history->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(false);
			int number_cell = static_cast<ListViewTableWidget*>(p_taixiu_list_match_history)->GetNumberSizeOfCell();

			bool checkIndex0 = true;
			if (number_cell > 1)
			{
				checkIndex0 = static_cast<ListViewTableWidget*>(p_taixiu_list_match_history)->IsIndexInIndiceList(number_cell - 1);
			}
			static_cast<ListViewTableWidget*>(p_taixiu_list_match_history)->SetNumberSizeOfCell(p_taixiu_match->getUserBets().size() , checkIndex0);
			
			if (p_taixiu_match->getUserBets().size() >= static_cast<ListViewTableWidget*>(p_taixiu_list_match_history)->GetListWidgetSample().size())
			{
				_base_screen->PushEvent([this](BaseScreen * scr) {
					
					//static_cast<ListViewTableWidget*>(p_taixiu_list_match_history)->JumpToEnd();
				}, 0.1f);
			}
		}
		else
		{
			p_taixiu_list_match_history->Visible(false);
			p_taixiu_list_match_history->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(true);
		}

	});
}

void TaiXiuLayer::OnReceiveTaiXiuMatchList(TaiXiuMatchList* data)
{
	SAFE_DELETE(p_taixiu_match_list);
	p_taixiu_match_list = data;

	auto size_list = p_taixiu_match_list->getMatchList().size();
	auto last_id = p_taixiu_match_list->getMatchList()[size_list - 1];

	static_cast<TextWidget*>(p_layout_taixiu_chart->GetWidgetChildByName(".title_bg.txt0"))->SetText(StringUtils::format(LANG_STR("tx_last_match").GetString() , last_id.getId()) , true);
	
	static_cast<TextWidget*>(p_layout_taixiu_chart->GetWidgetChildByName(".title_bg.txt1"))->SetText(
		StringUtils::format((last_id.getResult() == 0 ? LANG_STR("tx_tai_id").GetString() : LANG_STR("tx_xiu_id").GetString()),
			last_id.getDice1() + last_id.getDice2() + last_id.getDice3(), last_id.getDice1(), last_id.getDice2(), last_id.getDice3()), true);

	OnShowChartPanelAtIdx(_idx_chart_panel);
}

WidgetEntity * TaiXiuLayer::GetNodeChartAtPoint(int i, int j)
{
	int idx = (i * DRIG_H) + (i % 2 != 0 ? j : (DRIG_H - 1 - j));
	auto w_node = p_list_node_layout->GetWidgetChildAtIdx(idx);
	return w_node;
}

void TaiXiuLayer::OnShowChartPanelAtIdx(int idx)
{
	if (idx < 0)
	{
		idx = 2;
	}
	else if (idx > 2)
	{
		idx = 0;
	}
	_idx_chart_panel = idx;
	if (_idx_chart_panel == 0)
	{
		OnShowChartDetail();
	}
	else if (_idx_chart_panel == 1)
	{
		OnShowChartListTotal();
	}
	else if (_idx_chart_panel == 2)
	{
		OnShowChartListTotalNear();
	}
}

void TaiXiuLayer::OnShowChartListTotal()
{
	if (!p_taixiu_match_list)
		return;

	p_layout_taixiu_chart->GetWidgetChildByName(".detail_chart_layout")->Visible(true);
	p_layout_taixiu_chart->GetWidgetChildByName(".dice_chart_layout")->Visible(false);
	p_layout_taixiu_chart->GetWidgetChildByName(".title_bg.layout_check_dice")->Visible(false);

	int idx = 0;
	int number_tai = 0;
	int number_xiu = 0;
	for (auto d : p_taixiu_match_list->getMatchList())
	{
		auto w_node = p_list_node_layout->GetWidgetChildAtIdx(idx);
		if (w_node)
		{
			static_cast<TextWidget*>(w_node->GetWidgetChildAtIdx(0))->Visible(false);
			if (d.getResult() == 0)
			{
				w_node->SetColor(COLOR_TAI);
				number_tai++;
			}
			else
			{
				w_node->SetColor(COLOR_XIU);
				number_xiu++;
			}
			w_node->Visible(true);
		}
		idx++;
	}
	auto bg = p_list_node_layout->GetParentWidget();
	static_cast<TextWidget*>(bg->GetWidgetChildByName(".thongketai_layout.txt"))->SetText(std::to_string(number_tai), true);
	static_cast<TextWidget*>(bg->GetWidgetChildByName(".thongkexiu_layout.txt"))->SetText(std::to_string(number_xiu), true);

	auto effect_icon = bg->GetWidgetChildByName(".effect_icon");
	effect_icon->SetVisible(true);
	effect_icon->SetActionCommandWithName("SET_POS_TOTAL", CallFunc::create([effect_icon]() {
		effect_icon->ForceFinishAction();
		effect_icon->SetActionCommandWithName("RUN_EFFECT");
	}));
}

void TaiXiuLayer::OnShowChartListTotalNear()
{
	if (!p_taixiu_match_list)
		return;

	p_layout_taixiu_chart->GetWidgetChildByName(".detail_chart_layout")->Visible(true);
	p_layout_taixiu_chart->GetWidgetChildByName(".dice_chart_layout")->Visible(false);
	p_layout_taixiu_chart->GetWidgetChildByName(".title_bg.layout_check_dice")->Visible(false);

	int idx = 0;
	for (auto d : p_taixiu_match_list->getMatchList())
	{
		auto w_node = p_list_node_layout->GetWidgetChildAtIdx(idx);
		if (w_node)
		{
			w_node->Visible(false);
		}
		idx++;
	}

	_base_screen->PushEvent([this](BaseScreen * scr) 
	{
		int idx_w = DRIG_W - 1;
		int idx_h = 0;
		int number_tai = 0;
		int number_xiu = 0;

		for (int i = p_taixiu_match_list->getMatchList().size() - 1; i >= 0; i--)
		{
			auto d = p_taixiu_match_list->getMatchList()[i];
			if (i < p_taixiu_match_list->getMatchList().size() - 1)
			{
				if (d.getResult() == p_taixiu_match_list->getMatchList()[i + 1].getResult())
				{
					idx_h++;
				}
				else
				{
					idx_h = 0;
					idx_w--;
				}
			}
			if (idx_h >= DRIG_H)
			{
				idx_h = 0;
				idx_w--;
			}

			if (idx_w < 0)
			{
				break;
			}

			auto w_node = GetNodeChartAtPoint(idx_w, idx_h);
			if (w_node)
			{
				static_cast<TextWidget*>(w_node->GetWidgetChildAtIdx(0))->Visible(true);
				static_cast<TextWidget*>(w_node->GetWidgetChildAtIdx(0))->SetText(std::to_string(d.getDice1() + d.getDice2() + d.getDice3()), true);
				if (d.getResult() == 0)
				{
					w_node->SetColor(COLOR_TAI);
					number_tai++;
				}
				else
				{
					w_node->SetColor(COLOR_XIU);
					number_xiu++;
				}
				w_node->Visible(true);
			}
		}

		auto bg = p_list_node_layout->GetParentWidget();
		static_cast<TextWidget*>(bg->GetWidgetChildByName(".thongketai_layout.txt"))->SetText(std::to_string(number_tai), true);
		static_cast<TextWidget*>(bg->GetWidgetChildByName(".thongkexiu_layout.txt"))->SetText(std::to_string(number_xiu), true);

		auto effect_icon = bg->GetWidgetChildByName(".effect_icon");
		effect_icon->SetVisible(true);
		effect_icon->SetActionCommandWithName("SET_POS_TOTAL_NEAR", CallFunc::create([effect_icon]() {
			effect_icon->ForceFinishAction();
			effect_icon->SetActionCommandWithName("RUN_EFFECT");
		}));
	});
}

void TaiXiuLayer::OnShowChartDetail()
{
	if (!p_taixiu_match_list)
		return;

	p_layout_taixiu_chart->GetWidgetChildByName(".detail_chart_layout")->Visible(false);
	p_layout_taixiu_chart->GetWidgetChildByName(".dice_chart_layout")->Visible(true);

	p_layout_taixiu_chart->GetWidgetChildByName(".title_bg.layout_check_dice")->Visible(true);

	//top dice

	int idx = p_taixiu_match_list->getMatchList().size() - 1;
	for (int i = p_layout_dice_chart_top->GetNumberChildWidget() - 1; i >= 0; i--)
	{
		auto w_node = p_layout_dice_chart_top->GetWidgetChildAtIdx(i);
		if (i % 2 != 0)//node
		{
			auto d = p_taixiu_match_list->getMatchList()[idx];
			if (w_node)
			{
				int total_dice = d.getDice1() + d.getDice2() + d.getDice3();
				static_cast<TextWidget*>(w_node->GetWidgetChildAtIdx(0))->Visible(true);
				static_cast<TextWidget*>(w_node->GetWidgetChildAtIdx(0))->SetText(std::to_string(total_dice), true);
				if (d.getResult() == 0)
				{
					w_node->SetColor(COLOR_TAI);
				}
				else
				{
					w_node->SetColor(COLOR_XIU);
				}
				float size_h = p_layout_dice_chart_top->GetSize().y;
				Vec2 pos_node = w_node->GetPosition();
				if (total_dice == 3)
				{
					w_node->SetPosition(Vec2(pos_node.x, 0));
				}
				else if (total_dice == 18)
				{
					w_node->SetPosition(Vec2(pos_node.x, size_h));
				}
				else
				{
					float delta = 18 - 3;
					float h = (total_dice - 3.f) * size_h / delta;
					w_node->SetPosition(Vec2(pos_node.x, h));
				}

				w_node->Visible(true);

				if (i == p_layout_dice_chart_top->GetNumberChildWidget() - 1)
				{
					auto effect_icon = p_layout_dice_chart_top->GetParentWidget()->GetWidgetChildByName(".effect_icon");
					effect_icon->SetVisible(true);
					effect_icon->SetPosition(w_node->GetPosition());
					effect_icon->SetActionCommandWithName("RUN_EFFECT");
				}
			}
			idx--;
		}
		else //line
		{
			if (i == p_layout_dice_chart_top->GetNumberChildWidget() - 2)
			{
				w_node->SetVisible(false);
			}
			else
			{
				w_node->SetVisible(true);
				Vec2 pos_front_node = p_layout_dice_chart_top->GetWidgetChildAtIdx(i + 3)->GetPosition();
				Vec2 pos_cur_node = p_layout_dice_chart_top->GetWidgetChildAtIdx(i + 1)->GetPosition();

				w_node->SetPosition(pos_cur_node);
				Vec2 vec_point = Vec2(pos_front_node.x - pos_cur_node.x, pos_front_node.y - pos_cur_node.y);
				float distance = pos_front_node.distance(pos_cur_node);
				float rotate = Vec2::angle(vec_point, Vec2(1, 0)) * 180.f / 3.141f;

				w_node->SetSize(Vec2(distance, 1));


				w_node->SetRotate((pos_cur_node.y > pos_front_node.y) ? rotate : (360 - rotate));
			}
		}
	}

	OnParseChartDice(0);
	OnParseChartDice(1);
	OnParseChartDice(2);
}

void TaiXiuLayer::OnShowChartDice(int idx_dice, bool visible)
{
	auto panel = p_layout_dice_chart_bot_dice1;
	if (idx_dice == 1)
	{
		panel = p_layout_dice_chart_bot_dice2;
	}
	else if (idx_dice == 2)
	{
		panel = p_layout_dice_chart_bot_dice3;
	}

	panel->Visible(visible);
}

void TaiXiuLayer::OnParseChartDice(int idx_dice)
{
	//bot dice 
	int idx = p_taixiu_match_list->getMatchList().size() - 1;

	auto panel = p_layout_dice_chart_bot_dice1;
	if (idx_dice == 1)
	{
		panel = p_layout_dice_chart_bot_dice2;
	}
	else if (idx_dice == 2)
	{
		panel = p_layout_dice_chart_bot_dice3;
	}
	//dice1
	for (int i = panel->GetNumberChildWidget() - 1; i >= 0; i--)
	{
		auto w_node = panel->GetWidgetChildAtIdx(i);
		if (i % 2 != 0)//node
		{
			auto d = p_taixiu_match_list->getMatchList()[idx];
			if (w_node)
			{
				int dice = (idx_dice == 0 ? d.getDice1() : (idx_dice == 1 ? d.getDice2(): d.getDice3() ));
				float size_h = panel->GetSize().y;
				Vec2 pos_node = w_node->GetPosition();
				if (dice == 1)
				{
					w_node->SetPosition(Vec2(pos_node.x, 0));
				}
				else if (dice == 6)
				{
					w_node->SetPosition(Vec2(pos_node.x, size_h));
				}
				else
				{
					float delta = 6 - 1;
					float h = (dice - 1.f) * size_h / delta;
					w_node->SetPosition(Vec2(pos_node.x, h));
				}

				w_node->Visible(true);
			}
			idx--;
		}
		else //line
		{
			if (i == panel->GetNumberChildWidget() - 2)
			{
				w_node->SetVisible(false);
			}
			else
			{
				w_node->SetVisible(true);
				Vec2 pos_front_node = panel->GetWidgetChildAtIdx(i + 3)->GetPosition();
				Vec2 pos_cur_node = panel->GetWidgetChildAtIdx(i + 1)->GetPosition();

				w_node->SetPosition(pos_cur_node);
				Vec2 vec_point = Vec2(pos_front_node.x - pos_cur_node.x, pos_front_node.y - pos_cur_node.y);
				float distance = pos_front_node.distance(pos_cur_node);
				float rotate = Vec2::angle(vec_point, Vec2(1, 0)) * 180.f / 3.141f;

				w_node->SetSize(Vec2(distance, 1));
				w_node->SetRotate((pos_cur_node.y > pos_front_node.y) ? rotate : (360 - rotate));
			}
		}
	}
}

void TaiXiuLayer::ResetAndSendSerVer()
{
	ChangeButtonWinRubyTaiXiu();
	ScrMgr->SetScreenIDDirty();
	// GlobalService.sendScreenIdToServer(ScreenID.SCREEN_TAIXIU_RUBY);
	GlobalService::getTaiXiuInfo(GameController::myInfo->name);
}

void TaiXiuLayer::ChangeButtonWinRubyTaiXiu()
{
	if (GameController::typeMoneyTaiXiu == BASE_MONEY)
	{
		p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".left_panel_act")->Visible(true);
		p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".right_panel_act")->Visible(false);

		p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_win_layout")->Visible(true);
		p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_ruby_layout")->Visible(false);
	}
	else
	{
		p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".left_panel_act")->Visible(false);
		p_panel_taixiu_bg_top_layout->GetWidgetChildByName(".right_panel_act")->Visible(true);

		p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_win_layout")->Visible(false);
		p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_ruby_layout")->Visible(true);
	}
}

void TaiXiuLayer::OnCheckTextInputMoney(WidgetEntity * _widget)
{
	s64 current_input_money = atoll(static_cast<TextFieldWidget*>(_widget)->GetText().GetString());
	s64 current_money = ((GameController::typeMoneyTaiXiu == BASE_MONEY) ? GameController::myInfo->money : GameController::myInfo->ruby);
	if (current_money >= 1000)
	{
		if (current_input_money > current_money)
		{
			int mul_money = current_money % 1000;
			current_money = current_money - mul_money;
			static_cast<TextFieldWidget*>(_widget)->ClearText();
			static_cast<TextFieldWidget*>(_widget)->SetText(std::to_string(current_money));
		}
		else
		{
			int mul_money = current_input_money % 1000;
			current_input_money = current_input_money - mul_money;
			static_cast<TextFieldWidget*>(_widget)->ClearText();
			if (current_input_money < 1000)
			{
				current_input_money = 1000;
			}
			static_cast<TextFieldWidget*>(_widget)->SetText(std::to_string(current_input_money));
		}
	}
	else
	{
		static_cast<TextFieldWidget*>(_widget)->ClearText();
		static_cast<TextFieldWidget*>(_widget)->SetText(std::to_string(current_money));
	}
}

void TaiXiuLayer::OnShowPanelBetTaixiu(bool show, CallFunc * call_back)
{
	if (current_stage_bet_taixiu == 0 || current_stage_bet_taixiu == -1)
	{
		if (show)
		{
			p_layout_taixiu_bet->GetWidgetChildByName(".taxiu_bet_tai_btn")->Visible(true);
			p_layout_taixiu_bet->GetWidgetChildByName(".taxiu_bet_xiu_btn")->Visible(false);
			auto w = p_layout_taixiu_bet->GetWidgetChildByName(".panel_bg");
			if (GameController::typeMoneyTaiXiu == RUBY_MONEY)
			{
				w->Visible(false);
				w = p_layout_taixiu_bet->GetWidgetChildByName(".panel_bg_ruby");
			}
			w->Visible(true);
			auto current_scale = w->GetScale();
			if (current_scale.x > 0)
			{
				w->SetScale(Vec2(-current_scale.x, current_scale.y));
			}
			else
			{
				w->SetScale(Vec2(current_scale.x, current_scale.y));
			}
			p_layout_taixiu_bet->SetActionCommandWithName("ON_SHOW_TAI", call_back);
		}
		else
		{
			p_layout_taixiu_bet->SetActionCommandWithName("ON_HIDE_TAI", call_back);
		}
	}
	else if (current_stage_bet_taixiu == 1 || current_stage_bet_taixiu == -1)
	{
		if (show)
		{
			p_layout_taixiu_bet->GetWidgetChildByName(".taxiu_bet_tai_btn")->Visible(false);
			p_layout_taixiu_bet->GetWidgetChildByName(".taxiu_bet_xiu_btn")->Visible(true);
			auto w = p_layout_taixiu_bet->GetWidgetChildByName(".panel_bg");
			if (GameController::typeMoneyTaiXiu == RUBY_MONEY)
			{
				w->Visible(false);
				w = p_layout_taixiu_bet->GetWidgetChildByName(".panel_bg_ruby");
			}
			w->Visible(true);
			auto current_scale = w->GetScale();
			if (current_scale.x > 0)
			{
				w->SetScale(Vec2(current_scale.x, current_scale.y));
			}
			else
			{
				w->SetScale(Vec2(-current_scale.x, current_scale.y));
			}
			p_layout_taixiu_bet->SetActionCommandWithName("ON_SHOW_XIU", call_back);
		}
		else
		{
			p_layout_taixiu_bet->SetActionCommandWithName("ON_HIDE_XIU", call_back);
		}
	}

	if (show)
	{
		SliderWidget * slider = static_cast<SliderWidget*>(GetSliderBet());
		s64 current_money = GetCurrentMoney();
		if (current_money <= 1000)
		{
			slider->SetPart(0);
		}
		else
		{
			int number_part = current_money / 1000;
			slider->SetPart(number_part);
		}
	}
}

void TaiXiuLayer::SetInputMoneyTextBet(s64 money)
{
	if (money > 1000)
	{
		int mul = money % 1000;
		money = money - mul;
	}

	if (GameController::typeMoneyTaiXiu == BASE_MONEY)
	{
		if (money < 1000)
		{
			money = 1000;
		}
	}
	else if (GameController::typeMoneyTaiXiu == RUBY_MONEY)
	{
		if (money < 1)
		{
			money = 1;
		}
	}

	if (current_stage_bet_taixiu == 0)
	{
		auto wi = p_main_taixiu_layout->GetWidgetChildByName(".left_input_bg")->GetWidgetChildAtIdx(3);
		static_cast<TextFieldWidget*>(wi)->ClearText();
		static_cast<TextFieldWidget*>(wi)->SetText(std::to_string(money));
	}
	else if (current_stage_bet_taixiu == 1)
	{
		auto wi = p_main_taixiu_layout->GetWidgetChildByName(".right_input_bg")->GetWidgetChildAtIdx(3);
		static_cast<TextFieldWidget*>(wi)->ClearText();
		static_cast<TextFieldWidget*>(wi)->SetText(std::to_string(money));
	}
	else
	{
		auto wi = p_main_taixiu_layout->GetWidgetChildByName(".left_input_bg")->GetWidgetChildAtIdx(3);
		static_cast<TextFieldWidget*>(wi)->ClearText();
		static_cast<TextFieldWidget*>(wi)->SetText(std::to_string(money));

		wi = p_main_taixiu_layout->GetWidgetChildByName(".right_input_bg")->GetWidgetChildAtIdx(3);
		static_cast<TextFieldWidget*>(wi)->ClearText();
		static_cast<TextFieldWidget*>(wi)->SetText(std::to_string(money));
	}
}

WidgetEntity * TaiXiuLayer::GetSliderBet()
{
	WidgetEntity * wi = nullptr;
	if (GameController::typeMoneyTaiXiu == BASE_MONEY)
	{
		wi = (p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_win_layout.slider_bet_win"));
	}
	else
	{
		wi = (p_layout_taixiu_bet->GetWidgetChildByName(".slider_bet_ruby_layout.slider_bet_ruby"));
	}
	return wi;
}

s64 TaiXiuLayer::GetCurrentMoney()
{
	s64 current_money = ((GameController::typeMoneyTaiXiu == BASE_MONEY) ? GameController::myInfo->money : GameController::myInfo->ruby);
	return current_money;
}

void TaiXiuLayer::BetMoney(int stage)
{
	s64 current_money = 0;
	if (stage == 0)
	{
		auto wi = p_main_taixiu_layout->GetWidgetChildByName(".left_input_bg")->GetWidgetChildAtIdx(3);
		auto str = static_cast<TextFieldWidget*>(wi)->GetText();
		current_money = Utils::GetNumberFromFormat(str.GetString());
	}
	else if (stage == 1)
	{
		auto wi = p_main_taixiu_layout->GetWidgetChildByName(".right_input_bg")->GetWidgetChildAtIdx(3);
		auto str = static_cast<TextFieldWidget*>(wi)->GetText();
		current_money = Utils::GetNumberFromFormat(str.GetString());
	}
	else
	{
		PASSERT2(false, "wrong here");
	}

	GlobalService::taiXiuBet(GameController::myInfo->name, current_money, stage);
}

void TaiXiuLayer::SetNumberBetResult(s64 money_total, int state)
{
	WidgetEntity * wi = nullptr;
	if (state == 0)
	{
		wi = p_main_taixiu_layout->GetWidgetChildByName(".left_input_bg")->GetWidgetChildAtIdx(1);
	}
	else if (state == 1)
	{
		wi = p_main_taixiu_layout->GetWidgetChildByName(".right_input_bg")->GetWidgetChildAtIdx(1);
	}
	else
	{
		PASSERT2(false, "wrong here");
	}
	s64 current_number = Utils::GetNumberFromFormat(static_cast<TextWidget*>(wi)->GetText().GetString());
	money_total += current_number;
	static_cast<TextWidget*>(wi)->TextRunEffect(
		money_total, current_number, 1.f,
		[](s64 money)->std::string
	{
		return Utils::formatNumber_dot(abs(money));
	},
		CallFunc::create([wi, money_total]()
	{
		static_cast<TextWidget*>(wi)->SetText(Utils::formatNumber_dot(money_total), true);
	}));

	//reset text field tai and xiu

	SetInputMoneyTextBet(1000);
}


void TaiXiuLayer::OnShowResultTaiXiu(int result, bool show)
{
	auto tai_effect = p_main_taixiu_layout->GetWidgetChildByName(".tai_panel_bg")->GetWidgetChildAtIdx(1);
	auto xiu_effect = p_main_taixiu_layout->GetWidgetChildByName(".xiu_panel_bg")->GetWidgetChildAtIdx(1);
	if (show)
	{
		if (result == 0) //tai
		{
			tai_effect->Visible(true);
			tai_effect->SetActionCommandWithName("ACTION");
			xiu_effect->Visible(false);
		}
		else
		{
			xiu_effect->Visible(true);
			xiu_effect->SetActionCommandWithName("ACTION");
			tai_effect->Visible(false);
		}
	}
	else
	{
		xiu_effect->ForceFinishAction();
		tai_effect->ForceFinishAction();

		xiu_effect->Visible(false);
		tai_effect->Visible(false);
	}
}

void TaiXiuLayer::OnParseTaiXiuLeaderboard(WidgetEntity* _widget, iwinmesage::TaiXiuUserOrder * user, int idx)
{
	if (!user || !_widget)
		return;

	if (idx % 2 == 0)
	{
		_widget->GetWidgetChildAtIdx(0)->Visible(true);
		_widget->GetWidgetChildAtIdx(1)->Visible(false);
	}
	else
	{
		_widget->GetWidgetChildAtIdx(0)->Visible(false);
		_widget->GetWidgetChildAtIdx(1)->Visible(true);
	}

	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".stt_id"))->SetText(std::to_string(user->getOrder()) , true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(user->getUsername(), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatNumber_dot(user->getWin()), true);
}


void TaiXiuLayer::OnParseTaiXiuMatchHistory(WidgetEntity* _widget, iwinmesage::TaiXiuUserBet * his, int idx)
{
	if (!his || !_widget)
		return;

	if (idx % 2 == 0)
	{
		_widget->GetWidgetChildAtIdx(0)->Visible(true);
		_widget->GetWidgetChildAtIdx(1)->Visible(false);
	}
	else
	{
		_widget->GetWidgetChildAtIdx(0)->Visible(false);
		_widget->GetWidgetChildAtIdx(1)->Visible(true);
	}

	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".txt0"))->SetText(Utils::ConvertTime(his->getTime()), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".txt2"))->SetText(his->getUsername(), true);
	
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".txt1"))->SetText(his->getBetChoice() == 0 ? "taixiu_tai" : "taixiu_xiu");
	_widget->GetWidgetChildByName(".txt1")->SetColor(his->getBetChoice() == 0 ? COLOR_TAI : COLOR_XIU);

	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".txt3"))->SetText(Utils::formatNumber_dot(his->getBetMoney()), true);
	_widget->GetWidgetChildByName(".txt3")->SetColor(his->getBetChoice() == 0 ? COLOR_TAI : COLOR_XIU);


}

void TaiXiuLayer::OnParseTaiXiuHistory(WidgetEntity* _widget, iwinmesage::TaiXiuUserHistory * his, int idx)
{
	if (!his || !_widget)
		return;

	if (idx % 2 == 0)
	{
		_widget->GetWidgetChildAtIdx(0)->Visible(true);
		_widget->GetWidgetChildAtIdx(1)->Visible(false);
	}
	else
	{
		_widget->GetWidgetChildAtIdx(0)->Visible(false);
		_widget->GetWidgetChildAtIdx(1)->Visible(true);
	}

	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".phien_id"))->SetText(std::to_string(his->getMatchId()), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".time_id"))->SetText(Utils::ConvertTime(his->getTime()), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".bet_way_id"))->SetText(his->getBetChoice() == 0 ? "taixiu_tai" : "taixiu_xiu");
	_widget->GetWidgetChildByName(".bet_way_id")->SetColor(his->getBetChoice() == 0 ? COLOR_TAI : COLOR_XIU);

	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".result_id"))->SetText(his->getResult(), true);
	_widget->GetWidgetChildByName(".result_id")->SetColor(his->getResultType() == 0 ? COLOR_TAI : COLOR_XIU);

	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".bet_id"))->SetText(Utils::formatNumber_dot_short(his->getBetWin()), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".return_id"))->SetText(Utils::formatNumber_dot_short(his->getReturnWin()), true);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".receive_id"))->SetText(Utils::formatNumber_dot_short(his->getReceivedWin()), true);

}

void TaiXiuLayer::OnShowResultCirlceWithTimer(bool show, double long total_time, double long current_time)
{
	if (show)
	{
		p_taixiu_circle_cd_layout->Visible(false);
		p_taixiu_circle_result_layout->Visible(true);
		static_cast<TimeProcessWidget*>(p_taixiu_circle_layout->GetWidgetChildByName(".circle_time_process"))->SetTimer(total_time, current_time);
	}
	else
	{
		p_taixiu_circle_cd_layout->Visible(true);
		p_taixiu_circle_result_layout->Visible(false);
		static_cast<TimeDigitalWidget*>(p_taixiu_circle_cd_layout->GetWidgetChildByName(".circle_time"))->SetTimer(total_time, current_time);
	}
}

void TaiXiuLayer::OnShowResultCircle()
{
	if (!p_taixiu_circle_small_result_layout->Visible())
	{
		p_taixiu_circle_small_result_layout->SetActionCommandWithName("ON_SHOW");
	}
	if (p_result_taixiu == 0) // tai
	{
		p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_tai")->Visible(true);
		p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_xiu")->Visible(false);
	}
	else
	{
		p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_tai")->Visible(false);
		p_taixiu_circle_result_layout->GetWidgetChildByName(".circle_xiu")->Visible(true);
	}
	this->OnShowResultTaiXiu(p_result_taixiu, true);
}

int TaiXiuLayer::GetStateTaiXiu()
{
	if (p_layout_tai_xiu_panel->Visible())
	{
		if (GameController::typeMoneyTaiXiu == BASE_MONEY)
		{
			return 3;
		}
		else
		{
			return 4;
		}
	}
	return -1;
}


void TaiXiuLayer::OnShowTaiXiuLayoutAtIdx(int idx)
{
	if (idx == BASE_MONEY)
	{
		GameController::typeMoneyTaiXiu = BASE_MONEY;
		p_have_change_tab_taixiu = true;
		p_taixiu_already_have_result = false;
		ResetAndSendSerVer();

	}
	else if (idx == RUBY_MONEY)
	{
		GameController::typeMoneyTaiXiu = RUBY_MONEY;
		p_have_change_tab_taixiu = true;
		p_taixiu_already_have_result = false;
		ResetAndSendSerVer();
	}
}

void TaiXiuLayer::OnInsertNodeChartToList(int i, int j)
{
	auto n = p_chart_node->Clone();
	cocos2d::Size size_each_node = cocos2d::Size(p_list_node_layout->GetSize().x / DRIG_W, p_list_node_layout->GetSize().y / DRIG_H);

	n->SetPosition(Vec2(i * size_each_node.width + (size_each_node.width / 2), j* size_each_node.height + (size_each_node.height / 2)));
	//n->Visible(true);

	p_list_node_layout->InsertChildWidget(n);
}

void TaiXiuLayer::OnInsertNodeToTopDice(int i)
{
	auto n = p_chart_node->Clone();
	auto l = p_line_node->Clone();
	l->SetColor(Color3B(100,255,100));
	float distance_each_line_drig = p_layout_dice_chart_top->GetSize().x / DRIG_W;

	n->SetPosition(Vec2(i * distance_each_line_drig, 0));
	n->SetZOrder(100 + i);
	l->SetPosition(n->GetPosition());
	l->SetZOrder(i);

	p_layout_dice_chart_top->InsertChildWidget(l);
	p_layout_dice_chart_top->InsertChildWidget(n);
}

void TaiXiuLayer::OnInsertNodeToBotDice(int i, int idx_dice)
{
	auto panel = p_layout_dice_chart_bot_dice1;
	if (idx_dice == 1)
	{
		panel = p_layout_dice_chart_bot_dice2;
	}
	else if (idx_dice == 2)
	{
		panel = p_layout_dice_chart_bot_dice3;
	}

	auto n = p_chart_node_e->Clone();
	auto l = p_line_node->Clone();
	float distance_each_line_drig = panel->GetSize().x / DRIG_W;

	n->SetPosition(Vec2(i * distance_each_line_drig, 0));
	n->SetZOrder(100 + i);
	l->SetPosition(n->GetPosition());
	l->SetZOrder(i);

	panel->InsertChildWidget(l);
	panel->InsertChildWidget(n);
}
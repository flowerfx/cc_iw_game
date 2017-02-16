#include "MiniPokerLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wPanel.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wParticle.h"

#include "Screens/CommonScreen.h"
#include "Screens/Object/Card.h"
#include "Common/IwinListViewTable.h"
#include "Common/IwinListItemCircle.h"

#include "Network/Global/Globalservice.h"
#include "Network/JsonObject/Nohu/NoHuBetRequest.h"
#include "Network/JsonObject/Nohu/NoHuBetResult.h"
#include "Network/JsonObject/Nohu/NoHuGameInfo.h"
#include "Network/JsonObject/Nohu/NoHuHistoryRequest.h"
#include "Network/JsonObject/Nohu/NoHuListUser.h"
#include "Network/JsonObject/Nohu/NoHuUnsupportedList.h"
#include "Network/JsonObject/Nohu/NoHuUserHistoryList.h"
#include "Network/JsonObject/Nohu/NoHuUserHistoryServer.h"
#include "InputManager.h"
using namespace iwinmesage;

MiniPokerLayer::MiniPokerLayer()
{
	_nohu_game_info = nullptr;
	_nohu_bet_result = nullptr;
	_nohu_my_history = nullptr;
	_nohu_list_user = nullptr;
	_have_first_set_rotate_card = true;
	p_list_img_jar.clear();
	p_list_img_jar_ruby.clear();

	_money_bonus = 0;
	_delta_move_stick_tx = 0;
	_id_win_no_hu = 0;
	_have_no_hu = false;
	_money_receive_no_hu = 0;
	_auto_bet_nohu = false;

	p_list_circle_jar = nullptr;
	p_list_circle_jar_ruby = nullptr;
}

MiniPokerLayer::~MiniPokerLayer()
{
	SAFE_DELETE(_nohu_game_info);
	SAFE_DELETE(_nohu_bet_result);
	SAFE_DELETE(_nohu_my_history);
	SAFE_DELETE(_nohu_list_user);

	p_list_img_jar.clear();
	p_list_img_jar_ruby.clear();

	SAFE_DELETE(p_list_circle_jar);
	SAFE_DELETE(p_list_circle_jar_ruby);
}

int MiniPokerLayer::Update(float dt)
{
	if (!IsVisibleLayer())
		return 0;

	static_cast<IwinListViewTable*>(p_poker_listplayer_listview)->UpdateReload(dt);
	static_cast<IwinListViewTable*>(p_poker_myhistory_listview)->UpdateReload(dt);

	OnDragStick();

	p_list_circle_jar->update(dt);
	p_list_circle_jar_ruby->update(dt);
	return 1;
}

bool MiniPokerLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_close_poker_panel")
		{
			OnShowMiniPokerLayout(false);
			return true;
		}
		else if (name == "poker_btn_leaderboard")
		{
			OnShowListPlayerPokerLayout(true);
			return true;
		}
		else if (name == "btn_close_lb_poker")
		{
			OnShowListPlayerPokerLayout(false);
			return true;
		}
		else if (name == "btn_forward_next")
		{
			GlobalService::nohu_sendTopHistory(_page_user + 1);
			return true;
		}
		else if (name == "btn_forward_end")
		{
			GlobalService::nohu_sendTopHistory(_nohu_list_user ? _nohu_list_user->getTotalPage() - 1 : 15);
			return true;
		}
		else if (name == "btn_backward_next")
		{
			GlobalService::nohu_sendTopHistory(_page_user - 1);
			return true;
		}
		else if (name == "btn_backward_end")
		{
			GlobalService::nohu_sendTopHistory(0);
			return true;
		}

		else if (name == "poker_btn_myhistory")
		{
			OnShowListMyHistoryPokerLayout(true);
			return true;
		}
		else if (name == "btn_close_history_poker")
		{
			OnShowListMyHistoryPokerLayout(false);
			return true;
		}
		else if (name == "btn_forward_next_his")
		{
			GlobalService::nohu_sendMyHistory(_page_my_history + 1);
			return true;
		}
		else if (name == "btn_forward_end_his")
		{
			GlobalService::nohu_sendMyHistory(_nohu_my_history ? _nohu_my_history->getTotalPage() - 1 : 1);
			return true;
		}
		else if (name == "btn_backward_next_his")
		{
			GlobalService::nohu_sendMyHistory(_page_my_history - 1);
			return true;
		}
		else if (name == "btn_backward_end_his")
		{
			GlobalService::nohu_sendMyHistory(0);
			return true;
		}

		else if (name == "left_panel_btn")
		{
			ChangeStateMoneyNohu(BASE_MONEY);
			return true;
		}
		else if (name == "right_panel_btn")
		{
			ChangeStateMoneyNohu(RUBY_MONEY);
			return true;
		}

		else if (name == "jar_forward_btn")
		{
			if (GameController::typeMoneyNohu == BASE_MONEY)
			{
				p_list_circle_jar->MoveToPreviousItem();
			}
			else
			{
				p_list_circle_jar_ruby->MoveToPreviousItem();
			}
			return true;
		}
		else if (name == "jar_backward_btn")
		{		
			if (GameController::typeMoneyNohu == BASE_MONEY)
			{
				p_list_circle_jar->MoveToNextItem();
			}
			else
			{
				p_list_circle_jar_ruby->MoveToNextItem();
			}
			return true;
		}
		
	}
	else if (type_widget == UI_TYPE::UI_CHECK_BOX)
	{
		if (name == "check_box_auto_play_SELECTED")
		{
			_auto_bet_nohu = true;
			return true;
		}
		else if (name == "check_box_auto_play_UNSELECTED")
		{
			_auto_bet_nohu = false;
			return true;
		}
	}
	return false;
}

void MiniPokerLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);

	auto mini_poker_layout = screen->GetWidgetChildByName("common_screen.mini_poker_layout");
	p_mini_poker_panel = mini_poker_layout->GetWidgetChildByName(".poker_panel");

	p_poker_top_layer =  p_mini_poker_panel->GetWidgetChildByName(".poker_top_layout");

	p_mini_poker_list_player_layout = mini_poker_layout->GetWidgetChildByName(".poker_list_player_layout.poker_list_player_layout");
	p_poker_listplayer_listview = p_mini_poker_list_player_layout->GetWidgetChildByName(".nohu_list_player");

	auto nh_list_player_lb = static_cast<IwinListViewTable*>(p_poker_listplayer_listview);
	nh_list_player_lb->InitWithParam(
		nullptr,
		nullptr,
		[this, nh_list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = nh_list_player_lb->GetCurrentIdxTo();
			if ((size_t)current_idx_to >= this->_nohu_list_user->getUsers().size())
			{
				PASSERT2(false, "problem here !");
				current_idx_to = this->_nohu_list_user->getUsers().size() - 1;
			}
			OnParseListPlayerNohu(_widget, _nohu_list_user->getUsers()[current_idx_to], current_idx_to);
		}
	}, nullptr, nullptr, nullptr);

	p_mini_poker_myhistory_layout = mini_poker_layout->GetWidgetChildByName(".poker_myhistory_layout.poker_myhistory_layout");
	p_poker_myhistory_listview = p_mini_poker_myhistory_layout->GetWidgetChildByName(".nohu_list_myhistory");
	auto nh_list_history_lb = static_cast<IwinListViewTable*>(p_poker_myhistory_listview);
	nh_list_history_lb->InitWithParam(
		nullptr,
		nullptr,
		[this, nh_list_history_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = nh_list_history_lb->GetCurrentIdxTo();
			if ((size_t)current_idx_to >= this->_nohu_my_history->getHistory().size())
			{
				PASSERT2(false, "problem here !");
				current_idx_to = this->_nohu_my_history->getHistory().size() - 1;
			}
			OnParseListHistoryNohu(_widget, _nohu_my_history->getHistory()[current_idx_to], current_idx_to);
		}
	}, nullptr, nullptr, nullptr);

	_page_user = 0;
	_page_my_history = 0;

	//panel right
	auto right_panel = p_mini_poker_panel->GetWidgetChildByName(".panel_right");
	p_stick_layout = right_panel->GetWidgetChildByName(".otruc_panel");
	p_stick_sphere = p_stick_layout->GetWidgetChildByName(".taycan_panel");
	p_stick_pivot = p_stick_layout->GetWidgetChildByName(".taytruc_panel");
	_origin_size_stick = p_stick_pivot->GetSize();
	//panel card rotate
	p_rotate_card_layer = p_mini_poker_panel->GetWidgetChildByName(".rotate_card_layer");
	for (int ii = 1; ii < p_rotate_card_layer->GetNumberChildWidget() ; ii++)
	{
		auto w = p_rotate_card_layer->GetWidgetChildAtIdx(ii);

		auto list_1 = static_cast<IwinListViewTable*>(w->GetWidgetChildByName(".list_detail"));
		list_1->InitWithParam(
				nullptr,
				nullptr,
				[list_1, this](Ref* r, EventType_SetCell e, WidgetEntity* _widget)
		{
			int current_idx_to = list_1->GetCurrentIdxTo();
			OnParseCardDetailToWidget(_widget, current_idx_to);
		}, nullptr, nullptr, nullptr);

		if (ii == p_rotate_card_layer->GetNumberChildWidget() - 1)
		{
			list_1->InitOptionalParam(
				nullptr,
				nullptr,
				nullptr,
				[this](Ref* r, EventType_SetCell e, WidgetEntity* _widget)
			{
				this->OnRotateCardFinish();
			});
		}
	}

	//center top layer
	p_center_top_layer = p_mini_poker_panel->GetWidgetChildByName(".center_top_bg");
	p_light_animate = p_center_top_layer->GetWidgetChildByName(".round_light");
	p_title_top_layer = p_center_top_layer->GetWidgetChildByName(".title_top_layout");
	p_text_total_money = p_title_top_layer->GetWidgetChildByName(".tx_total_money");
	p_listview_jars = p_center_top_layer->GetWidgetChildByName(".list_scroll_jar");
	p_listview_jars_ruby = p_center_top_layer->GetWidgetChildByName(".list_scroll_jar_ruby");

	auto item_layout_jar = p_center_top_layer->GetWidgetChildByName(".layout_");
	p_list_circle_jar = new IwinListItemCircle();
	p_list_circle_jar->init(item_layout_jar, p_listview_jars, [this](WidgetEntity* _widget, int idx) {
		int current_idx_to = idx;
		if (current_idx_to >= 0)
		{
			if (_nohu_bet_result  && current_idx_to < _nohu_bet_result->getJars().size() && _nohu_bet_result->getMoneyType() == BASE_MONEY)
			{
				auto jar = _nohu_bet_result->getJars()[current_idx_to];
				OnParseJarDetail(_widget, jar, current_idx_to);
			}
			else
			{
				if (_nohu_game_info  && current_idx_to < _nohu_game_info->getJars().size())
				{
					auto jar = _nohu_game_info->getJars()[current_idx_to];
					OnParseJarDetail(_widget, jar, current_idx_to);
				}
			}
		}
	});
	p_list_circle_jar->addEventListener([this](Ref * ref, EventType_ListCircleMove e) {
		if (e == EventType_ListCircleMove::ON_MOVE)
		{
			p_light_animate->Visible(false);
		}
		else
		{
			p_light_animate->Visible(true);
		}
	});

	p_list_circle_jar_ruby = new IwinListItemCircle();
	p_list_circle_jar_ruby->init(item_layout_jar, p_listview_jars_ruby, [this](WidgetEntity* _widget, int idx) {
		int current_idx_to = idx;
		if (current_idx_to >= 0)
		{
			if (_nohu_bet_result  && current_idx_to < _nohu_bet_result->getJars().size() && _nohu_bet_result->getMoneyType() == RUBY_MONEY)
			{
				auto jar = _nohu_bet_result->getJars()[current_idx_to];
				OnParseJarRubyDetail(_widget, jar, current_idx_to);
			}
			else
			{
				if (_nohu_game_info  && current_idx_to < _nohu_game_info->getJars().size())
				{
					auto jar = _nohu_game_info->getJars()[current_idx_to];
					OnParseJarRubyDetail(_widget, jar, current_idx_to);
				}
			}
		}
	});
	p_list_circle_jar_ruby->addEventListener([this](Ref * ref, EventType_ListCircleMove e) {
		if (e == EventType_ListCircleMove::ON_MOVE)
		{
			p_light_animate->Visible(false);
		}
		else
		{
			p_light_animate->Visible(true);
		}
	});

	//fly money
	p_fly_money_layout = mini_poker_layout->GetWidgetChildByName(".layout_fly_user");
	p_sample_money_fly = mini_poker_layout->GetWidgetChildByName(".poker_user_bet_money");

	//effect nohu
	p_effect_nohu_layer = mini_poker_layout->GetWidgetChildByName(".effect_result_layer");
	p_small_effect_nohu_layer = p_effect_nohu_layer->GetWidgetChildByName(".effect_result_layout");
	p_small_particle_nohu = p_effect_nohu_layer->GetWidgetChildByName(".small_particle");
	//p_large_effect_nohu_layer;
	//p_large_particle_nohu;
}

void MiniPokerLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	ChangeStateMoneyNohu(BASE_MONEY);
	p_mini_poker_panel->GetParentWidget()->Visible(true);
	p_mini_poker_panel->SetActionCommandWithName("ON_SHOW", CallFunc::create([this , call_back](){
		p_mini_poker_panel->ForceFinishAction();
		p_mini_poker_panel->GetWidgetChildByName(".logo")->SetActionCommandWithName("ON_SHOW");
		static_cast<AnimWidget*>(p_light_animate)->RunAnim();
		this->RunTotalMoney();
		if (call_back)
		{
			call_back();
		}
	}));

	//show current win and ruby

	RefreshUserMoney();
}

void MiniPokerLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	GlobalService::nohu_sendClose();
	p_mini_poker_panel->GetParentWidget()->Visible(false);
	p_mini_poker_panel->Visible(false);
	p_mini_poker_panel->GetWidgetChildByName(".logo")->SetActionCommandWithName("ON_HIDE");
	ClearObjectFly();
}

bool MiniPokerLayer::IsVisibleLayer()
{
	return p_mini_poker_panel->GetParentWidget()->Visible();
}

void MiniPokerLayer::OnShowMiniPokerLayout(bool value)
{
	if (value)
	{
		OnShowLayer(nullptr);

		ScrMgr->SetScreenIDDirty();
		_base_screen->onMyInfoChanged();

		//p_have_change_tab_taixiu = true;
		//GlobalService::getTaiXiuInfo(GameController::myInfo->name);
		//p_taixiu_already_have_result = false;
	}
	else
	{

		//current_stage_bet_taixiu = -1;
		OnHideLayer(nullptr);
	}
}

void MiniPokerLayer::OnShowListPlayerPokerLayout(bool value)
{
	if (value)
	{
		_page_user = 0;
		p_mini_poker_list_player_layout->GetParentWidget()->Visible(true);
		p_mini_poker_list_player_layout->SetActionCommandWithName("ON_SHOW");
		GlobalService::nohu_sendTopHistory(_page_user);

		p_poker_listplayer_listview->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(true);
		p_poker_listplayer_listview->GetParentWidget()->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");
	}
	else
	{
		p_mini_poker_list_player_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
			p_mini_poker_list_player_layout->ForceFinishAction();
			p_mini_poker_list_player_layout->GetParentWidget()->Visible(false);
		}));
	}
}

void MiniPokerLayer::OnShowListMyHistoryPokerLayout(bool value)
{
	if (value)
	{
		_page_my_history = 0;
		p_mini_poker_myhistory_layout->GetParentWidget()->Visible(true);
		p_mini_poker_myhistory_layout->SetActionCommandWithName("ON_SHOW");
		GlobalService::nohu_sendMyHistory(_page_my_history);

		p_mini_poker_myhistory_layout->GetWidgetChildByName(".loading_circle")->Visible(true);
		p_mini_poker_myhistory_layout->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");
	}
	else
	{
		p_mini_poker_myhistory_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
			p_mini_poker_myhistory_layout->ForceFinishAction();
			p_mini_poker_myhistory_layout->GetParentWidget()->Visible(false);
		}));
	}
}

void MiniPokerLayer::onReceivedNoHuGameInfo(void * data)
{
	SAFE_DELETE(_nohu_game_info);
	_nohu_game_info = (NoHuGameInfo*)data;

	if (_nohu_game_info)
	{
		auto number_jar = _nohu_game_info->getJars().size();
		if (_nohu_game_info->getMoneyType() == BASE_MONEY)
		{
			p_list_circle_jar->setNumberItem(number_jar);
			_base_screen->PushEvent([this , number_jar](BaseScreen * scr) {			
				for (int i = 0; i < number_jar; i++)
				{
					p_list_circle_jar->callUpdateItemAtIdx(i);
				}
			});
		}
		else
		{
			p_list_circle_jar_ruby->setNumberItem(number_jar);
			_base_screen->PushEvent([this, number_jar](BaseScreen * scr) {
				for (int i = 0; i < number_jar; i++)
				{
					p_list_circle_jar_ruby->callUpdateItemAtIdx(i);
				}
			});
		}

		RunTotalMoney();
	}

	if (_have_first_set_rotate_card)
	{
		for (int i = 0; i < 5; i++)
		{
			_base_screen->PushEvent([this , i](BaseScreen * scr)
			{
				auto w = p_rotate_card_layer->GetWidgetChildAtIdx(i + 1);
				int random_to_idx = GetRandom(0, 51);

				static_cast<ListViewTableWidget*>(w->GetWidgetChildByName(".list_detail"))->JumpToItemIdx(random_to_idx);
			});
		}

		_have_first_set_rotate_card = false;
	}


}
void MiniPokerLayer::onReceivedNoHuBet(void * data)
{
	SAFE_DELETE(_nohu_bet_result);
	_nohu_bet_result = (NoHuBetResult*)data;

	if (_nohu_bet_result)
	{
		if (_nohu_bet_result->getUserName() == GameController::myInfo->name)
		{

				for (int i = 0; i < _nohu_bet_result->getCards().size(); i++)
				{
					_base_screen->PushEvent([this, i](BaseScreen * scr)
					{
						RotateCardAtIdxLayer(i, _nohu_bet_result->getCards()[i]);
					}, 0.1f);
				}

			_id_win_no_hu = _nohu_bet_result->getRewardId();
			_have_no_hu = _nohu_bet_result->getIsNohu();
			_money_receive_no_hu = _nohu_bet_result->getReceivedMoney();
		}

		OnHandleFlyUserMoney();
	}
}

void MiniPokerLayer::onReceivedNoHuUserHistory(void * data)
{
	SAFE_DELETE(_nohu_my_history);
	_nohu_my_history = (NoHuUserHistoryServer*)data;

	p_poker_myhistory_listview->Visible(false);

	_page_my_history = _nohu_my_history->getPage();
	int max_page = _nohu_my_history->getTotalPage();
	if (_page_my_history < 0)
		_page_my_history = 0;
	else if (_page_my_history >= max_page)
		_page_my_history = max_page - 1;

	auto bot_bg = p_mini_poker_myhistory_layout->GetWidgetChildByName(".bot_bg");

	static_cast<TextWidget*>(bot_bg->GetWidgetChildByName(".cur_txt"))->SetText(std::to_string(_page_my_history + 1), true);
	static_cast<TextWidget*>(bot_bg->GetWidgetChildByName(".max_txt"))->SetText(std::to_string(max_page), true);

	if (max_page <= 1)
	{
		bot_bg->GetWidgetChildByName(".forward_layout")->Visible(false);
		bot_bg->GetWidgetChildByName(".backward_layout")->Visible(false);
	}
	else if (_page_my_history == 0)
	{
		bot_bg->GetWidgetChildByName(".forward_layout")->Visible(true);
		bot_bg->GetWidgetChildByName(".backward_layout")->Visible(false);
	}
	else if (_page_my_history >= max_page - 1)
	{
		bot_bg->GetWidgetChildByName(".forward_layout")->Visible(false);
		bot_bg->GetWidgetChildByName(".backward_layout")->Visible(true);
	}
	else if (_page_my_history != max_page)
	{
		bot_bg->GetWidgetChildByName(".forward_layout")->Visible(true);
		bot_bg->GetWidgetChildByName(".backward_layout")->Visible(true);
	}
	
	static_cast<IwinListViewTable*>(p_poker_myhistory_listview)->SetHaveReload([this]() {
		p_poker_myhistory_listview->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(false);
		if (_nohu_my_history->getHistory().size() > 0)
		{
			p_poker_myhistory_listview->Visible(true);
			p_poker_myhistory_listview->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(false);
			static_cast<ListViewTableWidget*>(p_poker_myhistory_listview)->SetNumberSizeOfCell(_nohu_my_history->getHistory().size(), true);
		}
		else
		{
			p_poker_myhistory_listview->Visible(false);
			p_poker_myhistory_listview->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(true);
		}
	});
}

void MiniPokerLayer::OnParseListHistoryNohu(WidgetEntity * widget, const iwinmesage::NoHuUserHistoryDetail & his, int idx)
{
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".time_txt"))->SetText(Utils::ConvertTime(his.getTime()), true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".jar_txt"))->SetText(Utils::formatNumber(his.getBetMoney()), true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".money_txt"))->SetText(Utils::formatNumber(his.getReceivedMoney()), true);

	auto layout_card = widget->GetWidgetChildByName(".card_layout");
	for (int i = 0; i < his.getCards().size(); i++)
	{
		OnConvertToIcon(layout_card->GetWidgetChildAtIdx((i * 2) + 1), layout_card->GetWidgetChildAtIdx(i * 2), his.getCards()[i]);
	}
}

void MiniPokerLayer::onReceivedNoHuListUser(void * data)
{
	SAFE_DELETE(_nohu_list_user);
	_nohu_list_user = (NoHuListUser*)data;

	p_poker_listplayer_listview->Visible(false);


	_page_user = _nohu_list_user->getPage();
	int max_page = _nohu_list_user->getTotalPage();
	if (_page_user < 0)
		_page_user = 0;
	else if (_page_user >= max_page)
		_page_user = max_page - 1;

	auto bot_bg = p_mini_poker_list_player_layout->GetWidgetChildByName(".bot_bg");

	static_cast<TextWidget*>(bot_bg->GetWidgetChildByName(".cur_txt"))->SetText(std::to_string(_page_user + 1), true);
	static_cast<TextWidget*>(bot_bg->GetWidgetChildByName(".max_txt"))->SetText(std::to_string(max_page), true);
	if (max_page <= 1)
	{
		bot_bg->GetWidgetChildByName(".forward_layout")->Visible(false);
		bot_bg->GetWidgetChildByName(".backward_layout")->Visible(false);
	}
	else if (_page_user == 0)
	{
		bot_bg->GetWidgetChildByName(".forward_layout")->Visible(true);
		bot_bg->GetWidgetChildByName(".backward_layout")->Visible(false);
	}
	else if (_page_user >= max_page - 1)
	{
		bot_bg->GetWidgetChildByName(".forward_layout")->Visible(false);
		bot_bg->GetWidgetChildByName(".backward_layout")->Visible(true);
	}
	else if (_page_user != max_page)
	{
		bot_bg->GetWidgetChildByName(".forward_layout")->Visible(true);
		bot_bg->GetWidgetChildByName(".backward_layout")->Visible(true);
	}

	static_cast<IwinListViewTable*>(p_poker_listplayer_listview)->SetHaveReload([this]() {
		p_poker_listplayer_listview->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(false);
		if (_nohu_list_user->getUsers().size() > 0)
		{
			p_poker_listplayer_listview->Visible(true);
			p_poker_listplayer_listview->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(false);
			static_cast<ListViewTableWidget*>(p_poker_listplayer_listview)->SetNumberSizeOfCell(_nohu_list_user->getUsers().size(), true);
		}
		else
		{
			p_poker_listplayer_listview->Visible(false);
			p_poker_listplayer_listview->GetParentWidget()->GetWidgetChildByName(".txt0")->Visible(true);
		}
	});
}

void MiniPokerLayer::OnParseListPlayerNohu(WidgetEntity * widget, const iwinmesage::NoHuUser & user, int idx)
{
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".time_txt"))->SetText(Utils::ConvertTime(user.getTime()), true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".acc_txt"))->SetText(user.getUsername(), true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".jar_txt"))->SetText(Utils::formatNumber(user.getBetMoney()), true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".money_txt"))->SetText(Utils::formatNumber(user.getReceivedMoney()), true);

	auto layout_card = widget->GetWidgetChildByName(".card_layout");
	for (int i = 0; i < user.getCards().size() ; i ++)
	{
		OnConvertToIcon(layout_card->GetWidgetChildAtIdx((i * 2) + 1), layout_card->GetWidgetChildAtIdx(i * 2), user.getCards()[i]);
	}

}

void MiniPokerLayer::OnConvertToIcon(WidgetEntity * icon, WidgetEntity * text, int value_card)
{
	if (!icon || !text)
		return;

	ELEMENT_CARD ecard = ELEMENT_CARD::E_CLUB;
	NAME_CARD ncard = NAME_CARD::CARD_A;
	unsigned char value = 0;
	Card::ConvertToType(value_card, DECK_MAUBINH, ncard, ecard, value);

	RKString tx = "A";
	switch (ncard)
	{
	case CARD_A:
		tx = "A";
		break;
	case CARD_2:
		tx = "2";
		break;
	case CARD_3:
		tx = "3";
		break;
	case CARD_4:
		tx = "4";
		break;
	case CARD_5:
		tx = "5";
		break;
	case CARD_6:
		tx = "6";
		break;
	case CARD_7:
		tx = "7";
		break;
	case CARD_8:
		tx = "8";
		break;
	case CARD_9:
		tx = "9";
		break;
	case CARD_10:
		tx = "10";
		break;
	case CARD_JACK:
		tx = "J";
		break;
	case CARD_QUEEN:
		tx = "Q";
		break;
	case CARD_KING:
		tx = "K";
		break;
	default:
		break;
	}

	RKString path_icon = "gamescreen-newgui-game-common/nohu/bich.png";
	switch (ecard)
	{
	case E_SPADE:
		path_icon = "gamescreen-newgui-game-common/nohu/bich.png";
		break;
	case E_CLUB:
		path_icon = "gamescreen-newgui-game-common/nohu/chuong.png";
		break;
	case E_DIAMOND:
		path_icon = "gamescreen-newgui-game-common/nohu/ro.png";
		break;
	case E_HEART:
		path_icon = "gamescreen-newgui-game-common/nohu/co.png";
		break;
	default:
		break;
	}

	static_cast<TextWidget*>(text)->SetText(tx, true);
	static_cast<PanelWidget*>(icon)->SetImage(path_icon, true);
}

void MiniPokerLayer::OnDragStick()
{
	//if (_auto_bet_nohu)
	//	return;
	if (InputMgr->GetNumberTouch() > 0)
	{
		auto current_state_touch = InputMgr->GetStateTouchOnScreen();

		if (current_state_touch == Control::STATE_PRESS)
		{
			//click outside so visible this
			cocos2d::Vec2 wPos = InputMgr->GetPosAtId(0);// NODE(p_layout_tai_xiu_mini_btn)->convertTouchToNodeSpace(InputMgr->GetTouch());
			auto rect = p_stick_sphere->GetBound();
			auto _wPos = p_stick_layout->GetResource()->convertTouchToNodeSpace(InputMgr->GetTouch());
			if (rect.containsPoint(_wPos))
			{
				NODE(p_stick_sphere)->setTag(1);
				_previous_pos_touch = wPos;
			}
		}
		else if (current_state_touch == Control::STATE_HOLD)
		{
			if (NODE(p_stick_sphere)->getTag() == 1)
			{
				cocos2d::Vec2 wPos = InputMgr->GetPosAtId(0);
				Vec2 cur_pos_t = wPos;
				Vec2 delta_pos = Vec2(cur_pos_t.x - _previous_pos_touch.x, cur_pos_t.y - _previous_pos_touch.y);

				_delta_move_stick_tx += sqrt(delta_pos.x * delta_pos.x + delta_pos.y*delta_pos.y);

				Vec2 cur_pos_card = p_stick_sphere->GetPosition();
				Vec2 new_pos = Vec2(cur_pos_card.x, cur_pos_card.y + delta_pos.y);
				p_stick_sphere->SetPosition(new_pos);

				auto rect_btntx = p_stick_sphere->GetBound();
				auto rect_win = cocos2d::Rect(Vec2(0, 0), cocos2d::Size(p_stick_layout->GetSize().x, p_stick_layout->GetSize().y));
				bool reach_min = false;
				if (rect_btntx.getMidY() < rect_win.getMaxX() / 4)
				{
					p_stick_sphere->SetPosition(Vec2(new_pos.x, rect_win.getMaxX() / 4));
					reach_min = true;
				}
				else if (rect_btntx.getMidY() > rect_win.getMaxY())
				{
					p_stick_sphere->SetPosition(Vec2(new_pos.x, rect_win.getMaxY()));
				}

				auto current_size_stick = p_stick_pivot->GetSize();
				if (!reach_min)
				{
					current_size_stick.y += delta_pos.y;
				}

				if (current_size_stick.y > _origin_size_stick.y)
				{
					current_size_stick.y = _origin_size_stick.y;
				}
				else if (current_size_stick.y < - (3.f * _origin_size_stick.y / 4.f))
				{
					current_size_stick.y = - (3.f * _origin_size_stick.y / 4.f);
				}

				p_stick_pivot->SetSize(current_size_stick);
				_previous_pos_touch = cur_pos_t;

			}
		}

	}
	else if (InputMgr->GetNumberTouchRelease() > 0)
	{
		auto current_state_touch = InputMgr->GetStateTouchOnScreen();
		if (current_state_touch == Control::STATE_RELEASE)
		{
			if (NODE(p_stick_sphere)->getTag() == 1)
			{
				if (_delta_move_stick_tx <= 0)
				{
					
				}
				else
				{				
					OnRunActionBetJar();
				}
				auto current_pos = p_stick_sphere->GetPosition();
				p_stick_sphere->GetResource()->runAction(MoveTo::create(0.5f, Vec2(current_pos.x, p_stick_layout->GetSize().y)));
				//
				auto contain_size = p_stick_pivot->GetResource()->getContentSize();
				auto scale_to = Vec2(_origin_size_stick.x / contain_size.width, _origin_size_stick.y / contain_size.height);
				p_stick_pivot->GetResource()->runAction(ScaleTo::create(0.5f, scale_to.x, scale_to.y));
					//
				_delta_move_stick_tx = 0;
			}
			NODE(p_stick_sphere)->setTag(0);
		}
	}

}

void MiniPokerLayer::OnRunActionBetJar()
{
	int current_idx_jar = -1;
	if (GameController::typeMoneyNohu == BASE_MONEY)
	{
		//get center
		current_idx_jar = p_list_circle_jar->GetIndexOfVisibleView(1);
	}
	else
	{
		//get center
		current_idx_jar = p_list_circle_jar_ruby->GetIndexOfVisibleView(1);
	}
	bool res = OnBetJarAtIdx(current_idx_jar);

	if (res)
	{
		for (int i = 0; i < 5; i++)
		{
			_base_screen->PushEvent([this, i](BaseScreen * scr)
			{
				RotateCardAtIdxLayer(i, -1);
			});
		}
	}
}

bool MiniPokerLayer::OnBetJarAtIdx(int idx)
{
	if (_nohu_game_info)
	{
		if (idx >= 0)
		{
			auto bet_money = _nohu_game_info->getJars()[idx].getBetMoney();
			s64 current_money = -1;
			if (GameController::typeMoneyNohu == BASE_MONEY)
			{
				current_money = GameController::myInfo->money;
			}
			else
			{
				current_money = GameController::myInfo->ruby;
			}
			if (current_money >= bet_money)
			{
				GlobalService::nohu_sendBet(_nohu_game_info->getJars()[idx].getID(), bet_money);
				return true;
			}
			else
			{
				static_cast<CommonScreen*>(_base_screen)->OnShowToast((GameController::typeMoneyNohu == BASE_MONEY) ? "you_have_not_enough_win" : "you_have_not_enough_ruby");
			}
		}
	}
	return false;
}

void MiniPokerLayer::OnRotateCardFinish()
{
	if (_money_bonus > 0)
	{
		OnRunEffectNoHu();

		GetCommonScr->OnShowBonusMoney(_money_bonus, true, _nohu_game_info ? _nohu_game_info->getMoneyType() : BASE_MONEY);
		ScrMgr->MyInfoChanged();
	}
	ResetParamNohuEffect();
	_money_bonus = 0;

	if (_auto_bet_nohu)
	{
		auto current_pos = p_stick_sphere->GetPosition();
		p_stick_sphere->GetResource()->runAction(
			Sequence::create(
				MoveTo::create(0.5f, Vec2(current_pos.x, 0)), //move down
				MoveTo::create(0.5f, Vec2(current_pos.x, p_stick_layout->GetSize().y)), //move up
				CallFunc::create([this]() {
					OnRunActionBetJar();
				}),nullptr	
			));
		//
		auto contain_size = p_stick_pivot->GetResource()->getContentSize();
		auto scale_to = Vec2(_origin_size_stick.x / contain_size.width, _origin_size_stick.y / contain_size.height);
		p_stick_pivot->GetResource()->runAction(
			Sequence::create(
				ScaleTo::create(0.5f, scale_to.x, -scale_to.y),
				ScaleTo::create(0.5f, scale_to.x, scale_to.y),
				nullptr
			));
	}
}

void MiniPokerLayer::SetMoneyBonusNoHu(const s64 & money)
{
	_money_bonus = money;
}

void MiniPokerLayer::RotateCardAtIdxLayer(int idx, int id_to)
{
	if (idx < 0 || idx >= p_rotate_card_layer->GetNumberChildWidget())
		return;

	auto w = p_rotate_card_layer->GetWidgetChildAtIdx(idx + 1);
	auto list_detail = static_cast<ListViewTableWidget*>(w->GetWidgetChildByName(".list_detail"));
	if (id_to >= 0)
	{
		list_detail->ScrollToItemIdxLoop(id_to, 2.5f , 1, 3.0f);
	}
	else
	{
		int current_idx_to = list_detail->GetCurrentIdxTo();

		float time_to = (current_idx_to / 51.f) * 3.f;
		list_detail->ScrollToItemIdxLoop(0, time_to, -1, 3.0f);
	}

}

void MiniPokerLayer::OnParseCardDetailToWidget(WidgetEntity * card, int idx)
{
	ELEMENT_CARD ecard = ELEMENT_CARD::E_CLUB;
	NAME_CARD ncard = NAME_CARD::CARD_A;
	unsigned char value = 0;
	Card::ConvertToType(idx, DECK_MAUBINH, ncard, ecard, value);

	RKString tx = "A";
	switch (ncard)
	{
	case CARD_A:
		tx = "A";
		break;
	case CARD_2:
		tx = "2";
		break;
	case CARD_3:
		tx = "3";
		break;
	case CARD_4:
		tx = "4";
		break;
	case CARD_5:
		tx = "5";
		break;
	case CARD_6:
		tx = "6";
		break;
	case CARD_7:
		tx = "7";
		break;
	case CARD_8:
		tx = "8";
		break;
	case CARD_9:
		tx = "9";
		break;
	case CARD_10:
		tx = "10";
		break;
	case CARD_JACK:
		tx = "J";
		break;
	case CARD_QUEEN:
		tx = "Q";
		break;
	case CARD_KING:
		tx = "K";
		break;
	default:
		break;
	}

	RKString path_icon_small = "gamescreen-Cards/mb.png";
	Color3B color = Color3B::BLACK;
	switch (ecard)
	{
	case E_SPADE:
		path_icon_small = "gamescreen-Cards/mb.png";
		color = Color3B::BLACK;
		break;
	case E_CLUB:;
		path_icon_small = "gamescreen-Cards/mch.png";
		color = Color3B::BLACK;
		break;
	case E_DIAMOND:
		path_icon_small = "gamescreen-Cards/mr.png";
		color = Color3B::RED;
		break;
	case E_HEART:
		path_icon_small = "gamescreen-Cards/mc.png";
		color = Color3B::RED;
		break;
	default:
		break;
	}

	OnParseIcon(card->GetWidgetChildAtIdx(1), (int)ncard, (int)ecard);

	static_cast<PanelWidget*>(card->GetWidgetChildAtIdx(2))->SetImage(path_icon_small, true);

	static_cast<TextWidget*>(card->GetWidgetChildAtIdx(3))->SetText(tx, true);
	card->GetWidgetChildAtIdx(3)->SetColor(color);
}

void MiniPokerLayer::OnParseIcon(WidgetEntity * big_ico, int name, int element)
{
	NAME_CARD ncard = (NAME_CARD)name;
	ELEMENT_CARD ecard = (ELEMENT_CARD)element;
	RKString path_icon_big = "gamescreen-Cards/kb.png";
	switch (ncard)
	{
	case CARD_A:
	case CARD_2:
	case CARD_3:
	case CARD_4:
	case CARD_5:
	case CARD_6:
	case CARD_7:
	case CARD_8:
	case CARD_9:
	case CARD_10:
		switch (ecard)
		{
		case E_SPADE:
			if (ncard == CARD_A)
			{
				path_icon_big = "gamescreen-Cards/xi_bich.png";
			}
			else
			{
				path_icon_big = "gamescreen-Cards/b.png";
			}
			break;
		case E_CLUB:
			path_icon_big = "gamescreen-Cards/ch.png";
			break;
		case E_DIAMOND:
			path_icon_big = "gamescreen-Cards/r.png";
			break;
		case E_HEART:
			path_icon_big = "gamescreen-Cards/c.png";
			break;
		default:
			break;
		}
		break;
	case CARD_JACK:
		switch (ecard)
		{
		case E_SPADE:
		case E_CLUB:
			path_icon_big = "gamescreen-Cards/jb.png";
			break;
		case E_DIAMOND:
		case E_HEART:
			path_icon_big = "gamescreen-Cards/jc.png";
			break;
		default:
			break;
		}
		break;
	case CARD_QUEEN:
		switch (ecard)
		{
		case E_SPADE:
		case E_CLUB:
			path_icon_big = "gamescreen-Cards/qb.png";
			break;
		case E_DIAMOND:
		case E_HEART:
			path_icon_big = "gamescreen-Cards/qc.png";
			break;
		default:
			break;
		}
		break;
	case CARD_KING:
		switch (ecard)
		{
		case E_SPADE:
		case E_CLUB:
			path_icon_big = "gamescreen-Cards/kb.png";
			break;
		case E_DIAMOND:
		case E_HEART:
			path_icon_big = "gamescreen-Cards/kc.png";
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	static_cast<PanelWidget*>(big_ico)->SetImage(path_icon_big, true);
}

void MiniPokerLayer::OnParseJarRubyDetail(WidgetEntity * widget, const iwinmesage::NoHuJar & jar, int idx)
{
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".tx_num"))->SetText(Utils::formatNumber_dot(jar.getBetMoney()), true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".tx"))->SetText(Utils::formatNumber_dot(jar.getTotalMoney()), true);

	if (p_list_img_jar_ruby.size() > 0 && p_list_img_jar_ruby.find(idx) != p_list_img_jar_ruby.end())
	{
		if (widget->GetWidgetChildByName(".icon")->GetResource()->getChildren().size() == 0)
		{
			p_list_img_jar_ruby.at(idx)->setScale(1.2f);
			_base_screen->ParseSpriteToPanel(widget->GetWidgetChildByName(".icon"), p_list_img_jar_ruby.at(idx), 100, true, Vec2(0.5, 0));
		}
	}
	else
	{
		GameMgr->HandleURLImgDownload(Utils::getImgUrlCorrect(jar.getImgUrl()), jar.getImgUrl(),
			[this, idx, widget](void * data, void * str, int tag)
		{
			Image * img = (Image *)data;
			if (img && img->getWidth() <= 0)
			{
				CC_SAFE_DELETE(img);
				return;
			}
			Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str, cocos2d::Texture2D::PixelFormat::RGBA4444);
			p_list_img_jar_ruby.insert(idx, sprite_img);
			sprite_img->setScale(1.2f);
			_base_screen->ParseSpriteToPanel(widget->GetWidgetChildByName(".icon"), sprite_img, 100, true, Vec2(0.5, 0));

		});
	}
}

void MiniPokerLayer::OnParseJarDetail(WidgetEntity * widget, const iwinmesage::NoHuJar & jar, int idx)
{
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".tx_num"))->SetText(Utils::formatNumber_dot(jar.getBetMoney()), true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".tx"))->SetText(Utils::formatNumber_dot(jar.getTotalMoney()), true);

	if(p_list_img_jar.size() > 0 && p_list_img_jar.find(idx) != p_list_img_jar.end() )
	{
		if (widget->GetWidgetChildByName(".icon")->GetResource()->getChildren().size() == 0)
		{
			p_list_img_jar_ruby.at(idx)->setScale(1.2f);
			_base_screen->ParseSpriteToPanel(widget->GetWidgetChildByName(".icon"), p_list_img_jar.at(idx), 100, true, Vec2(0.5,0));
		}
	}
	else
	{

		GameMgr->HandleURLImgDownload(Utils::getImgUrlCorrect(jar.getImgUrl()), jar.getImgUrl(),
			[this, idx, widget](void * data, void * str, int tag)
		{
			Image * img = (Image *)data;
			if (img && img->getWidth() <= 0)
			{
				CC_SAFE_DELETE(img);
				return;
			}
			Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str,cocos2d::Texture2D::PixelFormat::RGBA4444);
			p_list_img_jar.insert(idx, sprite_img);
			sprite_img->setScale(1.2f);
			_base_screen->ParseSpriteToPanel(widget->GetWidgetChildByName(".icon"), sprite_img, 100, true, Vec2(0.5, 0));

		});
	}

}

void MiniPokerLayer::RefreshUserMoney()
{
	User * user = GetUser;
	if (user == nullptr)
	{
		//PASSERT2(false, "user is null");
		return;
	}

	s64 money = GameController::myInfo->money;
	s64 ruby = GameController::myInfo->ruby;

	auto win = p_poker_top_layer->GetWidgetChildByName(".poker_mymoney_txt");
	static_cast<TextWidget*>(win)->SetText(Utils::formatNumber_dot(money), true);
	auto wruby = p_poker_top_layer->GetWidgetChildByName(".poker_myruby_txt");
	static_cast<TextWidget*>(wruby)->SetText(Utils::formatNumber_dot(ruby), true);
}

void MiniPokerLayer::ParseUserUI()
{
	RefreshUserMoney();
}

void MiniPokerLayer::ChangeStateMoneyNohu(int state)
{
	if (state == 1)
	{
		GameController::typeMoneyNohu = BASE_MONEY;
		p_poker_top_layer->GetWidgetChildByName(".left_panel_act")->Visible(true);
		p_poker_top_layer->GetWidgetChildByName(".right_panel_act")->Visible(false);
		p_mini_poker_panel->GetWidgetChildByName(".panel_bg")->Visible(true);
		p_mini_poker_panel->GetWidgetChildByName(".panel_bg_ruby")->Visible(false);

		p_title_top_layer->GetWidgetChildByName(".icon_win")->Visible(true);
		p_title_top_layer->GetWidgetChildByName(".icon_ruby")->Visible(false);

		p_listview_jars->Visible(true);
		p_listview_jars_ruby->Visible(false);
	}
	else if (state == 2)
	{
		GameController::typeMoneyNohu = RUBY_MONEY;

		p_poker_top_layer->GetWidgetChildByName(".left_panel_act")->Visible(false);
		p_poker_top_layer->GetWidgetChildByName(".right_panel_act")->Visible(true);

		p_mini_poker_panel->GetWidgetChildByName(".panel_bg")->Visible(false);
		p_mini_poker_panel->GetWidgetChildByName(".panel_bg_ruby")->Visible(true);

		p_title_top_layer->GetWidgetChildByName(".icon_win")->Visible(false);
		p_title_top_layer->GetWidgetChildByName(".icon_ruby")->Visible(true);

		p_listview_jars->Visible(false);
		p_listview_jars_ruby->Visible(true);
	}
	ClearObjectFly();
	GlobalService::nohu_getInfoGame();
}

void MiniPokerLayer::RunTotalMoney()
{
	if (_nohu_game_info)
	{
		static_cast<TextWidget*>(p_text_total_money)->TextRunEffect(_nohu_game_info->getTotalMoney(), 0, 0.5f, Utils::formatNumber_dot, nullptr);
	}
}

void MiniPokerLayer::OnHandleFlyUserMoney()
{
	if (_nohu_bet_result->getUserName() != "" && IsVisibleLayer() && _nohu_bet_result->getCode() == 1)
	{
		auto tx_user = p_sample_money_fly->Clone();

		p_fly_money_layout->InsertChildWidget(tx_user);

		float ratio = GetGameSize().width / _base_screen->GetDesignSize().x;
		float delta_move = (50.f * ratio);
		float div = 100 * ratio;
		tx_user->SetVisible(true);
		tx_user->SetPosition(Vec2
		(
			GetRandom(div, GetGameSize().width - div) ,
			GetRandom(0, div)
		));


		auto pos_coin = p_title_top_layer->GetWidgetChildByName(".icon_win")->GetPosition_Middle();
		pos_coin = p_title_top_layer->GetResource()->convertToWorldSpace(pos_coin);

		auto wicon = tx_user->GetWidgetChildByName(".icon");

		wicon->GetWidgetChildAtIdx(0)->Visible(GameController::typeMoneyNohu == BASE_MONEY);
		wicon->GetWidgetChildAtIdx(1)->Visible(GameController::typeMoneyNohu != BASE_MONEY);
		wicon->SetActionCommandWithName("SHOW");

		auto lay_out_txt = tx_user->GetWidgetChildByName(".layout_txt");
		lay_out_txt->SetActionCommandWithName("SHOW");
		static_cast<TextWidget*>(lay_out_txt->GetWidgetChildAtIdx(0))->SetText(_nohu_bet_result->getUserName(), true);
		static_cast<TextWidget*>(lay_out_txt->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(_nohu_bet_result->getBetMoney()), true);


		int idx_of_jar_update = _nohu_bet_result->GetIndexOfJarId(_nohu_bet_result->getJarId());

		tx_user->GetResource()->runAction(Sequence::create(
			MoveTo::create(1.f, Vec2(tx_user->GetPosition().x, tx_user->GetPosition().y + delta_move)),
			MoveTo::create(0.5f, pos_coin) ,
			DelayTime::create(0.2f),
			CallFunc::create([tx_user, this, idx_of_jar_update]()
		{
			static_cast<TextWidget*>(p_title_top_layer->GetWidgetChildByName(".tx_total_money"))->SetText(Utils::formatNumber_dot(_nohu_bet_result->getTotalMoney()), true);
			p_title_top_layer->GetWidgetChildByName(".effect_icon")->SetActionCommandWithName("RUN_EFFECT");

			tx_user->Visible(false);

			if (_nohu_bet_result->getMoneyType() == BASE_MONEY)
			{
				
				p_list_circle_jar->callUpdateItemAtIdx(idx_of_jar_update);
			}
			else
			{
				
				p_list_circle_jar_ruby->callUpdateItemAtIdx(idx_of_jar_update);
			}

		}), nullptr));
	}
}

void MiniPokerLayer::ClearObjectFly()
{
	p_fly_money_layout->ClearChild();
}

void MiniPokerLayer::OnRunEffectNoHu(bool isLargeNohu)
{
	static_cast<ParticleWidget*>(p_small_particle_nohu)->RunParticle();
	if (_id_win_no_hu > 0)
	{
		p_effect_nohu_layer->Visible(true);
		auto string_reward = GetStringWinID(_id_win_no_hu);

		auto w = p_small_effect_nohu_layer->GetWidgetChildByName(".layout_text");
		static_cast<TextWidget*>(w->GetWidgetChildByName(".txt_name"))->SetText(string_reward, true);
		static_cast<TextWidget*>(w->GetWidgetChildByName(".txt_money"))->SetText("+ " + Utils::formatNumber_dot(_money_receive_no_hu), true);
		w->SetActionCommandWithName("ON_SHOW", CallFunc::create([this , w](){
			w->ForceFinishAction();
			w->Visible(false);
			p_effect_nohu_layer->Visible(false);
		}));
	}
	ResetParamNohuEffect();
}

const RKString MiniPokerLayer::GetStringWinID(int winID)
{
	RKString id_lang = "";
	switch (winID)
	{
	case 1:
		id_lang = "nohu_doi";
		break;
	case 2:
		id_lang = "nohu_thu";
		break;
	case 3:
		id_lang = "nohu_xam";
		break;
	case 4:
		id_lang = "nohu_sanh";
		break;
	case 5:
		id_lang = "nohu_thung";
		break;
	case 6:
		id_lang = "nohu_culu";
	case 7:
		id_lang = "nohu_tuquy";
		break;
	case 8:
		id_lang = "nohu_thungphasanh";
		break;
	case 9:
		id_lang = "nohu_thungphasanh_ha";
		break;
	case 10:
		id_lang = "nohu_thungphasanh_thuong";
		break;
	default:
		break;
	}

	if (id_lang == "")
	{
		return id_lang;
	}
	else
	{
		return LANG_STR(id_lang);
	}
}

void MiniPokerLayer::ResetParamNohuEffect()
{
	_id_win_no_hu = 0;
	_have_no_hu = false;
	_money_receive_no_hu = 0;
}
#include "MainScreen.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wPageView.h"
#include "UI/AdvanceUI/wRadioBtn.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wCheckBox.h"
#include "UI/AdvanceUI/wSlider.h"
#include "UI/AdvanceUI/wTimeProcessWidget.h"

#include "Network/Global/Globalservice.h"
#include "Common/Common.h"
#include "Network/JsonObject/Game/GameList.h"

#include "Network/JsonObject/Avatar/AvatarCatList.h"
#include "Network/JsonObject/Avatar/AvatarList.h"
#include "Network/JsonObject/Avatar/BuyingAvatar.h"

#include "Network/Core/ServerMgr.h"
#include "Common/FlyMoney.h"

#include "Features/GCSignInMgr.h"
#include "Features/FacebookMgr.h"
#include "Features/GGSignInMgr.h"

#include "LobbyScreen.h"
#include "NewLobbyScreen.h"
#include "LoginScreen.h"
#include "Services/GuideService.h"
#include "Common/GameController.h"
#define DEFAULT_SHOW 1
#define TIME_REQUEST_WAIT 1.f
const char listGameSupport[11] = { 3, 7 , 20, 11, 2 ,12, 13 , 16 , 5 , 22 };
//const char listGameSupport[11] = { 3, 7 , 20, 11, 2 ,12, 13 , 16 , 5 , 22,GameType::GAMEID_XAM };
bool IsGameSupport(char gameID)
{
	for (int i = 0; i < 11; i++)
	{
		if (listGameSupport[i] == gameID)
		{
			return true;
		}
	}
	return false;
}

MainScreen::MainScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
	p_panel_setting = nullptr;
	p_panel_list_game = nullptr;
	p_gamelist = nullptr;
	p_list_lang_choose = nullptr;

	p_panel_notice_12 = nullptr;
	p_layout_anoun_server = nullptr;
	p_stage_get_data = 1;

	p_annouce_is_running = false;
	p_force_stop_annouce = false;

	p_panel_info_game = nullptr;
	p_list_game_id.clear();

	p_current_time_request_list_wait = TIME_REQUEST_WAIT;
	p_already_parse_list_wait = true;

	p_isCallRequestFriend = false;
	p_panel_cd_change_server = nullptr;
	p_on_count_down = false;
	p_timer_count_down = 0.f;

	p_speed_move = SPEED_MOVE_ANNOUCE; //in 1 sec move 100 pixel
}


MainScreen::~MainScreen()
{
	p_panel_setting = nullptr;
	p_panel_list_game = nullptr;

	p_gamelist = nullptr;
	p_list_lang_choose = nullptr;
	p_panel_notice_12 = nullptr;
	p_layout_anoun_server = nullptr;
	p_stage_get_data = 1;

	p_isCallRequestFriend = false;
}

int MainScreen::Init()
{
	RKString _menu = "main_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);
	p_panel_setting = GetWidgetChildByName("main_screen.panel_setting.panel_setting");
	p_panel_choose_server = GetWidgetChildByName("main_screen.panel_choose_server.panel_choose_server");
	p_list_server = p_panel_choose_server->GetWidgetChildByName(".list_server");
	p_list_lang_choose = p_panel_setting->GetWidgetChildByName(".scroll_options.language.list_choose_lang");
	
	//for (int i = 0; i <= LangMgr->GetNumberLang(); i++)
	{
		//auto widget = static_cast<ListViewWidget*>(p_list_lang_choose)->CreateWidgetRefer(i);
		//static_cast<TextWidget*>(widget->GetWidgetChildByName(".title"))->SetText(LangMgr->GetLangDec((Utility::LANGUAGE_TYPE)(i + 1)));

		auto _list_choose_lang = static_cast<ListViewTableWidget*>(p_list_lang_choose);
		_list_choose_lang->addEventListener(
			[this, _list_choose_lang](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = _list_choose_lang->GetCurrentIdxTo();
				static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title"))->SetText(LangMgr->GetLangDec((Utility::LANGUAGE_TYPE)(current_idx_to + 1)), true);
			}
			else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
			{
				int current_idx_to = _list_choose_lang->GetCurrentIdxSelected();
				auto list_widget = _list_choose_lang->GetListWidgetSample();
				for (size_t i = 0; i < list_widget.size(); i++)
				{
					list_widget[i]->GetWidgetChildByName(".hight_light")->Visible(false);
				}
				_widget->GetWidgetChildByName(".hight_light")->Visible(true);

				auto lang_select = (LANGUAGE_TYPE)(current_idx_to + 1);
				RKString current_text = LangMgr->GetLangDec(lang_select);
				bool result = ScrMgr->ChangeLanguage(lang_select);
				if (result)
				{
					static_cast<TextWidget*>(
						p_list_lang_choose->GetParentWidget()->GetWidgetChildByName(".btn_input_lang.text_input_1"))->SetText(
							current_text , true
						);
					ScrMgr->SaveDataInt(STR_SAVE_LANGUAGE, lang_select);
				}
				OnShowListChooseLang(false);

			}
		});
		_list_choose_lang->InitTable();
		_list_choose_lang->SetNumberSizeOfCell(LangMgr->GetNumberLang());
	}

	UpdateServerList();

	CallInitComponent(true);

	return 1;
}

void MainScreen::UpdateServerList()
{
	p_list_server->ClearChild();
	auto list_server = GetServerMgr()->getServerList();
	for (size_t i = 0; i < list_server.size(); i++)
	{
		auto layout_ = static_cast<ListViewWidget*>(p_list_server)->CreateWidgetRefer(i);
		static_cast<TextWidget*>(layout_->GetWidgetChildByName(".server_choose_btn.server_name"))->SetText(list_server.at(i)._name);
		if (i == 0)
		{
			layout_->GetWidgetChildByName(".stick_icon")->Visible(true);
			layout_->GetWidgetChildByName(".highlights")->Visible(true);
			static_cast<TextWidget*>(GetWidgetChildByName("main_screen.btn_server.name_server"))->SetText(GetServerMgr()->getCurServer()._name);
		}
	}
}

int	MainScreen::InitComponent()
{
	p_panel_list_game = GetWidgetChildByName("main_screen.list_game_panel");
	p_bottom_menu = GetWidgetChildByName("main_screen.bottom_menu");
	p_panel_list_btn_choose_game = GetWidgetChildByName("main_screen.panel_list_btn_choose_game");
	panel_avatar_user = p_bottom_menu->GetWidgetChildByName(".btn_avatar.avatar_default");
	p_panel_list_btn = p_bottom_menu->GetWidgetChildByName(".panel_list_btn");

	auto cur_lang = LangMgr->GetCurLanguage();
	RKString dec_lang = LangMgr->GetLangDec(cur_lang);

	static_cast<TextWidget*>(
		p_list_lang_choose->GetParentWidget()->GetWidgetChildByName(".btn_input_lang.text_input_1"))->SetText(
			dec_lang
		);

	p_panel_notice_12 = GetWidgetChildByName("main_screen.panel_notice_server.panel_notice_12");
	p_layout_anoun_server = p_panel_notice_12->GetParentWidget()->GetWidgetChildByName(".layout_anoun_server");

	int idx_select = 0;
	if (ScrMgr->GetDataSaveInt(STR_SAVE_INVITE_STATE, idx_select))
	{
		static_cast<CheckBoxWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
			".notice_invite_play.check_box_invite_play"))->OnSetSelected(idx_select == 1 ? true : false);
		if (idx_select == 1)
		{
			GlobalService::sendStaticInviviteToPlayGame(true);
		}
		else
		{
			GlobalService::sendStaticInviviteToPlayGame(false);
		}
	}
	if (ScrMgr->GetDataSaveInt(STR_SAVE_BRIVATE_STATE, idx_select))
	{
		static_cast<CheckBoxWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
			".brivate_mode.check_box_brivate"))->OnSetSelected(idx_select == 1 ? true : false);
		if (idx_select == 1)
		{
			Utils::enableVibrate(true);
		}
		else
		{
			Utils::enableVibrate(false);
		}
	}
	/*if (ScrMgr->GetDataSaveInt(STR_SAVE_FB_STATE, idx_select))
	{
		static_cast<CheckBoxWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
			".facebook_login.check_box_fb_login"))->OnSetSelected(idx_select == 1 ? true : false);
		if (idx_select == 1)
		{
		}
		else
		{
		}
	}*/

	p_panel_info_game = GetWidgetChildByName("main_screen.panel_info_game");
	p_list_game_win.clear();
	p_list_game_ruby.clear();


	p_panel_cd_change_server = GetWidgetChildByName("main_screen.countdown_change_server.countdown_change_server");


	auto list_btn_choose_type_money = GetWidgetChildByName("main_screen.list_btn_choose_type_money");
	p_list_btn_game_win = list_btn_choose_type_money->GetWidgetChildByName(".list_game_btn_win");
	p_list_btn_game_ruby = list_btn_choose_type_money->GetWidgetChildByName(".list_game_btn_ruby");

	OnShowListBtnGame(false);

	return 1;
}

int MainScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (p_layout_anoun_server->Visible())
	{
		auto w_text = p_layout_anoun_server->GetWidgetChildAtIdx(1);

		auto current_pos = w_text->GetPosition();
		current_pos.x -= (p_speed_move * dt);
		w_text->SetPosition(current_pos);

		if (w_text->GetPosition_BottomRight().x < 0)
		{
			p_annouce_is_running = false;
			w_text->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, w_text]() {
				w_text->ForceFinishAction();
				this->p_layout_anoun_server->SetActionCommandWithName("ON_HIDE");
			}));
			GetCommonScr->ForceStopAnnouce(true);
		}
	}

	if (p_already_parse_list_wait)
	{
		p_current_time_request_list_wait -= dt;
		if (p_current_time_request_list_wait <= 0)
		{
			GlobalService::requestListWait();
			p_current_time_request_list_wait = TIME_REQUEST_WAIT;
			p_already_parse_list_wait = false;
		}
	}

	if (p_on_count_down)
	{
		p_timer_count_down -= dt;
		if (p_timer_count_down <= 0)
		{
			p_timer_count_down = 0;
			p_on_count_down = false;
		}
		static_cast<TextWidget*>(p_panel_cd_change_server->GetWidgetChildByName(".timer_txt"))->SetText(std::to_string((int)p_timer_count_down));
	}

	return 1;
}

void MainScreen::OnTouchMenuBegin(const cocos2d::Point & p)
{
	if (p_panel_choose_server->GetParentWidget()->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_choose_server,
			[&](void){
			OnVisiblePanelServer(false, nullptr);
		});
	}
	else if (p_list_lang_choose->Visible())
	{
		CheckTouchOutSideWidget(p, p_list_lang_choose,
			[&](void) {
			OnShowListChooseLang(false);
		});

	}
	else if (p_panel_setting->GetParentWidget()->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_setting,
			[&](void) {
			OnVisiblePanelSetting(false, []() {
				ScrMgr->SaveTheGame();
			});
		});
	}
}

void MainScreen::OnVisiblePanelSetting(bool value, const std::function<void(void)> & call_back)
{
	if (value)
	{
		p_panel_setting->GetParentWidget()->Visible(true);
		p_panel_setting->SetActionCommandWithName("ON_SHOW", CallFunc::create([this, call_back]() {
			this->p_panel_setting->ForceFinishAction();
			call_back();
		}));
	}
	else
	{
		p_panel_setting->SetActionCommandWithName("ON_HIDE", CallFunc::create(
			[this , call_back]() {
			this->p_panel_setting->ForceFinishAction();
			this->p_panel_setting->GetParentWidget()->Visible(false);
			call_back();
		}));
	}
}

void MainScreen::OnVisiblePanelServer(bool value, const std::function<void(void)> & call_back)
{
	if (value)
	{
		this->p_panel_choose_server->GetParentWidget()->Visible(true);
		p_panel_choose_server->SetActionCommandWithName("ON_SHOW", CallFunc::create(
			[this, call_back]()
		{
			this->p_panel_choose_server->ForceFinishAction();
			
			if (call_back)
			{
				call_back();
			}
		}));
	}
	else
	{
		p_panel_choose_server->SetActionCommandWithName("ON_HIDE", CallFunc::create(
			[this, call_back]()
		{
			this->p_panel_choose_server->ForceFinishAction();
			this->p_panel_choose_server->GetParentWidget()->Visible(false);
			if (call_back)
			{
				call_back();
			}
		}));
	}
}

void MainScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_setting")
		{
			if (!p_panel_setting->GetParentWidget()->Visible())
			{
				OnVisiblePanelSetting(true , [this]() {
					ScrMgr->SetScreenIDDirty();

					float current_sound_volume = GetSound->GetVolume();
					static_cast<SliderWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
						".sound_volume.slider_sound"))->SetPercent(current_sound_volume * 100.f);

					float current_music_volume = GetSound->GetVolume(false);
					static_cast<SliderWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
						".music_volume.slider_music"))->SetPercent(current_music_volume * 100.f);
				});
			}
		}
		else if (name == "btn_leaderboard")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			ScrMgr->SwitchToMenu(LEADERBOARD_SCREEN, MAIN_SCREEN);
		}
		else if (name == "btn_purchase_win" || name == "btn_purchase_ruby")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			ScrMgr->SwitchToMenu(IAP_SCREEN, MAIN_SCREEN);
			//ScrMgr->ShowShop(true);
		}
		else if (name == "btn_award")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			ScrMgr->SwitchToMenu(CHANGE_AWARD_SCREEN, MAIN_SCREEN);
			//ScrMgr->SwitchToMenu(CUSTOM_SCREEN, MAIN_SCREEN);
			//ScrMgr->ShowShop(true);
		}
		else if (name == "btn_event")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			ScrMgr->SwitchToMenu(EVENT_SCREEN, MAIN_SCREEN);
		}
		else if (name == "btn_friend")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			ScrMgr->SwitchToMenu(FRIEND_SCREEN, MAIN_SCREEN);
		}
		else if (name == "btn_iwin_game")
		{
			if (GameController::currentMoneyType != BASE_MONEY != 0 && p_gamelist)
			{			
				OnParseCustomUIDec(p_list_game_win.at(GAME_POPULAR).list_game, "test", p_list_sample_dec);
				OnShowHLBtnGameAtIdx(BASE_MONEY);
				OnShowHLBtnListGame(0);
			}

		}
		else if (name == "btn_ruby_game")
		{
			if (GameController::currentMoneyType != RUBY_MONEY  && p_gamelist)
			{
				OnParseCustomUIDec(p_list_game_ruby.at(GAME_POPULAR).list_game, "test", p_list_sample_dec);
				OnShowHLBtnGameAtIdx(RUBY_MONEY);
				OnShowHLBtnListGame(0);
			}
		}
		else if (name == "btn_avatar")
		{
			if (!GetUser || GetUser->id == -1)
				return;
			ScrMgr->SwitchToMenu(MENU_LAYER::PROFILE_SCREEN, MENU_LAYER::MAIN_SCREEN);
			//GlobalService::requestProfileOf(GameController::myInfo->IDDB);
		}
		else if (name == "log_out_btn")
		{
			OnVisiblePanelSetting(false,
				[this]() {
				ScrMgr->SaveTheGame();
				logout();
			
			});

		}
		else if (name == "server_choose_btn")
		{
			int idx_list_select = static_cast<ListViewWidget*>(p_list_server)->GetCurrentIdxSelected();
			if (idx_list_select >= 0)
			{

				auto current_layout = p_list_server->GetWidgetChildAtIdx(idx_list_select);
				current_layout->GetWidgetChildByName(".stick_icon")->Visible(true);
				current_layout->GetWidgetChildByName(".highlights")->Visible(true);


				int number_child_in_list = static_cast<ListView*>(p_list_server->GetResource())->getChildrenCount();
				for (int i = 0; i < number_child_in_list; i++)
				{
					if (i == idx_list_select)
					{
						continue;
					}
					auto current_layout_t = p_list_server->GetWidgetChildAtIdx(i);
					current_layout_t->GetWidgetChildByName(".stick_icon")->Visible(false);
					current_layout_t->GetWidgetChildByName(".highlights")->Visible(false);
				}
				auto current_server = GetServerMgr()->getCurServer();
				RKString previous_name_server = current_server._name;

				GetServerMgr()->selectServer(idx_list_select);
				RKString name_server = GetServerMgr()->getCurServer()._name;

				if (previous_name_server != name_server)
				{

					OnVisiblePanelServer(false, [this, idx_list_select]()
					{
						static_cast<LoginScreen*>(ScrMgr->GetMenuUI(LOGIN_SCREEN))->OnSwitchServer(idx_list_select);
						OnShowChangeServer();
					});
					
				}

				static_cast<TextWidget*>(GetWidgetChildByName("main_screen.btn_server.name_server"))->SetText(name_server);
			}

			static_cast<ListViewWidget*>(p_list_server)->SetCurrentIdxSelected(-1);
		}
		else if (name == "btn_server")
		{
			OnVisiblePanelServer(!p_panel_choose_server->GetParentWidget()->Visible(), nullptr);
		}
		else if (name == "btn_input_lang" )
		{
			if (p_list_lang_choose->Visible())
			{
				OnShowListChooseLang(false);
			}
			else
			{
				OnShowListChooseLang(true);
			}
		}
		else if (name == "btn_12")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildByName(".panel_notice_12");
			if(!widget->Visible())
			{
				widget->SetActionCommandWithName("ON_SHOW");
			}
		}
		else if (name == "close_annouce_panel")
		{
			if (p_annouce_is_running && p_layout_anoun_server->Visible())
			{
				p_annouce_is_running = false;
				auto w_text = p_layout_anoun_server->GetWidgetChildAtIdx(1);
				w_text->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, w_text]() {
					w_text->ForceFinishAction();
					this->p_layout_anoun_server->SetActionCommandWithName("ON_HIDE");
				}));
				GetCommonScr->ForceStopAnnouce(true);
			}
		}
		else if (name == "change_pass_btn")
		{
			OnVisiblePanelSetting(false,
				[this]() {
				ScrMgr->SaveTheGame();
				//scr_common->SetUser(this->p_user);
				GetCommonScr->OnShowLayer(COMMON_LAYER::CHANGE_PASS_LAYER);

			});
		}
		else if (name == "show_tutorial_btn")
		{
			OnVisiblePanelSetting(false,
				[this]() {
				ScrMgr->SaveTheGame();
				GetCommonScr->OnShowLayer(COMMON_LAYER::TUTORIAL_LAYER);
			});
		}
		else if (name == "call_support_btn")
		{
			if (GET_GUIDE->getHelp() != "")
			{
				OPEN_URL(GET_GUIDE->getHelp());
			}
			else
			{
				GetGuideService()->ShowIwinPage();
			}
		}
		else if (name == "email_btn")
		{
			Platform::openEmail("hotro@iwin.me" , L"Góp Ý");
		}
	}
	else if (type_widget == UI_TYPE::UI_LISTVIEW)
	{
		
			auto p_radio_btn_layer = static_cast<RadioBtnWidget*>(_widget->GetParentWidget()->GetWidgetChildAtIdx(1));
			if (name.Contains("_MOVE_NEXT"))
			{
				static_cast<RadioBtnWidget*>(p_radio_btn_layer)->MoveToNextBtn();
			}
			else if (name.Contains("_MOVE_BACK"))
			{
				static_cast<RadioBtnWidget*>(p_radio_btn_layer)->MovePreviousBtn();
			}
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		
	}
	else if (type_widget == UI_TYPE::UI_SLIDER)
	{
		float value = static_cast<SliderWidget*>(_widget)->GetPercent();
		RKString state_name_save = "";
		if (name == "slider_sound_SLIDEBALL_UP")
		{
			state_name_save = STR_SAVE_SOUND_VOLUME;
			float value1 = value / 100.f;
			GetSound->SetVolume(value1);
		}
		else if (name == "slider_music_SLIDEBALL_UP")
		{
			state_name_save = STR_SAVE_MUSIC_VOLUME;
			float value1 = value / 100.f;
			GetSound->SetVolume(value1, false);
		}
		ScrMgr->SaveDataFloat(state_name_save, value);
		ScrMgr->SaveTheGame();
	}
	else if (type_widget == UI_TYPE::UI_CHECK_BOX)
	{
		auto sep = name.SplitLast("_");
		RKString state = sep.GetAt(1);
		RKString _name = sep.GetAt(0);
		RKString state_name_save = "";
		if (_name == "check_box_invite_play")
		{
			if (state == "SELECTED")
			{
				GlobalService::sendStaticInviviteToPlayGame(true);
			}
			else if (state == "UNSELECTED")
			{
				GlobalService::sendStaticInviviteToPlayGame(false);
			}
			state_name_save = STR_SAVE_INVITE_STATE;
		}
		else if (_name == "check_box_brivate")
		{
			if (state == "SELECTED")
			{
				Utils::enableVibrate(true);
			}
			else if (state == "UNSELECTED")
			{
				Utils::enableVibrate(false);
			}
			state_name_save = STR_SAVE_BRIVATE_STATE;
		}
		else if (_name == "check_box_fb_login")
		{
			if (state == "SELECTED")
			{

			}
			else if (state == "UNSELECTED")
			{

			}
			state_name_save = STR_SAVE_FB_STATE;
		}
		ScrMgr->SaveDataInt(state_name_save, state == "SELECTED" ? 1 : 0);
		ScrMgr->SaveTheGame();
	}
	else if (type_widget == UI_TYPE::UI_TIME_PROCESS)
	{
		if (_widget->GetParentWidget()->GetName() == "countdown_change_server")
		{
			p_on_count_down = false;
		}
	}
}

void MainScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(MAIN_SCREEN);
	float current_sound_volume = 0;
	float current_music_volume = 0;
	p_panel_notice_12->Visible(false);

	ScrMgr->SaveTheGame();
	p_menu_show_next = MENU_NONE;
}

void MainScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
    //ScrMgr->OnHideDialog();

	auto btn = p_panel_list_btn->GetWidgetChildByName(".btn_award");
	btn->GetWidgetChildByName(".icon_star")->SetActionCommandWithName("ROTATE");
}

void MainScreen::PlayMusic()
{
	PLAY_MUSIC("MUSIC_MAIN");
}

void MainScreen::OnBeginFadeIn()
{
	auto list_server = GetServerMgr()->getServerList();
	RKString name_server = GetServerMgr()->getCurServer()._name;
	for (size_t i = 0; i < list_server.size(); i++)
	{
		auto current_layout_t = p_list_server->GetWidgetChildAtIdx(i);
		current_layout_t->GetWidgetChildByName(".stick_icon")->Visible(false);
		current_layout_t->GetWidgetChildByName(".highlights")->Visible(false);

		RKString current_name = static_cast<TextWidget*>(current_layout_t->GetWidgetChildByName(".server_choose_btn.server_name"))->GetText();
		if (current_name == name_server)
		{
			current_layout_t->GetWidgetChildByName(".stick_icon")->Visible(true);
			current_layout_t->GetWidgetChildByName(".highlights")->Visible(true);
		}
	}

	static_cast<TextWidget*>(GetWidgetChildByName("main_screen.btn_server.name_server"))->SetText(name_server);

	

	if (!p_isCallRequestFriend) 
	{
		GlobalService::requestFriendList();
		p_isCallRequestFriend = true;
	}

	p_panel_notice_12->GetParentWidget()->GetWidgetChildByName(".btn_12")->SetActionCommandWithName("SET_POS");
}

/*
void MainScreen::onMyInfoChanged()
{
	//Player myinfo = GameController.myInfo;
	//userData = GameController.myProfile;
	//setNickNameAndFullName();
	//UIFactory.packLayout(lblMyAccountName);

	//btnMyMoney.setText(Util.getShortMoney(myInfo->getMoney()));
	//btnMyRuby.setText(Util.getShortMoney(myInfo->getRuby()));

	//UIFactory.packLayout(btnMyMoney);
	//UIFactory.packLayout(btnMyRuby);

	//boolean notLoginOrGuest = !GameController.isLoggedIn() || GameController.getLoginType() == AccountTypes.guest;

	//lblMyMoney.setVisible(!notLoginOrGuest);
	//lblMyAccountName.setVisible(!notLoginOrGuest);
	//btnLogin.setVisible(notLoginOrGuest);
	//lblNotLogin.setVisible(notLoginOrGuest);
	//if (notLoginOrGuest) {
	//	imgMyAvatar.setColor(CLR_AVATAR_BACKGROUND_NOT_LOGIN);
	//	imgMyAvatar.setAvatarDefaultAvatar();

	//	//			imgPromotionNotification.setVisible(false);
	//	// star.setVisible(false);
	//}
	//else {
	//	imgMyAvatar.setColor(Color.WHITE);
	//	imgMyAvatar.setAvatar(myInfo->name, myInfo->avatar);

	//	//			imgPromotionNotification.setVisible(true);
	//	// star.setVisible(true);
	//}
}
*/

void MainScreen::InsertGameType(LIST_GAME type, const std::vector<int> & list_game_win, const std::vector<int> & list_game_ruby)
{
	p_list_game_win.insert({ type , ListGame(type,list_game_win) });
	bool contain_any_support_game = false;
	for (auto c : list_game_win)
	{
		if (IsGameSupport(c))
		{
			contain_any_support_game = true;
			break;
		}
	}
	if (contain_any_support_game)
	{
		GenerateBtnListGameType(type, BASE_MONEY);
	}
	if (list_game_ruby.size() > 0)
	{
		p_list_game_ruby.insert({ type , ListGame(type,list_game_ruby) });
		contain_any_support_game = false;
		for (auto c : list_game_ruby)
		{
			if (IsGameSupport(c))
			{
				contain_any_support_game = true;
				break;
			}
		}
		if (contain_any_support_game)
		{
			GenerateBtnListGameType(type, RUBY_MONEY);
		}
	}

}

void MainScreen::OnParseGameList()
{
	//for (size_t i = 0; i < p_panel_list_game->GetNumberChildWidget(); i++)
	{
		p_panel_list_game->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0)->ClearChild();
	}

	//clear top btn
	ClearListBtnGame();

	//win & ruby
	auto list_game_ruby = p_gamelist->getGameHasRuby();
	if (p_gamelist->getGamePopular().size() > 0)
	{
		std::vector<int> list_game_;
		std::vector<int> list_game_ru;
		for (auto g : p_gamelist->getGamePopular())
		{
			list_game_.push_back(g);
			for (auto p : list_game_ruby)
			{
				if (p == g)
				{
					list_game_ru.push_back(g);
				}
			}
		}
		InsertGameType(GAME_POPULAR, list_game_, list_game_ru);
	}
	if (p_gamelist->getGameCard().size() > 0)
	{
		std::vector<int> list_game_;
		std::vector<int> list_game_ru;
		for (auto g : p_gamelist->getGameCard())
		{
			list_game_.push_back(g);
			for (auto p : list_game_ruby)
			{
				if (p == g)
				{
					list_game_ru.push_back(g);
				}
			}
		}
		InsertGameType(GAME_GAMBLE, list_game_, list_game_ru);
	}
	if (p_gamelist->getGameChess().size() > 0)
	{
		std::vector<int> list_game_;
		std::vector<int> list_game_ru;
		for (auto g : p_gamelist->getGameChess())
		{
			list_game_.push_back(g);
			for (auto p : list_game_ruby)
			{
				if (p == g)
				{
					list_game_ru.push_back(g);
				}
			}
		}
		InsertGameType(GAME_CHESS, list_game_, list_game_ru);
	}
	if (p_gamelist->getGameOther().size() > 0)
	{
		std::vector<int> list_game_;
		std::vector<int> list_game_ru;
		for (auto g : p_gamelist->getGameOther())
		{
			list_game_.push_back(g);
			for (auto p : list_game_ruby)
			{
				if (p == g)
				{
					list_game_ru.push_back(g);
				}
			}
		}
		InsertGameType(GAME_OTHER, list_game_, list_game_ru);
	}
	if (p_gamelist->getGameHot().size() > 0)
	{
		std::vector<int> list_game_;
		std::vector<int> list_game_ru;
		for (auto g : p_gamelist->getGameHot())
		{
			list_game_.push_back(g);
			for (auto p : list_game_ruby)
			{
				if (p == g)
				{
					list_game_ru.push_back(g);
				}
			}
		}
		InsertGameType(GAME_HOT, list_game_, list_game_ru);
	}
	if (p_gamelist->getGameNew().size() > 0)
	{
		std::vector<int> list_game_;
		std::vector<int> list_game_ru;
		for (auto g : p_gamelist->getGameNew())
		{
			list_game_.push_back(g);
			for (auto p : list_game_ruby)
			{
				if (p == g)
				{
					list_game_ru.push_back(g);
				}
			}
		}
		InsertGameType(GAME_NEW, list_game_, list_game_ru);
	}
	if (p_gamelist->getGameTournaments().size() > 0)
	{
		std::vector<int> list_game_;
		std::vector<int> list_game_ru;
		for (auto g : p_gamelist->getGameTournaments())
		{
			list_game_.push_back(g);
			for (auto p : list_game_ruby)
			{
				if (p == g)
				{
					list_game_ru.push_back(g);
				}
			}
		}
		InsertGameType(GAME_TOURNAMENT, list_game_, list_game_ru);
	}


	XMLMgr->OnLoadXMLData<MainScreen>("ListGameDec", std::mem_fn(&MainScreen::LoadXMLListGame), this);

	if (!GetUser || GetUser->id == -1)
	{
		p_bottom_menu->GetWidgetChildByName(".btn_purchase_win")->Visible(false);
		p_bottom_menu->GetWidgetChildByName(".btn_purchase_ruby")->Visible(false);
	}
	else
	{
		if (GameController::currentMoneyType == BASE_MONEY)
		{
			p_bottom_menu->GetWidgetChildByName(".btn_purchase_win")->Visible(true);
			p_bottom_menu->GetWidgetChildByName(".btn_purchase_ruby")->Visible(false);
		}
		else
		{
			p_bottom_menu->GetWidgetChildByName(".btn_purchase_ruby")->Visible(true);
			p_bottom_menu->GetWidgetChildByName(".btn_purchase_win")->Visible(false);
		}
	}
}

void MainScreen::onReceivedGameList(void* gamelist)
{
	if (p_gamelist)
	{
		delete p_gamelist;
		p_gamelist = nullptr;
	}

	p_list_game_ruby.clear();
	p_list_game_win.clear();

	p_gamelist = (GameList*)gamelist;

	this->PushEvent([](BaseScreen * scr) {
		MainScreen *main_scr = (MainScreen*)scr;
		main_scr->OnParseGameList();
	});
}

void MainScreen::LoadXMLListGame(TiXmlDocument * p_objectXML)
{
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return;
	}

	auto sample_ui_dec = Root->FirstChildElement("sample_ui_dec");
	if (!sample_ui_dec)
	{
		PASSERT2(false, "XML loader fail to load!");
		return;
	}

	//std::map<RKString , xml::UILayerWidgetDec*> p_list_sample_dec;

	auto sample_ui_dec_xml = sample_ui_dec->FirstChildElement("Widget");
	do
	{
		if (sample_ui_dec_xml)
		{
			auto _widget_root = XMLMgr->OnParseWidgetEntity1(sample_ui_dec_xml, nullptr, XMLMgr->GetUIWidgetByName(name_ui), nullptr);
			p_list_sample_dec.insert(std::pair<RKString, xml::UILayerWidgetDec*>(_widget_root->NameWidget , _widget_root));
			sample_ui_dec_xml = sample_ui_dec_xml->NextSiblingElement();
		}
	} while (sample_ui_dec_xml);
	
#if (DEFAULT_SHOW == 0)
	{
		if (p_list_game_win.size() > 0)
		{
			OnParseCustomUIDec(p_list_game_win.at(GAME_GAMBLE).list_game, "test", p_list_sample_dec);
			OnShowHLBtnGameAtIdx(BASE_MONEY);
			OnShowHLBtnListGame(0);
		}
	}
#else
	{
		if (p_list_game_ruby.size() > 0 && (p_menu_come_from != NEW_LOBBY_SCREEN && p_menu_come_from != LOBBY_SCREEN))
		{
			OnParseCustomUIDec(p_list_game_ruby.at(GAME_GAMBLE).list_game, "test", p_list_sample_dec);
			OnShowHLBtnGameAtIdx(RUBY_MONEY);
			OnShowHLBtnListGame(0);
		}
		else if (p_list_game_win.size() > 0 && (p_menu_come_from == NEW_LOBBY_SCREEN || p_menu_come_from == LOBBY_SCREEN))
		{
			OnParseCustomUIDec(p_list_game_win.at(GAME_GAMBLE).list_game, "test", p_list_sample_dec);
			OnShowHLBtnGameAtIdx(BASE_MONEY);
			OnShowHLBtnListGame(0);
		}
	}
#endif
}

void MainScreen::OnShowListBtnGame(bool value)
{
	if (value)
	{
		p_panel_list_game->GetWidgetChildAtIdx(0)->Visible(true);
		p_panel_list_game->GetWidgetChildAtIdx(1)->Visible(false);
	}
	else
	{
		p_panel_list_game->GetWidgetChildAtIdx(0)->Visible(false);
		p_panel_list_game->GetWidgetChildAtIdx(1)->Visible(true);
		p_panel_list_game->GetWidgetChildAtIdx(1)->SetActionCommandWithName("ROTATE");
	}
}

void MainScreen::OnParseCustomUIDec(vector<int> list_game, RKString name, std::map<RKString, xml::UILayerWidgetDec*> list_sample_dec)
{

	std::vector<int> list_game_support;

	for (auto i : list_game)
	{
		if (IsGameSupport(i))
		{
			list_game_support.push_back(i);
		}
	}
	int size_game = list_game_support.size();
	if (size_game <= 0)
	{
		return;
	}
	OnShowListBtnGame(true);

	p_panel_list_game->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0)->ClearChild();

	auto page_ = static_cast<ListViewWidget*>(p_panel_list_game->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0));
	auto page_layout = p_panel_list_game->GetWidgetChildAtIdx(2);
	auto menu_widget_xml = XMLMgr->GetUIWidgetByName(name_ui);
	auto parent_layer_ = menu_widget_xml->GetElementChildByName("main_screen")->GetChildWidgetByName("list_game_panel")->GetChildWidgetByName("layout_list_game_")->GetChildWidgetByName("list_game_");

	auto layer_game = GetLayerChildByName("main_screen");
	int actual_size = 0;
	for (int i = 0; i < size_game; i++)
	{
		int id = list_game_support.at(i);

		if (ScrMgr->GetGameDetailByType((GameType)id).id == GameType::NULL_TYPE)
		{
			continue;
		}
		actual_size++;
		auto dec_game = ScrMgr->GetGameDetailByType((GameType)id);

		auto ui_sample = (list_sample_dec.at("ico_")->Clone());

        ui_sample->NameWidget = "ico_" + std::string(dec_game.str.GetString());
		auto btn_ = ui_sample->GetChildWidgetByName("panel_card")->GetChildWidgetByName("btn_");
		btn_->NameWidget = "btn_playgame_" + std::to_string(id);
		btn_->GeneralValue->ReplaceDataChar("source", dec_game.img.GetString());
		btn_->GeneralValue->ReplaceDataChar("press", dec_game.img.GetString());
		btn_->GeneralValue->ReplaceDataChar("disable", dec_game.img.GetString());
		btn_->p_menu_widget = menu_widget_xml;

		RKString game_name = dec_game.name;
		RKString game_name_server = ServerMgr::getInstance()->getCurServer()._name;

		auto text_game_name = ui_sample->GetChildWidgetByName("panel_card")->GetChildWidgetByName("name_game");
		text_game_name->GeneralValue->ReplaceDataChar("source", game_name.GetString());
		text_game_name->p_menu_widget = menu_widget_xml;

		ui_sample->p_parent_layer = parent_layer_;
		ui_sample->p_menu_widget = menu_widget_xml;
		ui_sample->RecorrectLayerParent(ui_sample);
		ui_sample->RecorrectMenuParent(menu_widget_xml);

		auto _widget_child = layer_game->InitWidgetEntity(ui_sample, layer_game, page_);

		auto btn_w = _widget_child->GetWidgetChildByName(".panel_card.btn_playgame_" + std::to_string(id));
		static_cast<ButtonWidget*>(btn_w)->SetActionBtn(
			[this , id, game_name , game_name_server]()
		{
				GameType current_type = (GameType)id;
				if (this->p_gamelist->hasNewLobby(current_type))
				{
					ScrMgr->SwitchToMenu(MENU_LAYER::NEW_LOBBY_SCREEN, MENU_LAYER::MAIN_SCREEN);
					RKString game_ = LangMgr->GetString(game_name);
					auto ui_screen = ScrMgr->GetMenuUI(MENU_LAYER::NEW_LOBBY_SCREEN);
					static_cast<NewLobbyScreen*>(ui_screen)->setTitle((game_name_server + "-" + game_).GetString());
					static_cast<NewLobbyScreen*>(ui_screen)->SetCurrentGameID(id);
				}
				else
				{
					ScrMgr->SwitchToMenu(MENU_LAYER::LOBBY_SCREEN, MENU_LAYER::MAIN_SCREEN);
					RKString game_ = LangMgr->GetString(game_name);
					static_cast<LobbyScreen*>(ScrMgr->GetMenuUI(MENU_LAYER::LOBBY_SCREEN))->SetTextTitle(game_name_server + "-" + game_);
					static_cast<LobbyScreen*>(ScrMgr->GetMenuUI(MENU_LAYER::LOBBY_SCREEN))->SetCurrentGameID(id);
				}
		});

		if(p_list_game_id.size() > 0 && p_list_game_id.find(id) != p_list_game_id.end())
		{
			auto current_obj_data = p_list_game_id.at(id);
			static_cast<TextWidget*>(btn_w->GetWidgetChildByName(".page_.panel_2.number_player"))->SetText(std::to_string(current_obj_data->user));
			static_cast<TextWidget*>(btn_w->GetWidgetChildByName(".page_.panel_1.number_room"))->SetText(std::to_string(current_obj_data->room));
		}

		page_->ResizeAndPosChildWidget(_widget_child);
		page_->PushBackItem(_widget_child);

		delete ui_sample;
		ui_sample = nullptr;
	}
	int number_panel_each_screen = 0;
	if (size_game > 0)
	{
		auto size_each_panel = page_->GetWidgetChildAtIdx(0)->GetSize();
		number_panel_each_screen = (GetGameSize().width / size_each_panel.x);
		if (number_panel_each_screen >= actual_size)
		{
			number_panel_each_screen = actual_size;
		}

		static_cast<ListView*>(page_->GetResource())->SetNumberItemMove(number_panel_each_screen);

		if (number_panel_each_screen >= actual_size)
		{
			//static_cast<ListView*>(page_->GetResource())->SetNumberItemMove(number_panel_each_screen);
			//static_cast<ListView*>(page_->GetResource())->SetAsPageView(true);
			static_cast<ListView*>(page_->GetResource())->setMagneticType(ListView::MagneticType::CENTER);
		}
		else
		{
			//static_cast<ListView*>(page_->GetResource())->SetAsPageView(false);
			static_cast<ListView*>(page_->GetResource())->setMagneticType(ListView::MagneticType::BOTH_END);
		}
		page_->SetNumberEachSlide(number_panel_each_screen);

		auto p_radio_btn_layer = page_->GetParentWidget()->GetWidgetChildAtIdx(1);
		
		int number_radio_btn = (actual_size / number_panel_each_screen) + ((actual_size % number_panel_each_screen) > 0 ? 1 : 0);
		static_cast<RadioBtnWidget*>(p_radio_btn_layer)->SetNumberRadioBtn(number_radio_btn);
	}

	this->PushEvent([number_panel_each_screen](BaseScreen * scr) {
		MainScreen * main_scr = (MainScreen*)scr;
		auto page_ = static_cast<ListViewWidget*>(main_scr->p_panel_list_game->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0));
		if (number_panel_each_screen >= page_->GetNumberChildWidget())
		{
			if (number_panel_each_screen % 2 == 0)
			{
				int idx_scroll_to = number_panel_each_screen / 2.f;
				static_cast<ListView*>(page_->GetResource())->scrollToItem(idx_scroll_to, Vec2(0.5, 0.5), Vec2(0, 0.5));
			}
			else
			{
				int idx_scroll_to = (number_panel_each_screen / 2);
				static_cast<ListView*>(page_->GetResource())->scrollToItem(idx_scroll_to, Vec2(0.5, 0.5), Vec2(0.5, 0.5), 1.0f);
			}
		}
		else
		{
			int idx_scroll_to = number_panel_each_screen / 2.f;
			static_cast<ListView*>(page_->GetResource())->scrollToItem(idx_scroll_to, Vec2(0.5, 0.5), Vec2(0, 0.5));
		}
	}, 0.2f);

}

RKString MainScreen::GetIDNameGameByType(LIST_GAME type)
{
	switch (type)
	{
	case GAME_GAMBLE:
		return "card_games";
		break;
	case GAME_CHESS:
		return "chess_games";
		break;
	case GAME_OTHER:
		return "more_games";
		break;
	case GAME_HOT:
		return "hot_games";
		break;
	case GAME_NEW:
		return "new_games";
		break;
	case GAME_UPDATE:
		return "update";
		break;
	case GAME_TOURNAMENT:
		return "tournament";
		break;
	case GAME_POPULAR:
		return "popular_games";
		break;
	default:
		break;
	}
	return "";
}

//handle generate list btn select type game list at top
void MainScreen::GenerateBtnListGameType(LIST_GAME type, int money_type)
{
	WidgetEntity * list_widget = nullptr;
	if (money_type == BASE_MONEY)
	{
		list_widget = p_list_btn_game_win;
	}
	else if (money_type == RUBY_MONEY)
	{
		list_widget = p_list_btn_game_ruby;
	}
	auto list_btn_choose_type_money = GetWidgetChildByName("main_screen.list_btn_choose_type_money");
	auto sample_btn = list_btn_choose_type_money->GetWidgetChildByName(".btn_iwin_gamble");
	auto star_sample = list_btn_choose_type_money->GetWidgetChildByName(".sao");

	//clone star and btn
	auto insert_btn = sample_btn->Clone();
	insert_btn->Visible(true);
	auto insert_star = star_sample->Clone();
	insert_star->Visible(true);
	float delta_distance = 30.f * GetGameSize().width / this->GetDesignSize().x;
	//set name again
	static_cast<TextWidget*>(insert_btn->GetWidgetChildAtIdx(0))->SetText(GetIDNameGameByType(type));
	//insert btn and set position
	auto contain_size = list_widget->GetResource()->getContentSize();

	int idx = list_widget->GetNumberChildWidget();
	idx = idx / 2;

	insert_btn->SetPosition(Vec2(contain_size.width, insert_btn->GetPosition().y));
	list_widget->InsertChildWidget(insert_btn);
	insert_star->SetPosition(Vec2(contain_size.width + insert_btn->GetSize().x + delta_distance, insert_btn->GetPosition().y));
	list_widget->InsertChildWidget(insert_star);
	//set content size again when insert btn
	list_widget->GetResource()->setContentSize(cocos2d::Size(contain_size.width + insert_btn->GetSize().x + delta_distance + insert_star->GetSize().x + delta_distance, contain_size.height));

	list_widget->SetPosition(list_widget->GetPosition());

	if (money_type == BASE_MONEY)
	{
		static_cast<ButtonWidget*>(insert_btn)->SetActionBtn([this , type , idx]() {
			if (p_list_game_win.size() > 0 && p_list_game_win.find(type) != p_list_game_win.end())
			{
				OnShowHLBtnListGame(idx);
				OnParseCustomUIDec(p_list_game_win.at(type).list_game, "test", p_list_sample_dec);
			}
		});
	}
	else if (money_type == RUBY_MONEY)
	{
		static_cast<ButtonWidget*>(insert_btn)->SetActionBtn([this , type , idx]() {
			if (p_list_game_ruby.size() > 0 && p_list_game_ruby.find(type) != p_list_game_ruby.end())
			{
				OnShowHLBtnListGame(idx);
				OnParseCustomUIDec(p_list_game_ruby.at(type).list_game, "test", p_list_sample_dec);
			}
		});
	}
}

//clear list game
void MainScreen::ClearListBtnGame()
{
	float ratio = GetGameSize().width / this->GetDesignSize().x;
	cocos2d::Size origin_content_size = cocos2d::Size(30 * ratio, 30 * ratio);
	p_list_btn_game_win->ClearChild();
	p_list_btn_game_win->GetResource()->setContentSize(origin_content_size);
	p_list_btn_game_ruby->ClearChild();
	p_list_btn_game_ruby->GetResource()->setContentSize(origin_content_size);
}

void MainScreen::ParseUserUI()
{
	User * user = GetUser;
	if (user == nullptr)
	{
		//PASSERT2(false, "user is null");
		return;
	}
	RKString nick_name = user->nickName;
	u64 money = GameController::myInfo->money;
	u64 ruby = GameController::myInfo->ruby;

	auto w_name = p_panel_list_btn->GetParentWidget()->GetWidgetChildByName(".panel_name.title_username");
	auto w_money = p_panel_list_btn->GetParentWidget()->GetWidgetChildByName(".btn_purchase_win");
	auto w_ruby = p_panel_list_btn->GetParentWidget()->GetWidgetChildByName(".btn_purchase_ruby");

	static_cast<TextWidget*>(w_name)->SetText(user->fullname + " (" + nick_name + ")");

	static_cast<TextWidget*>(w_money->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(money));
	static_cast<TextWidget*>(w_ruby->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(ruby));

	//check user is active or not
	bool isActive = GameController::myInfo->isActived;

	if (isActive)
	{
		p_panel_list_btn->GetWidgetChildByName(".btn_award")->SetActionCommandWithName("POS_2");
		p_panel_list_btn->GetWidgetChildByName(".btn_friend")->SetActionCommandWithName("POS_2");
		p_panel_list_btn->GetWidgetChildByName(".btn_event")->Visible(true);
		GlobalService::getCountQuestList();
	}
	else
	{
		p_panel_list_btn->GetWidgetChildByName(".btn_award")->SetActionCommandWithName("POS_1");
		p_panel_list_btn->GetWidgetChildByName(".btn_friend")->SetActionCommandWithName("POS_1");
		p_panel_list_btn->GetWidgetChildByName(".btn_event")->Visible(false);
	}

	//realign position
	for (int i = 2; i < p_panel_list_btn->GetNumberChildWidget(); i++)
	{
		p_panel_list_btn->GetWidgetChildAtIdx(i)->SetActionCommandWithName("POS_1");
	}

	GameMgr->HandleAvatarDownload(
		user->avatarID, user->nickName, [this](void * data, void * str, int tag)
	{
		this->ParseImgToPanel(this->panel_avatar_user, data, str, tag);
	});

	ScrMgr->setBtnTaixiuDirty();
}

void MainScreen::SetAnnouncementAction()
{
	if (p_force_stop_annouce)
	{
		p_force_stop_annouce = false;
		return;
	}
	auto tex = static_cast<TextWidget*>(p_layout_anoun_server->GetWidgetChildAtIdx(1));
	tex->SetText(p_announce_text, true);
	p_layout_anoun_server->SetActionCommandWithName("ON_SHOW", CallFunc::create(
		[this, tex]()
	{
		auto size = tex->GetResource()->getContentSize();
		this->p_layout_anoun_server->ForceFinishAction();
		this->p_annouce_is_running = true;
		tex->SetActionCommandWithName("ON_SHOW");
	}));
}

void MainScreen::OnReceiveAnnouncementFromServer(RKString text)
{
	text.ReplaceAll("\n", " ");

	if (!p_layout_anoun_server->Visible())
	{		
		p_announce_text = text;
		this->PushEvent([](BaseScreen* scr) {
			MainScreen * main_screen = (MainScreen*)scr;
			main_screen->SetAnnouncementAction();
		});
	}
	else
	{
		p_announce_text.Append("    |    ");
		p_announce_text.Append(text);
		auto w_text = this->p_layout_anoun_server->GetWidgetChildAtIdx(1);
		static_cast<TextWidget*>(w_text)->SetText(this->p_announce_text, true);
	}

	GetCommonScr->OnReceiveAnnouncementFromServer(text);
}

iwinmesage::GameList* MainScreen::GetGameList() 
{
	return p_gamelist;
}

void MainScreen::OnShowHLBtnListGame(int idx)
{
	WidgetEntity * list_btn = nullptr;
	if (p_list_btn_game_win->Visible())
	{
		list_btn = p_list_btn_game_win;
	}
	else
	{
		list_btn = p_list_btn_game_ruby;
	}

	for (int i = 0; i < list_btn->GetNumberChildWidget(); i++)
	{
		if (i % 2 == 0)
		{
			list_btn->GetWidgetChildAtIdx(i)->GetWidgetChildAtIdx(0)->SetColor(Color3B(150, 150, 150));
		}
	}
	if (idx >= 0 && idx < list_btn->GetNumberChildWidget())
	{
		list_btn->GetWidgetChildAtIdx(idx * 2)->GetWidgetChildAtIdx(0)->SetColor(Color3B(255, 210, 10));
	}
}

void MainScreen::OnShowHLBtnGameAtIdx(int idx)
{
	if (idx >= (int)p_panel_list_btn_choose_game->GetNumberChildWidget())
	{
		PASSERT2(false, "idx out of range");
		return;
	}

	GameController::currentMoneyType = idx;

	for (size_t i = 0; i < 2; i++)
	{
		auto w_panel = p_panel_list_btn_choose_game->GetWidgetChildAtIdx(i);
		w_panel->GetWidgetChildAtIdx(1)->Visible(true); //no choose
		w_panel->GetWidgetChildAtIdx(2)->Visible(false); //choose
	}
	p_panel_list_btn_choose_game->GetWidgetChildAtIdx(idx-1)->GetWidgetChildAtIdx(1)->Visible(false);
	p_panel_list_btn_choose_game->GetWidgetChildAtIdx(idx-1)->GetWidgetChildAtIdx(2)->Visible(true);

	auto w_money = p_panel_list_btn->GetParentWidget()->GetWidgetChildByName(".btn_purchase_win");
	auto w_ruby = p_panel_list_btn->GetParentWidget()->GetWidgetChildByName(".btn_purchase_ruby");
	if (idx == BASE_MONEY) //win
	{
		//show display screen
		GetWidgetChildByName("main_background.background_1")->Visible(true);
		GetWidgetChildByName("main_background.background_2")->Visible(false);
		//show btn iap
		w_money->Visible(true);
		w_ruby->Visible(false);

		p_list_btn_game_win->Visible(true);
		p_list_btn_game_ruby->Visible(false);
	}
	else
	{
		GetWidgetChildByName("main_background.background_1")->Visible(false);
		GetWidgetChildByName("main_background.background_2")->Visible(true);
		w_money->Visible(false);
		w_ruby->Visible(true);

		p_list_btn_game_win->Visible(false);
		p_list_btn_game_ruby->Visible(true);
	}
}

void MainScreen::onReceiveAvatarCatList(void * data)
{
	AvatarCatList * value = (AvatarCatList *)data;
	int ttt = 0;
	ttt++;
}

void MainScreen::onReceiveAvatarList(void * data)
{
	AvatarList * value = (AvatarList *)data;
	int ttt = 0;
	ttt++;
}

void MainScreen::onBuyingAvatarResult(void * data)
{
	BuyingAvatar * value = (BuyingAvatar *)data;
	int ttt = 0;
	ttt++;
}

void MainScreen::OnParsePlayerNumberToEachGame()
{
	auto page_ = static_cast<ListViewWidget*>(p_panel_list_game->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0));

	for (size_t i = 0; i < page_->GetNumberChildWidget(); i++)
	{
		auto w_child = page_->GetWidgetChildAtIdx(i);
		auto btn_panel = w_child->GetWidgetChildByName(".panel_card")->GetWidgetChildAtIdx(0);
		RKString name = btn_panel->GetName();
		RKString id = name.SplitLast("_").GetAt(1);
		int p_id = atoi(id.GetString());

		auto current_obj_data = p_list_game_id.at(p_id);
		static_cast<TextWidget*>(btn_panel->GetWidgetChildByName(".page_.panel_2.number_player"))->SetText(std::to_string(current_obj_data->user));
		static_cast<TextWidget*>(btn_panel->GetWidgetChildByName(".page_.panel_1.number_room"))->SetText(std::to_string(current_obj_data->room));

	}
}

void MainScreen::onReceiveListWait(void* data1, void* data2, void* data3)
{
	int countRoom = *(int*)data1;
	int countUser = *(int*)data2;

	for (auto it = p_list_game_id.begin(); it != p_list_game_id.end(); it++)
	{
		delete it->second;
	}
	p_list_game_id.clear();

	p_list_game_id = *(std::map<int , GameObjectData*>*) data3;
	
	static_cast<TextWidget*>(p_panel_info_game->GetWidgetChildAtIdx(3))->SetText(std::to_string(countUser));
	static_cast<TextWidget*>(p_panel_info_game->GetWidgetChildAtIdx(4))->SetText(std::to_string(countRoom));
	OnParsePlayerNumberToEachGame();
	p_already_parse_list_wait = true;
}

void MainScreen::forceStopAnnouce(bool value)
{
	p_annouce_is_running = !value;
	p_layout_anoun_server->Visible(!value);
	p_force_stop_annouce = value;
}

void MainScreen::SetNumWinfree(int number)
{
	if (number > 0)
	{
		auto w = p_panel_list_btn->GetWidgetChildByName(".btn_event.icon_number");
		w->Visible(true);
		static_cast<TextWidget*>(w->GetWidgetChildAtIdx(1))->SetText(std::to_string(number));
	}
	else
	{
		auto w = p_panel_list_btn->GetWidgetChildByName(".btn_event.icon_number");
		w->Visible(false);
	}
}


void MainScreen::SetNumUnReadMsg(int number)
{
	if (number > 0)
	{
		auto w = p_panel_list_btn->GetWidgetChildByName(".btn_friend.icon_number");
		w->Visible(true);
		static_cast<TextWidget*>(w->GetWidgetChildAtIdx(1))->SetText(std::to_string(number));
	}
	else
	{
		auto w = p_panel_list_btn->GetWidgetChildByName(".btn_friend.icon_number");
		w->Visible(false);
	}
}

void MainScreen::OnReceiveFriendList()
{
	p_isCallRequestFriend = true;
}

void MainScreen::SetActionAfterShow(int action)
{
	if (action == 0)
	{
		this->PushEvent([](BaseScreen * scr) 
		{
			MainScreen * main_scr = (MainScreen*)scr;
			main_scr->OnVisiblePanelSetting(true, [main_scr]() {
				ScrMgr->SetScreenIDDirty();

				float current_sound_volume = GetSound->GetVolume();
				static_cast<SliderWidget*>(main_scr->p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
					".sound_volume.slider_sound"))->SetPercent(current_sound_volume * 100.f);

				float current_music_volume = GetSound->GetVolume(false);
				static_cast<SliderWidget*>(main_scr->p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
					".music_volume.slider_music"))->SetPercent(current_music_volume * 100.f);
			});
		}, TIME_ACTION);
	}
	else if (action == 1)
	{
		this->PushEvent([](BaseScreen * scr) {
			MainScreen * main_scr = (MainScreen*)scr;
			main_scr->p_have_layout_logout_appear = true;
			ScrMgr->OnShowDialog("sign_out", "do_you_want_to_log_ou",
				TYPE_DIALOG::DIALOG_TWO_BUTTON,
				"",
				[main_scr](const char * call_back, const char* btn_name)
			{
				main_scr->p_have_layout_logout_appear = false;

				ScrMgr->ForceLogOutToLoginScreen();
			},
				[main_scr](const char * call_back, const char* btn_name)
			{
				main_scr->p_have_layout_logout_appear = false;
			});

		}, TIME_ACTION);
	}
	else
	{

	}
}

int MainScreen::GetStateMainScreen()
{
	if (p_panel_setting->Visible())
	{
		return 0;
	}
	else
	{
		if (p_have_layout_logout_appear)
		{
			return 1;
		}
	}
	return 2;
}

void MainScreen::OnShowChangeServer()
{
	if (p_on_count_down)
	{
		return;
	}

	p_panel_cd_change_server->GetParentWidget()->Visible(true);
	p_timer_count_down = 10.f;
	

	p_on_count_down = true;

	p_panel_cd_change_server->GetParentWidget()->Visible(true);
	static_cast<TextWidget*>(p_panel_cd_change_server->GetWidgetChildByName(".timer_text"))->SetText( StringUtils::format(
		LANG_STR("connecting_to_serve").GetString() , GetServerMgr()->getCurServer()._name.c_str()
	), true);
	static_cast<TimeProcessWidget*>(p_panel_cd_change_server->GetWidgetChildByName(".progress_1"))->SetTimer(p_timer_count_down, 0);
	static_cast<TimeProcessWidget*>(p_panel_cd_change_server->GetWidgetChildByName(".progress_2"))->SetTimer(p_timer_count_down, 0, [this](cocos2d::Ref * ref, EventType_Process event_t) {
		if (event_t == EventType_Process::ON_TIMER_ZERO)
		{
            
			p_panel_cd_change_server->GetParentWidget()->SetVisible(false);
            
            //[TO DO : check save login server by type in previous and call again]
//            ScrMgr->SaveDataInt(STR_SAVE_TYPE_LOGIN, LOGIN_APPLE)
            int loginType = LOGIN_IWIN;
            ScrMgr->GetDataSaveInt(STR_SAVE_TYPE_LOGIN, loginType);
            
//            LOGIN_NONE = -1,
//            LOGIN_IWIN = 0,
//            LOGIN_FACEBOOK,
//            LOGIN_GOOGLE_PLAY,
//            LOGIN_APPLE,
//            LOGIN_GUEST,
//            LOGIN_MOBO,
//            LOGIN_COUNT
            switch(loginType)
            {
                case LOGIN_TYPE::LOGIN_FACEBOOK:
                {
//                    static_cast<LoginScreen*>(_base_screen)->startWaitingDialog();
                    Platform::FBUtils::loginFacebook([](bool res, void* data) {
                        if(res)
                        {
                            GameMgr->Network_loginWithToken(MainGame::TYPE_FACEBOOK, Platform::FBUtils::getAccessToken());
                        }
                    });
                    break;
                }
                case LOGIN_TYPE::LOGIN_GOOGLE_PLAY:
                {
//                    static_cast<LoginScreen*>(_base_screen)->startWaitingDialog();
                    if (!GetGGSignInMgr()->isConnected())
                    {
                        GetGGSignInMgr()->signIn();
                    }
                    else
                    {
                        GameMgr->Network_loginWithToken(MainGame::TYPE_GOOGLE, GetGGSignInMgr()->getGGToken());
                    }
                    break;
                }
                case LOGIN_TYPE::LOGIN_APPLE:
                {
//                    static_cast<LoginScreen*>(_base_screen)->startWaitingDialog();
                    if (!GetGCSignInMgr()->isSignedIn())
                    {
                        GetGCSignInMgr()->signIn();
                    }
                    else
                    {
                        GetGCSignInMgr()->getAccessToken([=](std::string accessToken) {
                            GameMgr->Network_loginWithToken(MainGame::TYPE_APPLEID, accessToken);
                        });
                    }
                    break;
                }
                case LOGIN_TYPE::LOGIN_IWIN:
                {
                    auto login_screen = static_cast<LoginScreen*>(ScrMgr->GetMenuUI(LOGIN_SCREEN));
                    RKString user = login_screen->GetUsername();
                    RKString pass = login_screen->GetPassword();
                    GameMgr->Network_LoginToGame(user,pass);
                    break;
                }
                default:
                {
                    break;
                }
            }
		}
	});

	GlobalService::sendSingleCommand(IwinProtocol::LOGOUT);
}

void MainScreen::ClearComponent()
{
	SAFE_DELETE(p_gamelist);

	for (auto it : p_list_sample_dec)
	{
		delete it.second;
	}
	p_list_sample_dec.clear();

	for (auto it : p_list_game_id)
	{
		delete it.second;
	}
	p_list_game_id.clear();

	p_list_game_win.clear();
	p_list_game_ruby.clear();

	(p_panel_list_game->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->ClearChild();
	OnShowListBtnGame(false);
	GameController::currentMoneyType = BASE_MONEY;

	//clear top btn
	ClearListBtnGame();

	p_isCallRequestFriend = false;
	p_stage_get_data = 1;
}

void MainScreen::OnKeyBack()
{
	if (OnChangingServer())
		return;
	if (p_panel_choose_server->GetParentWidget()->Visible())
	{
		OnVisiblePanelServer(false , nullptr);
	}
	else
	{
		if (p_panel_setting->GetParentWidget()->Visible())
		{
			OnVisiblePanelSetting(false,
				[this]() {
				ScrMgr->SaveTheGame();
				logout();
			});
		}
		else
		{
			logout();
		}
	}
}

void MainScreen::ChangeToNewServer()
{

}

bool MainScreen::OnChangingServer()
{
	return p_panel_cd_change_server->GetParentWidget()->Visible();
}

cocos2d::Rect MainScreen::GetWorldObjectPosIntutorial(int state, WidgetEntity *& obj)
{
	cocos2d::Rect rect(0, 0, 0, 0);

	if (state == 0)
	{
		Vec2 s = p_panel_list_btn_choose_game->GetSize();
		Vec2 pos_bot_left = p_panel_list_btn_choose_game->GetPosition_BottomLeft();
		rect.size = cocos2d::Size(s.x, s.y);
		rect.origin = pos_bot_left;
		obj = p_panel_list_btn_choose_game;
	}
	else if (state == 1)
	{
		auto btn = p_bottom_menu->GetWidgetChildByName(".btn_purchase_win");
		if (!btn->Visible())
		{
			btn = p_bottom_menu->GetWidgetChildByName(".btn_purchase_ruby");
		}

		Vec2 s = btn->GetSize();
		Vec2 pos_bot_left = btn->GetPosition_BottomLeft();
		rect.size = cocos2d::Size(s.x, s.y);
		rect.origin = pos_bot_left;
		obj = btn;
	}
	else if (state == 2)
	{
		auto btn = p_panel_list_btn->GetWidgetChildByName(".btn_event");
		Vec2 s = btn->GetSize();
		Vec2 pos_bot_left = btn->GetPosition_BottomLeft();
		Vec2 w_pos = btn->GetParentWidget()->GetResource()->convertToWorldSpace(pos_bot_left);
		rect.size = cocos2d::Size(s.x, s.y);
		rect.origin = w_pos;
		obj = btn;
	}
	else if (state == 3)
	{
		
	}
	else if (state == 4)
	{
		auto btn = p_panel_list_btn->GetWidgetChildByName(".btn_award");
		Vec2 s = btn->GetSize();
		Vec2 pos_bot_left = btn->GetPosition_BottomLeft();
		Vec2 w_pos = btn->GetParentWidget()->GetResource()->convertToWorldSpace(pos_bot_left);
		rect.size = cocos2d::Size(s.x, s.y);
		rect.origin = w_pos;
		obj = btn;
	}

	return rect;
}

void MainScreen::OnShowListChooseLang(bool visible)
{
	auto current_pos = p_list_lang_choose->GetParentWidget()->GetPosition();
	//auto ac_pos = 
	if (visible)
	{
		p_list_lang_choose->SetActionCommandWithName("ON_SHOW");
	}
	else
	{
		p_list_lang_choose->SetActionCommandWithName("ON_HIDE");
	}
}

void MainScreen::SetBtnInviteNotifySelected(bool value)
{
	static_cast<CheckBoxWidget*>(p_panel_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
		".notice_invite_play.check_box_invite_play"))->OnSetSelected(value);
}

void MainScreen::logout()
{
    //[TO DO] logout each platform and trans to login screen
    //        1.detect login type
    //        2.logout by type
    //            a. if fb/gg --> show dialog confirm logout platform
    //            b. show dialog logout confirm
    //        3.logout normal iwin acc
    //        4.trans to login screen
    
    int loginType = -1;
    ScrMgr->GetDataSaveInt(STR_SAVE_TYPE_LOGIN, loginType);
    bool needConfirmLogout = loginType == LOGIN_FACEBOOK
                    || loginType == LOGIN_GOOGLE_PLAY;
    
    if (needConfirmLogout)
    {
        //show logout dialog confirm with logout platform
        this->PushEvent([loginType](BaseScreen * scr) {
            MainScreen * main_scr = (MainScreen*)scr;
            main_scr->p_have_layout_logout_appear = true;
            ScrMgr->OnShowDialog("sign_out", "do_you_want_to_log_ou",
                                 (loginType == LOGIN_FACEBOOK || loginType == LOGIN_GOOGLE_PLAY) ?TYPE_DIALOG::DIALOG_LOGOUT_ACC : TYPE_DIALOG::DIALOG_TWO_BUTTON,
                                 "",
                                 [main_scr, loginType](const char * call_back, const char* btn_name)
                                 {
                                     main_scr->p_have_layout_logout_appear = false;
                                     
                                     //log out platform depend on check or not
                                     //temp fake to test
                                     bool isCheck = (call_back != "");
                                     if(isCheck)
                                     {
                                         if(loginType == LOGIN_FACEBOOK)
                                         {
                                             GetFacebookMgr()->logout();
                                         }
                                         else if(loginType == LOGIN_GOOGLE_PLAY)
                                         {
                                             GetGGSignInMgr()->signOut();
                                         }
                                     }
                                     
                                     ScrMgr->ForceLogOutToLoginScreen();
									 static_cast<LoginScreen*>(ScrMgr->GetMenuUI(LOGIN_SCREEN))->OnClearPassInput();
                                 },
                                 [main_scr](const char * call_back, const char* btn_name)
                                 {
                                     main_scr->p_have_layout_logout_appear = false;
                                 });
        });
        
        return;
    }
    
    //show logout dialog confirm normal
    this->logoutByType(loginType);
    
}

void MainScreen::logoutByType(int loginType)
{
    this->PushEvent([loginType](BaseScreen * scr) {
        MainScreen * main_scr = (MainScreen*)scr;
        main_scr->p_have_layout_logout_appear = true;
        ScrMgr->OnShowDialog("sign_out", "do_you_want_to_log_ou",
                             TYPE_DIALOG::DIALOG_TWO_BUTTON,
                             "",
                             [main_scr, loginType](const char * call_back, const char* btn_name)
                             {
                                 if(loginType == LOGIN_APPLE)
                                 {
                                     GetGCSignInMgr()->signOut();
                                 }
                                 
                                 main_scr->p_have_layout_logout_appear = false;
                                 
                                 ScrMgr->ForceLogOutToLoginScreen();
								 static_cast<LoginScreen*>(ScrMgr->GetMenuUI(LOGIN_SCREEN))->OnClearPassInput();
                             },
                             [main_scr](const char * call_back, const char* btn_name)
                             {
                                 main_scr->p_have_layout_logout_appear = false;
                             });
    });
}

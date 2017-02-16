#include "HUDScreen.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTimeProcessWidget.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wSlider.h"

#include "Models/Category.h"
#include "Models/Player.h"
#include "Models/item.h"
#include "Common/GameController.h"
#include "Network/Game/GameLogicHandler.h"

#include "Network/JsonObject/Avatar/AvatarCatList.h"
#include "Network/JsonObject/Avatar/AvatarList.h"
#include "Network/JsonObject/Avatar/BuyingAvatar.h"

#include "Network/Game/GameService.h"

#include "Screens/ProfileScreen.h"
#include "Common/IwinListViewTable.h"
#include "Common/IwinTextfield.h"

#include "RKThread_Code/RKThreadWrapper.h"
#include "Object/PlayerGroup.h"
#include "Common/ChatBoard.h"
#include "Services/GuideService.h"
#define MAX_CHAT 20
#define DEF_CHAT 8
#define POPUP_GROUP_TIME_SHORT 5
#define POPUP_GROUP_TIME_LONG 10

std::wstring list_chat[DEF_CHAT] = {
	L"Nhanh lên pa!",
	L"Tạm biệt em!",
	L"Hehe",
	L"Cho chết! Hihi",
	L"Thua đi cưng",
	L"Mày hả bưởi",
	L"Nhanh đi, nghỉ lâu vậy",
	L"Sợ rùi à?"
};


HUDScreen::HUDScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
	p_list_player_layout = nullptr;
	p_panel_igm = nullptr;
	p_panel_ig_setting = nullptr;
	p_panel_bottom = nullptr;
	p_on_count_down = false;

	p_panel_chat_root = nullptr;
	p_btn_recent_chat = nullptr;
	p_btn_emo_chat = nullptr;
	p_list_recent_chat = nullptr;
	p_list_emoticon_chat = nullptr;
	p_panel_send_gift = nullptr;

	p_max_player = 0;
	p_list_pos_dec.clear();

	p_text_input_chat = nullptr;

	p_thread_parse_ui = nullptr;


	p_sample_dec = nullptr;
	p_list_player_waiting.clear();
	p_list_avatar_download.clear();
	_chat_board = nullptr;

	p_current_gift_layout_idx = -1;
	p_list_items.clear();

	p_current_id_items_select = -1;
	p_current_iddb_player_gift = -1;

	_idx_load_btn = 0;
	_idx_load_emo = 0;
	_recent_chat.clear();

}

HUDScreen::~HUDScreen()
{
	XMLMgr->RemoveUIMEnuWidgetDec("hud_screen");
	p_list_player_layout = nullptr;
	SAFE_DELETE_VECTOR(p_list_player_group);
	SAFE_DELETE_VECTOR(p_list_player);
	p_panel_igm = nullptr;
	p_panel_ig_setting = nullptr;
	p_panel_bottom = nullptr;
	p_panel_send_gift = nullptr;

	p_panel_chat_root = nullptr;
	p_btn_recent_chat = nullptr;
	p_btn_emo_chat = nullptr;
	p_list_recent_chat = nullptr;
	p_list_emoticon_chat = nullptr;
	p_text_input_chat = nullptr;


	p_max_player = 0;
	p_list_pos_dec.clear();
	SAFE_DELETE(p_thread_parse_ui);
	SAFE_DELETE(p_sample_dec);
	SAFE_DELETE_VECTOR(p_list_player_waiting);
	static_cast<ListViewTableWidget*>(p_list_player_invite)->ResetListView();
	p_list_avatar_download.clear();
	SAFE_DELETE(_chat_board);
	SAFE_DELETE_VECTOR(p_gift_categories);
	SAFE_DELETE_VECTOR(p_list_items);

	p_current_id_items_select = -1;

	p_panel_custom->ClearChild();
}

int HUDScreen::Init()
{
	this->_func_action = [&](LayerEntity * layer) {
		layer->OverloadRegisterUIWidget<IwinListViewTable>(xml::WIDGET_LIST_VIEW_TABLE);
		layer->OverloadRegisterUIWidget<IwinTextFieldHandle>(xml::WIDGET_TEXT_FIELD);
		layer->OverloadRegisterUIWidget<IwinEditBoxdHandle>(xml::WIDGET_EDIT_BOX);
	};

	XMLMgr->OnLoadXMLData1("UIWidget_table",[&](TiXmlDocument * objectXMl){XMLMgr->OnLoadUIWidgetDecTableXML(objectXMl, "common");});

	RKString _menu = "hud_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);
	_recent_chat.clear();

	return 1;
}

int	HUDScreen::InitComponent()
{
	p_hud_layout = GetWidgetChildByName("HUD_screen.hud_screen_layout");

	p_panel_send_gift = GetWidgetChildByName("HUD_screen.gift_player_layout.gift_player");
	p_panel_igm = GetWidgetChildByName("HUD_screen.panel_in_game_menu");
	p_panel_ig_setting = GetWidgetChildByName("HUD_screen.panel_in_game_setting");
	p_list_player_layout = GetWidgetChildByName("HUD_screen.layout_list_user");
	p_panel_chat_root = GetWidgetChildByName("HUD_screen.layout_chat");
	p_btn_recent_chat = p_panel_chat_root->GetWidgetChildByName(".panel_chat.btn_recent_chat");
	p_btn_emo_chat = p_panel_chat_root->GetWidgetChildByName(".panel_chat.btn_emotion");
	p_btn_recent_chat->GetWidgetChildByName(".icon")->Visible(true);
	p_list_recent_chat = p_panel_chat_root->GetWidgetChildByName(".panel_chat.list_btn_recent_chat");
	p_list_emoticon_chat = p_panel_chat_root->GetWidgetChildByName(".panel_chat.list_btn_emoticon_chat");
	p_text_input_chat = p_panel_chat_root->GetWidgetChildByName(".panel_chat.panel_input_chat.text_input_chat");

	p_panel_custom = GetWidgetChildByName("HUD_screen.panel_custom_menu");

	p_panel_game_screen = GetWidgetChildByName("HUD_screen.panel_game_screen");
	p_text_min_bet = p_panel_game_screen->GetWidgetChildByName(".panel_detail.min_bet");
	p_text_gamename = p_panel_game_screen->GetWidgetChildByName(".panel_detail.game_name");
	p_text_other1 = p_panel_game_screen->GetWidgetChildByName(".panel_detail.other_text");

	p_layout_invite_player = GetWidgetChildByName("HUD_screen.invite_play_layout");
	p_list_player_invite = p_layout_invite_player->GetWidgetChildByName(".list_player_invite");
	p_top_title_invite = p_layout_invite_player->GetWidgetChildByName(".top_title");
	
	p_btn_ingame_menu   = p_hud_layout->GetWidgetChildByName(".btn_in_game_menu");
	p_btn_purchase_iwin = p_hud_layout->GetWidgetChildByName(".btn_purchase_win");
	p_btn_purchase_ruby = p_hud_layout->GetWidgetChildByName(".btn_purchase_ruby");
	p_text_name			= p_hud_layout->GetWidgetChildByName(".name_title");
	btn_quick_play		= p_hud_layout->GetWidgetChildByName(".btn_quick_play");
	p_text_wait			= p_hud_layout->GetWidgetChildByName(".txt_wait_player");
	p_layout_gift_send_to = p_hud_layout->GetWidgetChildByName(".layout_gift_send");
	if (p_list_player_invite)
	{
		auto list_player_lb = static_cast<IwinListViewTable*>(p_list_player_invite);
		list_player_lb->InitWithParam(
			p_layout_invite_player->GetWidgetChildByName(".layout_pull_update"),
			nullptr,
			[this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
			{
				int current_idx_to = list_player_lb->GetCurrentIdxTo();
				OnParsePlayerToInviteList(_widget, current_idx_to);
			},
			[this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
			{
				size_t current_idx = list_player_lb->GetCurrentIdxSelected();
				if (current_idx >= 0 && current_idx < p_list_player_waiting.size())
				{
					auto p = p_list_player_waiting[current_idx];
					p->is_check = !p->is_check;

					_widget->GetWidgetChildByName(".on_check")->Visible(p->is_check);
					_widget->GetWidgetChildByName(".off_check")->Visible(!p->is_check);

					ModAllCheck(p->is_check);
				}
				else
				{
						PASSERT2(false, "have problem here!");
				}
			},
			[this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
			{
				GameService::requestWaitingListInGame();
			},
			nullptr);
	}


	p_list_lang_choose = p_panel_ig_setting->GetWidgetChildByName(".list_options.language.list_choose_lang");

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
							current_text, true
						);
					ScrMgr->SaveDataInt(STR_SAVE_LANGUAGE, lang_select);
				}
				OnShowListChooseLang(false);

			}
		});
	_list_choose_lang->InitTable();
	_list_choose_lang->SetNumberSizeOfCell(LangMgr->GetNumberLang());

	//////////////

	this->PushEvent([](BaseScreen* scr) {
		HUDScreen * hud_scr = (HUDScreen*)scr;
		hud_scr->LoadTheRecentChat();
		hud_scr->_idx_load_btn = 0;
		hud_scr->LoadFunctionLoop([](BaseScreen* scr) ->bool
		{
			HUDScreen * hud_scr = (HUDScreen*)scr;
			RKString text_chat = "";
			int idx = hud_scr->_idx_load_btn;
			if (idx < hud_scr->_recent_chat.size())
			{
				text_chat = hud_scr->_recent_chat[idx];
			}
			else
			{
				text_chat = list_chat[idx - hud_scr->_recent_chat.size()];
			}
			hud_scr->OnCreateBtnRecentChat(text_chat);
			idx++;
			if (idx >= DEF_CHAT + hud_scr->_recent_chat.size() || idx >= MAX_CHAT)
			{
				return false;
			}
			hud_scr->_idx_load_btn = idx;
			return true;
		}, 0.25);	
	});


	////
	p_panel_count_down = GetWidgetChildByName("HUD_screen.countdown_timer");

	p_panel_count_down->SetZOrder(1000);

	auto p_scrollview_chat = p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat.scrollview_chat");
	auto p_scrollitem_chat = p_panel_chat_root->GetWidgetChildByName(".chat_text");
	_chat_board = new ChatBoard();
	_chat_board->init((ListViewWidget*)(p_scrollview_chat), p_scrollitem_chat);

	p_layout_title_gift = p_panel_send_gift->GetWidgetChildByName(".title_layout");
	p_layout_list_gift = p_panel_send_gift->GetWidgetChildByName(".items_layout");

	p_bg_table_win = p_panel_game_screen->GetWidgetChildByName(".bg_table_win");
	p_bg_table_rubby = p_panel_game_screen->GetWidgetChildByName(".bg_table_ruby");

	p_layout_effect_rank = GetWidgetChildByName("HUD_screen.layout_list_user_rank");

	InitTextFieldValue();
	return 1;
}

void HUDScreen::InitTextFieldValue()
{
	auto _widget = p_text_input_chat;
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_chat_btn"));
	w_tx->InitCallBack(
		nullptr);
}

int HUDScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}


	if (p_on_count_down)
	{
		p_timer_count_down -= dt;
		if (p_timer_count_down <= 0)
		{
			p_timer_count_down = 0;
			p_on_count_down = false;
			p_panel_count_down->SetVisible(false);
		}
		static_cast<TextWidget*>(p_panel_count_down->GetWidgetChildByName(".timer_txt"))->SetText(std::to_string((int)p_timer_count_down));
	}

	//update player group
	for (auto p : p_list_player_group)
	{
		p->Update(dt);
	}

	static_cast<IwinListViewTable*>(p_list_player_invite)->UpdateReload(dt);

	return 1;
}

void HUDScreen::OnTouchMenuBegin(const cocos2d::Point & p)
{
	if (p_panel_igm->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_igm,
			[&](void)
		{
			p_panel_igm->SetActionCommandWithName("ON_HIDE");
		});
	}
	else if (p_panel_chat_root->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_chat_root->GetWidgetChildByName(".panel_chat"),
			[&](void)
		{
			CheckTouchOutSideWidget(p, p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat"),
				[&](void)
			{
				p_panel_chat_root->GetWidgetChildByName(".panel_chat")->SetActionCommandWithName("ON_HIDE", CallFunc::create(
					[this]()
				{
					this->p_panel_chat_root->GetWidgetChildByName(".panel_chat")->ForceFinishAction();
					this->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->SetActionCommandWithName("ON_HIDE", CallFunc::create([this] {
						this->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->ForceFinishAction();
						this->p_panel_chat_root->SetActionCommandWithName("ON_HIDE");
					}));
				}));
			});
		});
	}
	else if (p_layout_invite_player->Visible())
	{
		CheckTouchOutSideWidget(p, p_layout_invite_player,
			[&](void)
		{
			p_layout_invite_player->SetActionCommandWithName("ON_HIDE");
		});
	}
	else if (p_panel_ig_setting->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_ig_setting,
			[&](void)
		{
			this->p_panel_ig_setting->SetActionCommandWithName("ON_HIDE");
		});
	}
	else if (p_list_lang_choose->Visible())
	{
		CheckTouchOutSideWidget(p, p_list_lang_choose,
			[&](void) {
			OnShowListChooseLang(false);
		});

	}
}

void HUDScreen::OnShowListChooseLang(bool visible)
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

void HUDScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_TIME_PROCESS)
	{
		if (_widget->GetParentWidget()->GetName() == "layout_timer")
		{
			_widget->GetParentWidget()->Visible(false);
		}
		else if (_widget->GetParentWidget()->GetName() == "countdown_timer")
		{
			_widget->GetParentWidget()->Visible(false);
			p_on_count_down = false;
		}
	}
	else if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_in_game_menu")
		{
			if (!p_panel_igm->Visible())
			{
				p_panel_igm->SetActionCommandWithName("ON_SHOW");
			}
			else
			{
				p_panel_igm->SetActionCommandWithName("ON_HIDE");
			}
		}
		else if (name == "guide_ui_btn")
		{
			GetGuideService()->ShowHelpGameType();
			p_panel_igm->SetActionCommandWithName("ON_HIDE");
		}
		else if (name == "btn_send_message")
		{
			auto text_chat_widget = _widget->GetParentWidget()->GetWidgetChildByName(".panel_input_chat.text_input_chat");
			RKString str_chat = static_cast<TextFieldWidget*>(text_chat_widget)->GetText();
			if (str_chat == "")
				return;

			this->PushEvent([str_chat](BaseScreen * scr) {
				HUDScreen * hud_scr = (HUDScreen *)scr;
				bool have_press_def = false;
				for (int i = 0; i < DEF_CHAT; i++)
				{
					if (list_chat[i] == str_chat.ToUnicode())
					{
						have_press_def = true;
						break;
					}
				}

				if (!have_press_def)
				{
					hud_scr->SaveTheRecentChat(str_chat);
				}
			});

			SendChatToScreen(str_chat);
		}
		else if (name == "btn_recent_chat")
		{
			p_btn_recent_chat->GetWidgetChildByName(".icon")->Visible(true);
			p_btn_emo_chat->GetWidgetChildByName(".icon")->Visible(false);
			p_list_emoticon_chat->Visible(false);
			p_list_recent_chat->Visible(true);
		}
		else if (name == "btn_emotion")
		{
			p_btn_recent_chat->GetWidgetChildByName(".icon")->Visible(false);
			p_btn_emo_chat->GetWidgetChildByName(".icon")->Visible(true);
			p_list_emoticon_chat->Visible(true);
			p_list_recent_chat->Visible(false);
		}
		else if (name == "btn_chat")
		{
			if (!p_panel_chat_root->Visible())
			{
				p_panel_chat_root->SetActionCommandWithName("ON_SHOW", CallFunc::create(
					[this]()
				{
					p_panel_chat_root->ForceFinishAction();

					this->PushEvent([](BaseScreen* scr) {
						HUDScreen * game_scr = (HUDScreen *)scr;
						game_scr->p_panel_chat_root->GetWidgetChildByName(".panel_chat")->SetActionCommandWithName("ON_SHOW", CallFunc::create([game_scr]() {
							game_scr->p_panel_chat_root->GetWidgetChildByName(".panel_chat")->ForceFinishAction();
							game_scr->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->SetActionCommandWithName("ON_SHOW");
						}));
					});


				}));
			}			
		}
		else if ((name == "btn_purchase_win") || (name == "btn_purchase_ruby"))
		{
			ScrMgr->SwitchToMenu(IAP_SCREEN, MENU_NONE);
		}
		else if (name.Contains("btn_add_user_"))
		{
			HandleButtonAddUser();
		}
		else if (name == "btn_check_invite_all_friend")
		{
			auto parent = _widget->GetParentWidget();
			if (parent->GetWidgetChildByName(".on_check")->Visible() == true)
			{
				parent->GetWidgetChildByName(".on_check")->Visible(false);
				parent->GetWidgetChildByName(".off_check")->Visible(true);
				CheckAllWaitingList(false);
			}
			else
			{
				parent->GetWidgetChildByName(".on_check")->Visible(true);
				parent->GetWidgetChildByName(".off_check")->Visible(false);
				CheckAllWaitingList(true);
			}
		}
		else if (name == "invite_btn")
		{
			p_layout_invite_player->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_layout_invite_player->ForceFinishAction();

				for (auto p : p_list_player_waiting)
				{
					if (p->is_check)
					{
						GameService::requestInvite(p->player->IDDB);
					}
				}

			}));
		}
		else if (name == "setting_ui_btn")
		{
			p_panel_igm->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_panel_igm->ForceFinishAction();
				this->p_panel_ig_setting->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
					this->p_panel_ig_setting->ForceFinishAction();
					ScrMgr->SetScreenIDDirty();

					//
					float current_sound_volume = GetSound->GetVolume();
					static_cast<SliderWidget*>(p_panel_ig_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
						".sound_volume.slider_sound"))->SetPercent(current_sound_volume * 100.f);

					float current_music_volume = GetSound->GetVolume(false);
					static_cast<SliderWidget*>(p_panel_ig_setting->GetWidgetChildAtIdx(0)->GetWidgetChildByName(
						".music_volume.slider_music"))->SetPercent(current_music_volume * 100.f);
					//
				}));
			}));
		}
		else if (name == "chat_ui_btn")
		{
			p_panel_igm->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_panel_igm->ForceFinishAction();
				ScrMgr->SwitchToMenu(FRIEND_SCREEN, MENU_NONE);
				this->setVisible(false);
			}));
		}
		else if (name == "btn_input_lang")
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


	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		auto w_tx = static_cast<IWIN_TF*>(_widget);
		w_tx->HandleEventTextField(name);
	}
	else if (type_widget == UI_TYPE::UI_SLIDER)
	{
		float value = static_cast<SliderWidget*>(_widget)->GetPercent();
		if (name == "slider_sound_SLIDEBALL_UP")
		{
			float value1 = value / 100.f;
			GetSound->SetVolume(value1);
			ScrMgr->SaveDataFloat(STR_SAVE_SOUND_VOLUME, value);
			ScrMgr->SaveTheGame();
		}
		else if (name == "slider_music_SLIDEBALL_UP")
		{
			float value1 = value / 100.f;
			GetSound->SetVolume(value1, false);
			ScrMgr->SaveDataFloat(STR_SAVE_MUSIC_VOLUME, value);
			ScrMgr->SaveTheGame();
		}
	}
	else if (type_widget == UI_TYPE::UI_CHECK_BOX)
	{
		auto sep = name.SplitLast("_");
		RKString state = sep.GetAt(1);
		RKString _name = sep.GetAt(0);
		RKString state_name_save = "";		
		if (_name == "check_box_brivate")
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
		ScrMgr->SaveDataInt(state_name_save, state == "SELECTED" ? 1 : 0);
		ScrMgr->SaveTheGame();
	}
}

void HUDScreen::OnDeactiveCurrentMenu()
{
	//GetWidgetChildByName("HUD_screen.panel_game_screen")->GetResource()->removeAllChildrenWithCleanup(true);

	ScrMgr->CloseCurrentMenu(HUD_SCREEN);
	SAFE_DELETE_MAP(p_list_pos_dec)
	SAFE_DELETE_VECTOR(p_list_player_group)
	
	static_cast<ListViewWidget*>(p_list_recent_chat)->ClearChild();
	static_cast<ListViewWidget*>(p_list_emoticon_chat)->ClearChild();
	p_list_player_layout->ClearChild();

	p_menu_show_next = MENU_NONE;

	ScrMgr->DeinitGameScreen();
}

void HUDScreen::start_thread_parse_ui()
{
	SAFE_DELETE(p_thread_parse_ui);
	p_thread_parse_ui = new ThreadWrapper();
	p_thread_parse_ui->CreateThreadWrapper("thread_parse_ui",
		([](void * data) -> uint32
		{
			HUDScreen * scr = (HUDScreen*)data;
			scr->p_thread_parse_ui->OnCheckUpdateThreadWrapper
			([scr]()
			{
				XMLMgr->OnLoadXMLData<HUDScreen>("ListPlayer", std::mem_fn(&HUDScreen::LoadXMLEmotion), scr);
			});
			return 1;
		}),(void*)this);
}

void HUDScreen::OnBeginFadeIn()
{
	if (_chat_board)
	{
		_chat_board->reset();
	}
}

PlayerGroup * HUDScreen::GetPlayerGroupAtIdx(int idx)
{
	if (p_list_player_group.size() <= 0 || (size_t)idx >= p_list_player_group.size())
		return nullptr;
	return p_list_player_group.at(idx);
}

PlayerGroup * HUDScreen::getPlayerGroup(int playerID)
{
	for (PlayerGroup* p : p_list_player_group)
	{
		if (p != nullptr && p->getPlayer() != nullptr && p->getPlayer()->IDDB == playerID)
		{
			return p;
		}
	}
	return nullptr;
}

void HUDScreen::PlayMusic()
{
	PLAY_MUSIC_SE;
}

void HUDScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
	start_thread_parse_ui();
}

void HUDScreen::OnShow()
{
	Screen::OnShow();

	GameType current_gameplay = GameController::getCurrentGameType();

	int max_player = GameController::getMaxPlayerOf(current_gameplay);
}

void HUDScreen::OnKeyBack()
{
	if (p_panel_count_down->Visible())
		return;

	if (p_panel_chat_root->Visible())
	{
		p_panel_chat_root->GetWidgetChildByName(".panel_chat")->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]()
			{
				this->p_panel_chat_root->GetWidgetChildByName(".panel_chat")->ForceFinishAction();
				this->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->SetActionCommandWithName("ON_HIDE", CallFunc::create([this] {
					this->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->ForceFinishAction();
					this->p_panel_chat_root->SetActionCommandWithName("ON_HIDE");
				}));
			}));
	}
	else if (p_panel_send_gift->GetParentWidget()->Visible())
	{
		p_panel_send_gift->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
			p_panel_send_gift->ForceFinishAction();
			p_panel_send_gift->GetParentWidget()->Visible();
		}));
	}
	else if (p_layout_invite_player->Visible())
	{
		p_layout_invite_player->SetActionCommandWithName("ON_HIDE");
	}
	else if (p_panel_ig_setting->Visible())
	{
		p_panel_ig_setting->SetActionCommandWithName("ON_HIDE");
	}
	else if (p_panel_igm->Visible())
	{
		p_panel_igm->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
			p_panel_igm->ForceFinishAction();
			this->DoBack();
		}));
	}
	else
	{
		DoBack();
	}
}

void HUDScreen::SetMaxPlayer(int max_player)
{
	p_max_player = max_player;
}

void HUDScreen::showGameScreen(s16 roomID, s16 boardID, int ownerID, int money, std::vector<Player*> players)
{
	GameController::showGameScreen(roomID, boardID, ownerID, money, players);
}

void HUDScreen::OnHideCountDown() {

	p_on_count_down = false;
	p_timer_count_down = 0.f;
	p_panel_count_down->Visible(false);
	static_cast<TimeProcessWidget*>(p_panel_count_down->GetWidgetChildByName(".progress_1"))->StopTimeProcess();
	static_cast<TimeProcessWidget*>(p_panel_count_down->GetWidgetChildByName(".progress_2"))->StopTimeProcess();


}

void HUDScreen::OnShowCountDown(float time, float cur_time)
{
	if (p_on_count_down)
	{
		return;
	}

	p_timer_count_down = cur_time;
	if (p_timer_count_down <= 0)
	{
		p_timer_count_down = time - cur_time;
	}

	p_on_count_down = true;

	p_panel_count_down->Visible(true);
	static_cast<TimeProcessWidget*>(p_panel_count_down->GetWidgetChildByName(".progress_1"))->SetTimer(time,cur_time);
	static_cast<TimeProcessWidget*>(p_panel_count_down->GetWidgetChildByName(".progress_2"))->SetTimer(time,cur_time);
}

void HUDScreen::LoadFunctionLoop(const std::function<bool(BaseScreen * scr)> & func, float delta_time)
{
	this->PushEvent([func , delta_time](BaseScreen* scr) {
		bool res = func(scr);
		if (res)
		{
			HUDScreen * new_scr = (HUDScreen *)scr;
			new_scr->LoadFunctionLoop(func, delta_time);
		}
	}, delta_time);
}

void HUDScreen::LoadXMLEmotion(TiXmlDocument * p_objectXML)
{
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return;
	}

	auto sample_ui_dec = Root->FirstChildElement("list_widget");
	if (!sample_ui_dec)
	{
		PASSERT2(false, "XML loader fail to load!");
		return;
	}

	auto sample_ui_dec_xml = sample_ui_dec->FirstChildElement("Widget");
	do
	{
		if (sample_ui_dec_xml)
		{
			RKString name = sample_ui_dec_xml->Attribute("name");
			if (name == "btn_emotion_")
			{
				auto _widget_root = XMLMgr->OnParseWidgetEntity1(sample_ui_dec_xml, nullptr, XMLMgr->GetUIWidgetByName(name_ui), nullptr);
				p_sample_dec = _widget_root;
				break;
			}
			sample_ui_dec_xml = sample_ui_dec_xml->NextSiblingElement();
		}
	} while (sample_ui_dec_xml);

	_idx_load_emo = 0;
	this->LoadFunctionLoop([](BaseScreen* scr) ->bool
	{
		HUDScreen * hud_scr = (HUDScreen*)scr;

		hud_scr->OnCreateBtnEmoticonChat(hud_scr->_idx_load_emo + 1, hud_scr->p_sample_dec->Clone());
		hud_scr->_idx_load_emo++;
		if (hud_scr->_idx_load_emo >= 59)
		{
			return false;
		}
		return true;

	}, 0.25);

	//CC_SAFE_DELETE(p_sample_dec);
}

void HUDScreen::OnSetTextBtn(WidgetEntity * w_text, RKString text)
{
	auto current_text = static_cast<TextWidget*>(w_text);

	current_text->SetText(text, true);

	int number_line = current_text->GetNumberOfLine();
	if (number_line > 1)
	{
		auto text_at_line = current_text->GetStringAtLine(1);
		text_at_line.replace(text_at_line.size() - 3, 3, "...");
		current_text->SetText(text_at_line, true);
	}
}

void HUDScreen::OnRefreshTextChatBtn()
{
	auto _list_chat = static_cast<ListViewWidget*>(p_list_recent_chat);

	std::vector<RKString> chat_list;

	for (int i = 0; i < MAX_CHAT ; i++)
	{
		if (i < _recent_chat.size())
		{
			chat_list.push_back(_recent_chat[i]);
		}
		else if (i >= _recent_chat.size() && i < _recent_chat.size() + DEF_CHAT)
		{
			chat_list.push_back(list_chat[i - _recent_chat.size()]);
		}
		else
		{
			break;
		}
	}

	int idx = 0;
	for (int i = 0; i < _list_chat->GetNumberChildWidget() ; i++)
	{
		auto tex0 = chat_list[i* 2];
		auto tex1 = chat_list[i* 2 + 1];

		auto widget_i = _list_chat->GetWidgetChildAtIdx(i);

		auto btn0 = widget_i->GetWidgetChildByName(".btn_chat_0");
		static_cast<ButtonWidget*>(btn0)->SetActionBtn(
			[tex0, this]()
		{
				this->HandleChatText(tex0);
		});
		OnSetTextBtn(btn0->GetWidgetChildAtIdx(0), tex0);
		idx++;

		auto btn1 = widget_i->GetWidgetChildByName(".btn_chat_1");
		if (btn1->Visible())
		{
			static_cast<ButtonWidget*>(btn1)->SetActionBtn(
				[tex1, this]()
			{
				this->HandleChatText(tex1);
			});
			OnSetTextBtn(btn1->GetWidgetChildAtIdx(0), tex1);
			idx++;
		}
	}

	if (idx < chat_list.size() - 1)
	{
		int delta = chat_list.size() - 1 - idx;
		for (int i = idx; i < chat_list.size(); i++)
		{
			OnCreateBtnRecentChat(chat_list[i]);
		}
	}
}

void HUDScreen::OnCreateBtnRecentChat(RKString text)
{
	auto list_chat = static_cast<ListViewWidget*>(p_list_recent_chat);

	int current_layout = (int)(list_chat->GetResource()->getChildrenCount());
	
	if (current_layout == 0)
	{
		auto _widget_layout = list_chat->CreateWidgetRefer(current_layout);

		static_cast<ButtonWidget*>(_widget_layout->GetWidgetChildByName(".btn_chat_0"))->SetActionBtn(
			[text, this]()
		{
				this->HandleChatText(text);
		});

		auto current_text = _widget_layout->GetWidgetChildByName(".btn_chat_0.notice_content");
		OnSetTextBtn(current_text, text);
		

	}
	else
	{
		auto _widget_layout = list_chat->GetWidgetChildAtIdx(current_layout - 1);
		auto current_text_1 = _widget_layout->GetWidgetChildByName(".btn_chat_0.notice_content");
		auto current_text_2 = _widget_layout->GetWidgetChildByName(".btn_chat_1.notice_content");

		RKString cur_tex_1 = static_cast<TextWidget*>(current_text_1)->GetText();
		RKString cur_tex_2 = static_cast<TextWidget*>(current_text_2)->GetText();
		if (cur_tex_2 == "test_WIP" && cur_tex_1 != "test_WIP")
		{
			OnSetTextBtn(current_text_2, text);
			static_cast<ButtonWidget*>(_widget_layout->GetWidgetChildByName(".btn_chat_1"))->SetActionBtn(
				[text, this]()
			{
					this->HandleChatText(text);
			});
			_widget_layout->GetWidgetChildByName(".btn_chat_1")->Visible(true);
		}
		else if (cur_tex_2 != "test_WIP" && cur_tex_1 != "test_WIP")
		{
			auto _widget_layout = list_chat->CreateWidgetRefer(current_layout);
			auto current_text = _widget_layout->GetWidgetChildByName(".btn_chat_0.notice_content");
			OnSetTextBtn(current_text, text);

			static_cast<ButtonWidget*>(_widget_layout->GetWidgetChildByName(".btn_chat_0"))->SetActionBtn(
				[text, this]()
			{
					this->HandleChatText(text);
			});
			_widget_layout->GetWidgetChildByName(".btn_chat_1")->Visible(false);

		}
		else
		{
			PASSERT2(false, "this case is not handle!");
		}
	}
}

void HUDScreen::OnCreateBtnEmoticonChat(int idx , xml::UILayerWidgetDec* xml_dec)
{
	auto menu_widget_xml = XMLMgr->GetUIWidgetByName(name_ui);
	xml_dec->p_menu_widget = menu_widget_xml;

	xml_dec->p_parent_layer = menu_widget_xml->GetElementChildByName("HUD_screen"
	)->GetChildWidgetByName("layout_chat")->GetChildWidgetByName("panel_chat"
	)->GetChildWidgetByName("list_btn_emoticon_chat")->GetChildWidgetByName("layout_emoticon_chat_");

	auto layer_root = GetLayerChildByName("HUD_screen");

	auto list_emotion = static_cast<ListViewWidget*>(p_list_emoticon_chat);
	int current_layout = (int)(list_emotion->GetResource()->getChildrenCount());

	WidgetEntity * layout_list_emo = nullptr;

	int cur_index = 0;
	if (current_layout == 0)
	{
		layout_list_emo = list_emotion->CreateWidgetRefer(current_layout);
		cur_index = 0;
	}
	else
	{
		auto layout_ = list_emotion->GetWidgetChildAtIdx(current_layout - 1);
		int number_child = (int)(layout_->GetResource()->getChildrenCount());
		if (number_child < 10)
		{
			layout_list_emo = layout_;
			cur_index = number_child;
		}
		else
		{
			layout_list_emo = list_emotion->CreateWidgetRefer(current_layout);
			cur_index = 0;
		}
	}
	PASSERT2(layout_list_emo != nullptr, "have problem in get list emo layout !");
	
	float pos_x = 45 * (cur_index + 1) + 44 * cur_index;
	float pos_y = 33;
	Vec2 design_size = menu_widget_xml->DesignSize;
	xml_dec->OriginValue->ReplaceDataVector2("pos_ratio", Vec2(pos_x / design_size.x, pos_y / design_size.y));
	xml_dec->FirstValue->ReplaceDataVector2("pos_ratio", Vec2(pos_x / design_size.x, pos_y / design_size.y));

	xml_dec->NameWidget = xml_dec->NameWidget + std::to_string(idx);
	xml_dec->GeneralValue->ReplaceDataChar("source",  ("loadingscreen/emotions/e (" + std::to_string(idx) + ").png").c_str());
	xml_dec->GeneralValue->ReplaceDataChar("press",   ("loadingscreen/emotions/e (" + std::to_string(idx) + ").png").c_str());
	xml_dec->GeneralValue->ReplaceDataChar("disable", ("loadingscreen/emotions/e (" + std::to_string(idx) + ").png").c_str());


	auto widget_emo = layer_root->InitWidgetEntity(xml_dec, layer_root, layout_list_emo);
	layout_list_emo->ResizeAndPosChildWidget(widget_emo);

	static_cast<ButtonWidget*>(widget_emo)->SetActionBtn(
		[idx , this]() 
	{

			this->HandleChatText(Utils::getEmotionStr(idx) , false);
	});

	widget_emo->SetMenuParent(layout_list_emo->GetMenuParent());
	widget_emo->SetParentWidget(layout_list_emo);
	widget_emo->SetParentLayer(layout_list_emo->GetParentLayer());
	layout_list_emo->InsertChildWidget(widget_emo);

	SAFE_DELETE(xml_dec);
}

void HUDScreen::SaveTheRecentChat(RKString str_chat)
{
	_recent_chat.insert(_recent_chat.begin(),str_chat);
	if (_recent_chat.size() > MAX_CHAT)
	{
		_recent_chat.erase(_recent_chat.begin() + _recent_chat.size() - 1);
	}

	OnRefreshTextChatBtn();
	//save the chat into file
	RKString text_save = "";
	for (int i = 0 ; i < _recent_chat.size() ; i ++)
	{
		text_save += _recent_chat[i];
		if (i < _recent_chat.size() - 1)
		{
			text_save += "<--!-->";
		}
	}

	ScrMgr->SaveDataString(STR_SAVE_RECENT_CHAT, text_save);
	ScrMgr->SaveTheGame();
}

void HUDScreen::LoadTheRecentChat()
{
	RKString text_save = "";
	bool res = ScrMgr->GetDataSaveString(STR_SAVE_RECENT_CHAT, text_save);
	if (res)
	{
		auto all_text_chat = text_save.Split("<--!-->");
		for (int i = 0; i < all_text_chat.Size(); i++)
		{
			_recent_chat.push_back(all_text_chat.GetAt(i));
		}
	}
}

void HUDScreen::onSetData(void* data)
{
	User * user = (User*)data;

	for (size_t i = 0; i < p_list_player_group.size(); i++)
	{
		if(p_list_player_group[i]->GetIDDB() == user->id)
		{
			p_list_player_group[i]->OnSetData(user);
			return;
		}
	}
}


void HUDScreen::onReceiveAvatarCatList(void * data)
{
	AvatarCatList * value = (AvatarCatList *)data;
	int ttt = 0;
	ttt++;
}

void HUDScreen::onReceiveAvatarList(void * data)
{
	AvatarList * value = (AvatarList *)data;
	int ttt = 0;
	ttt++;
}

void HUDScreen::onBuyingAvatarResult(void * data)
{
	BuyingAvatar * value = (BuyingAvatar *)data;
	int ttt = 0;
	ttt++;
}

void HUDScreen::OnParsePlayerToInviteList(WidgetEntity * widget, int idx, bool isPlayer/* = true*/)
{
	if (idx < 0 || idx >= p_list_player_waiting.size())
		return;
	Player * player = p_list_player_waiting[idx]->player;

	if (!player)
	{
		PASSERT2(false, "player is null");
		return;
	}

	if (idx % 2 == 0)
	{
		widget->GetWidgetChildByName(".hl_panel")->Visible(true);
	}
	else
	{
		widget->GetWidgetChildByName(".hl_panel")->Visible(false);
	}

	static_cast<TextWidget*>(widget->GetWidgetChildByName(".title_id"))->SetText(player->name, true);
	static_cast<TextWidget*>(widget->GetWidgetChildByName(".number_win_id"))->SetText(Utils::formatNumber_win(player->money), true);

	//static_cast<TextWidget*>(widget->GetWidgetChildByName(".number_ruby_id"))->SetText(Utils::formatNumber_ruby(player->ruby), true);

	widget->GetWidgetChildByName(".on_check")->Visible(p_list_player_waiting[idx]->is_check);
	widget->GetWidgetChildByName(".off_check")->Visible(!p_list_player_waiting[idx]->is_check);

	auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");
	if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(player->IDDB) != p_list_avatar_download.end())
	{
		this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(player->IDDB), 100);
	}
}

void HUDScreen::OnReceiveWaitingList(std::vector<Player*> l)
{
	SAFE_DELETE_VECTOR(p_list_player_waiting);
	for (auto p : l)
	{
		p_list_player_waiting.push_back(new PlayerWaiting(p));
	}

	p_list_player_invite->Visible(false);
	p_layout_invite_player->GetWidgetChildByName(".loading_circle")->Visible(true);
	p_layout_invite_player->GetWidgetChildByName(".txt_have_data")->Visible(false);
	p_layout_invite_player->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");
	static_cast<IwinListViewTable*>(p_list_player_invite)->SetNumberSizeOfCell(p_list_player_waiting.size(), true);

	static_cast<IwinListViewTable*>(p_list_player_invite)->SetHaveReload([this]() 
	{
		p_list_player_invite->Visible(true);
		p_layout_invite_player->GetWidgetChildByName(".loading_circle")->Visible(false);

		if (p_list_player_waiting.size() > 0)
		{
			p_layout_invite_player->GetWidgetChildByName(".txt_have_data")->Visible(false);
			//download avatar in save into the list
			for (size_t i = 0; i < p_list_player_waiting.size(); i++)
			{
				auto idx_player = p_list_player_waiting[i];
				GameMgr->HandleAvatarDownload(idx_player->player->avatar, idx_player->player->name,
						[this, i](void * data, void * str, int tag)
					{
						auto idx_player = p_list_player_waiting[i];
						Image * img = (Image *)data;
						if (this->p_list_player_waiting.size() > 0)
						{
							Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
							this->p_list_avatar_download.insert(idx_player->player->IDDB, sprite_img);

							auto widget_lb = static_cast<ListViewTableWidget*>(p_list_player_invite);
							if ((size_t)i < widget_lb->GetListWidgetSample().size())
							{
								widget_lb->UpdateCellAtIdx(i);
							}
						}
					});
			}
		}
		else
		{
			p_layout_invite_player->GetWidgetChildByName(".txt_have_data")->Visible(true);
		}
		//
	});

}

void HUDScreen::CheckAllWaitingList(bool ischeck)
{
	for (auto p : p_list_player_waiting)
	{
		p->is_check = ischeck;
	}
	auto list_widget = static_cast<ListViewTableWidget*>(p_list_player_invite)->GetListWidgetSample();

	for (auto widget : list_widget)
	{
		widget->GetWidgetChildByName(".on_check")->Visible(ischeck);
		widget->GetWidgetChildByName(".off_check")->Visible(!ischeck);
	}
}

bool HUDScreen::ModAllCheck(bool is_check)
{
	if (is_check)
	{
		bool have_un_check = false;
		for (auto p : p_list_player_waiting)
		{
			if (!p->is_check)
			{
				have_un_check = true;
				break;
			}
		}
		if (have_un_check)
		{
			p_top_title_invite->GetWidgetChildByName(".on_check")->Visible(false);
			p_top_title_invite->GetWidgetChildByName(".off_check")->Visible(true);
		}
		else
		{
			p_top_title_invite->GetWidgetChildByName(".off_check")->Visible(false);
			p_top_title_invite->GetWidgetChildByName(".on_check")->Visible(true);
		}
	}
	else
	{
		/*bool have_check = false;
		for (auto p : p_list_player_waiting)
		{
			if (p->is_check)
			{
				have_check = true;
				break;
			}
		}
		if (have_check)
		{
			p_top_title_invite->GetWidgetChildByName(".on_check")->Visible(true);
			p_top_title_invite->GetWidgetChildByName(".off_check")->Visible(false);
		}
		else
		{*/
			p_top_title_invite->GetWidgetChildByName(".off_check")->Visible(true);
			p_top_title_invite->GetWidgetChildByName(".on_check")->Visible(false);
		//}
	}

	return true;
}

void HUDScreen::PushTextChat(RKString name, RKString msg)
{
	_chat_board->addChat(name.GetString(), msg.GetString());
}

void HUDScreen::HandleChatText(RKString text, bool send_to_screen)
{
	if (send_to_screen)
	{
		SendChatToScreen(text);
	}
	else
	{

		RKString cur_text = static_cast<TextFieldWidget*>(p_text_input_chat)->GetText();
		cur_text += text;
		static_cast<TextFieldWidget*>(p_text_input_chat)->SetText(cur_text);
	}
}

void HUDScreen::SendChatToScreen(RKString text_chat)
{
	GetPlayerGroupAtIdx(0)->SetTextChat(text_chat);
	PushChatToServer(text_chat.GetString());
	p_panel_chat_root->GetWidgetChildByName(".panel_chat")->SetActionCommandWithName("ON_HIDE", CallFunc::create(
		[this]()
	{
		this->p_panel_chat_root->GetWidgetChildByName(".panel_chat")->ForceFinishAction();
		this->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->SetActionCommandWithName("ON_HIDE", CallFunc::create([this] {
			this->p_panel_chat_root->GetWidgetChildByName(".layout_gen_chat")->ForceFinishAction();
			this->p_panel_chat_root->SetActionCommandWithName("ON_HIDE");
		}));
	}
	));

	static_cast<TextFieldWidget*>(p_text_input_chat)->ClearText();
}

void HUDScreen::ShowListGiftAtIdx(int idx)
{
	for (int i = 0; i < p_layout_title_gift->GetNumberChildWidget(); i++)
	{
		auto wi = p_layout_title_gift->GetWidgetChildAtIdx(i);
		wi->GetWidgetChildAtIdx(1)->Visible(false);
		wi->GetWidgetChildAtIdx(2)->SetColor(Color3B(200,200,200));
	}

	if (idx >= 0 && idx < p_layout_title_gift->GetNumberChildWidget())
	{
		auto wi = p_layout_title_gift->GetWidgetChildAtIdx(idx);
		wi->GetWidgetChildAtIdx(1)->Visible(true);
		wi->GetWidgetChildAtIdx(2)->SetColor(Color3B(10, 10, 10));

		if (idx != p_current_gift_layout_idx)
		{
			GameService::requestListGift(p_gift_categories[idx]->categoryId, 0);
		}

		p_current_gift_layout_idx = idx;
	}
}

void HUDScreen::openGiftWithIDPlayer(int IDDB)
{
	if (IDDB < 0)
		return;
	GameService::getGift(8, (unsigned char)0, (unsigned char)0);

	this->p_panel_send_gift->GetParentWidget()->Visible(true);
	p_panel_send_gift->SetActionCommandWithName("ON_SHOW");

	p_current_iddb_player_gift = IDDB;
}

void HUDScreen::OnSelectItemGift(int id)
{
	auto list_item = (p_layout_list_gift->GetWidgetChildByName(".list_item"));

	for (int i = 0; i < list_item->GetNumberChildWidget(); i++)
	{
		auto w = list_item->GetWidgetChildAtIdx(i);
		if (p_list_items[i]->id == id)
		{
			w->GetWidgetChildByName(".bg_hl")->SetVisible(true);
			p_current_id_items_select = id;
		}
		else
		{
			w->GetWidgetChildByName(".bg_hl")->SetVisible(false);
		}
	}
}

void HUDScreen::setGiftCategories(std::vector<Category*> categories)
{
	SAFE_DELETE_VECTOR(p_gift_categories);
	p_gift_categories = categories;
	for (size_t i = 0; i < p_gift_categories.size(); i++)
	{
		if (i < p_layout_title_gift->GetNumberChildWidget())
		{
			auto wi = p_layout_title_gift->GetWidgetChildAtIdx(i);
			static_cast<TextWidget*>(wi->GetWidgetChildByName("._title"))->SetText(p_gift_categories[i]->name, true);
		}
	}
	ShowListGiftAtIdx(0);

	/*
	if (categories != null && categories.size > 0) {
			if (giftDialog == null) {
				giftDialog = new GiftDialog();
				giftDialog.sendGiftAction = new IActionListener() {

					@Override
					public void actionPerformed(Object o) {
						int type = (Integer) ((Object[]) o)[0];
						Item item = (Item) ((Object[]) o)[1];
						if (item != null) {
							Array<Integer> userIDs = new Array<Integer>();
							if (type == 0) {
								if (giftDialog.sendGiftuserID != -1)
									userIDs.add(giftDialog.sendGiftuserID);
							} else {
								for (int i = GameController.currentGameScreen.players.size; --i >= 0;) {
									userIDs.add(GameController.currentGameScreen.players.get(i).IDDB);
								}
							}
							if (userIDs.size > 0) {
								GameService.giveItem(GameController.currentGameScreen.roomID,
										GameController.currentGameScreen.boardID, item.id, userIDs);
							}
							giftDialog.remove();
						} else {
							DialogUtil.startOKDlg(L.gL(332));
						}
					}
				};
				giftDialog.setGiftCategorys(categories);
			} else {
				giftDialog.initForResuse();
			}
			giftDialog.sendGiftuserID = sendUserId;

			giftDialog.show(GameController.currentGameScreen);
		}
		// giftDialog.setGiftCategorys(categories);
	
	*/
}
void HUDScreen::setGiftItems(int categoryID, int totalpage, std::vector<Item*> items)
{
	SAFE_DELETE_VECTOR(p_list_items);
	p_list_items = items;

	auto list_item = static_cast<ScrollViewWidget*>(p_layout_list_gift->GetWidgetChildByName(".list_item"));
	auto w_item_temp = p_layout_list_gift->GetWidgetChildByName(".item");
	list_item->ClearChild();
	auto size_list = list_item->GetSize();
	auto size_item = w_item_temp->GetSize();
	float delta = 20 * GetGameSize().width / this->GetDesignSize().x;
	Vec2 idx = Vec2(1,1);
	for (auto i : p_list_items)
	{
		auto it = w_item_temp->Clone();
		it->Visible(true);
		Vec2 pos = Vec2(
			(idx.x * (delta + (size_item.x / 2))) + ((idx.x - 1)*(size_item.x / 2)),
			size_list.y - (
			(idx.y * (delta + (size_item.y / 2))) + 
				((idx.y - 1)*(size_item.y / 2)))
			);

		if (pos.x > size_list.x)
		{
			idx.x = 1;
			idx.y++;

			pos = Vec2(
				(idx.x * (delta + (size_item.x / 2))) + ((idx.x - 1)*(size_item.x / 2)),
				size_list.y - (
				(idx.y * (delta + (size_item.y / 2))) +
					((idx.y - 1)*(size_item.y / 2)))
			);
		}

		it->SetPosition(pos);
		int current_id = i->id;
		static_cast<TextWidget*>(it->GetWidgetChildByName(".title_name"))->SetText(i->name, true);
		static_cast<TextWidget*>(it->GetWidgetChildByName(".title_price"))->SetText(Utils::formatNumber(i->price), true);
		static_cast<ButtonWidget*>(it->GetWidgetChildByName(".btn_select"))->SetActionBtn([this, current_id]() 
		{
				OnSelectItemGift(current_id);	
		});
		auto widget_panel = it->GetWidgetChildByName(".img_layout");
		GameMgr->HandleAvatarDownload(i->id, i->name, [this , widget_panel, categoryID](void * data, void * str, int tag)
		{
			Image * img = (Image *)data;
			if (img && img->getWidth() <= 0)
			{
				CC_SAFE_DELETE(img);
				return;
			}
			if (p_current_gift_layout_idx >= 0 &&
				p_gift_categories.size() > 0 && 
				p_current_gift_layout_idx < p_gift_categories.size() &&
				p_gift_categories[p_current_gift_layout_idx]->categoryId == categoryID)
			{
				Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
				this->ParseSpriteToPanel(widget_panel, sprite_img, tag);
			}
		});

		list_item->InsertChildWidget(it);
		idx.x++;
	}

	/*
	giftDialog.setGiftItems(categoryID, totalpage, items);
	*/
}

void HUDScreen::sendGiftTo(int from, std::vector<int> tos, int itemID)
{
	//p_layout_gift_send_to->ClearChild();
	Vec2 StarPos(0, 0);
	auto pg = getPlayerGroup(from);
	if (pg == nullptr) {
		StarPos.x = p_panel_game_screen->GetPosition_Middle().x;
		StarPos.y = p_panel_game_screen->GetPosition_Middle().y;
	}
	else
	{
		auto pos_player = _WIDGET(pg)->GetPosition_Middle();
		auto w_pos = _WIDGET(pg)->GetResource()->getParent()->convertToWorldSpace(pos_player);
		StarPos = w_pos;
	}

	//std::vector<Vec2> list_end_pos;

	for (auto po : tos)
	{
		auto pl = getPlayerGroup(po);
		if (pl)
		{
			auto pos_player = _WIDGET(pl)->GetWidgetChildByName(".layout_player.btn_gift_user_")->GetPosition_Middle();
			auto w_pos = _WIDGET(pl)->GetWidgetChildByName(".layout_player")->GetResource()->convertToWorldSpace(pos_player);
			//list_end_pos.push_back(w_pos);

			GameMgr->HandleAvatarDownload(itemID, "", [this,StarPos, w_pos , pl](void * data, void * str, int tag)
			{
				Image * img = (Image *)data;
				if (img && img->getWidth() <= 0)
				{
					CC_SAFE_DELETE(img);
					return;
				}

				Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
				sprite_img->setTag(pl->GetIDDB());
				this->p_layout_gift_send_to->GetResource()->removeChildByTag(pl->GetIDDB());
				this->p_layout_gift_send_to->GetResource()->addChild(sprite_img);
				sprite_img->setPosition(StarPos);
				sprite_img->runAction(cocos2d::Sequence::create(MoveTo::create(1.f, w_pos), CallFunc::create([this , pl]() {
					_WIDGET(pl)->GetWidgetChildByName(".layout_player.btn_gift_user_")->SetOpacity(0);
				}),nullptr));

			});
		}
	}
}

int HUDScreen::GetStateHUDScreen()
{
	if (p_panel_ig_setting->IsVisible())
	{
		return 0;
	}
	return 1;
}

void HUDScreen::OnShowMessageFromServer(RKString msg)
{
	if (GameController::getCurrentGameType() == GameType::GAMEID_CO_UP || GameController::getCurrentGameType() == GameType::GAMEID_CHESS)
	{
		Utils::showDialogNotice(msg.GetString());
	}
	else
	{
		auto t = GetPlayerGroupAtIdx(0);
		if (t)
		{
			t->SetTextChat(msg);
		}
		else
		{
			ScrMgr->OnShowOKDialog(msg);
		}
	}
}

void HUDScreen::ShowRankNumberAtPos(bool value, int idx, Vec2 pos)
{
	if (idx < 0 || idx >= p_layout_effect_rank->GetNumberChildWidget())
		return;
	auto widget_rank = p_layout_effect_rank->GetWidgetChildAtIdx(idx);
	if (widget_rank)
	{
		widget_rank->SetVisible(value);
		if (value)
		{
			widget_rank->SetPosition(pos);
			static_cast<AnimWidget*>(widget_rank)->RunAnim();
		}	
	}
}

bool HUDScreen::IsTextChatVisible(int idx)
{
	auto p_layout_player_group_com = GetWidgetChildByName("HUD_screen.layout_list_user_com");
	if (idx < 0 || idx >= p_layout_player_group_com->GetNumberChildWidget())
		return false;

	auto widget_chat = p_layout_player_group_com->GetWidgetChildAtIdx(idx);
	if (widget_chat)
	{
		return widget_chat->GetWidgetChildByName(".panel_display_chat")->Visible();
	}
	return false;
}

void HUDScreen::SetVisiblePlayerCom(bool visible, int idx)
{
	auto p_layout_player_group_com = GetWidgetChildByName("HUD_screen.layout_list_user_com");
	if (idx < 0 || idx >= p_layout_player_group_com->GetNumberChildWidget())
		return;
	auto widget_chat = p_layout_player_group_com->GetWidgetChildAtIdx(idx);
	if (widget_chat)
	{
		widget_chat->Visible(visible);
	}
}

void HUDScreen::ShowTextChatAtIdx(bool value, int idx, RKString text, int state, const std::function<void(void)> &  call_back)
{
	auto p_layout_player_group_com = GetWidgetChildByName("HUD_screen.layout_list_user_com");
	if (idx < 0 || idx >= p_layout_player_group_com->GetNumberChildWidget())
		return;

	auto widget_chat = p_layout_player_group_com->GetWidgetChildAtIdx(idx);
	if (widget_chat)
	{
		auto panel_display_chat = widget_chat->GetWidgetChildByName(".panel_display_chat");
		auto text_chat = static_cast<TextWidget*>(panel_display_chat->GetWidgetChildByName(".text_chat"));
		text_chat->SetText(text, true);
		panel_display_chat->Visible(value);
		if (value)
		{

			Vec2 size_text = text_chat->GetActualContainSize();
			Vec2 ratio_scale = Vec2(GetGameSize().width / GetDesignSize().x, GetGameSize().height / GetDesignSize().y);
			Vec2 addition_scale = Vec2(10 * ratio_scale.x, 5 * ratio_scale.x);
			panel_display_chat->SetSize(Vec2(size_text.x + addition_scale.x, size_text.y + addition_scale.y));

			if (state == 2)
			{
				Vec2 size_text = text_chat->GetActualContainSize();
				xml::BasicDec dec;
				dec.InsertDataVector2("SET_POSITION", Vec2(5, -size_text.y));
				dec.InsertDataVector2("POSITION", Vec2(5, size_text.y));

				text_chat->SetHookCommand("ON_SHOW", "act0", 0, 2, dec, "SET_POSITION");
				text_chat->SetHookCommand("ON_SHOW", "act2", 0, 1, dec, "POSITION");

				text_chat->SetActionCommandWithName("ON_SHOW", CallFunc::create([text_chat , call_back]() {
					text_chat->ForceFinishAction();
					if (call_back)
					{
						call_back();
					}
				}));

				text_chat->ClearHookCommand("ON_SHOW", "act0", 0, 2);
				text_chat->ClearHookCommand("ON_SHOW", "act2", 0, 1);
			}
		}
	}
}

void HUDScreen::ShowTextBonusMoney(int idx, s64 bonus_money, Vec2 Pos)
{
	auto p_layout_player_group_com = GetWidgetChildByName("HUD_screen.layout_list_user_com");
	if (idx < 0 || idx >= p_layout_player_group_com->GetNumberChildWidget())
		return;

	auto _widget = p_layout_player_group_com->GetWidgetChildAtIdx(idx);
	if (!_widget)
	{
		return;
	}

	auto w_panel_text = _widget->GetWidgetChildByName(".layout_bonus");
	WidgetEntity * w_text = nullptr;
	if (bonus_money >= 0)
	{
		w_text = w_panel_text->GetWidgetChildByName(".bonus_iwin_add");
		w_panel_text->GetWidgetChildByName(".bonus_iwin_div")->Visible(false);
	}
	else
	{
		w_text = w_panel_text->GetWidgetChildByName(".bonus_iwin_div");
		w_panel_text->GetWidgetChildByName(".bonus_iwin_add")->Visible(false);
	}
	w_text->Visible(true);
	static_cast<TextWidget*>(w_text)->TextRunEffect(
		bonus_money, 0, 1.f, [bonus_money](s64 money)->std::string {
		auto text = Utils::formatNumber_dot(abs(money));
		if (bonus_money < 0)
		{
			text = "-" + text;
		}
		else
		{
			text = "+" + text;
		}
		return text;
	});
	xml::BasicDec dec;
	dec.InsertDataVector2("SET_POSITION", Pos);

	if (bonus_money < 0)
	{
		w_panel_text->SetHookCommand("ACTION_DIV", "act0", 0, 2, dec, "SET_POSITION");
		w_panel_text->SetActionCommandWithName("ACTION_DIV");
		w_panel_text->ClearHookCommand("ACTION_DIV", "act0", 0, 2);
	}
	else
	{
		w_panel_text->SetHookCommand("ACTION_ADD", "act0", 0, 2, dec, "SET_POSITION");
		w_panel_text->SetActionCommandWithName("ACTION_ADD");
		w_panel_text->ClearHookCommand("ACTION_ADD", "act0", 0, 2);
	}
}

void HUDScreen::DoBack()
{

}

void HUDScreen::HandleButtonAddUser()
{
	if (p_layout_invite_player->Visible())
	{
		p_layout_invite_player->SetActionCommandWithName("ON_HIDE");
	}
	else
	{
		p_layout_invite_player->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
			this->p_layout_invite_player->ForceFinishAction();

			p_layout_invite_player->GetWidgetChildByName(".txt_have_data")->Visible(false);
			p_layout_invite_player->GetWidgetChildByName(".loading_circle")->Visible(true);
			p_layout_invite_player->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");

			GameService::requestWaitingListInGame();
		}));
	}
}

void HUDScreen::ShowMessagePopup_New(std::string message)
{
	_showMessagePopUp_new(message, POPUP_GROUP_TIME_SHORT, false);
}

void HUDScreen::_showMessagePopUp_new(std::string text, float timeDelay, bool vibravate)
{
	auto w_text = GetWidgetChildByName("HUD_screen.text_msg_popup");
	w_text->Visible(true);
	w_text->ForceFinishAction();
	static_cast<TextWidget*>(w_text)->SetText(text, true);
	//
	float time_to_black = 0.5f;
	float time_delay = 0.45f;
	float time_to_red = 0.8f;

	w_text->SetOpacity(255);
	w_text->SetColor(Color3B::RED);
	w_text->SetPosition(Vec2(GetGameSize().width / 2, GetGameSize().height / 2));
	cocos2d::Vector<cocos2d::FiniteTimeAction*> list_ac;
	for (int i = 0; i < 6; i++)
	{
		list_ac.pushBack(TintTo::create(time_to_black, Color3B::BLACK));
		list_ac.pushBack(DelayTime::create(time_delay));
		list_ac.pushBack(TintTo::create(time_to_red, Color3B::RED));
	}
	w_text->GetResource()->runAction(
		Spawn::createWithTwoActions(
			Sequence::create(list_ac),
			Sequence::create(DelayTime::create(timeDelay), FadeOut::create(0.5f), 
				CallFunc::create([this](){
						this->PushEvent([](BaseScreen * scr) {
							HUDScreen * hud_scr = (HUDScreen*)scr;
							auto text = hud_scr->GetWidgetChildByName("HUD_screen.text_msg_popup");
							text->Visible(false);
							text->ForceFinishAction();
						});
			}), nullptr)
		)
	);
}
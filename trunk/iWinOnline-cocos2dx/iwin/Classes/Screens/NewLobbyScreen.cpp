
#include "NewLobbyScreen.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wScrollView.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wSlider.h"
#include "UI/AdvanceUI/wButton.h"


#include "Network/Global/Globalservice.h"
#include "Common/GameController.h"
#include "Common/IwinListView.h"
#include "Common/ChatBoard.h"
//#include "Common/EmotionBoard.h"
#include "Screens/CommonScreens/EmoBoardLayer.h"
#include "Social/SocialManager.h"
#include "Utils.h"
#include "Network/Core/ServerMgr.h"
#include "../Network/Game/GameService.h"

#include "Common/IwinListViewTable.h"
#include "Common/IwinTextfield.h"
#include "Common/CoPhoMgr.h"
#include "Screens/GameScreens/CoTuong/CoTuongScreen.h"

ClippingNode* createAvatarClippingNode(cocos2d::Size node_size)
{
	Sprite* clipping_spr = Sprite::createWithSpriteFrameName("mainscreen/newgui-mainscreen/khung_avatar_mainscreen.png");
	clipping_spr->setScale(node_size.width / clipping_spr->getContentSize().width);
	ClippingNode* node = ClippingNode::create(clipping_spr);
	node->setAlphaThreshold(.5f);
	return node;
}

NewLobbyScreen::NewLobbyScreen():
_cur_lobby_list_data(nullptr)
{
	p_current_game_id = 0;
	p_list_avatar_download.clear();
	p_list_avatar_copho_rival.clear();
	_have_return_from_game_screen = false;
	_max_money_slider = 0;
}

NewLobbyScreen::~NewLobbyScreen()
{
	CC_SAFE_DELETE(_listview_board);
	CC_SAFE_DELETE(_chat_board);
	SAFE_DELETE_VECTOR(p_list_text_chat);
	p_list_avatar_download.clear();
	p_list_avatar_copho_rival.clear();
}

int NewLobbyScreen::Init()
{
	//override the list view table default with the iwinlistview table
	this->_func_action = [&](LayerEntity * layer) {
		layer->OverloadRegisterUIWidget<IwinListViewTable>(xml::WIDGET_LIST_VIEW_TABLE);
		layer->OverloadRegisterUIWidget<IwinTextFieldHandle>(xml::WIDGET_TEXT_FIELD);
		layer->OverloadRegisterUIWidget<IwinEditBoxdHandle>(xml::WIDGET_EDIT_BOX);
	};
	//

	RKString _menu = "new_lobby_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}

	InitMenuWidgetEntity(_menu);

	p_panel_left = GetWidgetChildByName("lobby_ui.panel_left");
	p_scrollview_chat = p_panel_left->GetWidgetChildByName(".scrollview_chat");
	p_scrollitem_chat = GetWidgetChildByName("lobby_ui.chat_text");
	_chat_board = new ChatBoard();
	_chat_board->init((ListViewWidget*)(p_scrollview_chat), p_scrollitem_chat);

		
	CallInitComponent(true);

	auto pos_1 = p_scrollview_chat->GetPosition_BottomLeft();
	auto pos_2 = p_panel_left->GetWidgetChildByName(".Image_4")->GetPosition_TopLeft();
	auto pos_3 = p_panel_left->GetWidgetChildByName(".Image_5")->GetPosition_Middle();
	p_panel_left->GetWidgetChildByName(".Image_5")->SetPosition(Vec2(pos_3.x, pos_2.y + ((pos_1.y - pos_2.y) / 2)));

	_is_textfield_chat_focus = false;
	return 1;
}

int	NewLobbyScreen::InitComponent()
{

	p_panel_user_info = GetWidgetChildByName("lobby_ui.panel_user_info");
	p_panel_right_lobby = GetWidgetChildByName("lobby_ui.panel_right_lobby");
	p_panel_right_board = GetWidgetChildByName("lobby_ui.panel_right_board");
	p_panel_history = GetWidgetChildByName("lobby_ui.panel_right_history");
	p_top_panel = GetWidgetChildByName("lobby_ui.top_panel");

	p_chat_text_field = p_panel_left->GetWidgetChildByName(".Image_4.textfield_chat");

	p_money_value = p_panel_right_board->GetWidgetChildByName(".layout_play_with_money.layout_play_with_money.lbl_money_value");
	p_bet_slider = p_panel_right_board->GetWidgetChildByName(".layout_play_with_money.slider_money");

	p_img_history_normal = p_top_panel->GetWidgetChildByName(".layout_history.history_normal");
	p_img_history_active = p_top_panel->GetWidgetChildByName(".layout_history.history_active");

	p_scrollview_bet = GetWidgetChildByName("lobby_ui.panel_right_lobby.scrollview_bet");
	auto scrollview_bet = static_cast<IwinListViewTable*>(p_scrollview_bet);
	scrollview_bet->InitWithParam(
		scrollview_bet->GetParentWidget()->GetWidgetChildByName(".layout_pull_update"),
		nullptr,
		[this, scrollview_bet](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) {
			int current_idx_to = scrollview_bet->GetCurrentIdxTo();
			OnParseListLobbyBet(_widget, current_idx_to);
		},
		[this, scrollview_bet](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) 
		{
			//when the emtion appear do not click
			if (GetCommonScr->IsLayerVisible(COMMON_LAYER::EMOTION_LAYER) || _is_textfield_chat_focus)
				return;

			Vec2 current_pos = _widget->GetResource()->convertToNodeSpace(p_current_pos_touch);
			for (int i = 0; i < _widget->GetNumberChildWidget(); i++)
			{
				auto w = _widget->GetWidgetChildAtIdx(i);
				if (w->Visible())
				{
					auto r = w->GetBound();
					if (r.containsPoint(current_pos))
					{
						int current_idx = atoi(w->GetName().GetString());
						if (_cur_lobby_list_data && current_idx >= 0 && current_idx < _cur_lobby_list_data->getList().size())
						{
							auto item = _cur_lobby_list_data->getLobbyItem(current_idx);
							ScrMgr->PlayNowLobby(item->getMoneyType(), item->getMoney());
							//set bet range
							SetValueIntoBetUI(item->getMoney());
							//
							GetCommonScr->GetLayer(INVITE_LAYER)->OnHideLayer();
						}
						return;
					}

				}
			}

		},
		[this, scrollview_bet](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) {
			GlobalService::getLobbyList(GameController::getCurrentGameType(), GameController::currentMoneyType);
		},nullptr
	);


	p_list_player_lobby = p_panel_right_board->GetWidgetChildByName(".layout_list_player.tableview_board");
	auto list_player_lobby = static_cast<IwinListViewTable*>(p_list_player_lobby);
	list_player_lobby->InitWithParam(
		p_list_player_lobby->GetParentWidget()->GetWidgetChildByName(".layout_pull_update"),
		nullptr,
		[this , list_player_lobby](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) {
			_widget->Visible(true);
			int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_player_lobby)->GetCurrentIdxTo();
			updateItemListViewBoard(_widget, current_idx_to);
		},
		[this, list_player_lobby](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) {
			
			if (GetCommonScr->IsLayerVisible(COMMON_LAYER::EMOTION_LAYER) || _is_textfield_chat_focus)
				return;

			auto ldb = static_cast<ListViewTableWidget*>(this->p_list_player_lobby);
			size_t current_idx = ldb->GetCurrentIdxSelected();
			if (current_idx >= 0 && current_idx < _cur_list_board.size())
			{
				auto idx_player = _cur_list_board.at(current_idx);

				int room_id = idx_player.getRoomId();
				int board_id = idx_player.getBoardId();
				bool password = idx_player.getPassword();
				if (!password)
				{
					ScrMgr->JoinBoard(room_id, board_id, "");
					GetCommonScr->GetLayer(INVITE_LAYER)->OnHideLayer();
				}
			}
			else
			{
				PASSERT2(false, "have problem here!");
			}
		},
		[this, list_player_lobby](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) {
			OnReloadBoardGame(GameController::currentMoneyType, _current_bet);
		},
		nullptr
	);
	p_list_history = p_panel_history->GetWidgetChildByName(".tableview_board");
	auto list_history = static_cast<IwinListViewTable*>(p_list_history);
	list_history->InitWithParam(
		p_panel_history->GetWidgetChildByName(".layout_pull_update"),
		nullptr,
		[this, list_history](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) 
	{
		int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_history)->GetCurrentIdxTo();
		updateItemHistory(_widget, current_idx_to);
	},
		[this, list_history](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		auto ldb = static_cast<ListViewTableWidget*>(this->p_list_history);
		GameController::showReplay(ldb->GetCurrentIdxSelected(), p_current_game_id);
	},
		[this, list_history](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		GetCoPhoMgr()->load(GameController::myInfo->IDDB);
	},
		nullptr
		);


	showListBoard(true);
	InitTextFieldValue();

	showHistory(false);
	return 1;
}

void NewLobbyScreen::InitTextFieldValue()
{
	auto _widget = p_chat_text_field;
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".btn_clear_text"));
	w_tx->InitCallBack(
		[this, _widget]()
	{
		OnFocusTextFieldChat(true);
        if(GetCommonScr->IsLayerVisible(EMOTION_LAYER))
        {
            static_cast<EmoBoardLayer*>(GetCommonScr->GetLayer(EMOTION_LAYER))->ResetPosScreenWhenHaveKeyboard();
        }
	},
		[this, _widget]()
	{
		this->PushEvent([](BaseScreen * scr) {
			NewLobbyScreen * new_scr = (NewLobbyScreen *)scr;
			if (GetCommonScr->IsLayerVisible(COMMON_LAYER::EMOTION_LAYER))
			{
				GetCommonScr->OnHideLayer(COMMON_LAYER::EMOTION_LAYER);
				new_scr->p_chat_text_field->GetParentWidget()->GetWidgetChildByName(".btn_emotion")->SetColor(Color3B::WHITE);
			}
		});
		OnFocusTextFieldChat(false);
		/*
		this->PushEvent([](BaseScreen * scr){
		NewLobbyScreen * new_scr = (NewLobbyScreen *)scr;
		if(GetCommonScr->IsLayerVisible(COMMON_LAYER::EMOTION_LAYER))
		{
		GetCommonScr->OnHideLayer(COMMON_LAYER::EMOTION_LAYER);
		new_scr->p_chat_text_field->GetParentWidget()->GetWidgetChildByName(".btn_emotion")->SetColor(Color3B::WHITE);
		}
		});

		*/
	});
}

int NewLobbyScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (p_list_text_chat.size() > 0)
	{
		ChatLobbyItem* item = p_list_text_chat[0];
		_chat_board->addChat(item->getUserName(), Utils::chatConvert(item->getMessage()));
		p_list_text_chat.erase(p_list_text_chat.begin());
		delete item;
	}

	static_cast<IwinListViewTable*>(p_list_player_lobby)->UpdateReload(dt);
	static_cast<IwinListViewTable*>(p_list_history)->UpdateReload(dt);
	static_cast<IwinListViewTable*>(p_scrollview_bet)->UpdateReload(dt);

	return 1;
}


void NewLobbyScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_BUTTON)
	{
		if (name == "btn_back")
		{
			OnKeyBack();
		}
		else if (name == "btn_listboard")
		{
			/*LobbyItem* item = (LobbyItem*)(_widget->GetResource()->getUserData());
			OnReloadBoardGame(item->getMoneyType(), item->getMoney())*/;
			showListBoard(true);
		}
		else if (name == "btn_listlobby")
		{
			showListBoard(false);
		}
		else if (name == "btn_ruby")
		{
			showLobby(RUBY_TYPE);
		}
		else if (name == "btn_win")
		{
			showLobby(WIN_TYPE);
		}
		else if (name == "btn_emotion")
		{
			if (!GetCommonScr->IsLayerVisible(COMMON_LAYER::EMOTION_LAYER))
			{
				GetCommonScr->OnShowLayer(COMMON_LAYER::EMOTION_LAYER);
				GetCommonScr->SetCallBackEmotionBoard(CC_CALLBACK_1(NewLobbyScreen::handleChatEmotion, this));
				_widget->SetColor(Color3B::YELLOW);
                OnFocusTextFieldChat(true);
				//static_cast<TextFieldWidget*>(p_chat_text_field)->SetAttachIME();
			}
			else
			{
				GetCommonScr->OnHideLayer(COMMON_LAYER::EMOTION_LAYER);
				_widget->SetColor(Color3B::WHITE);
			}
		}
		else if (name == "btn_send")
		{
			RKString cur_text = static_cast<TextFieldWidget*>(p_chat_text_field)->GetText();
			if (cur_text != "")
			{
				GlobalService::sendLobbyChat(GameController::getCurrentGameType(), GameController::myInfo->IDDB, cur_text.GetString());
			}
			static_cast<TextFieldWidget*>(p_chat_text_field)->ClearText();
			OnFocusTextFieldChat(false);
		}
		else if (name == "btn_play_with_money")
		{
			ScrMgr->PlayNowLobby(GameController::currentMoneyType, _current_bet);
			GetCommonScr->GetLayer(INVITE_LAYER)->OnHideLayer();
		}
		else if (name == "btn_rich")
		{
			if (GameController::currentMoneyType == RUBY_MONEY)
			{				
				ShowRichestRuby();
			}
			else
			{
				ShowRichestWin();
			}
		}
		else if (name == "btn_strong")
		{
			ShowStrongest();
		}
		else if (name == "btn_history")
		{
			if (p_img_history_normal->IsVisible())
			{
				showHistory(true);
			}
			else
			{
				showHistory(false);
			}
		}
	}
	else if (type_widget == UI_TYPE::UI_SLIDER)
	{
		if (name == "slider_money_PERCENTAGE_CHANGED")
		{
			float value = static_cast<SliderWidget*>(_widget)->GetPercent();
			s64 money;
			int index = value / 100.0f * _max_money_slider;
			if (index >= 0 && index < Utils::GetBetLobby().size())
			{
				money = Utils::GetBetLobby()[index];
				static_cast<TextWidget*>(p_money_value)->SetText(StringUtils::format("%s", Utils::formatNumber_dot(money).c_str()));
				_current_bet = money;
			}
		}
		else if(name == "slider_money_SLIDEBALL_UP")
		{
			OnReloadBoardGame(GameController::currentMoneyType, _current_bet);
		}
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		auto w_tx = static_cast<IWIN_TF*>(_widget);
		w_tx->HandleEventTextField(name);
	}

}

void NewLobbyScreen::OnFocusTextFieldChat(bool value)
{
    auto panel = p_chat_text_field->GetParentWidget();
	if (value)
	{
		panel->SetActionCommandWithName("FOCUS", CallFunc::create([panel,this]() {
            this->PushEvent([panel](BaseScreen * scr){
                NewLobbyScreen * new_scr = (NewLobbyScreen*)scr;
			new_scr->_is_textfield_chat_focus = true;
			panel->ForceFinishAction();
			panel->GetWidgetChildByName(".btn_send")->SetActionCommandWithName("FOCUS_POS");
			Vec2 size_panel = panel->GetSize();
			static_cast<TextFieldWidget*>(new_scr->p_chat_text_field)->SetTouchZone(size_panel);
			static_cast<TextFieldWidget*>(new_scr->p_chat_text_field)->SetTextArea(Vec2(size_panel.x * 0.92, size_panel.y));
            });
		}));
	}
	else
	{
		panel->SetActionCommandWithName("DEFOCUS", CallFunc::create([panel, this]() {
            this->PushEvent([panel](BaseScreen * scr){
                NewLobbyScreen * new_scr = (NewLobbyScreen*)scr;
            panel->ForceFinishAction();
			panel->GetWidgetChildByName(".btn_send")->SetActionCommandWithName("DEFOCUS_POS");
			Vec2 size_panel = panel->GetSize();
			static_cast<TextFieldWidget*>(new_scr->p_chat_text_field)->SetTouchZone(Vec2(size_panel.x * 0.73, size_panel.y));
			static_cast<TextFieldWidget*>(new_scr->p_chat_text_field)->SetTextArea(Vec2(size_panel.x * 0.73, size_panel.y));
			new_scr->p_chat_text_field->SetColor(Color3B::WHITE);
			new_scr->_is_textfield_chat_focus = false;
                }, 0.5f);
		}));
	}
}

void NewLobbyScreen::OnDeactiveCurrentMenu()
{
	if (p_menu_show_next == HUD_SCREEN)
	{
		vector<RKString> name_list;
		SocialMgr->RemoveAllCallBackExcept(name_list);
	}

	ScrMgr->CloseCurrentMenu(NEW_LOBBY_SCREEN);

	p_menu_show_next = MENU_NONE;

	//hide this
	p_panel_right_board->GetWidgetChildByName(".lbl_no_board")->SetVisible(false);
	p_panel_right_board->GetWidgetChildByName(".icon_arrow")->SetVisible(false);
	p_panel_right_board->GetWidgetChildByName(".layout_play_with_money.menu_bar")->SetVisible(false);
	//when change state reset this value
	ScrMgr->resetLockJoinBoard();
}

void NewLobbyScreen::OnBeginFadeOut()
{
	auto panel = p_panel_right_board->GetWidgetChildByName(".layout_play_with_money.layout_play_with_money");
	auto wline = panel->GetWidgetChildByName(".line_glow_1");
	wline->GetResource()->stopAllActions();
	wline = panel->GetWidgetChildByName(".line_glow_2");
	wline->GetResource()->stopAllActions();
	wline = panel->GetWidgetChildByName(".star_glow");
	wline->GetResource()->stopAllActions();

	//visible this
	p_scrollview_bet->GetParentWidget()->GetWidgetChildByName(".txt_none_player")->Visible(false);

	//close history
	if (p_current_game_id == GAMEID_CHESS || p_current_game_id == GAMEID_CO_UP)
	{
		showHistory(false);
	}
}

void NewLobbyScreen::OnBeginFadeIn()
{
	GameController::doPlay((short)p_current_game_id);

    this->setPosition(Vec2(0,0));
	_chat_board->reset();
	if (p_current_game_id != GameType::GAMEID_CHESS && p_current_game_id != GameType::GAMEID_CO_UP)
	{
		showHistory(false);
	}
}

void NewLobbyScreen::ParseUserUI()
{
	User * user = GetUser;
	if (user == nullptr)
	{
		PASSERT2(false, "user is null");
		return;
	}
	RKString nick_name = user->nickName;

	p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13")->SetVisible(GameController::currentMoneyType != BASE_MONEY);
	p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13_0")->SetVisible(GameController::currentMoneyType == BASE_MONEY);

	static_cast<TextWidget*>(p_panel_user_info->GetWidgetChildByName(".lbl_username"))->SetText(nick_name , true);
	if (GameController::currentMoneyType == BASE_MONEY)
	{
		static_cast<TextWidget*>(p_panel_user_info->GetWidgetChildByName(".Image_12.lbl_money"))->SetText(Utils::formatNumber_dot(GameController::myInfo->money), true);
		p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13")->Visible(false);
		p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13_0")->Visible(true);
	}
	else
	{
		static_cast<TextWidget*>(p_panel_user_info->GetWidgetChildByName(".Image_12.lbl_money"))->SetText(Utils::formatNumber_dot(GameController::myInfo->ruby), true);
		p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13")->Visible(true);
		p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13_0")->Visible(false);
	}


	auto panel_avatar = p_panel_user_info->GetWidgetChildByName(".btn_avatar.panel_avatar");

	GameMgr->HandleAvatarDownload(user->avatarID, user->nickName,
		[this, panel_avatar](void * data, void * str, int tag)
	{
		this->ParseImgToPanel(panel_avatar, data, str, tag);
	});
}

void NewLobbyScreen::PlayMusic()
{
	PLAY_MUSIC_SE;
}

void NewLobbyScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
	auto panel = p_panel_right_board->GetWidgetChildByName(".layout_play_with_money.layout_play_with_money");
	auto wline = panel->GetWidgetChildByName(".line_glow_1");
	wline->SetActionCommandWithName("MOVE");
	wline = panel->GetWidgetChildByName(".line_glow_2");
	wline->SetActionCommandWithName("MOVE");
	wline = panel->GetWidgetChildByName(".star_glow");
	wline->SetActionCommandWithName("ROTATE");

	OnShowListBoardGame(false);

	this->PushEvent([](BaseScreen * scr) {
		NewLobbyScreen * new_scr = (NewLobbyScreen *)scr;
		new_scr->OnSetSliderBet();
	});
}

void NewLobbyScreen::onSetGameTypeSuccessed()
{
	this->PushEvent([](BaseScreen * scr) 
	{
		GlobalService::getLobbyList(GameController::getCurrentGameType(), GameController::currentMoneyType);
	});
	
	this->PushEvent([](BaseScreen * scr)
	{
		NewLobbyScreen * new_scr = (NewLobbyScreen*)scr;
		SAFE_DELETE_VECTOR(new_scr->p_list_text_chat);
		GlobalService::getGlobalChatList();
	});
	//std::string server_name = GetServerMgr()->getCurServer()._name;
	//std::string title = server_name + "-" + GameController::getCurrentGameName();
	//setTitle(title);

	ScrMgr->setBtnTaixiuDirty();

	if (GameController::currentMoneyType == BASE_MONEY)
	{
		static_cast<TextWidget*>(p_panel_user_info->GetWidgetChildByName(".Image_12.lbl_money"))->SetText(Utils::formatNumber_dot(GameController::myInfo->money), true);
		p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13")->Visible(false);
		p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13_0")->Visible(true);
	}
	else
	{
		static_cast<TextWidget*>(p_panel_user_info->GetWidgetChildByName(".Image_12.lbl_money"))->SetText(Utils::formatNumber_dot(GameController::myInfo->ruby), true);
		p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13")->Visible(true);
		p_panel_user_info->GetWidgetChildByName(".Image_12.Image_13_0")->Visible(false);
	}


	if (p_current_game_id == GameType::GAMEID_CHESS || p_current_game_id == GameType::GAMEID_CO_UP)
	{
		GetCoPhoMgr()->load(GameController::myInfo->IDDB);
		p_list_history->Visible(false);
		static_cast<ListViewTableWidget*>(p_list_history)->SetNumberSizeOfCell(GetCoPhoMgr()->getNumCoPho(), true);
		p_panel_history->GetWidgetChildByName(".loading_circle")->Visible(true);
		p_panel_history->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");
		p_panel_history->GetWidgetChildByName(".lbl_no_board")->Visible(false);
		static_cast<IwinListViewTable*>(p_list_history)->SetHaveReload([this]()
		{
			p_panel_history->GetWidgetChildByName(".loading_circle")->Visible(false);
			p_list_history->Visible(true);
			if (GetCoPhoMgr()->getNumCoPho() > 0)
			{
				p_panel_history->GetWidgetChildByName(".lbl_no_board")->Visible(false);
			}
			else
			{
				p_panel_history->GetWidgetChildByName(".lbl_no_board")->Visible(true);
			}
		});
	}
}

void NewLobbyScreen::showListBoard(bool visible)
{
	p_panel_right_lobby->SetVisible(!visible);
	p_panel_right_board->SetVisible(visible);
	p_top_panel->GetWidgetChildByName(".btn_listlobby")->SetVisible(visible);
	p_top_panel->GetWidgetChildByName(".btn_listboard")->SetVisible(!visible);
	p_top_panel->GetWidgetChildByName(".layout_history.btn_history")->SetVisible(visible);
	p_img_history_normal->SetVisible(visible);

	p_img_history_active->SetVisible(false);
	p_panel_history->SetVisible(false);

	ScrMgr->SetScreenIDDirty();

	_visible_board = p_panel_right_board->IsVisible();
}

void NewLobbyScreen::showLobby(int type)
{
	GlobalService::getLobbyList(GameController::getCurrentGameType(), type);
	p_panel_right_lobby->GetWidgetChildByName(".panel_button.tab_ruby_select")->SetVisible(type == RUBY_TYPE);
	p_panel_right_lobby->GetWidgetChildByName(".panel_button.tab_win_select")->SetVisible(type == WIN_TYPE);

}

void NewLobbyScreen::updateItemScrollViewBet(WidgetEntity* entity, LobbyItem* item, int idx)
{
	if (!item)
		return;

	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_count_board"))->SetText(StringUtils::format("%d", item->getBoardsWaitingCount()));
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_count_user"))->SetText(StringUtils::format("%d", item->getUsersWaitingCount()));
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_money"))->SetText(StringUtils::format("%s", Utils::formatNumber_dot(item->getMoney()).c_str()));
	cocos2d::Vec2 icon_size = entity->GetWidgetChildByName(".icon_coin")->GetSize();
	cocos2d::Size str_size = static_cast<TextBMFont*>(static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_money"))->GetResource())->getContentSize();
	float scale = static_cast<TextBMFont*>(static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_money"))->GetResource())->getScale();
	Vec2 item_size = entity->GetSize();
	int offsetx = (item_size.x - str_size.width * scale - icon_size.x) / 2;
	entity->GetWidgetChildByName(".icon_ruby")->SetVisible(item->getMoneyType() == RUBY_TYPE);
	entity->GetWidgetChildByName(".icon_coin")->SetVisible(item->getMoneyType() != RUBY_TYPE);
	Vec2 pos = entity->GetWidgetChildByName(".icon_coin")->GetPosition();
	entity->GetWidgetChildByName(".icon_coin")->SetPosition(Vec2(offsetx + icon_size.x / 2, pos.y));
	entity->GetWidgetChildByName(".icon_ruby")->SetPosition(Vec2(offsetx + icon_size.x / 2, pos.y));
	entity->GetWidgetChildByName(".lbl_money")->SetPosition(Vec2(offsetx + icon_size.x, pos.y));
	entity->SetName(std::to_string(idx));
	/*static_cast<ButtonWidget*>(entity->GetWidgetChildByName(".btn_play_now"))->SetActionBtn([this , item]() 
	{
			ScrMgr->PlayNowLobby(item->getMoneyType(), item->getMoney());
	});*/
}

void NewLobbyScreen::updateItemHistory(WidgetEntity* entity, int idx)
{
	CoPhoBoard board = GetCoPhoMgr()->getCoPhoIdx(idx);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_name"))->SetText(board.userNameRival, true);
	if (GameController::currentMoneyType == BASE_MONEY)
	{
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_money"))->SetText(Utils::formatMoney_dot_win(board.win), true);
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_bet"))->SetText(Utils::formatMoney_dot_win(board.winbet), true);
	}
	else
	{
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_money"))->SetText(Utils::formatMoney_dot_ruby(board.win), true);
		static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_bet"))->SetText(Utils::formatMoney_dot_ruby(board.winbet), true);
	}
	std::string status;
	if (board.result > 0)
	{
		status = "won";
		entity->GetWidgetChildByName(".lbl_status")->SetColor(Color3B::GREEN);
	}
	else if (board.result == 0)
	{
		status = "draw";
		entity->GetWidgetChildByName(".lbl_status")->SetColor(Color3B::WHITE);
	}
	else
	{
		status = "lose";
		entity->GetWidgetChildByName(".lbl_status")->SetColor(Color3B::RED);
	}
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_status"))->SetText(status);

	auto panel_avatar = entity->GetWidgetChildByName(".panel_avatar");
	entity->GetWidgetChildByName(".bkg_trongsuot")->SetVisible((idx % 2) != 0);
	if (p_list_avatar_copho_rival.size() > 0 && p_list_avatar_copho_rival.find(idx) != p_list_avatar_copho_rival.end())
	{
		this->ParseSpriteToPanel(panel_avatar, p_list_avatar_copho_rival.at(idx), 100);
	}
	else
	{
		GameMgr->HandleAvatarDownload(board.avatar, board.userNameRival,
			[this, board, idx](void * data, void * str, int tag)
		{
			Image * img = (Image *)data;
			{
				Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
				this->p_list_avatar_copho_rival.insert(idx, sprite_img);

				auto widget_lb = static_cast<ListViewTableWidget*>(p_list_history);
				if ((size_t)idx < widget_lb->GetListWidgetSample().size())
				{
					widget_lb->UpdateCellAtIdx(idx);
				}
			}
		});
	}


}

void NewLobbyScreen::updateItemListViewBoard(WidgetEntity* entity, int idx)
{
	BoardOwner owner = _cur_list_board.at(idx);
	entity->Visible(true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_name"))->SetText(owner.getUsername(), true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_money"))->SetText(Utils::formatNumber_dot(owner.getMoney()), true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_win"))->SetText(std::to_string(owner.getWin()), true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_lose"))->SetText(std::to_string(owner.getLose()), true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_draw"))->SetText(std::to_string(owner.getDraw()), true);
	static_cast<TextWidget*>(entity->GetWidgetChildByName(".lbl_quit"))->SetText(std::to_string(owner.getLeave()), true);

	for (int i = 0; i < 6; i++)
	{
		entity->GetWidgetChildByName(StringUtils::format(".icon_konguoi_%d", i))->SetVisible(i < owner.getCountMaxPlayer());
	}
	for (int i = 0; i < 6; i++)
	{
		entity->GetWidgetChildByName(StringUtils::format(".icon_conguoi_%d", i))->SetVisible(i < owner.getCountPlayer());
	}

	auto panel_avatar = entity->GetWidgetChildByName(".panel_avatar");

	if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(owner.getUsername()) != p_list_avatar_download.end())
	{
		this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(owner.getUsername()), 100);
	}

	if (idx % 2 == 0)
	{
		entity->GetWidgetChildByName(".hl_panel")->Visible(true);
	}
	else
	{
		entity->GetWidgetChildByName(".hl_panel")->Visible(false);
	}
}

void NewLobbyScreen::OnParseListLobbyBet(WidgetEntity * _widget, int idx_lobby)
{
	if (!_cur_lobby_list_data)
		return;
	int number_lobby_list = _cur_lobby_list_data->getList().size();
	int number_w_in_end_raw = number_lobby_list % 3;
	int number_raw = (number_lobby_list / 3) + (number_w_in_end_raw > 0 ? 1 : 0);
	if (idx_lobby < 0 || idx_lobby >= number_raw)
		return;

	auto w_0 = _widget->GetWidgetChildAtIdx(0);
	auto w_1 = _widget->GetWidgetChildAtIdx(1);
	auto w_2 = _widget->GetWidgetChildAtIdx(2);

	int number_idx_0 = idx_lobby * 3;
	int number_idx_1 = (idx_lobby * 3) + 1;
	int number_idx_2 = (idx_lobby * 3) + 2;

	if (idx_lobby < number_raw -1)
	{
		LobbyItem* item0 = _cur_lobby_list_data->getLobbyItem(number_idx_0);
		updateItemScrollViewBet(w_0, item0, number_idx_0);
		w_0->Visible(true);
		LobbyItem* item1 = _cur_lobby_list_data->getLobbyItem(number_idx_1);
		updateItemScrollViewBet(w_1, item1, number_idx_1);
		w_1->Visible(true);
		LobbyItem* item2 = _cur_lobby_list_data->getLobbyItem(number_idx_2);
		updateItemScrollViewBet(w_2, item2, number_idx_2);
		w_2->Visible(true);
	}
	else if (idx_lobby  == number_raw - 1)
	{
		if (number_w_in_end_raw == 1)
		{
			LobbyItem* item0 = _cur_lobby_list_data->getLobbyItem(number_idx_0);
			updateItemScrollViewBet(w_0, item0, number_idx_0);
			w_0->Visible(true);
			w_1->Visible(false);
			w_2->Visible(false);
		}
		else if (number_w_in_end_raw == 2)
		{
			LobbyItem* item0 = _cur_lobby_list_data->getLobbyItem(number_idx_0);
			updateItemScrollViewBet(w_0, item0, number_idx_0);
			w_0->Visible(true);
			LobbyItem* item1 = _cur_lobby_list_data->getLobbyItem(number_idx_1);
			updateItemScrollViewBet(w_1, item1, number_idx_1);
			w_1->Visible(true);
			w_2->Visible(false);
		}
	}
}

void NewLobbyScreen::SetPopularBet()
{
	if (!_cur_lobby_list_data)
		return;
	if (Utils::s_popular_bet.size() > 0)
		return;
	std::vector<LobbyItem*> list_lobby = _cur_lobby_list_data->getList();
	for (auto l : list_lobby)
	{
		Utils::s_popular_bet.push_back(l->getMoney());
	}

	OnSetSliderBet();
}

void NewLobbyScreen::OnSetSliderBet()
{
	auto bet_lobby = Utils::GetBetLobby();
	if (bet_lobby.size() <= 0)
	{
		return;
	}
	if (_have_return_from_game_screen)
	{
		_have_return_from_game_screen = false;
		return;
	}
	s64 findBoardMoney = 0;
	if (GameController::isBaseMoney()) {
		findBoardMoney = (s64)(GameController::myInfo->money / 10);
		//Utils::getMaxValidBetIdx(GameController::myInfo->money);
	}
	else
	{
		findBoardMoney = (s64)(GameController::myInfo->ruby / 10);
		// Utils::getMaxValidBetIdx(GameController::myInfo->ruby);
	}

	SetValueIntoBetUI(findBoardMoney);
}

void NewLobbyScreen::SetValueIntoBetUI(s64 money)
{
	auto bet_lobby = Utils::GetBetLobby();
	int index = 0;
	if (money < bet_lobby[0]) {
		money = bet_lobby[0];
		index = 0;
	}
	else 
	{
		for (size_t i = 0; i < bet_lobby.size(); i++)
		{
			if (bet_lobby[i] > money)
			{
				money = bet_lobby[i - 1];
				index = i - 1;
				break;
			}
		}
	}

	_max_money_slider = Utils::getNumberValidBetIdx();

	if (money > -1) {
		_current_bet = money;
		// lần đâu tiên vô từ tự động đăng nhập, set mức cược này
		static_cast<SliderWidget*>(p_bet_slider)->SetPercent(index * 1.0f / _max_money_slider * 100);
		static_cast<TextWidget*>(p_money_value)->SetText(StringUtils::format("%s", Utils::formatNumber_dot(bet_lobby[index]).c_str()));
	}
}

void NewLobbyScreen::onReceiveLobbyList(LobbyList* lobby_list)
{

	CC_SAFE_DELETE(_cur_lobby_list_data);
	_cur_lobby_list_data = lobby_list;

	if (!_cur_lobby_list_data)
	{
		PASSERT2(false, "null here");
	}

	this->PushEvent([](BaseScreen * scr) {
		NewLobbyScreen * new_scr = (NewLobbyScreen *)scr;
		new_scr->SetPopularBet();
	});

	//_scroll_view_with_bet->reload(_cur_lobby_list_data->getList().size());
	p_scrollview_bet->Visible(false);
	p_scrollview_bet->GetParentWidget()->GetWidgetChildByName(".txt_none_player")->Visible(false);
	p_scrollview_bet->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(true);
	p_scrollview_bet->GetParentWidget()->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");
	//
	static_cast<IwinListViewTable*>(p_scrollview_bet)->ResetListView();

	static_cast<IwinListViewTable*>(p_scrollview_bet)->SetHaveReload([this]() 
	{
		p_scrollview_bet->Visible(true);
		p_scrollview_bet->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(false);
		static_cast<IwinListViewTable*>(p_scrollview_bet)->SetNumberSizeOfCell((_cur_lobby_list_data->getList().size() / 3 ) + ((_cur_lobby_list_data->getList().size() % 3 ==  0 )? 0 : 1), true);
		if (_cur_lobby_list_data->getList().size() > 0)
		{
			p_scrollview_bet->GetParentWidget()->GetWidgetChildByName(".txt_none_player")->Visible(false);
		}
		else
		{
			p_scrollview_bet->GetParentWidget()->GetWidgetChildByName(".txt_none_player")->Visible(true);
		}
		OnReloadBoardGame(GameController::currentMoneyType, _current_bet);
	});
}

void NewLobbyScreen::OnParseLobbyRoom()
{
	static_cast<ListViewTableWidget*>(p_list_player_lobby)->SetNumberSizeOfCell(_cur_list_board.size());
	p_panel_right_board->GetWidgetChildByName(".lbl_no_board")->SetVisible(_cur_list_board.size() == 0);
	p_panel_right_board->GetWidgetChildByName(".icon_arrow")->SetVisible(_cur_list_board.size() == 0);
	p_panel_right_board->GetWidgetChildByName(".layout_play_with_money.menu_bar")->SetVisible(_cur_list_board.size() != 0);
	if (_cur_list_board.size() == 0) {
		updateNoBoardLabel(_current_bet);
	}


	//download avatar in save into the list
	for (size_t i = 0; i < _cur_list_board.size(); i++)
	{
		auto idx_player = _cur_list_board[i];
		{
			GameMgr->HandleAvatarDownload(idx_player.getAvatar(), idx_player.getUsername(),
				[this, idx_player, i](void * data, void * str, int tag)
			{
				Image * img = (Image *)data;
				if (this->_cur_list_board.size() > 0)
				{
					Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
					this->p_list_avatar_download.insert(idx_player.getUsername(), sprite_img);

					if ((size_t)i < static_cast<ListViewTableWidget*>(p_list_player_lobby)->GetListWidgetSample().size())
					{
						static_cast<ListViewTableWidget*>(p_list_player_lobby)->UpdateCellAtIdx(i);			
					}
				}
			});
		}
	}
}

void NewLobbyScreen::OnReloadBoardGame(int moneyType, int money)
{
	OnShowListBoardGame(false);
	GlobalService::getBoardFromLobby(moneyType, money);

	p_panel_right_board->GetWidgetChildByName(".lbl_no_board")->SetVisible(false);
	p_panel_right_board->GetWidgetChildByName(".icon_arrow")->SetVisible(false);
	p_panel_right_board->GetWidgetChildByName(".layout_play_with_money.menu_bar")->SetVisible(false);
}

void NewLobbyScreen::OnShowListBoardGame(bool value)
{
	if (value)
	{
		p_list_player_lobby->Visible(true);
		p_list_player_lobby->GetParentWidget()->GetWidgetChildByName(".txt_none_player")->Visible(false);
		p_list_player_lobby->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(false);

	}
	else
	{
		p_list_player_lobby->Visible(false);
		p_list_player_lobby->GetParentWidget()->GetWidgetChildByName(".txt_none_player")->Visible(false);
		p_list_player_lobby->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(true);
		p_list_player_lobby->GetParentWidget()->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");

	}
}

void NewLobbyScreen::onReceiveOwnerBoardList(std::vector<BoardOwner> boards, int win, int lose, int draw, int leave)
{
	if (_cur_list_board.size() > 0)
	{
		_cur_list_board.clear();
		static_cast<ListViewTableWidget*>(p_list_player_lobby)->ResetListView();
		p_list_avatar_download.clear();
	}

	_cur_list_board = boards;

	//OnShowListBoardGame(false);


	static_cast<IwinListViewTable*>(p_list_player_lobby)->SetHaveReload([this]() {
		OnParseLobbyRoom();
		OnShowListBoardGame(true);
	});

}

void NewLobbyScreen::onReceiveChatList(ChatLobbyList* chatLobbyList)
{
	if (chatLobbyList->getIsRequest() == 1)
	{
		_chat_board->reset();
	}

	std::vector<ChatLobbyItem*> lobby_items = chatLobbyList->getChatList();
	for (auto it : lobby_items)
	{
		p_list_text_chat.push_back(it);		
	}

	SAFE_DELETE(chatLobbyList);
}

void NewLobbyScreen::handleChatEmotion(int idx)
{
	RKString cur_text = static_cast<TextFieldWidget*>(p_chat_text_field)->GetText();
	cur_text += Utils::getEmotionStr(idx);
	static_cast<TextFieldWidget*>(p_chat_text_field)->SetText(cur_text);
}

void NewLobbyScreen::setTitle(std::string text)
{
	static_cast<TextWidget*>(p_top_panel->GetWidgetChildByName(".lbl_title"))->SetText(text , true);
}


void NewLobbyScreen::updateNoBoardLabel(s64 money)
{
	TextWidget* no_board_label = static_cast<TextWidget*>(p_panel_right_board->GetWidgetChildByName(".lbl_no_board"));

	std::string text = StringUtils::format(LANG_STR("no_board").GetString(), 
		(GameController::currentMoneyType == BASE_MONEY ? Utils::formatMoney_dot_win(money).c_str() : Utils::formatMoney_dot_ruby(money).c_str()));
	
	no_board_label->SetText(text);
	float scale = static_cast<TextBMFont*>(no_board_label->GetResource())->getScale();
	cocos2d::Size text_size = static_cast<TextBMFont*>(no_board_label->GetResource())->getContentSize();
	text_size.width *= scale;
	text_size.height *= scale;

	int pos_left = no_board_label->GetPosition().x - text_size.width / 2;
	WidgetEntity* arrow = p_panel_right_board->GetWidgetChildByName(".icon_arrow");
	Vec2 cur_pos = arrow->GetPosition();
	arrow->SetPosition(Vec2(pos_left, cur_pos.y));
}

void NewLobbyScreen::SetCurrentGameID(int id)
{

	if (p_current_game_id != id)
	{
		Utils::s_popular_bet.clear();
	}

	p_current_game_id = id;

	if (GameController::currentMoneyType == RUBY_MONEY)
	{
		GetWidgetChildByName("lobby_ui.background_1")->Visible(false);
		GetWidgetChildByName("lobby_ui.background_2")->Visible(true);
	}
	else
	{
		GetWidgetChildByName("lobby_ui.background_1")->Visible(true);
		GetWidgetChildByName("lobby_ui.background_2")->Visible(false);
	}

	if (id == GameType::GAMEID_CHESS || id == GameType::GAMEID_CO_UP)
	{
		//show btn history
		p_top_panel->GetWidgetChildByName(".layout_history")->Visible(true);
	}
	else
	{
		p_top_panel->GetWidgetChildByName(".layout_history")->Visible(false);
	}

	if (p_previous_game_id != id)
	{
		ResetLeaderboard();

		static_cast<ListViewTableWidget*>(p_list_player_lobby)->ResetListView();
		p_list_avatar_download.clear();
		p_list_avatar_copho_rival.clear();
		_cur_list_board.clear();
	}
	p_previous_game_id = id;
}

void NewLobbyScreen::ResetLeaderboard(bool cleanup)
{
	GetCommonScr->LB_ResetLeaderboard(cleanup);
}

void NewLobbyScreen::ShowRichestWin()
{
	GetCommonScr->LB_ShowRichestWin();
}

void NewLobbyScreen::ShowRichestRuby()
{
	GetCommonScr->LB_ShowRichestRuby();
}

void NewLobbyScreen::ShowStrongest()
{
	GetCommonScr->LB_ShowStrongest(p_current_game_id);
}

void NewLobbyScreen::SetActionAfterShow(int action)
{
	if (action == 0)
	{
		this->PushEvent([](BaseScreen * scr) 
		{
			NewLobbyScreen * nscr = (NewLobbyScreen*)scr;
			nscr->showListBoard(true);
		}, TIME_ACTION);
	}
	else if (action == 1)
	{
		this->PushEvent([](BaseScreen * scr)
		{
			NewLobbyScreen * nscr = (NewLobbyScreen*)scr;
			nscr->showListBoard(true);
		}, TIME_ACTION);
	}
}

void NewLobbyScreen::OnKeyBack()
{
	ScrMgr->SwitchToMenu(MAIN_SCREEN, NEW_LOBBY_SCREEN);
	if (GetCommonScr->IsLayerVisible(INVITE_LAYER))
	{
		GetCommonScr->OnHideLayer(INVITE_LAYER);
	}
}

int NewLobbyScreen::GetStateNewLobbyMenu()
{
	if (p_panel_right_board->Visible())
	{
		return 0;
	}
	else
	{
		if (p_panel_right_lobby->Visible())
		{
			return 1;
		}
	}
	return 2;
}

void NewLobbyScreen::showHistory( bool visible)
{
	p_img_history_normal->SetVisible(!visible);
	p_img_history_active->SetVisible(visible);
	p_panel_history->SetVisible(visible);
	if (visible)
	{
		_visible_board = p_panel_right_board->IsVisible();
		p_panel_right_board->SetVisible(false);
		p_panel_right_lobby->SetVisible(false);
	}
	else
	{
		p_panel_right_board->SetVisible(_visible_board);
		p_panel_right_lobby->SetVisible(!_visible_board);
	}
}

void NewLobbyScreen::ClearComponent()
{
	showListBoard(true);
	static_cast<IwinListViewTable*>(p_scrollview_bet)->ResetListView();
	static_cast<IwinListViewTable*>(p_list_player_lobby)->ResetListView();
	static_cast<IwinListViewTable*>(p_list_history)->ResetListView();

	CC_SAFE_DELETE(_cur_lobby_list_data);
	_cur_list_board.clear();
	p_list_avatar_download.clear();
	p_list_avatar_copho_rival.clear();
}

void NewLobbyScreen::SetHaveComeFromGame()
{
	_have_return_from_game_screen = true;
}
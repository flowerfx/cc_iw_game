#include "FriendScreen.h"
#include "Network/Global/Globalservice.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wListView.h"

#include "Models/ChatMessage.h"
#include "Models/iWinPlayer.h"

#include "Screens/GamePlayMgr.h"
#include "Screens/GameScreens/GameScreens.h"

#include "Common/IwinListViewTable.h"
#include "Common/IwinTextfield.h"
#include "Screens/CommonScreens/EmoBoardLayer.h"
FriendScreen::FriendScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;

	p_search_player_layout = nullptr;

	p_ADDFRIEND_MESSAGE.clear();
	p_ADMIN_MESSAGE.clear();
	p_UNREAD_MESSAGE.clear();
	p_listRecentChat.clear();
	p_listFriendChat.clear();

	p_listPlayerFriend.clear();

	p_stage_get_list_friend = 0;

	p_current_friend_search = "";
	p_current_friend_chat_search = "";
	p_current_idx_search_parse = 0;
	p_current_idx_search_friend_chat = 0;

	p_search_myfriend_layout = nullptr;
	p_chat_friend_layout = nullptr;
	p_main_friend_layout = nullptr;

	_state_press_layout_chat = -1;

	_root_layer = nullptr;

	_have_list_friend_chat_appear = false;
	_widget_cell_on_focus = nullptr;
	_on_move_layer = false;

	p_current_friend_chat_to = nullptr;
}


FriendScreen::~FriendScreen()
{
	ClearComponent();
	_root_layer = nullptr;
	p_current_friend_chat_to = nullptr;
}

int FriendScreen::Init()
{
	//override the list view table default with the iwinlistview table
	this->_func_action = [&](LayerEntity * layer) {
		layer->OverloadRegisterUIWidget<IwinListViewTable>(xml::WIDGET_LIST_VIEW_TABLE);
		layer->OverloadRegisterUIWidget<IwinTextFieldHandle>(xml::WIDGET_TEXT_FIELD);
		layer->OverloadRegisterUIWidget<IwinEditBoxdHandle>(xml::WIDGET_EDIT_BOX);
	};
	//

	RKString _menu = "friend_screen";

	InitMenuWidgetEntity(_menu);

	CallInitComponent(true);

	return 1;
}

int	FriendScreen::InitComponent()
{
	p_layout_list_friend_chat = GetWidgetChildByName("back.layout_list_friend_chat");
	p_list_friend_chat = p_layout_list_friend_chat->GetWidgetChildByName(".list_friend_chat");

	_root_layer = GetLayerChildByName("friend_ui");
	p_top_panel = GetWidgetChildByName("friend_ui.top_panel");

	p_main_friend_layout = GetWidgetChildByName("friend_ui.layout_main_friend_list");
	p_search_player_layout = GetWidgetChildByName("friend_ui.search_player_layout.search_player");
	p_list_friend_main = p_main_friend_layout->GetWidgetChildByName(".panel_list_friend.list_friend_");

	p_chat_friend_layout = GetWidgetChildByName("friend_ui.layout_friend_chat");
	p_count_friend_layout = p_list_friend_main->GetParentWidget()->GetWidgetChildByName(".count_friend_layout");
	auto list_friend_main = static_cast<IwinListViewTable*>(p_list_friend_main);
	list_friend_main->InitWithParam(
		p_list_friend_main->GetParentWidget()->GetWidgetChildAtIdx(1),
		nullptr,
		[this , list_friend_main](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) {
			int current_idx_to = list_friend_main->GetCurrentIdxTo();
			OnParseListFriendMain(_widget, current_idx_to, p_current_friend_search);
		},
		[this, list_friend_main](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) {
			int current_idx_to = list_friend_main->GetCurrentIdxSelected();
			
			if (!_widget->GetWidgetChildAtIdx(8)->Visible())
			{
				int idx_to = current_idx_to - p_ADDFRIEND_MESSAGE.size();
				if (idx_to >= 0 && idx_to < p_listPlayerFriend.size())
				{

					p_main_friend_layout->SetActionCommandWithName("ON_HIDE");
					p_chat_friend_layout->SetActionCommandWithName("ON_SHOW");

					p_top_panel->GetWidgetChildAtIdx(1)->SetActionCommandWithName("ON_HIDE");
					p_top_panel->GetWidgetChildAtIdx(3)->SetActionCommandWithName("ON_SHOW");

					auto p = p_listPlayerFriend[idx_to];
					ShowChatToFriend(p);
					_widget->GetWidgetChildAtIdx(9)->Visible(false);
				}
				
			}
		},
		[this, list_friend_main](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) {
			//reload list here
			GlobalService::requestFriendList();
		},nullptr);
	
	auto list_friend_chat = static_cast<IwinListViewTable*>(p_list_friend_chat);
	list_friend_chat->InitWithParam(
		list_friend_chat->GetParentWidget()->GetWidgetChildByName(".layout_pull_update"),
		nullptr,
		[this, list_friend_chat](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) 
	{
		int current_idx_to = list_friend_chat->GetCurrentIdxTo();
		OnParseListFriendChat(_widget, current_idx_to, p_current_friend_chat_search);
	},
		[this, list_friend_chat](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		//call to chat
		int current_idx_to = list_friend_chat->GetCurrentIdxSelected();
		if (current_idx_to >= 0 && current_idx_to < p_listPlayerFriend.size())
		{
			auto p = p_listPlayerFriend[current_idx_to];
			ShowChatToFriend(p);
			OnRootLayerMove(true);
		}
	},
		[this, list_friend_chat](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		//reload list here
		GlobalService::requestFriendList();
	}, nullptr);

	list_friend_chat->InitOptionalParam(
		[this, list_friend_chat](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		_widget_cell_on_focus = _widget;
		_delta_move_cell = Vec2(0, 0);

	},nullptr);

	p_chat_friend_input = p_chat_friend_layout->GetWidgetChildByName(".input_chat_layout.panel_chat_friend.chat_friend_input");
	InitTextFieldValue();


	p_layout_chat = p_chat_friend_layout->GetWidgetChildByName(".chat_layout");
	p_list_chat = p_layout_chat->GetWidgetChildByName(".list_chat");

	return 1;
}

void FriendScreen::InitTextFieldValue()
{
	auto _widget = p_main_friend_layout->GetWidgetChildByName(".panel_search_friend.name_search_input");
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".btn_clear_name_search"));
	w_tx->InitCallBack(
		nullptr,
		nullptr,
		[this, _widget]() 
	{
		p_current_friend_search = static_cast<TextFieldWidget*>(_widget)->GetText();
		p_current_idx_search_parse = 0;
		size_t number = GetNumberFriendContainName(p_current_friend_search);
		static_cast<ListViewTableWidget*>(p_list_friend_main)->SetNumberSizeOfCell(number, true);
	},
		[this, _widget]() 
	{
		p_current_friend_search = static_cast<TextFieldWidget*>(_widget)->GetText();
		p_current_idx_search_parse = 0;
		size_t number = GetNumberFriendContainName(p_current_friend_search);
		static_cast<ListViewTableWidget*>(p_list_friend_main)->SetNumberSizeOfCell(number, true);
	},
		nullptr
	);

	_widget = p_search_player_layout->GetWidgetChildByName(".search_player_panel.panel_input.search_player_input");
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".btn_clear_player"));


	_widget = p_chat_friend_input;
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".btn_clear"));
	w_tx->InitCallBack(
    [this, _widget]()
    {
        if(GetCommonScr->IsLayerVisible(EMOTION_LAYER))
        {
            static_cast<EmoBoardLayer*>(GetCommonScr->GetLayer(EMOTION_LAYER))->ResetPosScreenWhenHaveKeyboard();
        }
    },
    [this, _widget]()
	{
		GetCommonScr->OnHideLayer(COMMON_LAYER::EMOTION_LAYER);
		p_chat_friend_input->GetParentWidget()->GetWidgetChildByName(".btn_emo")->SetColor(Color3B::WHITE);
	});

	_widget = p_layout_list_friend_chat->GetWidgetChildByName(".search_friend.name_search_input");
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".btn_clear"));
	w_tx->InitCallBack(
		nullptr,
		nullptr,
		[this, _widget]()
	{
		p_current_friend_chat_search = static_cast<TextFieldWidget*>(_widget)->GetText();
		p_current_idx_search_friend_chat = 0;
		size_t number = GetNumberFriendChat(p_current_friend_chat_search);
		static_cast<ListViewTableWidget*>(p_list_friend_chat)->SetNumberSizeOfCell(number, true);
	},
		[this, _widget]()
	{
		p_current_friend_chat_search = static_cast<TextFieldWidget*>(_widget)->GetText();
		p_current_idx_search_friend_chat = 0;
		size_t number = GetNumberFriendChat(p_current_friend_chat_search);
		static_cast<ListViewTableWidget*>(p_list_friend_chat)->SetNumberSizeOfCell(number, true);
	},
		nullptr
		);

}

int FriendScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	static_cast<IwinListViewTable*>(p_list_friend_main)->UpdateReload(dt);
	static_cast<IwinListViewTable*>(p_list_friend_chat)->UpdateReload(dt);
	if (p_stage_get_list_friend >= 2)
	{
		int total_friend_main = GetNumberFriend();
		if (total_friend_main > 0)
		{
			p_list_friend_main->GetParentWidget()->GetWidgetChildAtIdx(2)->Visible(false);
		}
		else
		{
			p_list_friend_main->GetParentWidget()->GetWidgetChildAtIdx(2)->Visible(true);
		}

		static_cast<ListViewTableWidget*>(p_list_friend_main)->SetNumberSizeOfCell(total_friend_main , true);
		p_stage_get_list_friend = 0;

		static_cast<TextWidget*>(p_count_friend_layout->GetWidgetChildAtIdx(0))->SetText(StringUtils::format(
			(LANG_STR("friend_list") + "( %s )").GetString() , std::to_string(total_friend_main).c_str()), true
		);
	}

	return 1;
}

void FriendScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_back")
		{
			if (p_main_friend_layout->Visible())
			{
				ScrMgr->SwitchToMenu(p_menu_come_from, FRIEND_SCREEN);
			}
			else if (p_chat_friend_layout->Visible())
			{
				p_main_friend_layout->SetActionCommandWithName("ON_SHOW");
				p_chat_friend_layout->SetActionCommandWithName("ON_HIDE");

				p_top_panel->GetWidgetChildAtIdx(1)->SetActionCommandWithName("ON_SHOW");
				p_top_panel->GetWidgetChildAtIdx(3)->SetActionCommandWithName("ON_HIDE");
			}
		}
		else if (name == "btn_add_friend")
		{
			p_search_player_layout->GetParentWidget()->Visible(true);
			p_search_player_layout->SetActionCommandWithName("ON_SHOW");
		}
		else if (name == "btn_close_panel_search")
		{
			p_search_player_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_search_player_layout->ForceFinishAction();
				this->p_search_player_layout->GetParentWidget()->Visible(false);

			}));
		}
		else if (name == "btn_search_player")
		{
			auto w_text = _widget->GetParentWidget()->GetWidgetChildAtIdx(3)->GetWidgetChildAtIdx(0);
			auto current_text = static_cast<TextFieldWidget*>(w_text)->GetText();
			size_t leng_text = current_text.LengthUTF();
			if (leng_text <= 4)
			{

				ScrMgr->OnShowOKDialog("short_name");
			}
			else
			{
				GameController::searchNick = current_text.GetString();
				GlobalService::searchNick(current_text.GetString());
			}

		}
		else if (name == "btn_emo")
		{
			if (!GetCommonScr->IsLayerVisible(COMMON_LAYER::EMOTION_LAYER))
			{
				GetCommonScr->OnShowLayer(COMMON_LAYER::EMOTION_LAYER);
				GetCommonScr->SetCallBackEmotionBoard(CC_CALLBACK_1(FriendScreen::handleChatEmotion, this));
				_widget->SetColor(Color3B::YELLOW);
				//static_cast<TextFieldWidget*>(p_chat_friend_input)->SetAttachIME();
			}
			else
			{
				GetCommonScr->OnHideLayer(COMMON_LAYER::EMOTION_LAYER);
				_widget->SetColor(Color3B::WHITE);
			}
		}
		else if (name == "list_friend")
		{
			if (_have_list_friend_chat_appear)
			{
				OnRootLayerMove(true);
			}
			else
			{
				OnRootLayerMove();
			}
		}
		else if (name == "btn_send_friend_msg")
		{
			auto w = _widget->GetParentWidget()->GetWidgetChildByName(".chat_friend_input");
			auto text = static_cast<TextFieldWidget*>(w)->GetText();
			if (text != "")
			{
				if (p_current_friend_chat_to)
				{
					GlobalService::chatto_old(p_current_friend_chat_to->IDDB, text.GetString());
					SaveChatMgr->SetChatWithID(p_current_friend_chat_to->IDDB);
				}
				//insert to database chat
				//
				OnParseIntoChat(text.GetString(), 0);
				SaveChatMgr->SaveToDBChat(GameController::myInfo->IDDB, text);

				//
				static_cast<TextFieldWidget*>(w)->ClearText();
			}
		}
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		auto w_tx = static_cast<IWIN_TF*>(_widget);
		w_tx->HandleEventTextField(name);
	}

}

void FriendScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(FRIEND_SCREEN);
	p_menu_show_next = MENU_NONE;

	if (ScrMgr->GetGamePlayMgr() && ScrMgr->GetGamePlayMgr()->GetCurrentGameScreen() != nullptr)
	{
		ScrMgr->GetGamePlayMgr()->GetCurrentGameScreen()->setVisible(true);
	}

	GameMgr->updateUnreadMsg();
}

void FriendScreen::ShowChatToFriend(Player * p)
{
	bool first_chat = false;
	if (p_current_friend_chat_to == nullptr)
	{
		p_current_friend_chat_to = p;
		p_list_chat->ClearChild();
		first_chat = true;
	}
	if (p_current_friend_chat_to->IDDB != p->IDDB || first_chat)
	{
		p_list_chat->ClearChild();
		SaveChatMgr->LoadDBChatWithID(p->IDDB, [this](std::map<long long, ChatDetail> list_chat) {
			if (list_chat.size() <= 0)
			{
				return;
			}
			for (auto it : list_chat)
			{
				if (it.second.id == GameController::myInfo->IDDB)
				{
					this->OnParseIntoChat(it.second.msg, 0);
				}
				else
				{
					this->OnParseIntoChat(it.second.msg, 1);
				}
			}
			list_chat.clear();
		});
	}
	p_current_friend_chat_to = p;
	auto unread_msg = GetUnreadMessageFromFriend(p_current_friend_chat_to->IDDB, true);
	if (unread_msg.size() > 0)
	{
		for (auto p : unread_msg)
		{
			OnParseIntoChat(p->message, 1);
			delete p;
		}
		unread_msg.clear();
		GameMgr->updateUnreadMsg();
	}
	static_cast<TextWidget*>(p_top_panel->GetWidgetChildAtIdx(3)->GetWidgetChildByName(".title_name"))->SetText(p_current_friend_chat_to->name, true);
}

std::vector<IwinMessage*> FriendScreen::GET_MESSAGE(int state)
{
	if (state == 0)
	{
		return p_ADDFRIEND_MESSAGE;
	}
	else if (state == 1)
	{
		return p_ADMIN_MESSAGE;
	}
	else if (state == 2)
	{
		return p_UNREAD_MESSAGE;
	}
}

int FriendScreen::GetNumberFriend()
{
	return (p_listPlayerFriend.size() + p_ADDFRIEND_MESSAGE.size());
}

void FriendScreen::REMOVE_MESSAGE(int state, int id)
{
	int idx = 0;
	if (state == 0)
	{
		for (auto msg : p_ADDFRIEND_MESSAGE)
		{
			if (msg->fromID == id)
			{
				delete msg;
				p_ADDFRIEND_MESSAGE.erase(p_ADDFRIEND_MESSAGE.begin() + idx);
				return;
			}
			idx++;
		}
	}
	else if (state == 1)
	{
		for (auto msg : p_ADMIN_MESSAGE)
		{
			if (msg->fromID == id)
			{
				delete msg;
				p_ADMIN_MESSAGE.erase(p_ADMIN_MESSAGE.begin() + idx);
				return;
			}
			idx++;
		}
	}
	else if (state == 2)
	{
		for (auto msg : p_UNREAD_MESSAGE)
		{
			if (msg->fromID == id)
			{
				delete msg;
				p_UNREAD_MESSAGE.erase(p_UNREAD_MESSAGE.begin() + idx);
				return;
			}
			idx++;
		}
	}
}

void FriendScreen::PUSH_SINGLE_MESSAGE(int state, ChatMessage* msg)
{
	if (state == 0)
	{
		p_ADDFRIEND_MESSAGE.push_back(msg);

		int id_avatar = msg->avatar;
		int IDDB = msg->fromID;
		GameMgr->HandleAvatarDownload(id_avatar, msg->fromName,
				[this, IDDB, id_avatar](void * data, void * str, int tag)
			{
				Image * img = (Image *)data;
				if (img && img->getWidth() <= 0)
				{
					CC_SAFE_DELETE(img);
					return;
				}
				Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
				this->p_list_avatar_download.insert(IDDB, sprite_img);

				auto widget_lb = static_cast<ListViewTableWidget*>(p_list_friend_main);
				if (this->p_list_avatar_download.size() <= (int)widget_lb->GetListWidgetSample().size())
				{
					widget_lb->UpdateCellAtIdx(this->p_list_avatar_download.size() - 1);
				}
			});

		p_stage_get_list_friend++;
	}
	else
	{
		p_ADMIN_MESSAGE.push_back(msg);
	}
}

void FriendScreen::PUSH_MESSAGE(int state, std::vector<IwinMessage*> vec)
{
	if (state == 0)
	{
        for(auto msg : p_ADDFRIEND_MESSAGE)
        {
            delete msg;
        }
        p_ADDFRIEND_MESSAGE.clear();
        
        
		for (auto msg : vec)
		{
			p_ADDFRIEND_MESSAGE.push_back(msg);

			int id_avatar = msg->avatar;
			int IDDB = msg->fromID;
			GameMgr->HandleAvatarDownload(id_avatar, msg->fromName,
				[this, IDDB, id_avatar](void * data, void * str, int tag)
			{
				Image * img = (Image *)data;
				if (img && img->getWidth() <= 0)
				{
					CC_SAFE_DELETE(img);
					return;
				}
				Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data , str);
				this->p_list_avatar_download.insert(IDDB, sprite_img);

				auto widget_lb = static_cast<ListViewTableWidget*>(p_list_friend_main);
				if (this->p_list_avatar_download.size() <= (int)widget_lb->GetListWidgetSample().size())
				{
					widget_lb->UpdateCellAtIdx(this->p_list_avatar_download.size() - 1);
				}
			});
		}

		p_stage_get_list_friend++;
	}
	else
	{
        for(auto msg : p_ADMIN_MESSAGE)
        {
            delete msg;
        }
        p_ADMIN_MESSAGE.clear();
        
		for (auto msg : vec)
		{
			p_ADMIN_MESSAGE.push_back(msg);
		}
	}
}

void FriendScreen::OnParseFriendList()
{
	for (Player * p : p_listPlayerFriend)
	{
		int id_avatar = p->avatar;
		int IDDB = p->IDDB;

		GameMgr->HandleAvatarDownload(id_avatar, p->name,
			[this, IDDB, id_avatar](void * data, void * str, int tag)
		{
			Image * img = (Image *)data;
			if (img && img->getWidth() <= 0)
			{
				CC_SAFE_DELETE(img);
				return;
			}
			Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
			this->p_list_avatar_download.insert(IDDB, sprite_img);
			auto widget_lb = static_cast<ListViewTableWidget*>(p_list_friend_main);
			if (this->p_list_avatar_download.size() <= (int)widget_lb->GetListWidgetSample().size())
			{
				widget_lb->UpdateCellAtIdx(this->p_list_avatar_download.size() - 1);
			}
		});
	}

	p_stage_get_list_friend++;

}

void FriendScreen::ForceUpdateListFriendChat()
{
	p_list_friend_chat->Visible(false);
	p_list_friend_chat->GetParentWidget()->GetWidgetChildAtIdx(4)->Visible(true);
	p_list_friend_chat->GetParentWidget()->GetWidgetChildAtIdx(4)->SetActionCommandWithName("ROTATE");

	static_cast<IwinListViewTable*>(p_list_friend_chat)->SetHaveReload([this]() {
		static_cast<ListViewTableWidget*>(p_list_friend_chat)->SetNumberSizeOfCell(p_listFriendChat.size(), true);
		p_list_friend_chat->Visible(true);
		p_list_friend_chat->GetParentWidget()->GetWidgetChildAtIdx(4)->Visible(false);
	});
}

void FriendScreen::OnReceiveFriendList()
{
 	//PASSERT2(false, "not yet handle this, throw assert to remind!");
	p_listPlayerFriend = GameController::FriendList;
	p_current_friend_chat_to = nullptr;
	SAFE_DELETE_VECTOR(p_listFriendChat);
	for (auto p : p_listPlayerFriend)
	{
		p_listFriendChat.push_back(new iWinPlayer(p));
	}

	if (p_chat_friend_layout->Visible())
	{
		ForceUpdateListFriendChat();
	}
	else
	{
		p_list_friend_main->Visible(false);
		p_list_friend_main->GetParentWidget()->GetWidgetChildAtIdx(2)->Visible(false);
		p_list_friend_main->GetParentWidget()->GetWidgetChildAtIdx(3)->Visible(true);
		p_list_friend_main->GetParentWidget()->GetWidgetChildAtIdx(3)->SetActionCommandWithName("ROTATE");

		static_cast<IwinListViewTable*>(p_list_friend_main)->SetHaveReload([this]() {
			OnParseFriendList();
			p_list_friend_main->Visible(true);
			p_list_friend_main->GetParentWidget()->GetWidgetChildAtIdx(3)->Visible(false);
		});
	}

}

void FriendScreen::ParseListFriendChat(WidgetEntity* _widget, iWinPlayer * p)
{
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(p->name);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".msg_id"))->SetText(p->lastMEssage);

	if (p->state_ready == 0)
	{
		_widget->GetWidgetChildByName(".ico_online")->Visible(false);
		_widget->GetWidgetChildByName(".ico_offline")->Visible(true);
		_widget->GetWidgetChildByName(".ico_playing")->Visible(false);
	}
	else
	{
		_widget->GetWidgetChildByName(".ico_online")->Visible(true);
		_widget->GetWidgetChildByName(".ico_offline")->Visible(false);
		_widget->GetWidgetChildByName(".ico_playing")->Visible(false);
	}
}

void FriendScreen::ParseListFriend(WidgetEntity* _widget, Player * p)
{
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(p->name);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".msg_id"))->SetText(p->userProfile->status);

	_widget->GetWidgetChildAtIdx(8)->Visible(false);
	auto unread_msg = GetUnreadMessageFromFriend(p->IDDB);
	if (unread_msg.size() > 0)
	{
		_widget->GetWidgetChildAtIdx(9)->Visible(true);
		static_cast<TextWidget*>(_widget->GetWidgetChildAtIdx(9)->GetWidgetChildAtIdx(0))->SetText(std::to_string(unread_msg.size()), true);
	}
	else
	{
		_widget->GetWidgetChildAtIdx(9)->Visible(false);
	}
	
	if (p->isReady)
	{
		if (p->onlineInfo != "")
		{
			_widget->GetWidgetChildByName(".ico_online")->Visible(false);
			_widget->GetWidgetChildByName(".ico_playing")->Visible(true);
		}
		else
		{
			_widget->GetWidgetChildByName(".ico_online")->Visible(true);
			_widget->GetWidgetChildByName(".ico_playing")->Visible(false);

		}

		_widget->GetWidgetChildByName(".ico_offline")->Visible(false);
	}
	else
	{
		_widget->GetWidgetChildByName(".ico_online")->Visible(false);
		_widget->GetWidgetChildByName(".ico_offline")->Visible(true);
		_widget->GetWidgetChildByName(".ico_playing")->Visible(false);
	}
}

void FriendScreen::ParseListWaitingFriend(WidgetEntity* _widget, ChatMessage* player)
{
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(player->fromName);
	static_cast<TextWidget*>(_widget->GetWidgetChildByName(".msg_id"))->SetText(player->message);
	auto list_btn = _widget->GetWidgetChildAtIdx(8);
	list_btn->Visible(true);
	//_widget->GetWidgetChildByName(".msg_id");
	_widget->GetWidgetChildByName(".ico_online")->Visible(true);
	_widget->GetWidgetChildByName(".ico_offline")->Visible(false);
	_widget->GetWidgetChildByName(".ico_playing")->Visible(false);
	static_cast<ButtonWidget*>(list_btn->GetWidgetChildByName(".btn_accept_friend_request"))->SetActionBtn([this, player, _widget]() 
	{
			GlobalService::friendAcceptRequest(player->fromID);
			_widget->GetWidgetChildAtIdx(8)->Visible(false);
			REMOVE_MESSAGE(0, player->fromID);
			GameMgr->updateUnreadMsg();
	});
	static_cast<ButtonWidget*>(list_btn->GetWidgetChildByName(".btn_deny_friend_request"))->SetActionBtn([this, player, _widget]()
	{
			GlobalService::friendDenyRequest(player->fromID);
			REMOVE_MESSAGE(0, player->fromID);
			GameMgr->updateUnreadMsg();
	});
}

void FriendScreen::OnParseListFriendMain(WidgetEntity* _widget , int idx, RKString name_fill)
{
	int iddb = -1;

	if (name_fill != "")
	{
		if (p_current_idx_search_parse < (int)p_ADDFRIEND_MESSAGE.size())
		{
			for (int i = p_current_idx_search_parse; i < (int)p_ADDFRIEND_MESSAGE.size() ; i++)
			{
				auto p = p_ADDFRIEND_MESSAGE[i];
				p_current_idx_search_parse++;
				if (RKString(p->fromName).Contains(name_fill))
				{
					iddb = p->fromID;
					ParseListWaitingFriend(_widget, p);
					break;
				}
			}
		}

		if(p_current_idx_search_parse >= (int)p_ADDFRIEND_MESSAGE.size())
		{
			for (int i = (p_current_idx_search_parse - (int)p_ADDFRIEND_MESSAGE.size()); i < (int)p_listPlayerFriend.size(); i++ )
			{
				auto p = p_listPlayerFriend[i];
				p_current_idx_search_parse++;
				if (p_current_idx_search_parse >= (int)(p_ADDFRIEND_MESSAGE.size() + p_listPlayerFriend.size()))
				{
					p_current_idx_search_parse = 0;
				}
				if (RKString(p->name).Contains(name_fill))
				{
					iddb = p->IDDB;
					ParseListFriend(_widget, p);
					break;
				}
			}
		}
	}
	else
	{
		if (idx < (int)p_ADDFRIEND_MESSAGE.size())
		{
			ChatMessage* player = p_ADDFRIEND_MESSAGE[idx];
			iddb = player->fromID;
			ParseListWaitingFriend(_widget, player);
		}
		else
		{
			int id = idx - p_ADDFRIEND_MESSAGE.size();
			if (id >= p_listPlayerFriend.size())
			{
				id = p_listPlayerFriend.size() - 1;
			}
			Player * p = p_listPlayerFriend[id];
			if (p)
			{
				iddb = p->IDDB;
				ParseListFriend(_widget, p);
			}
		}
	}


	auto panel_avatar = _widget->GetWidgetChildByName(".panel_avatar_clip");
	if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(iddb) != p_list_avatar_download.end())
	{
		this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(iddb), 100);
	}
	
	_widget->GetWidgetChildByName(".bot_line")->Visible(idx % 2 == 0);
	
}

void FriendScreen::OnParseListFriendChat(WidgetEntity* _widget, int idx, RKString name_fill)
{
	int iddb = -1;

	if (name_fill != "")
	{
		for (int i = p_current_idx_search_friend_chat; i < (int)p_listFriendChat.size(); i++)
		{
			auto p = p_listFriendChat[i];
			p_current_idx_search_friend_chat++;
			if (p_current_idx_search_friend_chat >= (int)(p_listFriendChat.size()))
			{
				p_current_idx_search_friend_chat = 0;
			}
			if (RKString(p->name).Contains(name_fill))
			{
				iddb = p->IDDB;
				ParseListFriendChat(_widget->GetWidgetChildAtIdx(1), p);
				Vec2 current_pos = _widget->GetPosition();
				_widget->SetPosition(Vec2(0, current_pos.y));
				break;
			}
		}
	}
	else
	{
		auto  p = p_listFriendChat[idx];
		iddb = p->IDDB;
		ParseListFriendChat(_widget->GetWidgetChildAtIdx(1), p);

		Vec2 current_pos = _widget->GetPosition();
		_widget->SetPosition(Vec2(0, current_pos.y));
	}


	auto panel_avatar = _widget->GetWidgetChildAtIdx(1)->GetWidgetChildByName(".panel_avatar_clip");
	if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(iddb) != p_list_avatar_download.end())
	{
		this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(iddb), 100);
	}
}

size_t FriendScreen::GetNumberFriendChat(RKString name)
{
	size_t number = 0;
	for (auto p : p_listPlayerFriend)
	{
		if (RKString(p->name).Contains(name))
		{
			number++;
		}
	}
	return number;
}

size_t FriendScreen::GetNumberFriendContainName(RKString name)
{
	size_t number = 0; 
	for (auto p : p_ADDFRIEND_MESSAGE)
	{
		if (RKString(p->fromName).Contains(name))
		{
			number++;
		}
	}
	number += GetNumberFriendChat(name);
	return number;
}

void FriendScreen::ClearComponent()
{
	static_cast<ListViewTableWidget*>(p_list_friend_main)->ResetListView();
	static_cast<ListViewTableWidget*>(p_list_friend_chat)->ResetListView();

	SAFE_DELETE_VECTOR(p_ADDFRIEND_MESSAGE);
	SAFE_DELETE_VECTOR(p_ADMIN_MESSAGE);
	SAFE_DELETE_VECTOR(p_UNREAD_MESSAGE);
	SAFE_DELETE_VECTOR(p_listRecentChat);
	SAFE_DELETE_VECTOR(p_listFriendChat);

	p_listPlayerFriend.clear();

	for (auto it : p_list_avatar_download)
	{
		it.second->removeFromParentAndCleanup(true);
		it.second->release();
	}

	p_list_avatar_download.clear();

	p_stage_get_list_friend = 0;

	p_current_friend_search = "";
	p_current_friend_chat_search = "";
	p_current_idx_search_parse = 0;
	p_current_idx_search_friend_chat = 0;

	p_current_friend_chat_to = nullptr;
}

void FriendScreen::SetActionAfterShow(int action)
{
	this->PushEvent([action](BaseScreen * scr) {
		FriendScreen * fscr = (FriendScreen *)scr;
		if (action == 0)
		{
			fscr->p_search_player_layout->GetParentWidget()->Visible(true);
			fscr->p_search_player_layout->SetActionCommandWithName("ON_SHOW");
		}
		else if (action == 1)
		{

		}
	}, TIME_ACTION);
}

int FriendScreen::GetStateFriendMenu()
{
	if (p_search_player_layout && p_search_player_layout->GetParentWidget()->Visible())
	{
		return 0;
	}
	else
	{
		if (p_chat_friend_layout && p_chat_friend_layout->Visible())
		{
			return 1;
		}
	}
	return 2;
}

void FriendScreen::onReceiveServerWarning(RKString msg)
{
	ScrMgr->OnShowErrorDialog(msg);
	PLAY_SOUND("SFX_BUZZ");
}

void FriendScreen::handleChatEmotion(int idx)
{
	RKString cur_text = static_cast<TextFieldWidget*>(p_chat_friend_input)->GetText();
	cur_text += Utils::getEmotionStr(idx);
	static_cast<TextFieldWidget*>(p_chat_friend_input)->SetText(cur_text);
}

void FriendScreen::OnCellLayerMove(WidgetEntity * widget , bool move_back)
{
	float delta_max = 80.f * GetGameSize().width / this->GetDesignSize().x;
	if (move_back == false)
	{
		auto action = Sequence::createWithTwoActions(MoveTo::create(0.1f, Vec2(delta_max, 0)), CallFunc::create([this]() {
			//_have_list_friend_chat_appear = true;
			_state_press_layout_chat = -1;
		}));
		widget->GetResource()->runAction(action);
	}
	else
	{
		auto action = Sequence::createWithTwoActions(MoveTo::create(0.1f, Vec2(0, 0)), CallFunc::create([this]() {
			_state_press_layout_chat = -1;
			//_have_list_friend_chat_appear = false;
		}));
		widget->GetResource()->runAction(action);
	}
}

void FriendScreen::OnRootLayerMove(bool move_back)
{
	if (_on_move_layer)
		return;
	if (move_back == false)
	{
		_on_move_layer = true;
		p_layout_list_friend_chat->Visible(true);
		auto action = Sequence::createWithTwoActions(MoveTo::create(0.2f, Vec2(-GetGameSize().width / 2.f, 0)), CallFunc::create([this]() {
			_have_list_friend_chat_appear = true;
			_state_press_layout_chat = -1;
			_on_move_layer = false;

			this->PushEvent([](BaseScreen * scr) {
				FriendScreen * fscr = (FriendScreen*)scr;
				fscr->ForceUpdateListFriendChat();
			});
		}));
		_root_layer->runAction(action);
	}
	else
	{
		_on_move_layer = true;
		auto action = Sequence::createWithTwoActions(MoveTo::create(0.2f, Vec2(0, 0)), CallFunc::create([this]() {
			_state_press_layout_chat = -1;
			_have_list_friend_chat_appear = false;
			p_layout_list_friend_chat->Visible(false);
			_on_move_layer = false;
		}));
		_root_layer->runAction(action);
	}
}

void FriendScreen::OnTouchMenuBegin(const cocos2d::Point & p)
{
	//if (_state_press_layout_chat == -1)
	{
		_state_press_layout_chat = 0;
		_previous_pos = p;
		if (_have_list_friend_chat_appear)
		{
			_previous_pos_cell = p;
		}
		_delta_move = Vec2(0, 0);
	}
}

void FriendScreen::OnTouchMenuEnd(const cocos2d::Point & p)
{
	_state_press_layout_chat = -1;

	if (p_chat_friend_layout->Visible() && _root_layer->getBoundingBox().containsPoint(p))
	{
		_state_press_layout_chat = 2;
		if (abs(_delta_move.x) < _root_layer->GetSize().x / 25.f &&  _root_layer->getPosition().x < -GetGameSize().width / 8.f)
		{
			OnRootLayerMove(true);
			_delta_move = Vec2(0, 0);
			return;
		}

		if (_delta_move.x > 0)
		{
			if (abs(_delta_move.x) >= GetGameSize().width / 8.f)
			{
				OnRootLayerMove(true);
			}
			else
			{
				OnRootLayerMove();
			}
		}
		else if (_delta_move.x < 0 )
		{
			if (abs(_delta_move.x) >= GetGameSize().width / 8.f)
			{
				OnRootLayerMove();
			}
			else
			{
				OnRootLayerMove(true);
			}
		}
		_delta_move = Vec2(0, 0);
	}
	else if (p_chat_friend_layout->Visible() && _have_list_friend_chat_appear && _widget_cell_on_focus)
	{
		float delta_max = 80.f * GetGameSize().width / this->GetDesignSize().x;
		//if (abs(_delta_move_cell.y) < _widget_cell_on_focus->GetSize().y / 3.f)
		{
			if (_delta_move_cell.x < 0)
			{
				if (abs(_delta_move_cell.x) >= delta_max / 2.f)
				{
					OnCellLayerMove(_widget_cell_on_focus, true);
				}
				else
				{
					OnCellLayerMove(_widget_cell_on_focus);
				}
			}
			else if (_delta_move_cell.x > 0)
			{
				if (abs(_delta_move_cell.x) >= delta_max / 2.f)
				{
					OnCellLayerMove(_widget_cell_on_focus);
				}
				else
				{
					OnCellLayerMove(_widget_cell_on_focus, true);
				}
			}
		}

		_delta_move_cell = Vec2(0, 0);
		_widget_cell_on_focus = nullptr;
	}
}

void FriendScreen::OnTouchMenuHold(const cocos2d::Point & p)
{
	if (_state_press_layout_chat == 0 
		&& p_chat_friend_layout->Visible() 
		&& _root_layer->getBoundingBox().containsPoint(p) 
		&& p.x > (_root_layer->getContentSize().width * 2 / 5) && p.x < (_root_layer->getContentSize().width * 3 / 5))
	{
		Vec2 delta = Vec2(p.x - _previous_pos.x, p.y - _previous_pos.y);
		_delta_move.x += delta.x;
		Vec2 pre_pos = _root_layer->getPosition();
		Vec2 next_pos = Vec2(pre_pos.x + delta.x, pre_pos.y);
		if (next_pos.x > 0)
		{
			next_pos.x = 0;
		}
		else if (next_pos.x < -GetGameSize().width / 2.f)
		{
			next_pos.x = -GetGameSize().width / 2.f;
		}
		if (next_pos.x >= 0)
		{
			p_layout_list_friend_chat->Visible(false);
		}
		else
		{
			p_layout_list_friend_chat->Visible(true);
		}

		_root_layer->SetPosition(next_pos);

		_previous_pos = p;
	}
	else if (_state_press_layout_chat == 0 && p_chat_friend_layout->Visible() && _have_list_friend_chat_appear && _widget_cell_on_focus)
	{
		Vec2 delta = Vec2(p.x - _previous_pos_cell.x, p.y - _previous_pos_cell.y);
		_delta_move_cell = Vec2(_delta_move_cell.x + delta.x, _delta_move_cell.y + delta.y);
	
	}
}

void FriendScreen::OnParseIntoChat(RKString msg, int from)
{
	float delta_scale = GetGameSize().width / this->GetDesignSize().x;
	float min_size_y = 40 * delta_scale;
	float delta_div_text = 10 * delta_scale;

	auto list_chat = static_cast<ListViewWidget*>(p_list_chat);
	auto widget = list_chat->CreateWidgetReferNoPushBack(from);


	auto bg = widget->GetWidgetChildByName(".bg");
	auto w_text = static_cast<TextWidget*>(widget->GetWidgetChildByName(".msg"));


	w_text->SetText(msg, true);

	auto actual_size_text = w_text->GetActualContainSize();
	//w_text->SetTextAreaSize(actual_size_text);

	float target_y = actual_size_text.y + 2 * delta_div_text;
	if (target_y < min_size_y)
	{
		target_y = min_size_y;
	}

	widget->SetSize(Vec2(GetGameSize().width, target_y));
	bg->SetSize(Vec2(actual_size_text.x + 3 * delta_div_text, target_y));
	if (from == 0)
	{
		bg->SetPosition(widget->GetSize());
		w_text->SetPosition(Vec2(widget->GetSize().x - delta_div_text * 2.f, widget->GetSize().y - delta_div_text));
	}
	else if (from == 1)
	{
		bg->SetPosition(Vec2(0,widget->GetSize().y));
		w_text->SetPosition(Vec2(delta_div_text * 2.f, widget->GetSize().y - delta_div_text));
	}

	list_chat->PushBackItem(widget);
	static_cast<ListView*>(list_chat->GetResource())->jumpToBottom();
}

void FriendScreen::onReceiveMessage(IwinMessage * msg)
{
	SaveChatMgr->SetChatWithID(msg->fromID);
	SaveChatMgr->SaveToDBChat(msg->fromID, msg->message, msg->time);
	if (p_current_friend_chat_to && p_current_friend_chat_to->IDDB == msg->fromID && p_chat_friend_layout->Visible())
	{
		OnParseIntoChat(msg->message, 1);
		delete msg;
	}
	else
	{
		p_UNREAD_MESSAGE.push_back(msg);
		//swap child in friend
		if (p_listPlayerFriend.size() > 0)
		{
			Player * p = nullptr;
			int idx = 0;
			for (auto fp : p_listPlayerFriend)
			{
				if (fp->IDDB == msg->fromID)
				{
					p_listPlayerFriend.erase(std::find(p_listPlayerFriend.begin(), p_listPlayerFriend.end(), fp));
					idx++;
					p = fp;
					break;
				}
			}
			if (p)
			{
				p_listPlayerFriend.insert(p_listPlayerFriend.begin(), p);
				static_cast<IwinListViewTable*>(p_list_friend_main)->UpdateCellAtIdx(p_ADDFRIEND_MESSAGE.size() + 0);
				if (idx != 0)
				{
					static_cast<IwinListViewTable*>(p_list_friend_main)->UpdateCellAtIdx(p_ADDFRIEND_MESSAGE.size() + idx);
				}
			}
		}
	}
}

std::vector<ChatMessage*>	FriendScreen::GetUnreadMessageFromFriend(int iddb, bool remove_self /*false*/)
{
	std::vector<ChatMessage*> vec;
	for (auto msg : p_UNREAD_MESSAGE)
	{
		if (msg->fromID == iddb)
		{
			vec.push_back(msg);
		}
	}
	if (remove_self)
	{
		for (auto msg : vec)
		{
			auto it = std::find(p_UNREAD_MESSAGE.begin(), p_UNREAD_MESSAGE.end(), msg);
			p_UNREAD_MESSAGE.erase(it);
		}
	}

	return vec;
}
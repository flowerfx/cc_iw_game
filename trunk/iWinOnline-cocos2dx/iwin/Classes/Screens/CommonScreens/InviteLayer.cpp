#include "InviteLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wCheckBox.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"
#include "Screens/MainScreen.h"
#define DEFAULT_TIME 5.f
InviteLayer::InviteLayer()
{
	p_panel_invite = nullptr;
	p_check_deny_invite_layout = nullptr;
	p_text_message_invite = nullptr;
	avatar_user_invite_layout = nullptr;

	_time_to_close = DEFAULT_TIME;

}

InviteLayer::~InviteLayer()
{

}

int InviteLayer::Update(float dt)
{
	if (!p_panel_invite->GetParentWidget()->Visible())
		return 0;

	_time_to_close -= dt;
	if (_time_to_close <= 0)
	{
		OnHideInvite(CallFunc::create([this]() {
			this->p_panel_invite->ForceFinishAction();
			this->p_panel_invite->GetParentWidget()->Visible(false);
		}));
		_time_to_close = DEFAULT_TIME;
	}
	return 1;
}

bool InviteLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;

	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_agree_invite")
		{
			p_panel_invite->GetParentWidget()->Visible(false);

			_base_screen->PushEvent([this](BaseScreen * scr) {
				ScrMgr->JoinBoard(_roomId, _boardId, _password.GetString());
			});
			return true;
		}
		else if (name == "btn_deny_invite")
		{
			OnHideInvite(CallFunc::create([this]() {
				this->p_panel_invite->ForceFinishAction();
				this->p_panel_invite->GetParentWidget()->Visible(false);
			}));
			return true;
		}
		else if (name == "btn_close_invite")
		{
			OnHideInvite(CallFunc::create([this]() {
				this->p_panel_invite->ForceFinishAction();
				this->p_panel_invite->GetParentWidget()->Visible(false);
			}));
			return true;
		}
	}
	else if (type_widget == UI_TYPE::UI_CHECK_BOX)
	{
		if (name == "btn_check_invite_friend_UNSELECTED")
		{
			ScrMgr->SaveDataInt(STR_SAVE_INVITE_STATE, 1);
			ScrMgr->SaveTheGame();
			GlobalService::sendStaticInviviteToPlayGame(true);
			static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->SetBtnInviteNotifySelected(true);
			return true;
		}
		else if (name == "btn_check_invite_friend_SELECTED")
		{
			ScrMgr->SaveDataInt(STR_SAVE_INVITE_STATE, 0);
			ScrMgr->SaveTheGame();
			GlobalService::sendStaticInviviteToPlayGame(false);
			static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->SetBtnInviteNotifySelected(false);
			return true;
		}

	}

	return false;
}

void InviteLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	_gameId = 0;
	_roomId = 0;
	_boardId = 0;
	_avatarId = 0;
	_password = "";
	_message = "";

	p_panel_invite = _base_screen->GetWidgetChildByName("common_screen.banner_invite_layout.layout_invite");
	p_check_deny_invite_layout = p_panel_invite->GetWidgetChildByName(".check_deny_layout");
	p_text_message_invite = p_panel_invite->GetWidgetChildByName(".title_contain");
	avatar_user_invite_layout = p_panel_invite->GetWidgetChildByName(".panel_avatar");
}

void InviteLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	OnSetDataInvite(_gameId, _roomId, _boardId, _password, _message, _avatarId);
}


void InviteLayer::OnSetDataInvite(int gameid, int roomid, int boardid, RKString pass, RKString message, int idavatar)
{
	int current_data = 1;
	ScrMgr->GetDataSaveInt(STR_SAVE_INVITE_STATE, current_data);
	if (current_data == 0)
	{
		return;
	}

	_gameId = gameid;
	_roomId = roomid;
	_boardId = boardid;
	_avatarId = idavatar;
	_password = pass;
	_message = message;
	static_cast<CheckBoxWidget*>(p_check_deny_invite_layout->GetWidgetChildByName(".btn_check_invite_friend"))->OnSetSelected(false);

	static_cast<TextWidget*>(p_text_message_invite)->SetText(message,true);

	GameMgr->HandleAvatarDownload(
		idavatar, "avatar_user_invite", [this](void * data, void * str, int tag)
	{
		_base_screen->ParseImgToPanel(this->avatar_user_invite_layout, data, str, tag);
	});

	if (p_panel_invite->GetParentWidget()->Visible() == false)
	{
		p_panel_invite->GetParentWidget()->Visible(true);
		p_panel_invite->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
			this->p_panel_invite->ForceFinishAction();
			if (p_panel_invite->GetPosition().y != 0)
			{
				auto cur_pos = p_panel_invite->GetPosition();
				p_panel_invite->SetPosition(Vec2(cur_pos.x, 0));
				_time_to_close = DEFAULT_TIME;
			}
		}));
	}
	else
	{
		auto cur_pos = p_panel_invite->GetPosition();
		p_panel_invite->SetPosition(Vec2(cur_pos.x, 0));
		_time_to_close = DEFAULT_TIME;
	}

}

bool InviteLayer::IsVisibleLayer()
{
	if (p_panel_invite)
	{
		return p_panel_invite->GetParentWidget()->Visible();
	}
	return false;
}

void InviteLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	/*OnHideInvite(CallFunc::create([this]() {
		this->p_panel_invite->ForceFinishAction();
		this->p_panel_invite->GetParentWidget()->Visible(false);
	}));*/
	this->p_panel_invite->GetParentWidget()->Visible(false);
}

void InviteLayer::OnHideInvite(CallFunc * call_back)
{
	p_panel_invite->SetActionCommandWithName("ON_HIDE", call_back);
}

#include "LeftUserProfileLayer.h"
#include "Network/Global/Globalservice.h"
#include "UI/AdvanceUI/wText.h"

#include "Screens/ProfileScreen.h"
#include "Screens/IAPScreen.h"
LeftUserProfileLayer::LeftUserProfileLayer()
{
	p_status_dirty = false;
}

LeftUserProfileLayer::~LeftUserProfileLayer()
{
	p_status_dirty = false;
}

void LeftUserProfileLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	auto p_page_view_profile = screen->GetWidgetChildByName("profile_screen.main_profile_layout");
	p_page_profile_left = p_page_view_profile->GetWidgetChildByName(".left_layout");
	p_panel_avatar = p_page_profile_left->GetWidgetChildByName(".panel_avatar");


	auto _widget = p_page_profile_left->GetWidgetChildByName(".player_status.text_input_status");
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_cmnd"));
	w_tx->InitCallBack(
		nullptr,
		[this, _widget]()
	{
		this->UpdateStatus_ToServer(static_cast<TextFieldWidget*>(_widget)->GetText());
	});
}

void LeftUserProfileLayer::UpdateStatus_ToServer(const RKString & status)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->status = status.GetString();
	p_status_dirty = true;
}

void LeftUserProfileLayer::FuncWhenDeactiveLayer()
{
	if (p_status_dirty)
	{
		if (GetUser)
		{
			GlobalService::UpdateStatus(GetUser->status);
		}
		p_status_dirty = false;
	}
}

bool LeftUserProfileLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "add_iwin_panel")
		{
			ScrMgr->SwitchToMenu(IAP_SCREEN, PROFILE_SCREEN);
			static_cast<IAPScreen*>(ScrMgr->GetMenuUI(IAP_SCREEN))->SetIAPStyle(0);
			return true;
		}
		else if (name == "add_ruby_panel")
		{
			ScrMgr->SwitchToMenu(IAP_SCREEN, PROFILE_SCREEN);
			static_cast<IAPScreen*>(ScrMgr->GetMenuUI(IAP_SCREEN))->SetIAPStyle(1);
			return true;
		}

	}
	return false;
}

void LeftUserProfileLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	this->p_page_profile_left->Visible(true);
}

void LeftUserProfileLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	this->p_page_profile_left->Visible(false);
}
bool LeftUserProfileLayer::IsVisibleLayer()
{
	return p_page_profile_left->Visible();
}
int LeftUserProfileLayer::Update(float dt)
{
	return 1;
}

WidgetEntity * LeftUserProfileLayer::GetStatusWidget()
{
	return p_page_profile_left->GetWidgetChildByName(".player_status");
}

void LeftUserProfileLayer::ParseUserToLayer()
{
	User * p_user = GetUser;
	if (p_user == nullptr)
	{
		PASSERT2(false, "user is null");
		return;
	}

	SetAccountName(p_user->nickName);
	SetLevel(GameController::myInfo->getLevel());
	SetHourPlayed(p_user->playingTime);
	SetWinNumber(p_user->money);
	SetRubyNumber(p_user->ruby);
	if (p_user->status != "")
	{
		SetStatusPlayer(p_user->status);
	}

	long money = p_user->money;
	long ruby = p_user->ruby;

	auto w_money = p_page_profile_left->GetWidgetChildByName(".add_iwin_panel");
	auto w_ruby = p_page_profile_left->GetWidgetChildByName(".add_ruby_panel");

	static_cast<TextWidget*>(w_money->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(money));
	static_cast<TextWidget*>(w_ruby->GetWidgetChildAtIdx(1))->SetText(Utils::formatNumber_dot(ruby));

	GameMgr->HandleAvatarDownload(
		p_user->avatarID, p_user->nickName, [this](void * data, void * str, int tag)
	{
		_base_screen->ParseImgToPanel(p_panel_avatar, data, str, tag);
	});
}

void LeftUserProfileLayer::SetStatusPlayer(const RKString & status)
{
	auto wi = p_page_profile_left->GetWidgetChildByName(".player_status.text_input_status");
	static_cast<TextFieldWidget*>(wi)->SetText(status);
}

void LeftUserProfileLayer::SetAccountName(const RKString & name)
{
	auto wi = p_page_profile_left->GetWidgetChildByName(".title_name_player");
	static_cast<TextWidget*>(wi)->SetText(name);
}

void LeftUserProfileLayer::SetLevel(int level)
{
	RKString number_ = std::to_string(level);
	auto wi = p_page_profile_left->GetWidgetChildByName(".panel_text.title_level_detail");
	static_cast<TextWidget*>(wi)->SetText(number_, true);
}

void LeftUserProfileLayer::SetWinNumber(int number_win)
{
	RKString number_ = std::to_string(number_win);
	auto wi = p_page_profile_left->GetWidgetChildByName(".add_iwin_panel");
	static_cast<TextWidget*>(wi->GetWidgetChildAtIdx(1))->SetText(number_, true);
}

void LeftUserProfileLayer::SetRubyNumber(int number_ruby)
{
	RKString number_ = std::to_string(number_ruby);
	auto wi = p_page_profile_left->GetWidgetChildByName(".add_ruby_panel");
	static_cast<TextWidget*>(wi->GetWidgetChildAtIdx(1))->SetText(number_, true);
}

void LeftUserProfileLayer::SetHourPlayed(const RKString & hour)
{
	if (hour == "")
		return;
	RKString number_ = (hour);
	auto wi = p_page_profile_left->GetWidgetChildByName(".panel_text.title_time_play_detail");
	static_cast<TextWidget*>(wi)->SetText(number_, true);
}
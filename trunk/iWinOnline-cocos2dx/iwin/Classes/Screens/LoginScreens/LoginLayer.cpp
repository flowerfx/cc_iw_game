#include "LoginLayer.h"
#include "UI/AdvanceUI/wText.h"

#include "Network/Global/Globalservice.h"
#include "Screens/LoginScreen.h"

#include "Features/FacebookMgr.h"
#include "Features/GGSignInMgr.h"
#include "Features/GCSignInMgr.h"

#include "RKThread_Code/RKThreadWrapper.h"

ThreadWrapper * p_thread_login = nullptr;

LoginLayer::LoginLayer()
{
	p_panel_login_layout = nullptr;

	p_username = "";
	p_password = "";

	p_already_auto_login = false;
}

LoginLayer::~LoginLayer()
{
	p_panel_login_layout = nullptr;
}

void LoginLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	p_panel_login_layout = _base_screen->GetWidgetChildByName("main_login_screen.login_layout");
	
	p_panel_check_name_1 = p_panel_login_layout->GetWidgetChildByName(".panel_check_name");
	p_btn_login = p_panel_login_layout->GetWidgetChildByName(".login_game_btn");


	auto p_alter_login_layout = p_panel_login_layout->GetWidgetChildByName(".alter_login_layout");
	auto p_btn_game_center = p_alter_login_layout->GetWidgetChildByName(".game_center_btn");
	p_btn_game_center->Visible(GetGCSignInMgr()->isSupportSignInGC());
	p_alter_login_layout->SetActionCommandWithName(GetGCSignInMgr()->isSupportSignInGC() ? "SHOW_3" : "SHOW_2");

	auto _widget = p_panel_login_layout->GetWidgetChildByName(".panel_name_input.text_input_1");
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_name_btn"));
	w_tx->InitCallBack(
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		[this, _widget]() 
	{
		_base_screen->PushEvent([this](BaseScreen * scr) 
		{
			static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_pass_input.title_1"))->SetAttachIME();
		});
	});

	_widget = p_panel_login_layout->GetWidgetChildByName(".panel_pass_input.title_1");
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_pass_btn"));
	w_tx->InitCallBack(
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		[this, _widget]() 
	{
		_base_screen->PushEvent([this](BaseScreen * scr)
		{
			this->OnLoginGame();
		});
	}
	);

}

bool LoginLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "login_game_btn")
		{

			TextFieldWidget* txt_input = static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_name_input.text_input_1"));
			TextFieldWidget* title_input = static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_pass_input.title_1"));

			OnLoginGame();

			return true;
		}
		else if (name == "btn_icon_warning_name_input_1")
		{
			auto panel = _widget->GetParentWidget()->GetWidgetChildAtIdx(1);
			if (panel->Visible())
			{
				panel->Visible(false);
			}
			else
			{
				panel->Visible(true);
			}
			return true;
		}

	}
	return false;
}

void LoginLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_panel_login_layout->SetActionCommandWithName("ON_SHOW", call_back ? CallFunc::create([this , call_back]() {
		p_panel_login_layout->ForceFinishAction();
		call_back();
	}) : nullptr);
}

void LoginLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_panel_login_layout->Visible(false);
}
bool LoginLayer::IsVisibleLayer()
{
	return p_panel_login_layout->Visible();
}

void LoginLayer::OnAutoLogin()
{
	if (p_already_auto_login)
		return;
	p_already_auto_login = true;

	int typelogin = 0;
	if (ScrMgr->GetDataSaveInt(STR_SAVE_TYPE_LOGIN, typelogin))
	{
		switch ((LOGIN_TYPE)typelogin)
		{
		case LOGIN_NONE:
		{
			RKString user_name = "";
			ScrMgr->GetDataSaveString(STR_SAVE_NAME_LOGIN, user_name);
			static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_name_input.text_input_1"))->SetText(user_name);
            GameController::setLoginType(AccountTypes::invalid);
			break;
		}
		case LOGIN_IWIN:
		{
			RKString user_name = "";
			ScrMgr->GetDataSaveString(STR_SAVE_NAME_LOGIN, user_name);
			RKString pass_word = "";
			ScrMgr->GetDataSaveString(STR_SAVE_PASS_LOGIN, pass_word);

			static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_name_input.text_input_1"))->SetText(user_name);
			static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_pass_input.title_1"))->SetText(pass_word);
			p_username = user_name;
			p_password = pass_word;
			static_cast<LoginScreen*>(_base_screen)->startWaitingDialog((LOGIN_TYPE)typelogin);

			LoginInternal();
			break;
		}
		case LOGIN_FACEBOOK:
		{
			static_cast<LoginScreen*>(_base_screen)->startWaitingDialog((LOGIN_TYPE)typelogin);

//			if (!GetFacebookMgr()->isLoggedIn())
//			{
//				GetFacebookMgr()->login();
//			}
//			else
//			{
//				GameMgr->Network_loginWithToken(MainGame::TYPE_FACEBOOK, GetFacebookMgr()->getAccessToken());
//			}
            Platform::FBUtils::loginFacebook([](bool res, void* data) {
                if(res)
                {
                    GameMgr->Network_loginWithToken(MainGame::TYPE_FACEBOOK, Platform::FBUtils::getAccessToken());
                     GameController::setLoginType(AccountTypes::facebook);
                }
            });
            
			break;
		}
		case LOGIN_GOOGLE_PLAY:
		{
			static_cast<LoginScreen*>(_base_screen)->startWaitingDialog((LOGIN_TYPE)typelogin);

			if (!GetGGSignInMgr()->isConnected())
			{
				GetGGSignInMgr()->signIn();
                GameController::setLoginType(AccountTypes::google);
			}
			else
			{
				GameMgr->Network_loginWithToken(MainGame::TYPE_GOOGLE, GetGGSignInMgr()->getGGToken());
			}
			break;
		}
		case LOGIN_APPLE:
		{
			static_cast<LoginScreen*>(_base_screen)->startWaitingDialog((LOGIN_TYPE)typelogin);

			if (!GetGCSignInMgr()->isSignedIn())
			{
				GetGCSignInMgr()->signIn();
                GameController::setLoginType(AccountTypes::appleId);
			}
			else
			{
				GetGCSignInMgr()->getAccessToken([=](std::string accessToken) {
					GameMgr->Network_loginWithToken(MainGame::TYPE_APPLEID, accessToken);
				});
			}
			break;
		}
		default:
			break;
		}
	}
}

void LoginLayer::LoginInternal()
{
	if (!p_thread_login)
	{
		SAFE_DELETE(p_thread_login);
		p_thread_login = new ThreadWrapper();

		p_thread_login->CreateThreadWrapper("thread_login_layer", ([](void * data) -> uint32
		{
			LoginLayer * scr = (LoginLayer*)data;
			p_thread_login->OnCheckUpdateThreadWrapper
			([scr]()
			{
				//scr->LoginInternal();
				GameMgr->Network_LoginToGame(scr->p_password.GetString(), scr->p_username.GetString());
				ScrMgr->SaveDataInt(STR_SAVE_TYPE_LOGIN, LOGIN_IWIN);
				GameController::setLoginType(AccountTypes::iwin);
			});
			return 1;
		}), (void*)this);
	}
	else
	{
		p_thread_login->OnWakeUpAndRunThread();
	}
	
	//ScrMgr->OnShowDialog("logging_in", "logging_in_wit", DIALOG_WAITING_BUTTON);

	//static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_pass_input.title_1"))->ClearText();
	//GameController::myInfo->setName(username.GetString());
	//ScrMgr->SwitchToMenu(MENU_LAYER::LOADING_SCREEN, LOGIN_SCREEN);
}

void LoginLayer::OnLoginGame()
{
	RKString username = static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_name_input.text_input_1"))->GetText();
	RKString pass = static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_pass_input.title_1"))->GetText();
	if (username == "" || pass == "")
		return;

	static_cast<LoginScreen*>(_base_screen)->startWaitingDialog(LOGIN_IWIN);

	p_password = pass;
	p_username = username;
	ScrMgr->SaveDataString(STR_SAVE_NAME_LOGIN, username);
	ScrMgr->SaveDataString(STR_SAVE_PASS_LOGIN, pass);
	ScrMgr->SaveTheGame();

	LoginInternal();

}

RKString LoginLayer::getUserName()
{
	return p_username;
}

RKString LoginLayer::getPassword()
{
	return p_password;
}

void	LoginLayer::ClearTextPass()
{
	static_cast<TextFieldWidget*>(p_panel_login_layout->GetWidgetChildByName(".panel_pass_input.title_1"))->ClearText();
}

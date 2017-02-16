#include "LoginScreen.h"

#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wCheckBox.h"

#include "UI/AdvanceUI/wListView.h"

#include "Features/IapMgr.h"
#include "Features/FacebookMgr.h"
#include "Features/GGSignInMgr.h"
#include "Features/GCSignInMgr.h"

#include "Network/Core/ServerMgr.h"
#include "Platform/Platform.h"

#if defined WINDOWS
#include "Platform/WindowsPhone/WPPlatform.h"
#endif

#include "Network/Global/Globalservice.h"

#include "Services/GuideService.h"

#include "LoginScreens/LoginLayer.h"
#include "LoginScreens/RegisterLayer.h"
#include "LoginScreens/ForgetPassLayer.h"
#include "LoginScreens/OTPLoginlayer.h"


LoginScreen::LoginScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
	p_panel_choose_server = nullptr;

	p_panel_first_choose_login = nullptr;
	p_btn_sound = nullptr;
	p_layout_img_download = nullptr;
    

	_login_layer = nullptr;
	_register_layer = nullptr;
	_forget_pass_layer = nullptr;
	_otp_login_layer = nullptr;
}

LoginScreen::~LoginScreen()
{
	p_panel_choose_server = nullptr;


	p_panel_first_choose_login = nullptr;
    
	SAFE_DELETE(_login_layer);
	SAFE_DELETE(_register_layer);
	SAFE_DELETE(_forget_pass_layer);
	SAFE_DELETE(_otp_login_layer);
}

int LoginScreen::Init()
{
	//override the list view table default with the iwinlistview table
	this->_func_action = [&](LayerEntity * layer) {
		layer->OverloadRegisterUIWidget<IwinTextFieldHandle>(xml::WIDGET_TEXT_FIELD);
		layer->OverloadRegisterUIWidget<IwinEditBoxdHandle>(xml::WIDGET_EDIT_BOX);
	};

	RKString _menu = "login_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (cur_ratio <= 1.34) //ipad ratio
	{
//		_menu = "login_screen_ipad";
	}

	InitMenuWidgetEntity(_menu);
	CallInitComponent(true);
	p_list_server = p_panel_choose_server->GetWidgetChildByName(".list_server");
	UpdateServerList();

	return 1;
}

void LoginScreen::UpdateServerList()
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
			static_cast<TextWidget*>(GetWidgetChildByName("main_login_screen.btn_server.name_server"))->SetText(GetServerMgr()->getCurServer()._name);
		}
	}
}

int	LoginScreen::InitComponent()
{
	p_panel_first_choose_login = GetWidgetChildByName("main_login_screen.first_login_layout");
    p_panel_logo_iwin = GetWidgetChildByName("main_login_screen.panel_logo");
	p_panel_choose_server = GetWidgetChildByName("main_login_screen.panel_choose_server.panel_choose_server");
    
	_login_layer = new LoginLayer();
	_login_layer->InitLayer(this);

	_register_layer = new RegisterLayer();
	_register_layer->InitLayer(this);

	_forget_pass_layer = new ForgetPassLayer();
	_forget_pass_layer->InitLayer(this);

	_otp_login_layer = new OTPLoginLayer();
	_otp_login_layer->InitLayer(this);

	p_btn_sound = GetWidgetChildByName("main_login_screen.btn_sound");
	static_cast<CheckBoxWidget*>(p_btn_sound)->OnSetSelected(true);

	p_layout_img_download = p_panel_first_choose_login->GetWidgetChildByName(".title_img_layout");
	return 1;
}

int LoginScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	_login_layer->Update(dt);
	_register_layer->Update(dt);
	_forget_pass_layer->Update(dt);
	_otp_login_layer->Update(dt);

	return 1;
}

void LoginScreen::OnTouchMenuBegin(const cocos2d::Point & p)
{
	CheckTouchOutSideWidget(p, p_panel_choose_server,
		[&](void) {	
		if (p_panel_choose_server->GetParentWidget()->Visible())
		{
			p_panel_choose_server->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_panel_choose_server->ForceFinishAction();
				this->p_panel_choose_server->GetParentWidget()->Visible(false);
			}));
		}
	});
}

void LoginScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	_login_layer->ProcessWidget(name, type_widget, _widget);
	_register_layer->ProcessWidget(name, type_widget, _widget);
	_forget_pass_layer->ProcessWidget(name, type_widget, _widget);
	_otp_login_layer->ProcessWidget(name, type_widget, _widget);
	if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		auto w_tx = static_cast<IWIN_TF*>(_widget);
		w_tx->HandleEventTextField(name);
	}
	
	else if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "server_choose_btn")
		{
			int idx_list_select = static_cast<ListViewWidget*>(p_list_server)->GetCurrentIdxSelected();
			if (idx_list_select >= 0)
			{
				OnSwitchServer(idx_list_select);
			}

			static_cast<ListViewWidget*>(p_list_server)->SetCurrentIdxSelected(-1);

			p_panel_choose_server->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_panel_choose_server->ForceFinishAction();
				this->p_panel_choose_server->GetParentWidget()->Visible(false);
			}));
		}
		else if (name == "btn_server")
		{
			if (p_panel_choose_server->GetParentWidget()->Visible())
			{
				p_panel_choose_server->SetActionCommandWithName("ON_HIDE", CallFunc::create(
					[this]()
				{
					this->p_panel_choose_server->ForceFinishAction();
					this->p_panel_choose_server->GetParentWidget()->Visible(false);
				}
				));
			}
			else
			{
				p_panel_choose_server->SetActionCommandWithName("ON_SHOW");
				p_panel_choose_server->GetParentWidget()->Visible(true);
			}
		}

		//create account
		else if (name == "create_account_btn")
		{
			_login_layer->OnHideLayer();
			GameMgr->Network_GetCaptchaInfo(static_cast<RegisterLayer*>(_register_layer)->getIDCaptcha());
			_register_layer->OnShowLayer([]() {
				ScrMgr->SetScreenIDDirty();
			});
		}
		//return login screen from create acc
		else if (name == "have_acc_btn")
		{
			_login_layer->OnShowLayer([]() {
				ScrMgr->SetScreenIDDirty();
			});
			_register_layer->OnHideLayer();

		}
		//go to forget pass layer
		else if (name == "forget_pass_title")
		{
			_login_layer->OnHideLayer();
			_forget_pass_layer->OnShowLayer();

		}
		//return login layer from forget pass layer
		else if (name == "have_acc_btn_pass")
		{
			_login_layer->OnShowLayer();
			_forget_pass_layer->OnHideLayer();

		}
		//login method
		else if (name == "fb_btn" || name == "btn_login_fb")
		{
			startWaitingDialog(LOGIN_FACEBOOK);

//			if (!GetFacebookMgr()->isLoggedIn())
//			{
//				GetFacebookMgr()->login();
//			}
//			else
//			{
//                GameMgr->Network_loginWithToken(MainGame::TYPE_FACEBOOK, GetFacebookMgr()->getAccessToken());
//			}
            
            Platform::FBUtils::loginFacebook([](bool res, void* data){
                if(res)
                {
                    GameMgr->Network_loginWithToken(MainGame::TYPE_FACEBOOK, Platform::FBUtils::getAccessToken());
                }
            });
            
			ScrMgr->SaveDataInt(STR_SAVE_TYPE_LOGIN, LOGIN_FACEBOOK);
			ScrMgr->SaveTheGame();
		}
		else if (name == "gg_btn" || name == "btn_login_google")
		{
			//GetIapMgr()->purchase("iwin.vn.full.1", "aaa");
			startWaitingDialog(LOGIN_GOOGLE_PLAY);

			if (!GetGGSignInMgr()->isConnected())
			{
				GetGGSignInMgr()->signIn();
			}
			else
			{
				GameMgr->Network_loginWithToken(MainGame::TYPE_GOOGLE, GetGGSignInMgr()->getGGToken());
			}
			ScrMgr->SaveDataInt(STR_SAVE_TYPE_LOGIN, LOGIN_GOOGLE_PLAY);
			ScrMgr->SaveTheGame();
		}
		else if (name == "game_center_btn")
		{
			//login apple game center
			//GetIapMgr()->purchase("iwin.vn.full.1", "aaa");
			startWaitingDialog(LOGIN_APPLE);

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
			ScrMgr->SaveDataInt(STR_SAVE_TYPE_LOGIN, LOGIN_APPLE);
			ScrMgr->SaveTheGame();
		}
		//go to login layer from first choos login layer
		else if (name == "btn_login_iwin")
		{
			p_panel_first_choose_login->Visible(false);
			_login_layer->OnShowLayer();
		}

		else if (name == "btn_question")
		{
			//GetGuideService()->ShowCustomerService();
		}
		else if (name == "btn_back")
		{
            if(_otp_login_layer->IsVisibleLayer())
            {
				_otp_login_layer->OnHideLayer();
                p_panel_logo_iwin->Visible(true);
				_login_layer->OnShowLayer();
            }
            
            if(_login_layer->IsVisibleLayer())
            {
				_login_layer->OnHideLayer();
                p_panel_first_choose_login->SetActionCommandWithName("ON_SHOW");
            }
		}
		else if (name == "btn_custommer")
		{
			GetGuideService()->ShowCustomerService();
		}
	
	}
	else if (type_widget == UI_TYPE::UI_CHECK_BOX)
	{
		if (name == "btn_sound_UNSELECTED")
		{
			//enable sound
			GetSound->ResetVolume();
			GetSound->ResetVolume(false);

			ScrMgr->SaveDataFloat(STR_SAVE_SOUND_VOLUME, GetSound->GetVolume() * 100.f);
			ScrMgr->SaveDataFloat(STR_SAVE_MUSIC_VOLUME, GetSound->GetVolume(false) * 100.f);
			ScrMgr->SaveTheGame();	
		}
		else if (name == "btn_sound_SELECTED")
		{
			//disable sound
			GetSound->MuteVolume();
			GetSound->MuteVolume(false);

			ScrMgr->SaveDataFloat(STR_SAVE_SOUND_VOLUME, GetSound->GetVolume() * 100.f);
			ScrMgr->SaveDataFloat(STR_SAVE_MUSIC_VOLUME, GetSound->GetVolume(false) * 100.f);
			ScrMgr->SaveTheGame();
		}
	}
}

void LoginScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(LOGIN_SCREEN);
	p_menu_show_next = MENU_NONE;
}

void LoginScreen::OnFadeInComplete()
{
	BaseScreen::OnFadeInComplete();
	//
	static_cast<LoginLayer*>(_login_layer)->OnAutoLogin();
	
}

void LoginScreen::OnBeginFadeIn()
{
	this->p_layout_img_download->GetResource()->removeAllChildren();
	GameMgr->HandleURLImgDownload(
		"http://av.iwin.me/wf/logo_title/1/logo_title.png", "logo_title", [this](void * data, void * str, int tag)
	{
		this->ParseImgToPanel(this->p_layout_img_download, data, str, 100, cocos2d::Texture2D::PixelFormat::BGRA8888);
	} , false);
	//

	int idx_server_select = 0;
	if (ScrMgr->GetDataSaveInt(STR_SAVE_SERVER_CHOOSE, idx_server_select))
	{
		onSetServer(idx_server_select);
	}
    
    //GameMgr->Network_GetCaptchaInfo(static_cast<RegisterLayer*>(_register_layer)->getIDCaptcha());

	//GameMgr->Network_SetClientLanguage((int)LangMgr->GetCurLanguage());

	float current_sound = 0;
	if (ScrMgr->GetDataSaveFloat(STR_SAVE_SOUND_VOLUME, current_sound))
	{
		if (current_sound == 0.f)
		{
			static_cast<CheckBoxWidget*>(p_btn_sound)->OnSetSelected(true);
		}
		else
		{
			static_cast<CheckBoxWidget*>(p_btn_sound)->OnSetSelected(false);
		}
	}
	else
	{
		static_cast<CheckBoxWidget*>(p_btn_sound)->OnSetSelected(false);
	}
}

void LoginScreen::startWaitingDialog(LOGIN_TYPE login_type)
{
    
	std::string login_name = "iwin";
	switch (login_type)
	{
	case LOGIN_TYPE::LOGIN_IWIN:
	{
		login_name = "iWin";
	}
    break;
	case LOGIN_TYPE::LOGIN_APPLE:
	{
		login_name = "GAME CENTER";
	}
    break;
	case LOGIN_TYPE::LOGIN_FACEBOOK:
	{
		login_name = "FACEBOOK";
	}
    break;
	case LOGIN_TYPE::LOGIN_GOOGLE_PLAY:
	{
		login_name = "GOOGLE";
	}
    break;
	default:
		break;
	}
	ScrMgr->OnShowDialog(
		"logging_in", 
		StringUtils::format(LANG_STR("logging_in_wit").GetString(), login_name.c_str()),
		DIALOG_WAITING_BUTTON, "",
		[&](const char * call_back, const char* btn_name)
	{
		//SwithToMainScreen();
	}
	);
}

void LoginScreen::onSmsSyntaxInfo(s16 subId, std::string content, std::string address, std::string confirmMsg)
{
}

void LoginScreen::onCallBackCheckUserName(RKString msg)
{
	static_cast<RegisterLayer*>(_register_layer)->onCallBackCheckUserName(msg);
}

void LoginScreen::onCallBackCheckEmail(RKString msg)
{
	static_cast<RegisterLayer*>(_register_layer)->onCallBackCheckEmail(msg);
}

void LoginScreen::onCallBackRequestRegister(RKString msg)
{
	static_cast<RegisterLayer*>(_register_layer)->onCallBackRequestRegister(msg);
}

void LoginScreen::onSetServer(int idx_server_select)
{
	if ((size_t)idx_server_select >= p_list_server->GetNumberChildWidget())
	{
		idx_server_select = 0;
	}

	auto current_layout = p_list_server->GetWidgetChildAtIdx(idx_server_select);
	current_layout->GetWidgetChildByName(".stick_icon")->Visible(true);
	current_layout->GetWidgetChildByName(".highlights")->Visible(true);

	int number_child_in_list = static_cast<ListView*>(p_list_server->GetResource())->getChildrenCount();
	for (int i = 0; i < number_child_in_list; i++)
	{
		if (i == idx_server_select)
		{
			continue;
		}
		auto current_layout_t = p_list_server->GetWidgetChildAtIdx(i);
		current_layout_t->GetWidgetChildByName(".stick_icon")->Visible(false);
		current_layout_t->GetWidgetChildByName(".highlights")->Visible(false);
	}
	GetServerMgr()->selectServer(idx_server_select);
	RKString name_server = GetServerMgr()->getCurServer()._name;
	static_cast<TextWidget*>(GetWidgetChildByName("main_login_screen.btn_server.name_server"))->SetText(name_server);
}

void LoginScreen::onReceiveCaptcha(void * info)
{
	static_cast<RegisterLayer*>(_register_layer)->onReceiveCaptcha(info);
}

void LoginScreen::onReceiveCaptchaResult(void *  info)
{
	static_cast<RegisterLayer*>(_register_layer)->onReceiveCaptchaResult(info, _login_layer);
}

void LoginScreen::onReceiveRequireAuthenOTP(void *  data)
{
    p_panel_logo_iwin->Visible(false);
	_login_layer->OnHideLayer();
	p_panel_first_choose_login->Visible(false);
   
	static_cast<OTPLoginLayer*>(_otp_login_layer)->onReceiveRequireAuthenOTP(data);
}

void LoginScreen::onReceiveOTPStatus(void * list_data)
{
	static_cast<OTPLoginLayer*>(_otp_login_layer)->onReceiveOTPStatus(list_data);
}

RKString LoginScreen::GetPassword()
{
	return ((LoginLayer*)_login_layer)->getPassword();
}

 RKString LoginScreen::GetUsername()
{
    return ((LoginLayer*)_login_layer)->getUserName();
}

int LoginScreen::GetStateLoginMenu()
{
	if (_login_layer->IsVisibleLayer())
	{
		return 0;
	}
	else
	{
		if (_register_layer->IsVisibleLayer())
		{
			return 1;
		}
	}
	return 2;
}

void LoginScreen::OnSwitchServer(int idx)
{
	onSetServer(idx);
	ScrMgr->SaveDataInt(STR_SAVE_SERVER_CHOOSE, idx);
	ScrMgr->SaveTheGame();
}

void LoginScreen::SwithToMainScreen(bool have_otp )
{
	if (ScrMgr->GetCurrentMenuUI() == MENU_LAYER::LOGIN_SCREEN
		//handle case reconnect
		&& ScrMgr->GetMenuUI(LOGIN_SCREEN)->GetMenuShowNext() != HUD_SCREEN
		)
	{
		if (!have_otp)
		{
			if (!_otp_login_layer->IsVisibleLayer())
			{
				ScrMgr->SwitchToMenu(MENU_LAYER::MAIN_SCREEN, LOGIN_SCREEN);
			}
		}
		else
		{
			HideOTPPage();
			ScrMgr->SwitchToMenu(MENU_LAYER::MAIN_SCREEN, LOGIN_SCREEN);
		}
	}
	ScrMgr->SetStateLoading(100);
}

bool	LoginScreen::IsOTPVisible()
{
	return _otp_login_layer->IsVisibleLayer();
}

void	LoginScreen::HideOTPPage()
{
	_otp_login_layer->OnHideLayer();
	_login_layer->OnShowLayer();
}

void LoginScreen::onReceiveAuthorizeOTP(void * data)
{
	static_cast<OTPLoginLayer*>(_otp_login_layer)->onReceiveAuthorizeOTP(data);
}

void LoginScreen::SetActionAfterShow(int action)
{
	if (action == 0)
	{
		this->PushEvent([this](BaseScreen* scr) {
			
			_login_layer->OnShowLayer([]() {
				ScrMgr->SetScreenIDDirty();
			});
			_register_layer->OnHideLayer();
		}, TIME_ACTION);
	}
	else if (action == 1)
	{
		this->PushEvent([this](BaseScreen* scr) 
		{
			_login_layer->OnHideLayer();
			_register_layer->OnShowLayer([]() {
				ScrMgr->SetScreenIDDirty();
			});
		}, TIME_ACTION);
	}
}

void LoginScreen::OnClearPassInput()
{
	static_cast<LoginLayer*>(_login_layer)->ClearTextPass();
}

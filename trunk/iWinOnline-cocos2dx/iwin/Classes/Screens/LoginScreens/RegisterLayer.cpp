#include "RegisterLayer.h"
#include "UI/AdvanceUI/wText.h"

#include "Network/Global/Globalservice.h"
#include "Screens/LoginScreen.h"
#include <regex>
#include "Network/JsonObject/captcha/CaptchaInfo.h"
#include "Network/JsonObject/captcha/CaptchaResult.h"
RegisterLayer::RegisterLayer()
{
	p_panel_register_user = nullptr;
	p_id_captcha = "";
}

RegisterLayer::~RegisterLayer()
{
	p_panel_register_user = nullptr;
}

void RegisterLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);

	p_panel_register_user = _base_screen->GetWidgetChildByName("main_login_screen.register_layout");
	p_panel_check_email_input = p_panel_register_user->GetWidgetChildByName(".panel_check_email_input");
	p_panel_check_nick_input = p_panel_register_user->GetWidgetChildByName(".panel_check_nick_input");


	p_btn_create_user = p_panel_register_user->GetWidgetChildByName(".create_acc_btn");

	static_cast<ui::Button*>(p_btn_create_user->GetResource())->setEnabled(false);
	p_btn_create_user->SetActionCommandWithName("SET_DISABLE");

	auto btn_agree_term = p_panel_register_user->GetWidgetChildByName(".agree_term_of_use.agree_2");
	Vec2 pos = btn_agree_term->GetPosition();
	Vec2 size = btn_agree_term->GetSize();

	p_panel_register_user->GetWidgetChildByName(".agree_term_of_use.agree_1")->GetResource()->setPositionX(pos.x - ((size.x / 2.f) + 4));
	p_panel_register_user->GetWidgetChildByName(".agree_term_of_use.agree_3")->GetResource()->setPositionX(pos.x + ((size.x / 2.f) + 4));
	p_panel_captcha = p_panel_register_user->GetWidgetChildByName(".panel_captcha_input.captcha_panel");

	auto _widget = p_panel_register_user->GetWidgetChildByName(".panel_email_input.text_input_create_email");
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_email_btn"));
	w_tx->InitCallBack(
		[this, _widget]()
	{
		if (p_panel_check_email_input->Visible())
		{
			p_panel_check_email_input->GetWidgetChildAtIdx(1)->Visible(false);
		}

		if (p_panel_check_nick_input->Visible())
		{
			p_panel_check_nick_input->GetWidgetChildAtIdx(1)->Visible(false);
		}
	},
		[this, _widget]()
	{
		RKString email_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_email_input.text_input_create_email"))->GetText();

		if (!IsValidEmail(email_input.GetString()))
		{
			p_panel_check_email_input->Visible(true);
			//  p_panel_check_name_1->Visible(false);
			auto panel = p_panel_check_email_input->GetWidgetChildAtIdx(1);
			panel->Visible(true);
			panel->SetActionCommandWithName("SHOW_1");
			panel->GetWidgetChildAtIdx(0)->SetActionCommandWithName("TEXT_1");
		}
		else
		{
			GameMgr->Network_CheckEmailExist(email_input);
		}

		OnCheckEnableBtnCreateUser();
	},
		[this, _widget]()
	{
		p_panel_check_email_input->Visible(false);
		p_panel_check_nick_input->Visible(false);
	},
		nullptr,
		[this, _widget]()
	{
		_base_screen->PushEvent([this](BaseScreen * scr)
		{
			static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_nick_input.text_input_create_nick"))->SetAttachIME();
		});
	}
		);

	_widget = p_panel_register_user->GetWidgetChildByName(".panel_nick_input.text_input_create_nick");
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_create_nick_btn"));
	w_tx->InitCallBack(
		[this, _widget]()
	{
		if (p_panel_check_nick_input->Visible())
		{
			p_panel_check_nick_input->GetWidgetChildAtIdx(1)->Visible(false);
		}
	},
		[this, _widget]()
	{
		p_panel_check_nick_input->GetWidgetChildAtIdx(1)->Visible(false);

		RKString nick_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_nick_input.text_input_create_nick"))->GetText();

		if (!IsValidString(nick_input.GetString()))
		{
			p_panel_check_nick_input->Visible(true);
			auto panel = p_panel_check_nick_input->GetWidgetChildAtIdx(1);
			panel->Visible(true);
			panel->SetActionCommandWithName("SHOW_1");
			panel->GetWidgetChildAtIdx(0)->SetActionCommandWithName("TEXT_1");
		}
		else
		{
			GameMgr->Network_CheckUserNameExist(nick_input);
		}

		OnCheckEnableBtnCreateUser();
	},
		[this, _widget]()
	{
		p_panel_check_nick_input->Visible(false);
	},
		nullptr,
		[this, _widget]()
	{
		_base_screen->PushEvent([this](BaseScreen * scr)
		{
			static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_captcha_input.title_captcha"))->SetAttachIME();
		});
	}
		);

	_widget = p_panel_register_user->GetWidgetChildByName(".panel_captcha_input.title_captcha");
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_captcha_btn"));
	w_tx->InitCallBack(
		nullptr,
		[this, _widget]()
	{
		OnCheckEnableBtnCreateUser();
	},
		[this, _widget]()
	{
		OnCheckEnableBtnCreateUser();
	},
		[this, _widget]()
	{
		OnCheckEnableBtnCreateUser();
	},
		[this, _widget]()
	{
		
	}
		);

}

bool RegisterLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "create_acc_btn")
		{
			RKString captcha = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_captcha_input.title_captcha"))->GetText();
			ScrMgr->OnShowDialog(
				"create_account", "create_account", DIALOG_ZERO_BUTTON
			);

			GameMgr->Network_CheckCaptchaResult(p_id_captcha, captcha);
			return true;
		}
		else if (name == "reload_captcha")
		{
			GameMgr->Network_GetCaptchaInfo(p_id_captcha);
			return true;
		}
		else if (name == "btn_icon_warning_email_input")
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
		else if (name == "agree_2")
		{
			OPEN_URL(LINK_TERM);
			return true;
		}
	}
	return false;
}

void RegisterLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_panel_register_user->SetActionCommandWithName("ON_SHOW", call_back ? CallFunc::create([this , call_back]() {
		p_panel_register_user->ForceFinishAction();
		call_back();
	}) : nullptr);

	PLAY_MUSIC("MUSIC_01");
}

void RegisterLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_panel_register_user->Visible(false);
}

bool RegisterLayer::IsVisibleLayer()
{
	return p_panel_register_user->Visible();
}

int RegisterLayer::Update(float dt)
{

	return 1;
}

bool    RegisterLayer::IsValidEmail(std::string email)
{
	bool result = false;
	result = std::regex_match(email, std::regex("^[_A-Za-z0-9-]+(.[_A-Za-z0-9-]+)*@[A-Za-z0-9-]+(.[A-Za-z0-9-]+)*(.[A-Za-z]{2,})$"));

	return result;
}

bool RegisterLayer::IsValidInputUser()
{
	RKString nick_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_nick_input.text_input_create_nick"))->GetText();
	RKString email_input = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_email_input.text_input_create_email"))->GetText();
	RKString captcha = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_captcha_input.title_captcha"))->GetText();

	bool register_email_input_correct = !p_panel_check_email_input->Visible();
	bool register_nick_input_correct = !p_panel_check_nick_input->Visible();

	if (register_email_input_correct
		&& register_nick_input_correct
		&& captcha != "")
		return true;

	return false;
}

void RegisterLayer::OnCheckEnableBtnCreateUser()
{
	if (IsValidInputUser())
	{
		static_cast<ui::Button*>(p_panel_register_user->GetWidgetChildByName(".create_acc_btn")->GetResource())->setEnabled(true);
		p_panel_register_user->GetWidgetChildByName(".create_acc_btn")->SetActionCommandWithName("SET_ENABLE");
	}
	else
	{
		static_cast<ui::Button*>(p_panel_register_user->GetWidgetChildByName(".create_acc_btn")->GetResource())->setEnabled(false);
		p_panel_register_user->GetWidgetChildByName(".create_acc_btn")->SetActionCommandWithName("SET_DISABLE");
		GetSound->PlaySoundEffect("SFX_BUZZ");
	}
}

void RegisterLayer::SetIDCaptcha(RKString id)
{
	p_id_captcha = id;
}

void RegisterLayer::onReceiveCaptcha(void *data)
{
	iwinmesage::CaptchaInfo* info = (iwinmesage::CaptchaInfo*)data;
	p_id_captcha = info->getId();
	auto panel = p_panel_captcha;
	GameMgr->HandleURLImgDownload(info->getImage(), p_id_captcha, [this, panel](void * data, void * str, int tag) {

		_base_screen->ParseImgToPanel(panel, data, str, tag);
	});
}

void RegisterLayer::onReceiveCaptchaResult(void* data, CommonLayer* login_layer)
{
	iwinmesage::CaptchaResult* info = (iwinmesage::CaptchaResult*)data;
	if (info->getCode() == iwinmesage::CaptchaResult::CaptchaCheckType::SUCCESS)
	{
		RKString email = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_email_input.text_input_create_email"))->GetText();
		RKString nick = static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_nick_input.text_input_create_nick"))->GetText();

		static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_email_input.text_input_create_email"))->ClearText();
		static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_nick_input.text_input_create_nick"))->ClearText();
		static_cast<TextFieldWidget*>(p_panel_register_user->GetWidgetChildByName(".panel_captcha_input.title_captcha"))->ClearText();

		p_panel_register_user->Visible(false);


		login_layer->OnShowLayer([this, email, nick]() {
			static_cast<LoginScreen*>(_base_screen)->startWaitingDialog(LOGIN_IWIN);
			GlobalService::requestRegister(email.GetString(), nick.GetString());
		});
	}
	else
	{
		ScrMgr->OnShowErrorDialog(info->getDesc());
		GameMgr->Network_GetCaptchaInfo(p_id_captcha);
	}
}

void RegisterLayer::onCallBackCheckUserName(RKString msg)
{
	if (msg == "")
		return;

	p_panel_check_nick_input->Visible(true);
	auto panel = p_panel_check_nick_input->GetWidgetChildAtIdx(1);
	panel->Visible(true);
	panel->SetActionCommandWithName("SHOW_2");
	static_cast<TextWidget*>(panel->GetWidgetChildAtIdx(0))->SetText(msg);

	OnCheckEnableBtnCreateUser();
}

void RegisterLayer::onCallBackCheckEmail(RKString msg)
{
	if (msg == "")
		return;

	p_panel_check_email_input->Visible(true);
	auto panel = p_panel_check_email_input->GetWidgetChildAtIdx(1);
	panel->Visible(true);
	panel->SetActionCommandWithName("SHOW_3");
	static_cast<TextWidget*>(panel->GetWidgetChildAtIdx(0))->SetText(msg);

	OnCheckEnableBtnCreateUser();
}

void RegisterLayer::onCallBackRequestRegister(RKString msg)
{
	ScrMgr->OnShowDialog("notice", "register_success", TYPE_DIALOG::DIALOG_TWO_BUTTON, "", [this](const char* call_back, const char* btn_name) {
		if (strcmp("ok_2_btn", btn_name) == 0) {
			// action open mail
			//            if (Gdx.app.getType() == ApplicationType.Android) {
			//                Array<String> appSupportMail = Platform.instance.getAppEmail();
			//                if (appSupportMail.size > 0) {
			//                    String[] name = new String[appSupportMail.size];
			//                    for (int i = 0; i < Platform.instance.getAppEmail().size; i++) {
			//                        name[i] = appSupportMail.get(i);
			//                    }
			//                    // DialogUtil.startChooseDlg("test",
			//                    // "test
			//                    // dialog", name);
			//                    if (name.length == 1) {
			//                        Platform.instance.openAppEmailWithName(name[0]);
			//                    } else
			//                        DialogUtil.dialogOpenMail(L.gL(1304), name);
			//                }
			//            } else if (Gdx.app.getType() == ApplicationType.iOS) {
			//                Platform.instance.openAppEmailWithName("");
			//            }
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
			std::vector<std::string> listAppEmail;
			Platform::getListAppEmail(listAppEmail);
			if (listAppEmail.size() > 0)
			{
				//[TODO: show dialog with number button equal to number of list app email]

				Platform::openAppEmailWithName(listAppEmail.at(0));
				//                if(listAppEmail.size() == 1)
				//                {
				//                    Platform::openAppEmailWithName(listAppEmail.at(0));
				//                }
			}
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
			Platform::openAppEmailWithName("");
#endif

		}
	}, nullptr, nullptr, LangMgr->GetString("open_email"), LangMgr->GetString("ok"));
}

void RegisterLayer::CheckInputPass(RKString str_pass, RKString str_username)
{
	int result = IsValidPass(str_pass.GetString());
	bool res_1 = str_pass.Contains(str_username);
	bool res_2 = str_username.Contains(str_pass);

	if ((result != 0 || res_1 || res_2) && str_pass != "")
	{
		p_panel_check_nick_input->Visible(true);
		auto panel = p_panel_check_nick_input->GetWidgetChildAtIdx(1);
		panel->Visible(true);
		GetSound->PlaySoundEffect("SFX_BUZZ");
		if (res_1)
		{
			panel->GetWidgetChildAtIdx(6)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(7)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(6)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(7)->SetColor(Color3B::GREEN);
		}

		if (res_2)
		{
			panel->GetWidgetChildAtIdx(8)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(9)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(8)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(9)->SetColor(Color3B::GREEN);
		}


		if (result & 1)
		{
			panel->GetWidgetChildAtIdx(0)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(1)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(0)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(1)->SetColor(Color3B::GREEN);
		}

		if (result & 2)
		{
			panel->GetWidgetChildAtIdx(2)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(3)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(2)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(3)->SetColor(Color3B::GREEN);
		}

		if (result & 4)
		{
			panel->GetWidgetChildAtIdx(4)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(5)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(4)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(5)->SetColor(Color3B::GREEN);
		}
	}
	else
	{
		p_panel_check_nick_input->Visible(false);
	}

}

bool RegisterLayer::IsValidString(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		int number_anscii = (unsigned char)str[i];

		if (!
			((number_anscii >= 48 && number_anscii <= 57)
				||
				(number_anscii >= 64 && number_anscii <= 90)
				||
				(number_anscii >= 97 && number_anscii <= 122))
			)
		{
			return false;
		}
	}
	return true;
}

int  RegisterLayer::IsValidPass(std::string str)
{
	int result = 0;
	bool have_number = false;
	bool have_char = false;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (have_number && have_char)
			break;
		int number_anscii = (unsigned char)str[i];

		if ((number_anscii >= 48 && number_anscii <= 57))
		{
			have_number = true;
		}

		if ((number_anscii >= 65 && number_anscii <= 90)
			||
			(number_anscii >= 97 && number_anscii <= 122))
		{
			have_char = true;
		}
	}
	if (!have_number)
	{
		result |= 1;
	}
	if (!have_char)
	{
		result |= 2;
	}
	if (str.length() < 6)
	{
		result |= 4;
	}
	return result;
}

RKString RegisterLayer::getIDCaptcha()
{
	return p_id_captcha;
}




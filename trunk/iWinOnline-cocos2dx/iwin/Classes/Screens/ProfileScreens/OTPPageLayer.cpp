#include "OTPPageLayer.h"

#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wCheckBox.h"

#include "Network/Global/Globalservice.h"
#include "Network/JsonObject/ActiveOTP/AuthorizeOTP.h"
#include "Network/JsonObject/ActiveOTP/RequireAuthenOTP.h"
#include "Network/JsonObject/ActiveOTP/ResponseAuthorizeOTP.h"
#include "Models/OTPStatus.h"

#include "Screens/ProfileScreen.h"
OTPPageLayer::OTPPageLayer()
{
	p_idx_choose_type_otp = 0;

	p_delta_time_otp_cd = 0.f;
	p_have_otp_time_cd = false;
	p_authen_otp_require = nullptr;
}

OTPPageLayer::~OTPPageLayer()
{
	SAFE_DELETE(p_authen_otp_require);
}

void OTPPageLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);

	auto p_page_view_profile = screen->GetWidgetChildByName("profile_screen.main_profile_layout");
	p_page_otp = p_page_view_profile->GetWidgetChildByName(".page_profile_.page_OTP");
	p_layout_method_otp = p_page_otp->GetWidgetChildByName(".layout_method_otp");
	p_layout_otp_input = screen->GetWidgetChildByName("profile_screen.active_otp_layout.otp_layout");
}

bool OTPPageLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_active_phone_otp")
		{
			GlobalService::sendDeactiveOTP(ActiveOTP::PHONE_AUTH);
			return true;
		}
		else if (name == "btn_deactive_phone_otp")
		{
			GlobalService::sendActiveOTP(ActiveOTP::PHONE_AUTH);
			return true;
		}
		else if (name == "btn_deactive_gg_otp")
		{
			GlobalService::sendDeactiveOTP(ActiveOTP::GOOGLE_AUTH);
			return true;
		}
		else if (name == "btn_active_gg_otp")
		{
			GlobalService::sendActiveOTP(ActiveOTP::GOOGLE_AUTH);
			return true;
		}
		else if (name == "btn_download_app_otp" || name == "download_otp_app_btn")
		{
	#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
			OPEN_URL(LINK_WP_APP_GOOGLE_AUTHENTICATION);
	#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID 
			OPEN_URL(LINK_ANDROID_APP_GOOGLE_AUTHENTICATION);
	#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
			OPEN_URL(LINK_IOS_APP_GOOGLE_AUTHENTICATION);
	#else
			OPEN_URL("https://gauth.apps.gbraad.nl/#main");
	#endif
			return true;
		}

		else if (name == "btn_deactive_otp")
		{
			GlobalService::sendDeactiveOTP(ActiveOTP::EMAIL_AUTH);
			return true;
		}
		else if (name == "btn_active_otp")
		{
			if (GetUser->isEmailValidate)
			{
				ScrMgr->OnShowWaitingDialog();
				GlobalService::sendActiveOTP(ActiveOTP::EMAIL_AUTH);
			}
			else
			{
				ScrMgr->OnShowDialog("notice", "confirm_OTP_email", DIALOG_TWO_BUTTON, "",
					[this](const char * str, const char * btn_name)
				{
					static_cast<ProfileScreen*>(_base_screen)->ShowLayoutState(0);
				});
			}
			return true;
		}
		else if (name == "active_otp_btn")
		{
			auto w_textfield = p_layout_otp_input->GetWidgetChildByName(".panel_otp_input.text_input_otp");
			RKString current_text_otp = static_cast<TextFieldWidget*>(w_textfield)->GetText();
			static_cast<TextFieldWidget*>(w_textfield)->ClearText();
			if (current_text_otp != "")
			{
				ScrMgr->OnShowWaitingDialog();
				GlobalService::sendAuthorizeOTP(current_text_otp.GetString(), p_authen_otp_require->getTypeOTP(), p_authen_otp_require->getPurpose());
			}
			else
			{
				ScrMgr->OnShowOKDialog("enter_code_otp");
			}
			return true;
		}
		else if (name == "resend_otp_btn")
		{
			GlobalService::requestResendOTP(p_authen_otp_require->getTypeOTP());
			//
			p_delta_time_otp_cd = p_authen_otp_require->getExpireTime();
			p_have_otp_time_cd = true;
			static_cast<Button*>(p_layout_otp_input->GetWidgetChildByName(".resend_otp_btn")->GetResource())->setEnabled(false);
			//ScrMgr->OnShowOKDialog("have_sen_otp_mail");
			return true;
		}
		else if (name == "btn_copy_secret_key")
		{
			if (p_authen_otp_require->getSecretKey() != "")
			{
				_widget->Visible(false);
				Platform::setClipboardText(p_authen_otp_require->getSecretKey());
				_widget->GetParentWidget()->GetWidgetChildByName(".ico_already_copy")->Visible(true);
			}
			return true;
		}
		else if (name == "btn_close_layout_otp")
		{
			p_layout_otp_input->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				p_layout_otp_input->ForceFinishAction();
				p_layout_otp_input->GetParentWidget()->Visible(false);
			}));
			return true;
		}

	}
	else if (type_widget == UI_TYPE::UI_CHECK_BOX)
	{
		auto state_list = name.SplitLast("_");
		RKString name_check = state_list[0];
		RKString state = state_list[1];
		if (name_check == "btn_select_phone")
		{
			if (state == "SELECTED" && p_idx_choose_type_otp != 0)
			{
				OnChooseTypeOTP(0);
			}
			else if (state == "UNSELECTED" && p_idx_choose_type_otp == 0)
			{
				static_cast<CheckBoxWidget*>(_widget)->OnSetSelected(true);
			}
			return true;
		}
		else if (name_check == "btn_select_email")
		{
			if (state == "SELECTED" && p_idx_choose_type_otp != 1)
			{
				OnChooseTypeOTP(1);
			}
			else if (state == "UNSELECTED" && p_idx_choose_type_otp == 1)
			{
				static_cast<CheckBoxWidget*>(_widget)->OnSetSelected(true);
			}
			return true;
		}
		else if (name_check == "btn_select_gg")
		{
			if (state == "SELECTED" && p_idx_choose_type_otp != 2)
			{
				OnChooseTypeOTP(2);
			}
			else if (state == "UNSELECTED" && p_idx_choose_type_otp == 2)
			{
				static_cast<CheckBoxWidget*>(_widget)->OnSetSelected(true);
			}
			return true;
		}
	}
	return false;
}

void OTPPageLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_page_otp->SetActionCommandWithName("SET_POS", CallFunc::create(
		[this, call_back]() {
		this->p_page_otp->ForceFinishAction();
		this->p_page_otp->Visible(true);

		if (call_back)
		{
			call_back();
		}
	}));

	p_page_otp->GetWidgetChildByName(".loading_circle")->Visible(true);
	p_page_otp->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");
	GlobalService::requestOTPStatus();
}

void OTPPageLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_page_otp->Visible(false);
}
bool OTPPageLayer::IsVisibleLayer()
{
	return 	p_page_otp->Visible();
}
int OTPPageLayer::Update(float dt)
{
	if (p_have_otp_time_cd)
	{
		p_delta_time_otp_cd -= dt;
		if (p_delta_time_otp_cd < 0)
		{
			p_delta_time_otp_cd = 0;
			p_have_otp_time_cd = false;
			static_cast<Button*>(p_layout_otp_input->GetWidgetChildByName(".resend_otp_btn")->GetResource())->setEnabled(true);
			static_cast<TextWidget*>(p_layout_otp_input->GetWidgetChildByName(".resend_otp_btn.title"))->SetText("resend_otp");
		}
		static_cast<TextWidget*>(p_layout_otp_input->GetWidgetChildByName(".resend_otp_btn.title"))->SetText(
			StringUtils::format((LANG_STR("resend_otp") + "(%s)").GetString(),
				std::to_string(int(p_delta_time_otp_cd)).c_str())
		);
	}

	return 1;
}

void OTPPageLayer::OnChooseTypeOTP(int idx, bool send_server)
{
	_base_screen->PushEvent([this, idx, send_server](BaseScreen * scr) {


		WidgetEntity * _otp_page_phone = p_layout_method_otp->GetWidgetChildByName(".layout_phone_otp");
		WidgetEntity * _otp_page_email = p_layout_method_otp->GetWidgetChildByName(".layout_email_otp");
		WidgetEntity * _otp_page_gg = p_layout_method_otp->GetWidgetChildByName(".layout_gg_otp");

		static_cast<CheckBoxWidget*>(_otp_page_phone->GetWidgetChildAtIdx(0))->OnSetSelected(false);
		static_cast<CheckBoxWidget*>(_otp_page_email->GetWidgetChildAtIdx(0))->OnSetSelected(false);
		static_cast<CheckBoxWidget*>(_otp_page_gg->GetWidgetChildAtIdx(0))->OnSetSelected(false);

		if (idx == 0)
		{
			if (send_server)
			{
				GlobalService::sendChangeDefaultOTP(ActiveOTP::PHONE_AUTH);
			}
			else
			{
				static_cast<CheckBoxWidget*>(_otp_page_phone->GetWidgetChildAtIdx(0))->OnSetSelected(true);
			}
		}
		else if (idx == 1)
		{
			if (send_server)
			{
				GlobalService::sendChangeDefaultOTP(ActiveOTP::EMAIL_AUTH);
			}
			else
			{
				static_cast<CheckBoxWidget*>(_otp_page_email->GetWidgetChildAtIdx(0))->OnSetSelected(true);
			}
		}
		else if (idx == 2)
		{
			if (send_server)
			{
				GlobalService::sendChangeDefaultOTP(ActiveOTP::GOOGLE_AUTH);
			}
			else
			{
				static_cast<CheckBoxWidget*>(_otp_page_gg->GetWidgetChildAtIdx(0))->OnSetSelected(true);
			}
		}
		if (idx > 0)
		{
			p_idx_choose_type_otp = idx;
		}

	});
}

void OTPPageLayer::OnActiveShowOTP(bool value)
{
	if (!value)
	{
		p_page_otp->GetWidgetChildByName(".btn_active_otp")->Visible(true);
		p_page_otp->GetWidgetChildByName(".btn_deactive_otp")->Visible(false);
		p_layout_method_otp->Visible(false);
	}
	else
	{
		p_page_otp->GetWidgetChildByName(".btn_active_otp")->Visible(false);
		p_page_otp->GetWidgetChildByName(".btn_deactive_otp")->Visible(true);
		p_layout_method_otp->Visible(true);
	}
}

void OTPPageLayer::onReceiveAuthorizeOTP(void * data)
{
	ResponseAuthorizeOTP * autho = (ResponseAuthorizeOTP*)data;

	int type_otp = autho->getTypeOTP();
	p_default_Otp = type_otp;
	_base_screen->PushEvent([this](BaseScreen * scr)
	{
		ProfileScreen * profile_scr = (ProfileScreen*)scr;
		if (p_default_Otp == ActiveOTP::EMAIL_AUTH)
		{
			OnChooseTypeOTP(1, false);
		}
		else if (p_default_Otp == ActiveOTP::PHONE_AUTH)
		{
			OnChooseTypeOTP(0, false);
		}
		else if (p_default_Otp == ActiveOTP::GOOGLE_AUTH)
		{
			OnChooseTypeOTP(2, false);
		}
	});

	SAFE_DELETE(autho);
}

void OTPPageLayer::onReceiveOTPStatus(void * list_data)
{
	std::vector<OTPStatus*> list_otp_status = *((std::vector<OTPStatus*>*)list_data);
	p_page_otp->GetWidgetChildByName(".loading_circle")->Visible(false);

	if (list_otp_status.size() > 0)
	{
		bool isActiveAnyOTP = false;
		for (auto st : list_otp_status)
		{
			if (st->getStatus() == OTPStatus::STATUS_ACTIVE)
			{
				isActiveAnyOTP = true;
			}
			else if (st->getStatus() == OTPStatus::STATUS_DEACTIVE
				&& st->getType() == ActiveOTP::EMAIL_AUTH)
			{
				isActiveAnyOTP = false;
			}
		}
		OnActiveShowOTP(isActiveAnyOTP);

		if (isActiveAnyOTP)
		{
			WidgetEntity * _otp_page_email = p_layout_method_otp->GetWidgetChildByName(".layout_email_otp");
			WidgetEntity * _otp_page_phone = p_layout_method_otp->GetWidgetChildByName(".layout_phone_otp");
			WidgetEntity * _otp_page_gg = p_layout_method_otp->GetWidgetChildByName(".layout_gg_otp");

			static_cast<TextWidget*>(_otp_page_email->GetWidgetChildByName(".title_"))->SetText("Email");
			static_cast<TextWidget*>(_otp_page_phone->GetWidgetChildByName(".title_"))->SetText("phone_number");

			for (auto st : list_otp_status)
			{
				auto status = st->getStatus();
				auto type = st->getType();
				bool isdefault = st->getIsDefault();
				if (isdefault)
				{
					p_default_Otp = type;
					_base_screen->PushEvent([this](BaseScreen * scr)
					{
						if (p_default_Otp == ActiveOTP::EMAIL_AUTH)
						{
							OnChooseTypeOTP(1, false);
						}
						else if (p_default_Otp == ActiveOTP::PHONE_AUTH)
						{
							OnChooseTypeOTP(0, false);
						}
						else if (p_default_Otp == ActiveOTP::GOOGLE_AUTH)
						{
							OnChooseTypeOTP(2, false);
						}
					});
				}
				if (type == ActiveOTP::EMAIL_AUTH)
				{
					if (status == 1)
					{
						//active 
					}
					static_cast<TextWidget*>(_otp_page_email->GetWidgetChildByName(".title_"))->SetText("Email (" + GetUser->email + ")", true);
				}
				else if (type == ActiveOTP::PHONE_AUTH)
				{
					if (status == 1)
					{
						//active 
						_otp_page_phone->GetWidgetChildByName(".btn_active_phone_otp")->Visible(false);
						_otp_page_phone->GetWidgetChildByName(".btn_deactive_phone_otp")->Visible(true);
					}
					else
					{
						//active 
						_otp_page_phone->GetWidgetChildByName(".btn_active_phone_otp")->Visible(true);
						_otp_page_phone->GetWidgetChildByName(".btn_deactive_phone_otp")->Visible(false);
					}
					static_cast<TextWidget*>(_otp_page_phone->GetWidgetChildByName(".title_"))->SetText(
						StringUtils::format((LANG_STR("phone_number") + " (%s)").GetString(), GetUser->phoneNo.c_str()), true
					);
				}
				else if (type == ActiveOTP::GOOGLE_AUTH)
				{
					if (status == 1)
					{
						//active 
						_otp_page_gg->GetWidgetChildByName(".btn_active_gg_otp")->Visible(false);
						_otp_page_gg->GetWidgetChildByName(".btn_deactive_gg_otp")->Visible(true);
					}
					else
					{
						//active 
						_otp_page_gg->GetWidgetChildByName(".btn_active_gg_otp")->Visible(true);
						_otp_page_gg->GetWidgetChildByName(".btn_deactive_gg_otp")->Visible(false);
					}
				}
			}
		}
	}
	else
	{
		OnActiveShowOTP(false);
	}
	//turn off otp layout
	if (p_layout_otp_input->GetParentWidget()->Visible())
	{
		p_layout_otp_input->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
			p_layout_otp_input->ForceFinishAction();
			p_layout_otp_input->GetParentWidget()->Visible(false);
		}));
	}

	SAFE_DELETE_VECTOR(list_otp_status);
}

void OTPPageLayer::onReceiveCHangeOtpDefault(int otpId, bool status, std::string msg)
{
	if (status == true)
	{
		if (otpId == ActiveOTP::EMAIL_AUTH)
		{
			OnChooseTypeOTP(1, false);
		}
		else if (otpId == ActiveOTP::PHONE_AUTH)
		{
			OnChooseTypeOTP(0, false);
		}
		else if (otpId == ActiveOTP::GOOGLE_AUTH)
		{
			OnChooseTypeOTP(2, false);
		}
		p_default_Otp = otpId;
	}
	else
	{
		ScrMgr->OnShowOKDialog(msg);
		_base_screen->PushEvent([this](BaseScreen * scr)
		{
			ProfileScreen * profile_scr = (ProfileScreen*)scr;
			if (this->p_default_Otp == ActiveOTP::EMAIL_AUTH)
			{
				this->OnChooseTypeOTP(1, false);
			}
			else if (this->p_default_Otp == ActiveOTP::PHONE_AUTH)
			{
				this->OnChooseTypeOTP(0, false);
			}
			else if (this->p_default_Otp == ActiveOTP::GOOGLE_AUTH)
			{
				this->OnChooseTypeOTP(2, false);
			}
		});
	}
}

void OTPPageLayer::onShowActiveOTPDialog(void * data)
{
	SAFE_DELETE(p_authen_otp_require);
	p_authen_otp_require = (RequireAuthenOTP*)data;

	float ratio_scale = GetGameSize().width / _base_screen->GetDesignSize().x;
	//PASSERT2(false, "not handle this");
	ScrMgr->OnHideDialog();
	p_layout_otp_input->GetParentWidget()->Visible(true);
	p_layout_otp_input->SetActionCommandWithName("ON_SHOW");

	auto w_text_msg = static_cast<TextWidget*>(p_layout_otp_input->GetWidgetChildByName(".msg"));
	w_text_msg->SetText(p_authen_otp_require->getDesc(), true);

	if (p_authen_otp_require->getExpireTime() > 0)
	{
		p_have_otp_time_cd = true;
		p_delta_time_otp_cd = p_authen_otp_require->getExpireTime();
		p_layout_otp_input->GetWidgetChildByName(".resend_otp_btn")->Visible(true);
		static_cast<Button*>(p_layout_otp_input->GetWidgetChildByName(".resend_otp_btn")->GetResource())->setEnabled(false);
		p_layout_otp_input->GetWidgetChildByName(".download_otp_app_btn")->Visible(false);
	}
	else
	{
		p_layout_otp_input->GetWidgetChildByName(".resend_otp_btn")->Visible(false);
		p_layout_otp_input->GetWidgetChildByName(".download_otp_app_btn")->Visible(true);
	}

	auto w_secret_panel = p_layout_otp_input->GetWidgetChildByName(".panel_otp_secrect_key");
	if (p_authen_otp_require->getSecretKey() != "")
	{
		w_secret_panel->Visible(true);
		static_cast<TextFieldWidget*>(w_secret_panel->GetWidgetChildByName(".panel_input_secrect_key.textfield_secret_otp"))->SetText(p_authen_otp_require->getSecretKey());
		w_secret_panel->GetWidgetChildByName(".ico_already_copy")->Visible(false);
		w_secret_panel->GetWidgetChildByName(".btn_copy_secret_key")->Visible(true);
	}
	else
	{
		w_secret_panel->Visible(false);
	}

	auto w_bar_code = p_layout_otp_input->GetWidgetChildByName(".layout_barcode");
	if (p_authen_otp_require->getData() != "")
	{
		w_text_msg->SetTextAreaSize(Vec2(400 * ratio_scale, 0));
		w_bar_code->Visible(true);
		GameMgr->HandleURLImgDownload(p_authen_otp_require->getData(), "bar_code",
			[this, w_bar_code](void * data, void * str, int tag)
		{
			Image * img = (Image *)data;
			if (img && img->getWidth() <= 0)
			{
				CC_SAFE_DELETE(img);
				return;
			}
			//dont cache the texture bar
			Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str, Texture2D::PixelFormat::RGB565, false);
			this->_base_screen->ParseSpriteToPanel(w_bar_code, sprite_img, 100);
		});
	}
	else
	{
		w_bar_code->Visible(false);
		w_text_msg->SetTextAreaSize(Vec2(580 * ratio_scale, 0));
	}
}

bool OTPPageLayer::isOTPInputVisible()
{
	return p_layout_otp_input->GetParentWidget()->Visible();
}

void OTPPageLayer::CloseOTPInput()
{
	p_layout_otp_input->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
		p_layout_otp_input->ForceFinishAction();
		p_layout_otp_input->GetParentWidget()->Visible(false);
	}));
}
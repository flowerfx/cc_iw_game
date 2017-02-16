#include "OTPLoginLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wCheckBox.h"

#include "Network/Global/Globalservice.h"
#include "Screens/LoginScreen.h"

#include "Network/JsonObject/ActiveOTP/AuthorizeOTP.h"
#include "Network/JsonObject/ActiveOTP/RequireAuthenOTP.h"
#include "Network/JsonObject/ActiveOTP/ResponseAuthorizeOTP.h"
#include "Models/OTPStatus.h"

OTPLoginLayer::OTPLoginLayer()
{
	p_panel_otp_login_layout = nullptr;
	p_panel_otp_login_desc = nullptr;
	p_panel_otp_input = nullptr;

	p_btn_request_otp = nullptr;
	p_btn_otp_login_gg = nullptr;
	p_btn_otp_login_sms = nullptr;
	p_btn_otp_login_email = nullptr;

	p_have_resend_otp = false;
	p_delta_time_send_otp = 0.f;
	p_require_authen_otp = nullptr;

	p_email = "";
	p_otp = "";
}

OTPLoginLayer::~OTPLoginLayer()
{
	p_panel_otp_login_layout = nullptr;
	p_panel_otp_login_desc = nullptr;
	p_panel_otp_input = nullptr;

	p_btn_request_otp = nullptr;
	p_btn_otp_login_gg = nullptr;
	p_btn_otp_login_sms = nullptr;
	p_btn_otp_login_email = nullptr;

	p_have_resend_otp = false;
	p_delta_time_send_otp = 0.f;
	p_require_authen_otp = nullptr;

	SAFE_DELETE(p_require_authen_otp);
}

void OTPLoginLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);

	p_panel_otp_login_layout = _base_screen->GetWidgetChildByName("main_login_screen.otp_login_layout");
	p_panel_otp_login_desc = p_panel_otp_login_layout->GetWidgetChildByName(".login_otp_desc");
	p_panel_otp_input = p_panel_otp_login_layout->GetWidgetChildByName(".panel_otp_input");

	auto layout_list_btn_other = p_panel_otp_login_layout->GetWidgetChildByName(".list_btn_otp_other");


	p_btn_otp_login_gg = layout_list_btn_other->GetWidgetChildByName(".gg_authen_btn");
	p_btn_otp_login_sms = layout_list_btn_other->GetWidgetChildByName(".sms_authen_btn");
	p_btn_otp_login_email = layout_list_btn_other->GetWidgetChildByName(".email_authen_btn");
	p_btn_request_otp = p_panel_otp_login_layout->GetWidgetChildByName(".request_otp_btn");

	auto _widget = p_panel_otp_input->GetWidgetChildByName(".text_input_otp");
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(p_panel_otp_input->GetWidgetChildByName(".btn_clear_text"));
	w_tx->InitCallBack(
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	);

}

bool OTPLoginLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "active_authen_btn")
		{
			ScrMgr->OnShowWaitingDialog();
			auto w_textfield = p_panel_otp_input->GetWidgetChildByName(".text_input_otp");
			RKString current_text_otp = static_cast<TextFieldWidget*>(w_textfield)->GetText();
			static_cast<TextFieldWidget*>(w_textfield)->ClearText();
			auto w_check = p_panel_otp_login_layout->GetWidgetChildByName(".check_remember.btn_check_save_on_device");
			bool is_check = static_cast<CheckBoxWidget*>(w_check)->IsSelected();
			GlobalService::sendAuthorizeOTP(current_text_otp.GetString(), p_require_authen_otp->getTypeOTP(), p_require_authen_otp->getPurpose(), is_check);
			return true;
		}
		else if (name == "request_otp_btn")
		{
			GlobalService::requestResendOTP(p_require_authen_otp->getTypeOTP());
			GlobalService::requestOTPStatus();
			p_delta_time_send_otp = p_require_authen_otp->getExpireTime();
			p_have_resend_otp = true;
			return true;
		}
		else if (name == "email_authen_btn" || name == "gg_authen_btn" || name == "sms_authen_btn")
		{
			//change type authen otp
			ActiveOTP typeChanged = name == "email_authen_btn" ? ActiveOTP::EMAIL_AUTH
				: name == "gg_authen_btn" ? ActiveOTP::GOOGLE_AUTH
				: ActiveOTP::PHONE_AUTH;

			GlobalService::requestChangeOTPLoginType(typeChanged);
			GlobalService::requestOTPStatus();
			return true;
		}

	}
	return false;
}

void OTPLoginLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_panel_otp_login_layout->Visible(true);
}

void OTPLoginLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_panel_otp_login_layout->Visible(false);
}

bool OTPLoginLayer::IsVisibleLayer()
{
	return p_panel_otp_login_layout->Visible();
}
int OTPLoginLayer::Update(float dt)
{
	if (p_have_resend_otp)
	{
		static_cast<Button*>(p_btn_otp_login_gg->GetResource())->setEnabled(false);
		static_cast<Button*>(p_btn_otp_login_sms->GetResource())->setEnabled(false);
		static_cast<Button*>(p_btn_otp_login_email->GetResource())->setEnabled(false);
		RKString resendOTP = LANG_STR("resend_otp").GetString();
		p_delta_time_send_otp -= dt;
		if (p_delta_time_send_otp < 0)
		{
			p_have_resend_otp = false;
			static_cast<Button*>(p_btn_otp_login_gg->GetResource())->setEnabled(p_btn_otp_login_gg->IsVisible());
			static_cast<Button*>(p_btn_otp_login_sms->GetResource())->setEnabled(p_btn_otp_login_sms->IsVisible());
			static_cast<Button*>(p_btn_otp_login_email->GetResource())->setEnabled(p_btn_otp_login_email->IsVisible());

			static_cast<Button*>(p_btn_request_otp->GetResource())->setEnabled(true);
			static_cast<TextWidget*>(p_btn_request_otp->GetWidgetChildAtIdx(0))->SetText(resendOTP, true);
		}
		else
		{
			static_cast<TextWidget*>(p_btn_request_otp->GetWidgetChildAtIdx(0))->SetText(resendOTP + " (" + std::to_string(int(p_delta_time_send_otp)) + ")", true);
		}

	}

	return 1;
}



void OTPLoginLayer::onReceiveRequireAuthenOTP(void *  data)
{
	SAFE_DELETE(p_require_authen_otp);
	p_require_authen_otp = (RequireAuthenOTP*)data;

	ScrMgr->OnHideDialog();

	static_cast<TextWidget*>(p_panel_otp_login_desc)->SetText(p_require_authen_otp->getDesc(), true);
	RKString resendOTP = LANG_STR("resend_otp").GetString();
	static_cast<TextWidget*>(p_btn_request_otp->GetWidgetChildAtIdx(0))->SetText(resendOTP, true);
	p_delta_time_send_otp = p_require_authen_otp->getExpireTime();
	//incase authen with gg --> disable resend otp
	p_have_resend_otp = p_require_authen_otp->getTypeOTP() != ActiveOTP::GOOGLE_AUTH;
	static_cast<Button*>(p_btn_request_otp->GetResource())->setEnabled(false);
	p_panel_otp_login_layout->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
		this->p_panel_otp_login_layout->ForceFinishAction();
		ScrMgr->SetScreenIDDirty();
	}));
	p_panel_otp_login_layout->GetWidgetChildByName(".list_btn_otp_other")->Visible(false);

	GlobalService::requestOTPStatus();
}

void OTPLoginLayer::OnShowBtnOTPActive()
{
	_base_screen->PushEvent([this](BaseScreen * scr) {
		LoginScreen * login_scr = (LoginScreen*)scr;
		if (!p_require_authen_otp)
			return;

		ActiveOTP type = (ActiveOTP)(p_require_authen_otp->getTypeOTP());
		if (type == ActiveOTP::EMAIL_AUTH)
		{
			p_btn_otp_login_email->Visible(false);

			p_btn_otp_login_gg->Visible(true);
			p_btn_otp_login_sms->Visible(true);
		}
		else if (type == ActiveOTP::GOOGLE_AUTH)
		{
			p_btn_otp_login_gg->Visible(false);

			p_btn_otp_login_email->Visible(true);
			p_btn_otp_login_sms->Visible(true);

			p_btn_otp_login_email->SetActionCommandWithName("SET_POS_2");
		}
		else if (type == ActiveOTP::PHONE_AUTH)
		{
			p_btn_otp_login_sms->Visible(false);

			p_btn_otp_login_email->Visible(true);
			p_btn_otp_login_gg->Visible(true);

			p_btn_otp_login_email->SetActionCommandWithName("SET_POS_1");
		}
	});

}

void OTPLoginLayer::onReceiveOTPStatus(void * list_data)
{
	std::vector<OTPStatus*> list_otp_status = *((std::vector<OTPStatus*>*)list_data);
	if (list_otp_status.size() > 0)
	{
		bool isExistOtherMethod = false;
		for (auto st : list_otp_status)
		{
			if (st->getStatus() == OTPStatus::STATUS_ACTIVE
				&& st->getType() != p_require_authen_otp->getTypeOTP())
			{
				isExistOtherMethod = true;
				break;
			}
		}

		if (isExistOtherMethod)
		{
			p_panel_otp_login_layout->GetWidgetChildByName(".list_btn_otp_other")->Visible(true);

			//			OnShowBtnOTPActive();
			p_btn_otp_login_gg->Visible(false);
			p_btn_otp_login_sms->Visible(false);
			p_btn_otp_login_email->Visible(false);
			int index = 1;
			for (auto st : list_otp_status)
			{
				int status = st->getStatus();
				int type = st->getType();

				if (type == ActiveOTP::EMAIL_AUTH && type != p_require_authen_otp->getTypeOTP())
				{
					_base_screen->PushEvent([status, index, this](BaseScreen * scr) {
						LoginScreen * login_scr = (LoginScreen*)scr;
						if (status == OTPStatus::STATUS_ACTIVE)
						{
							p_btn_otp_login_email->Visible(true);
							static_cast<ui::Button*>(p_btn_otp_login_email->GetResource())->setEnabled(true);
							p_btn_otp_login_email->SetActionCommandWithName(StringUtils::format("SET_POS_%d", index));
						}
					});
					index++;
				}
				else if (type == ActiveOTP::PHONE_AUTH && type != p_require_authen_otp->getTypeOTP())
				{
					_base_screen->PushEvent([status, index , this](BaseScreen * scr) {
						LoginScreen * login_scr = (LoginScreen*)scr;
						if (status == OTPStatus::STATUS_ACTIVE)
						{
							p_btn_otp_login_sms->Visible(true);
							static_cast<ui::Button*>(p_btn_otp_login_sms->GetResource())->setEnabled(true);
							p_btn_otp_login_sms->SetActionCommandWithName(StringUtils::format("SET_POS_%d", index));
						}
					});
					index++;
				}
				else if (type == ActiveOTP::GOOGLE_AUTH && type != p_require_authen_otp->getTypeOTP())
				{
					_base_screen->PushEvent([status, index , this](BaseScreen * scr) {
						LoginScreen * login_scr = (LoginScreen*)scr;
						if (status == OTPStatus::STATUS_ACTIVE)
						{
							p_btn_otp_login_gg->Visible(true);
							static_cast<ui::Button*>(p_btn_otp_login_gg->GetResource())->setEnabled(true);
							p_btn_otp_login_gg->SetActionCommandWithName(StringUtils::format("SET_POS_%d", index));
						}
					});
					index++;
				}
			}
		}
	}
	else
	{
		p_panel_otp_login_layout->GetWidgetChildByName(".list_btn_otp_other")->Visible(false);
	}

	SAFE_DELETE_VECTOR(list_otp_status);
}

void OTPLoginLayer::onReceiveAuthorizeOTP(void * data)
{
	ResponseAuthorizeOTP * autho = (ResponseAuthorizeOTP*)data;

	SAFE_DELETE(autho);
}
#ifndef __OTP_LOGIN_LAYER_H__
#define __OTP_LOGIN_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;
namespace iwinmesage
{
	class RequireAuthenOTP;
}
class OTPLoginLayer : public CommonLayer
{
public:
	OTPLoginLayer();
	virtual ~OTPLoginLayer();

private:
	//otp feature
	WidgetEntity * p_panel_otp_login_layout;
	WidgetEntity * p_panel_otp_login_desc;
	WidgetEntity * p_panel_otp_input;
	WidgetEntity * p_btn_request_otp;
	WidgetEntity * p_btn_otp_login_gg;
	WidgetEntity * p_btn_otp_login_sms;
	WidgetEntity * p_btn_otp_login_email;

	float		  p_delta_time_send_otp;
	bool		  p_have_resend_otp;
	iwinmesage::RequireAuthenOTP * p_require_authen_otp;


	RKString    p_email;
	RKString    p_otp;
protected:
	void	OnShowBtnOTPActive();
public:

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual int  Update(float dt) override;

	//otp
	void onReceiveRequireAuthenOTP(void * info);
	void onReceiveOTPStatus(void * list_data);
	void onReceiveAuthorizeOTP(void * data);
};

#endif //__OTP_LOGIN_LAYER_H__


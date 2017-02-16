#ifndef __OTP_PAGE_LAYER_H__
#define __OTP_PAGE_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;
namespace iwinmesage
{
	class RequireAuthenOTP;
}
class OTPPageLayer : public CommonLayer
{
public:
	OTPPageLayer();
	virtual ~OTPPageLayer();
private:
	//otp
	WidgetEntity * p_page_otp;

	int			  p_idx_choose_type_otp;
	int		      p_default_Otp;
	WidgetEntity * p_layout_method_otp;
	WidgetEntity * p_layout_otp_input;

	bool			p_have_otp_time_cd;
	float			p_delta_time_otp_cd;
	RequireAuthenOTP * p_authen_otp_require;
protected:
	void OnChooseTypeOTP(int idx, bool send_server = true);
public:

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual int  Update(float dt) override;
	void OnActiveShowOTP(bool value);

	void onReceiveAuthorizeOTP(void * data);
	void onReceiveOTPStatus(void * list_data);
	void onReceiveCHangeOtpDefault(int otpId, bool status, std::string msg);
	void onShowActiveOTPDialog(void * authen);

	bool isOTPInputVisible();
	void CloseOTPInput();
};

#endif //__FORGET_PASS_LAYER_H__


#ifndef __REGISTER_LAYER_H__
#define __REGISTER_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class RegisterLayer : public CommonLayer
{
public:
	RegisterLayer();
	virtual ~RegisterLayer();
private:
	WidgetEntity * p_panel_register_user;

	WidgetEntity * p_panel_check_email_input;
	WidgetEntity * p_panel_check_nick_input;
	WidgetEntity * p_btn_create_user;
	WidgetEntity * p_panel_captcha;

	RKString	p_id_captcha;
protected:
	bool    IsValidEmail(std::string email);
	void	OnCheckEnableBtnCreateUser();
	bool	IsValidInputUser();

	bool	IsValidString(std::string  str);
	int		IsValidPass(std::string  str);
	void	CheckInputPass(RKString str_pass, RKString str_username);
public:

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual int  Update(float dt) override;

	void SetIDCaptcha(RKString id);
	void onReceiveCaptcha(void* info);
	void onReceiveCaptchaResult(void* info, CommonLayer* login_layer);

	void onCallBackCheckUserName(RKString msg);
	void onCallBackCheckEmail(RKString msg);
	void onCallBackRequestRegister(RKString msg);

	RKString getIDCaptcha();


};

#endif //__REGISTER_LAYER_H__


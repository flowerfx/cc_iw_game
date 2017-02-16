#ifndef __LOGIN_LAYER_H__
#define __LOGIN_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class LoginLayer : public CommonLayer
{
public:
	LoginLayer();
	virtual ~LoginLayer();
private:
	WidgetEntity * p_panel_login_layout;

	WidgetEntity * p_panel_check_name_1;
	WidgetEntity * p_btn_login;

	RKString	p_password;
	RKString    p_username;

	bool		p_already_auto_login;
protected:
	void	LoginInternal();
	void	OnLoginGame();
public:

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;

	RKString getUserName();
	RKString getPassword();

	void	OnAutoLogin();

	void	ClearTextPass();
};

#endif //LOGIN_LAYER_H__


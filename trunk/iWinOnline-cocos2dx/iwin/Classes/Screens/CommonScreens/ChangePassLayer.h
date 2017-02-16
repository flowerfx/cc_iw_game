#ifndef __CHANGE_PASS_LAYER_H__
#define __CHANGE_PASS_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class ChangePassLayer : public CommonLayer
{
public:
	ChangePassLayer();
	virtual ~ChangePassLayer();
private:
	/*
	use : change pass layout
	*/

	WidgetEntity * p_layout_change_pass;
	WidgetEntity * p_panel_check_pass_input;
	WidgetEntity * p_panel_check_repass_input;
	WidgetEntity * p_textfield_pass_input;
	WidgetEntity * p_textfield_repass_input;
	WidgetEntity * p_textfield_oldpass_input;
	WidgetEntity * p_btn_changepass;
protected:
	/*
	use : change pass layout
	*/
	int IsValidPass(std::string  str);
	void CheckInputPass(const RKString & str_pass, const RKString & str_username);
	void OnCheckEnableBtnChangePass();
	bool IsValidInputPass();
public:
	virtual void InitLayer(BaseScreen* screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
};

#endif //__CHANGE_PASS_LAYER_H__


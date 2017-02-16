#ifndef __FORGET_PASS_LAYER_H__
#define __FORGET_PASS_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class ForgetPassLayer : public CommonLayer
{
public:
	ForgetPassLayer();
	virtual ~ForgetPassLayer();
private:
	WidgetEntity * p_panel_forget_pass;
	int			p_stage_choose_method_getpass;
protected:

public:

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual int  Update(float dt) override;


};

#endif //__FORGET_PASS_LAYER_H__


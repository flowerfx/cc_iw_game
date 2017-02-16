#ifndef __CONFIRM_CODE_LAYER_H__
#define __CONFIRM_CODE_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class ConfirmCodeLayer : public CommonLayer
{
public:
	ConfirmCodeLayer();
	virtual ~ConfirmCodeLayer();
private:
	/*
	use : show confirm box
	*/
	WidgetEntity * p_confirm_code_panel;
public:
	/*
	handle show confirm box
	*/
	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;

	virtual bool IsVisibleLayer() override;
};

#endif //__ANNOUCE_SERVER_LAYER_H__


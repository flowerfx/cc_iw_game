#ifndef __TOAST_LAYER_H__
#define __TOAST_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class ToastLayer : public CommonLayer
{
private:
	WidgetEntity* p_panel_toast;


public:
	ToastLayer();
	virtual ~ToastLayer();

	float			  _time_to_close;


public:
	virtual void InitLayer(BaseScreen * screen)		override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual int Update(float dt) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	void SetContent(std::string content);
};

#endif //__TOAST_LAYER_H__


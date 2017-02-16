#ifndef __LOADING_CIRCLE_LAYER__
#define __TOAST_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class LoadingCircleLayer : public CommonLayer
{
private:
	WidgetEntity* p_loading_circle_layer;
	WidgetEntity* p_loading_circle;
	float p_time_show;
public:
	LoadingCircleLayer();
	virtual ~LoadingCircleLayer();
public:
	virtual void InitLayer(BaseScreen * screen)		override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual int Update(float dt);
};

#endif //__TOAST_LAYER_H__


#ifndef __TUTORIAL_LAYER_H__
#define __TUTORIAL_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class TutorialLayer : public CommonLayer
{
public:
	TutorialLayer();
	virtual ~TutorialLayer();
private:
	WidgetEntity *     _tutorial_layer;
	WidgetEntity *     _tutorial_msg_layer;
	WidgetEntity *     _tutorial_txt;
	WidgetEntity *     _tutorial_txt_bg;
	WidgetEntity *     _show_item_tutorial;

	int					p_current_step_tutorial;
protected:
	void CloseTutorial(CallFunc * call_back = nullptr);
	void MoveStepTutorial(int step);
public:
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)	override;
	virtual void InitLayer(BaseScreen * screen)												override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
};

#endif //__INVITE_LAYER_H__


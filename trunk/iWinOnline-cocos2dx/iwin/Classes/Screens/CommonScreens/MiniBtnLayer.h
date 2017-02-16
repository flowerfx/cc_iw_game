#ifndef __MINI_BTN_LAYER_H__
#define __MINI_BTN_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class MiniBtnLayer : public CommonLayer
{
private:
	WidgetEntity * p_layout_mini_btn;
	WidgetEntity * p_time_panel;
	WidgetEntity * p_list_btn_mini_layout;
	float	p_delta_move_btn_tx;
	Vec2	_previous_pos_touch;

protected:
	void OnDragBtnMini();
public:
	MiniBtnLayer();
	virtual ~MiniBtnLayer();

	virtual void InitLayer(BaseScreen * screen)		override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual int Update(float dt) override;
	virtual bool IsVisibleLayer() override;

	void ResetTimer(bool value ,float time1, float timer2);
	void SetTimer(s64 time_bet_remain, s64 time_result_remain, s64 time_bet_total, s64 time_result_total, s64 default_time_bet, s64 default_time_result);
	cocos2d::Rect GetPosBtnMini(WidgetEntity * &obj);

	void OnShowMiniBtn(bool value);
	void OnEnableBtnNoHu(bool value);
};

#endif //__MINI_BTN_LAYER_H__


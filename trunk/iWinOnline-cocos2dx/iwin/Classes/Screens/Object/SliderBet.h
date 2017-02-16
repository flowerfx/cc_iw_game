#ifndef __SLIDER_BET__H__
#define __SLIDER_BET__H__
#include "Screens/ScreenManager.h"

namespace Utility
{
	namespace UI_Widget
	{
		class WidgetEntity;
	}
}
class SliderBet
{
private:

	s64  _minBet;
	s64  _maxBet;
	s64  _currentBet;
	float	   _value;
	float	  _pos_value_money_y;
	std::vector<s64> _range;
	Utility::UI_Widget::WidgetEntity* p_layer_bet;
	Utility::UI_Widget::WidgetEntity* p_lb_value;
	Utility::UI_Widget::WidgetEntity* p_lb_min;
	Utility::UI_Widget::WidgetEntity* p_lb_max;
	Utility::UI_Widget::WidgetEntity* p_slider;


protected:
	void SetRangeValue();
public:
	SliderBet(Utility::UI_Widget::WidgetEntity* xmlLayer);
	~SliderBet();
	void setMinMax(s64 min , s64 max);
	void setValue(int val);
	void onChangeValue(bool isHold);

	float getValue();
	s64 getCurrentBet();
	s64 getMaxValue();
	s64 getMinValue();
	s64 getDeltaValue();

	void setDeltaPercent(int percent);

	std::vector<s64> rangeValue();
	void layoutBtnBet();
};
#endif //__SLIDER_BET__H__


#include "SliderBet.h"
#include "UI/AdvanceUI/WidgetEntity.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wSlider.h"
#include "Utils.h"


SliderBet::SliderBet(Utility::UI_Widget::WidgetEntity* xmlLayer)
{
	p_layer_bet = xmlLayer;

	p_lb_max = p_layer_bet->GetWidgetChildByName(".sp_layout_btn_bet_max.btn_bet_rand_value_max.title_");
	p_lb_min = p_layer_bet->GetWidgetChildByName(".sp_layout_btn_bet_min.btn_bet_rand_value_min.title_");
	p_lb_value = p_layer_bet->GetWidgetChildByName(".lb_win_value");
	p_slider = p_layer_bet->GetWidgetChildByName(".slider_bet_money");
	_pos_value_money_y = p_lb_value->GetPosition().y;
	SetRangeValue();
	_currentBet = -1;
	setMinMax(Utils::GetBetValueNew(0), Utils::GetBetValueNew(10));
}
SliderBet::~SliderBet()
{

}

void SliderBet::setMinMax(s64  min, s64 max)
{
	_minBet = min;
	_maxBet = max;
	_range = rangeValue();
	if (_range.size() > 0)
	{
		int idx_min = -1;
		for (size_t i = 0; i < _range.size() - 1; i++)
		{
			if (min >= _range[i] && min < _range[i + 1])
			{
				idx_min = i;
				min = _range[i];
				break;
			}
		}
		if (idx_min < 0)
		{
			if (min >= _range[_range.size() - 1])
			{
				min = _range[_range.size() - 1];
			}
			else
			{
				max = _range[0];
			}
		}

		int idx_max = _range.size() - 1;
		if (idx_max < 0)
		{
			if (min >= _range[_range.size() - 1])
			{
				max = _range[_range.size() - 1];
			}
			else
			{
				max = _range[0];
			}
		}

		int delta = abs(idx_max - idx_min);
	}
	//static_cast<SliderWidget*>(p_slider)->SetPart(delta);
	static_cast<TextWidget*>(p_lb_min)->SetText(Utils::formatNumber(min));
	static_cast<TextWidget*>(p_lb_max)->SetText(Utils::formatNumber(max));
	if (_currentBet < 0)
	{
		static_cast<TextWidget*>(p_lb_value)->SetText(Utils::formatNumber(min));
	}
	
	layoutBtnBet();
}

void SliderBet::setValue(int val)
{
	int index = Utils::GetIndexOfValue(_range,val);
	float sumMax = _maxBet - _minBet;
	float rValue = _range[index] - _minBet;
	float percent = (rValue / sumMax) * 100.f;
	static_cast<SliderWidget*>(p_slider)->SetPercent(percent);
	float percentY = static_cast<SliderWidget*>(p_slider)->GetPercent() / 100;
	static_cast<TextWidget*>(p_lb_value)->SetText(Utils::formatNumber(_range[index]));
	p_lb_value->SetPosition(Vec2(p_lb_value->GetPosition().x, _pos_value_money_y + percentY *p_slider->GetResource()->getContentSize().width));
	_currentBet = _value = val;
}

void SliderBet::setDeltaPercent(int percent)
{
	float val = getMaxValue() * percent / 100.f;
	int index = Utils::GetIndexOfValue(_range, val);
	if (index == 0 && _range.size() >= 5)
	{
		index = 1;
	}
	float sumMax = _maxBet - _minBet;
	float rValue = _range[index] - _minBet;
	float t_percent = (rValue / sumMax) * 100.f;
	static_cast<SliderWidget*>(p_slider)->SetPercent(t_percent);
	float percentY = static_cast<SliderWidget*>(p_slider)->GetPercent() / 100;
	static_cast<TextWidget*>(p_lb_value)->SetText(Utils::formatNumber(_range[index]));
	p_lb_value->SetPosition(Vec2(p_lb_value->GetPosition().x, _pos_value_money_y + percentY *p_slider->GetResource()->getContentSize().width));
	
	_currentBet = _value = _range[index];
}

void SliderBet::onChangeValue(bool isHold)
{
	//if (!isHold)
	//{
	//	//static_cast<TextWidget*>(p_lb_value)->SetText(Utils::formatNumber(_value));
	//}
	//else
	{
		float value = static_cast<SliderWidget*>(p_slider)->GetPercent();
		auto range = rangeValue();
		int index = 0;
		if (range.size() <= 0)
			return;
		for (int i = 0; i < range.size() - 1; i++)
		{
			float match_1 = (float)((range[i] - range[0]) * 100 / (_maxBet - _minBet));
			float match_2 = (float)((range[i + 1] - range[0]) * 100 / (_maxBet - _minBet));


			if (value <= match_2 && value > match_1)
			{
				index = i + 1;
				break;
			}
			else if (value == match_1)
			{
				index = i;
				break;
			}

		}

		if (index >= range.size())
		{
			index = range.size() - 1;
		}
		if (range.size() > 0)
		{
			CCLOG("slider bet idx: %d with value : %d and max value:  %d ", index, (int)range[index], (int)(range[range.size() - 1]));

			auto _value = range[index];
			static_cast<TextWidget*>(p_lb_value)->SetText(Utils::formatNumber(_value));
			_currentBet = _value;

			int percentY1 = (float)((_value - range[0])*100.f / (_maxBet - _minBet));
			float percentY = percentY1 / 100.f;

			p_lb_value->SetPosition(Vec2(
				p_lb_value->GetPosition().x, 
				_pos_value_money_y + percentY * p_slider->GetResource()->getContentSize().width));

			if (_value != getValue())
			{
				static_cast<SliderWidget*>(p_slider)->SetPercent(percentY1);
			}
			CCLOG("slider bet percent : %d", percentY1);
		}
		CCLOG("__ log range slider : %f", value);
	}
	
	//int money = Utils::getMaxValidBet(value / 100 * GameController::myInfo->money);
}

s64 SliderBet::getCurrentBet()
{
	return _currentBet;
}

float SliderBet::getValue()
{
	return _value;
}

s64 SliderBet::getMaxValue()
{
	return _maxBet;
}

s64 SliderBet::getMinValue()
{
	return _minBet;
}

s64 SliderBet::getDeltaValue()
{
	auto val = _maxBet - _minBet;
	if (val <= 0)
	{
		val = 0;
	}
	return val;
}

std::vector<s64> SliderBet::rangeValue()
{
	std::vector<s64> range;
	std::vector<s64> lsR = Utils::GetBetPopularNew();
	int i = 0;
	for (auto r : lsR)
	{
		if (r >= _minBet && r < _maxBet)
		{
			range.push_back(r);
		}
		i++;
	}
	if (range.size() <= 0)
		return range;
	if (_maxBet - range[range.size() - 1] > _minBet)
	{
		range.push_back(_maxBet);
	}
	else
	{
		range[range.size() - 1] = _maxBet;
	}
	
	return range;
}

void SliderBet::layoutBtnBet()
{
	if (_range.size() <= 0)
		return;
	
	int index = Utils::GetIndexOfValue(_range,getMaxValue() / 4);
	if (index == 0 && _range.size() >= 5)
	{
		index = 1;
	}
	float sumMax = _maxBet - _minBet;
	float rValue = _range[index] - _minBet;
	float percent = rValue / sumMax;
	auto layer14 = p_layer_bet->GetWidgetChildByName(".sp_layout_btn_bet_14");
	Vec2 pos_14 = Vec2(layer14->GetPosition().x, _pos_value_money_y + (percent * p_slider->GetResource()->getContentSize().width));	
	layer14->SetPosition(pos_14);
	static_cast<TextWidget*>(layer14->GetWidgetChildByName(".btn_bet_rand_value_14.title_"))->SetText(Utils::formatNumber(_range[index]));
	
	index = Utils::GetIndexOfValue(_range, getMaxValue() / 2);
	rValue = _range[index] - _minBet;
	percent = rValue / sumMax;
	auto layer12 = p_layer_bet->GetWidgetChildByName(".sp_layout_btn_bet_12");
	Vec2 pos_12 = Vec2(layer12->GetPosition().x, _pos_value_money_y + percent * p_slider->GetResource()->getContentSize().width);
	layer12->SetPosition(pos_12);
	static_cast<TextWidget*>(layer12->GetWidgetChildByName(".btn_bet_rand_value_12.title_"))->SetText(Utils::formatNumber(_range[index]));

	index = Utils::GetIndexOfValue(_range, getMaxValue() * 3 / 4 );
	rValue = _range[index] - _minBet;
	percent = rValue / sumMax;
	auto layer34 = p_layer_bet->GetWidgetChildByName(".sp_layout_btn_bet_34");
	Vec2 pos_34 = Vec2(layer34->GetPosition().x, _pos_value_money_y + percent *p_slider->GetResource()->getContentSize().width);
	layer34->SetPosition(pos_34);
	static_cast<TextWidget*>(layer34->GetWidgetChildByName(".btn_bet_rand_value_34.title_"))->SetText(Utils::formatNumber(_range[index]));
}

void SliderBet::SetRangeValue()
{
	_range = Utils::GetBetPopularNew();
}
#ifndef __BONUS_MONEY_LAYER_H__
#define __BONUS_MONEY_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class BonusMoneyLayer : public CommonLayer
{
private:
	WidgetEntity* p_layout_bonus;
	WidgetEntity* p_effect_animate_bonus;
public:
	BonusMoneyLayer();
	virtual ~BonusMoneyLayer();

	virtual void InitLayer(BaseScreen * screen)		override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual int Update(float dt) override;
	virtual bool IsVisibleLayer() override;

	void ShowTextBonusMoney(s64 bonus_money, bool isBonus, int typeMoney);
};

#endif //__BONUS_MONEY_LAYER_H__


#include "BonusMoneyLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "Screens/CommonScreen.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
BonusMoneyLayer::BonusMoneyLayer()
{

}

BonusMoneyLayer::~BonusMoneyLayer()
{

}

int BonusMoneyLayer::Update(float dt)
{

	return 1;
}


void BonusMoneyLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);

	p_layout_bonus = _base_screen->GetWidgetChildByName("common_screen.layout_bonus.layout_bonus");
	p_effect_animate_bonus = p_layout_bonus->GetParentWidget()->GetWidgetChildByName(".animate_effect_bonus");
}

void BonusMoneyLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_layout_bonus->GetParentWidget()->Visible(true);
}

void BonusMoneyLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_layout_bonus->GetParentWidget()->Visible(false);
}

bool BonusMoneyLayer::IsVisibleLayer()
{
	return p_layout_bonus->GetParentWidget()->Visible();
}

void BonusMoneyLayer::ShowTextBonusMoney(s64 bonus_money, bool isBonus, int typeMoney)
{
	OnShowLayer();

	WidgetEntity * w_text = nullptr;
	p_effect_animate_bonus->Visible(false);
	if (bonus_money >= 0)
	{
		if (isBonus)
		{
			p_effect_animate_bonus->Visible(true);
			static_cast<AnimWidget*>(p_effect_animate_bonus)->RunAnim(CallFunc::create([this]() {
				this->p_effect_animate_bonus->Visible(false);
			}));

			PLAY_SOUND("SFX_COIN_DROP");
		}

		w_text = p_layout_bonus->GetWidgetChildByName(".bonus_iwin_add");
		p_layout_bonus->GetWidgetChildByName(".bonus_iwin_div")->Visible(false);
	}
	else
	{
		w_text = p_layout_bonus->GetWidgetChildByName(".bonus_iwin_div");
		p_layout_bonus->GetWidgetChildByName(".bonus_iwin_add")->Visible(false);
	}
	w_text->Visible(true);
	static_cast<TextWidget*>(w_text)->TextRunEffect(
		bonus_money, 0, 1.f, [bonus_money](s64 money)->std::string {
		auto text = Utils::formatNumber_dot(abs(money));
		if (bonus_money < 0)
		{
			text = "-" + text;
		}
		else
		{
			text = "+" + text;
		}
		return text;
	});

	if (bonus_money < 0)
	{
		p_layout_bonus->SetActionCommandWithName("ACTION_DIV", CallFunc::create([this]() {
			this->p_layout_bonus->ForceFinishAction();
			this->p_layout_bonus->GetParentWidget()->Visible(false);
		}));
	}
	else
	{
		p_layout_bonus->SetActionCommandWithName("ACTION_ADD", CallFunc::create([this]() {
			this->p_layout_bonus->ForceFinishAction();
			this->p_layout_bonus->GetParentWidget()->Visible(false);
		}));
	}
}
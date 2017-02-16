#include "ToastLayer.h"
#include "UI/AdvanceUI/wText.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"
#define DEFAULT_TIME 3.f
ToastLayer::ToastLayer()
{

}

ToastLayer::~ToastLayer()
{

}

int ToastLayer::Update(float dt)
{
	if (!p_panel_toast->Visible())
		return 0;

	_time_to_close -= dt;
	if (_time_to_close <= 0)
	{
		OnHideLayer();
		_time_to_close = DEFAULT_TIME;
	}
	return 1;
}

bool ToastLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{

	return false;
}

void ToastLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);


	p_panel_toast = _base_screen->GetWidgetChildByName("common_screen.toast_layer");
}

void ToastLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_panel_toast->ForceFinishAction();
	p_panel_toast->SetActionCommandWithName("ON_SHOW", CallFunc::create([this, call_back]() {
			this->p_panel_toast->ForceFinishAction();
			if (p_panel_toast->GetPosition().y != 0)
			{
				_time_to_close = DEFAULT_TIME;
			}
			if (call_back)
			{
				call_back();
			}
		}));
}

void ToastLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_panel_toast->SetActionCommandWithName("ON_HIDE", cocos2d::CallFunc::create([call_back , this]() {
		this->p_panel_toast->ForceFinishAction();
		this->p_panel_toast->Visible(false);
		if (call_back)
		{
			call_back();
		}
	}));
}

bool ToastLayer::IsVisibleLayer()
{
	return p_panel_toast->Visible();
}


void ToastLayer::SetContent(std::string content)
{
	static_cast<TextWidget*>(p_panel_toast->GetWidgetChildByName(".content"))->SetText(content.c_str());
}
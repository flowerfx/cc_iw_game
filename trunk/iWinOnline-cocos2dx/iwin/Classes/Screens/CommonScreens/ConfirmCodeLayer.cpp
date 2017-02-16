#include "ConfirmCodeLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTextField.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"

#include "Common/IwinTextfield.h"

ConfirmCodeLayer::ConfirmCodeLayer()
{
	p_confirm_code_panel = nullptr;

}

ConfirmCodeLayer::~ConfirmCodeLayer()
{
	p_confirm_code_panel = nullptr;
}

void ConfirmCodeLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	p_confirm_code_panel = _base_screen->GetWidgetChildByName("common_screen.confirm_layout.confirm_code");
	
	auto _widget = p_confirm_code_panel->GetWidgetChildByName(".confirm_code_panel.layput_current_code.code_confirm_input");
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildAtIdx(1));
	w_tx->InitCallBack(
		nullptr);

}

bool ConfirmCodeLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;

	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_close_confirm_box" || name == "btn_cancel_confirm" || name == "btn_confirm")
		{
			OnHideLayer(nullptr);

			if (name == "btn_confirm")
			{
				auto w_text = _widget->GetParentWidget()->GetWidgetChildByName(".layput_current_code.code_confirm_input");
				RKString confrim_code_txt = static_cast<TextFieldWidget*>(w_text)->GetText();
				GlobalService::sendConfirmSMSCode(confrim_code_txt.GetString());
			}

			return true;
		}
	}

	return false;
}

void ConfirmCodeLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_confirm_code_panel->GetParentWidget()->Visible(true);
	p_confirm_code_panel->SetActionCommandWithName("ON_SHOW", call_back ? CallFunc::create([call_back , this]() {
		p_confirm_code_panel->ForceFinishAction();
		call_back();
	}) : nullptr);

}
void ConfirmCodeLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_confirm_code_panel->SetActionCommandWithName("ON_HIDE", CallFunc::create([this , call_back]() {
		this->p_confirm_code_panel->ForceFinishAction();
		this->p_confirm_code_panel->GetParentWidget()->Visible(false);
		if (call_back)
		{
			call_back();
		}
	}));
}


bool ConfirmCodeLayer::IsVisibleLayer()
{
return p_confirm_code_panel->GetParentWidget()->Visible();
}
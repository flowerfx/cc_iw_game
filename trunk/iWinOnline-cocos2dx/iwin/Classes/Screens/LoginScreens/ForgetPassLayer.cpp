#include "ForgetPassLayer.h"
#include "Network/Global/Globalservice.h"

ForgetPassLayer::ForgetPassLayer()
{
	p_panel_forget_pass = nullptr;
	p_stage_choose_method_getpass = 0;
}

ForgetPassLayer::~ForgetPassLayer()
{
	p_panel_forget_pass = nullptr;
}

void ForgetPassLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);

	p_panel_forget_pass = _base_screen->GetWidgetChildByName("main_login_screen.forget_pass_layout");

	auto _widget = p_panel_forget_pass->GetWidgetChildByName(".panel_name_input_pass.text_input_getpass");
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_getpass_btn"));
	w_tx->InitCallBack(
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	);
}

bool ForgetPassLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "reget_pass_btn")
		{
			RKString username = static_cast<TextFieldWidget*>(p_panel_forget_pass->GetWidgetChildByName(".panel_name_input_pass.text_input_getpass"))->GetText();
			if (username != "")
			{
				ScrMgr->OnShowWaitingDialog();
				if (p_stage_choose_method_getpass == 0)
				{
					GameMgr->Network_HandleGetForgottenPassword(0, username);
				}
				else if (p_stage_choose_method_getpass == 1)
				{
					//GameMgr->Network_getSMSInfoForgotPassword(username);
					GameMgr->Network_HandleGetForgottenPassword(1, username);
				}
			}
			return true;
		}
		else if (name == "btn_select_1")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(2)->Visible(true);
			_widget->GetParentWidget()->GetParentWidget()->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(2)->Visible(false);
			p_stage_choose_method_getpass = 0;
			return true;
		}
		else if (name == "btn_select_2")
		{
			_widget->GetParentWidget()->GetWidgetChildAtIdx(2)->Visible(true);
			_widget->GetParentWidget()->GetParentWidget()->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(2)->Visible(false);
			p_stage_choose_method_getpass = 1;
			return true;
		}
		else if (name == "btn_icon_warning_pass_input")
		{
			auto panel = _widget->GetParentWidget()->GetWidgetChildAtIdx(1);
			if (panel->Visible())
			{
				panel->Visible(false);
			}
			else
			{
				panel->Visible(true);
			}
			return true;
		}


	}
	return false;
}

void ForgetPassLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_panel_forget_pass->Visible(true);
}

void ForgetPassLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_panel_forget_pass->Visible(false);
}
bool ForgetPassLayer::IsVisibleLayer()
{
	return p_panel_forget_pass->Visible();
}
int ForgetPassLayer::Update(float dt)
{

	return 1;
}



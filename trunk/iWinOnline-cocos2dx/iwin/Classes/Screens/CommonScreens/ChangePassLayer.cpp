#include "ChangePassLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTextField.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"
#include "Common/IwinTextfield.h"
ChangePassLayer::ChangePassLayer()
{

}

ChangePassLayer::~ChangePassLayer()
{

}

void ChangePassLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);

	p_layout_change_pass = _base_screen->GetWidgetChildByName("common_screen.change_pass_layout.change_pass");
	p_panel_check_pass_input = p_layout_change_pass->GetWidgetChildByName(".change_pass_panel.layput_new_pass.panel_check_pass_input");
	p_panel_check_repass_input = p_layout_change_pass->GetWidgetChildByName(".change_pass_panel.layput_confirm_new_pass.panel_check_repass_input");

	p_textfield_pass_input = p_panel_check_pass_input->GetParentWidget()->GetWidgetChildByName(".panel_input.new_pass_input");
	p_textfield_repass_input = p_panel_check_repass_input->GetParentWidget()->GetWidgetChildByName(".panel_input.confirm_new_pass_input");
	p_textfield_oldpass_input = p_layout_change_pass->GetWidgetChildByName(".change_pass_panel.layput_current_pass.panel_input.old_pass_input");

	p_btn_changepass = p_layout_change_pass->GetWidgetChildByName(".change_pass_panel.btn_change_pass");

	auto _widget = p_textfield_pass_input;
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildAtIdx(1));
	w_tx->InitCallBack(
		[this, _widget]() 
	{
		p_panel_check_pass_input->GetWidgetChildAtIdx(1)->Visible(false);
	},
		[this, _widget]()
	{
		p_panel_check_pass_input->GetWidgetChildAtIdx(1)->Visible(false);
		OnCheckEnableBtnChangePass();
	},
		[this, _widget]()
	{
		RKString pass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		CheckInputPass(pass_input, GetUser->nickName);
	},
		[this, _widget]()
	{
		RKString pass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		CheckInputPass(pass_input, GetUser->nickName);
	});

	_widget = p_textfield_oldpass_input;
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildAtIdx(1));
	w_tx->InitCallBack(
		nullptr,
		[this, _widget]()
	{
		OnCheckEnableBtnChangePass();
	});

	_widget = p_textfield_repass_input;
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildAtIdx(1));
	w_tx->InitCallBack(
		[this, _widget]()
	{
		RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		RKString pass_input = static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText();

		if (string_input != pass_input)
		{
			p_panel_check_repass_input->Visible(true);
			p_panel_check_repass_input->GetWidgetChildAtIdx(1)->Visible(false);
		}
	},
		[this, _widget]()
	{
		RKString repass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		RKString pass_input = static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText();

		if (repass_input != pass_input)
		{
			p_panel_check_repass_input->Visible(true);
			p_panel_check_repass_input->GetWidgetChildAtIdx(1)->Visible(true);
		}
		OnCheckEnableBtnChangePass();
	},
		[this, _widget]()
	{
		RKString repass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		RKString pass_input = static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText();

		if (repass_input != pass_input)
		{
			p_panel_check_repass_input->Visible(true);
			p_panel_check_repass_input->GetWidgetChildAtIdx(1)->Visible(false);
		}
		else
		{
			p_panel_check_repass_input->Visible(false);
		}
	},
		[this, _widget]()
	{
		RKString repass_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		RKString pass_input = static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText();

		if (repass_input == pass_input)
		{
			p_panel_check_repass_input->Visible(false);
		}
	});
}

bool ChangePassLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;

	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_clear_old_pass")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildAtIdx(0);
			static_cast<TextFieldWidget*>(widget)->ClearText();

			return true;
		}
		else if (name == "btn_clear_confirm_new_pass")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildAtIdx(0);
			static_cast<TextFieldWidget*>(widget)->ClearText();
			return true;
		}
		else if (name == "btn_clear_new_pass")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildAtIdx(0);
			static_cast<TextFieldWidget*>(widget)->ClearText();
			return true;
		}
		else if (name == "btn_icon_warning_repass_input")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildAtIdx(1);
			widget->Visible(!widget->Visible());
			return true;
		}
		else if (name == "btn_icon_warning_pass_input")
		{
			auto widget = _widget->GetParentWidget()->GetWidgetChildAtIdx(1);
			widget->Visible(!widget->Visible());
			return true;
		}
		else if (name == "btn_close_change_pass")
		{
			OnHideLayer();
			return true;
		}
		else if (name == "btn_change_pass")
		{
			OnHideLayer([this]() {
				//send request change pass to server
				GlobalService::changePassword(
					static_cast<TextFieldWidget*>(p_textfield_oldpass_input)->GetText().GetString(),
					static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText().GetString());
			});			
			return true;
		}
	}

	return false;
}

void ChangePassLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_layout_change_pass->GetParentWidget()->Visible(true);
	p_layout_change_pass->SetActionCommandWithName("ON_SHOW", call_back ? CallFunc::create([call_back, this]() {
		this->p_layout_change_pass->ForceFinishAction();
		call_back();
	}) : nullptr );
	OnCheckEnableBtnChangePass();
}

void ChangePassLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_layout_change_pass->SetActionCommandWithName("ON_HIDE", CallFunc::create(
		[this , call_back]() {
		this->p_layout_change_pass->ForceFinishAction();
		this->p_layout_change_pass->GetParentWidget()->Visible(false);
		if (call_back)
		{
			call_back();
		}
	}));
}

bool ChangePassLayer::IsVisibleLayer()
{
	return p_layout_change_pass->GetParentWidget()->Visible();
}

int  ChangePassLayer::IsValidPass(std::string str)
{
	int result = 0;
	bool have_number = false;
	bool have_char = false;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (have_number && have_char)
			break;
		int number_anscii = (unsigned char)str[i];

		if ((number_anscii >= 48 && number_anscii <= 57))
		{
			have_number = true;
		}

		if ((number_anscii >= 65 && number_anscii <= 90)
			||
			(number_anscii >= 97 && number_anscii <= 122))
		{
			have_char = true;
		}
	}
	if (!have_number)
	{
		result |= 1;
	}
	if (!have_char)
	{
		result |= 2;
	}
	if (str.length() < 6)
	{
		result |= 4;
	}
	return result;
}

void ChangePassLayer::CheckInputPass(const RKString & str_pass, const RKString & str_username)
{
	int result = IsValidPass(str_pass.GetString());
	bool res_1 = str_pass.Contains(str_username);
	bool res_2 = str_username.Contains(str_pass);

	if ((result != 0 || res_1 || res_2) && str_pass != "")
	{
		p_panel_check_pass_input->Visible(true);
		auto panel = p_panel_check_pass_input->GetWidgetChildAtIdx(1);
		panel->Visible(true);

		if (res_1)
		{
			panel->GetWidgetChildAtIdx(6)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(7)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(6)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(7)->SetColor(Color3B::GREEN);
		}

		if (res_2)
		{
			panel->GetWidgetChildAtIdx(8)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(9)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(8)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(9)->SetColor(Color3B::GREEN);
		}


		if (result & 1)
		{
			panel->GetWidgetChildAtIdx(0)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(1)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(0)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(1)->SetColor(Color3B::GREEN);
		}

		if (result & 2)
		{
			panel->GetWidgetChildAtIdx(2)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(3)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(2)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(3)->SetColor(Color3B::GREEN);
		}

		if (result & 4)
		{
			panel->GetWidgetChildAtIdx(4)->SetColor(Color3B::RED);
			panel->GetWidgetChildAtIdx(5)->SetColor(Color3B::WHITE);
		}
		else
		{
			panel->GetWidgetChildAtIdx(4)->SetColor(Color3B::GREEN);
			panel->GetWidgetChildAtIdx(5)->SetColor(Color3B::GREEN);
		}
	}
	else
	{
		p_panel_check_pass_input->Visible(false);
	}

}

bool ChangePassLayer::IsValidInputPass()
{
	RKString pass_input = static_cast<TextFieldWidget*>(p_textfield_pass_input)->GetText();
	RKString repass_input = static_cast<TextFieldWidget*>(p_textfield_repass_input)->GetText();
	RKString old_pass_input = static_cast<TextFieldWidget*>(p_textfield_oldpass_input)->GetText();

	bool register_pass_input_correct = !p_panel_check_pass_input->Visible();
	bool register_repass_input_correct = !p_panel_check_repass_input->Visible();

	if (register_pass_input_correct
		&& register_repass_input_correct
		&& (pass_input == repass_input)
		&& old_pass_input != ""
		&& pass_input != "")
		return true;

	return false;
}

void ChangePassLayer::OnCheckEnableBtnChangePass()
{
	if (IsValidInputPass())
	{
		static_cast<ui::Button*>(p_btn_changepass->GetResource())->setEnabled(true);
		p_btn_changepass->SetActionCommandWithName("SET_ENABLE");
	}
	else
	{
		static_cast<ui::Button*>(p_btn_changepass->GetResource())->setEnabled(false);
		p_btn_changepass->SetActionCommandWithName("SET_DISABLE");
	}
}

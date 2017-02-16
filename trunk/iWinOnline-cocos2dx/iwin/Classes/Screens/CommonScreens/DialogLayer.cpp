#include "DialogLayer.h"

#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wCheckBox.h"
DialogLayer::DialogLayer()
{
	p_dialog_layout = nullptr;
	p_Function0 = nullptr;
	p_Function1 = nullptr;
	p_Function2 = nullptr;

	p_nameBtnProcess = "";
	p_current_dialog = TYPE_DIALOG::DIALOG_NONE;

	p_panel_1_btn = nullptr;
	p_panel_2_btn = nullptr;
	p_panel_0_btn = nullptr;
	p_panel_fb_btn = nullptr;

	p_time_show_button = -1;

	in_stage_show = 0;

}

DialogLayer::~DialogLayer()
{

}
void DialogLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	in_stage_show = 0;
	p_dialog_layout = _base_screen->GetWidgetChildByName("common_screen.dialog_screen.panel_contain");
	p_panel_1_btn = p_dialog_layout->GetWidgetChildByName(".layout_.layout_btn.panel_1_btn");
	p_panel_2_btn = p_dialog_layout->GetWidgetChildByName(".layout_.layout_btn.panel_2_btn");
	p_panel_0_btn = p_dialog_layout->GetWidgetChildByName(".layout_.layout_btn.panel_0_btn");
	p_panel_fb_btn = p_dialog_layout->GetWidgetChildByName(".layout_.layout_btn.panel_fb_btn");

	p_text_title = p_dialog_layout->GetWidgetChildByName(".layout_.title_notice");
	p_text_content = p_dialog_layout->GetWidgetChildByName(".layout_.notice_content");

}


int DialogLayer::Update(float dt)
{
	if (p_dialog_layout->GetParentWidget()->Visible())
	{
		if (p_current_dialog == TYPE_DIALOG::DIALOG_WAITING_BUTTON || p_current_dialog == TYPE_DIALOG::DIALOG_ZERO_BUTTON_WAIT)
		{
			if (p_time_show_button > -1.f)
			{
				p_time_show_button -= dt;
				if (p_time_show_button <= 0.f)
				{
					if (p_current_dialog == TYPE_DIALOG::DIALOG_WAITING_BUTTON)
					{
						p_panel_1_btn->Visible(true);
					}
					else
					{
						OnHideDialog();
					}
					p_time_show_button = -1.f;
				}
			}
		}
		return 1;
	}
	return 0;
}

bool DialogLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;

	if (type_widget == UI_TYPE::UI_BUTTON && p_dialog_layout->GetParentWidget()->Visible())
	{
		p_nameBtnProcess = name;
		OnHideDialog();
		return true;
	}
	return false;
}

void DialogLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	OnShowDialog("title", "message", DIALOG_ONE_BUTTON);
}

bool DialogLayer::IsVisibleLayer()
{
	return this->p_dialog_layout->GetParentWidget()->Visible();
}

void DialogLayer::OnShowDialog(
	const RKString & str_title,
	const RKString & str_message,
	TYPE_DIALOG type,
	const char * strCB,
	const std::function<void(const char *, const char*)> & func,
	const std::function<void(const char *, const char*)> & func1,
	const std::function<void(const char *, const char*)> & func2,
	const RKString & title_btn_0,
	const RKString & title_btn_1
)
{

	p_current_dialog = type;
	p_time_show_button = -1.f;

	static_cast<TextWidget*>(p_text_title)->SetText(str_title);
	static_cast<TextWidget*>(p_text_content)->SetText(str_message);

	if (p_current_dialog == TYPE_DIALOG::DIALOG_ONE_BUTTON)
	{
		p_panel_1_btn->Visible(true);
		p_panel_2_btn->Visible(false);
		p_panel_0_btn->Visible(false);
		p_panel_fb_btn->Visible(false);
	}
	else if (p_current_dialog == TYPE_DIALOG::DIALOG_TWO_BUTTON)
	{
		p_panel_1_btn->Visible(false);
		p_panel_2_btn->Visible(true);
		p_panel_0_btn->Visible(false);
		p_panel_fb_btn->Visible(false);
	}
	else if (p_current_dialog == TYPE_DIALOG::DIALOG_ZERO_BUTTON || p_current_dialog == DIALOG_ZERO_BUTTON_WAIT)
	{
		p_panel_1_btn->Visible(false);
		p_panel_2_btn->Visible(false);
		p_panel_0_btn->Visible(true);
		p_panel_fb_btn->Visible(false);
		if (p_current_dialog == DIALOG_ZERO_BUTTON_WAIT)
		{
			p_time_show_button = 2.f;
		}
	}
	else if (p_current_dialog == TYPE_DIALOG::DIALOG_LOGOUT_ACC)
	{
		p_panel_1_btn->Visible(false);
		p_panel_2_btn->Visible(false);
		p_panel_0_btn->Visible(false);
		p_panel_fb_btn->Visible(true);
	}
	else if (p_current_dialog == TYPE_DIALOG::DIALOG_WAITING_BUTTON)
	{
		p_panel_1_btn->Visible(false);
		p_panel_2_btn->Visible(false);
		p_panel_0_btn->Visible(false);
		p_panel_fb_btn->Visible(false);
		p_time_show_button = 3.0f;
	}


	p_Function0 = func;
	p_Function1 = func1;
	p_Function2 = func2;

	if (title_btn_0 != "")
	{
		static_cast<TextWidget*>(p_panel_1_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText(title_btn_0, true);
		static_cast<TextWidget*>(p_panel_2_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText(title_btn_0, true);
		static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText(title_btn_0, true);
	}
	else
	{
		static_cast<TextWidget*>(p_panel_1_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText("agree");
		static_cast<TextWidget*>(p_panel_2_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText("agree");
		static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0))->SetText("agree");
	}

	if (title_btn_1 != "")
	{
		static_cast<TextWidget*>(p_panel_2_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText(title_btn_1, true);
		static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText(title_btn_1, true);
	}
	else
	{
		static_cast<TextWidget*>(p_panel_2_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText("cancel");
		static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildAtIdx(1)->GetWidgetChildAtIdx(0))->SetText("cancel");
	}

	AligneSizePanel();

	if (!p_dialog_layout->GetParentWidget()->Visible() || in_stage_show == 3)
	{
		if (in_stage_show == 3)
		{
			this->p_dialog_layout->ForceFinishAction(); 
		}

		cocos2d::Size size_target = p_dialog_layout->GetResource()->getContentSize();

		xml::BasicDec xml_value;
		xml_value.InsertDataVector2("SIZE", size_target * 1.2f);
		p_dialog_layout->SetHookCommand("ON_SHOW", "act0", 0, 0, xml_value, "SIZE");
		xml::BasicDec xml_value1;
		xml_value1.InsertDataVector2("SIZE", size_target);
		p_dialog_layout->SetHookCommand("ON_SHOW", "act0", 0, 1, xml_value1, "SIZE");

		p_dialog_layout->GetParentWidget()->Visible(true);
		p_dialog_layout->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
			this->p_dialog_layout->ForceFinishAction();
			this->in_stage_show = 1;
		}));

		p_dialog_layout->ClearHookCommand("ON_SHOW", "act0", 0, 0);
		p_dialog_layout->ClearHookCommand("ON_SHOW", "act0", 0, 1);
	}

	in_stage_show = 2;

}

void DialogLayer::OnProcessBtnFunc()
{
	if (p_current_dialog == DIALOG_ONE_BUTTON ||
		((p_current_dialog == DIALOG_WAITING_BUTTON || p_current_dialog == DIALOG_ZERO_BUTTON_WAIT) && p_nameBtnProcess == "") ||
		p_current_dialog == DIALOG_ZERO_BUTTON ||
		p_nameBtnProcess == "ok_2_btn" ||
		p_nameBtnProcess == "ok_fb_btn"
		)
	{
		if (p_Function0 != nullptr)
		{
            bool selected = false;
            if(p_current_dialog == DIALOG_LOGOUT_ACC)
            {
                selected = static_cast<CheckBoxWidget*>(p_panel_fb_btn->GetWidgetChildByName(".check_logout"))->IsSelected() ;
            }
            p_Function0(((selected  == true)? "1" : ""), p_nameBtnProcess.GetString());
		}
	}
	else if (p_nameBtnProcess == "cancel_fb_btn" || p_nameBtnProcess == "cancel_2_btn")
	{
		if (p_Function1 != nullptr)
		{
			p_Function1("", p_nameBtnProcess.GetString());
		}
	}

	p_Function0 = nullptr;
	p_Function1 = nullptr;
	p_Function2 = nullptr;
	p_nameBtnProcess = "";

}

void DialogLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	OnHideDialog();
}

void DialogLayer::OnHideDialog()
{
	if (p_dialog_layout->GetParentWidget()->Visible())
	{
		in_stage_show = 3;
		p_dialog_layout->ForceFinishAction();
		p_dialog_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
			this->p_dialog_layout->ForceFinishAction();
			this->p_dialog_layout->GetParentWidget()->Visible(false);
			this->OnProcessBtnFunc();
			this->in_stage_show = 0;
		}));
	}
}

void DialogLayer::AligneSizePanel()
{
	auto _w_msg = static_cast<TextWidget*>(p_text_content);
	RKString current_string = _w_msg->GetText();
	int leng_txt = current_string.LengthUTF();
	float font_size = static_cast<TextWidget*>(_w_msg)->GetFontSize();
	Vec2 delta_scale = Vec2(GetGameSize().width / _base_screen->GetDesignSize().x, GetGameSize().height / _base_screen->GetDesignSize().y);
	WidgetEntity * hlbg = p_dialog_layout->GetWidgetChildByName(".layout_.hl_panel_");
	float actual_size = font_size * leng_txt;
	if (actual_size > _w_msg->GetTextAreaSize().x * 1.2f)
	{
		int delta_size = 10 * delta_scale.x;
		float target_x = 650 * delta_scale.x;
		float target_y = 0;
		float header_size_h = 0;
		auto back_panel = p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0);
		auto sprite_frame = back_panel->GetFrameSource();
		Vec2 current_size_text = _w_msg->GetActualContainSize();
		Vec2 size_layout_btn = p_panel_1_btn->GetParentWidget()->GetSize();
        if(p_panel_fb_btn->Visible())
        {
            size_layout_btn.y += (60 *delta_scale.y);
        }
		if (sprite_frame)
		{
			header_size_h = (sprite_frame->getRect().size.height) * delta_scale.x;
		}
		/*if (GetGameSize().width < 1000)
		{
			header_size_h = delta_size;
		}*/

		target_y = size_layout_btn.y + delta_size + current_size_text.y +  delta_size + header_size_h;
        
		if (target_y < 200.f * delta_scale.x)
		{
			target_y = 200.f * delta_scale.x;
		}
		else if (target_y > 400.f * delta_scale.x)
		{
			p_text_content->Visible(false);
			WidgetEntity * scroll_text = p_dialog_layout->GetWidgetChildByName(".layout_.scroll_content");
			scroll_text->Visible(true);
			WidgetEntity * text_contain = scroll_text->GetWidgetChildAtIdx(0);
			static_cast<TextWidget*>(text_contain)->SetText(current_string, true);
			scroll_text->SetPosition(Vec2(target_x / 2, size_layout_btn.y + delta_size));
			static_cast<ScrollView*>(scroll_text->GetResource())->setInnerContainerSize(cocos2d::Size(target_x, current_size_text.y));
			text_contain->SetPosition(Vec2(target_x / 2, current_size_text.y));
			target_y = 400.f * delta_scale.x;
		}

		p_dialog_layout->GetResource()->setContentSize(cocos2d::Size(target_x, target_y));

		//auto back_panel = p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0);
		back_panel->SetSize(Vec2(target_x, target_y));
		back_panel->SetPosition(Vec2(target_x / 2, 0));

		auto widget_panel_btn = p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(5);
		widget_panel_btn->SetPosition(Vec2(target_x / 2, 0));
//        _w_msg->SetTextAreaSize(Vec2(target_x * 3.f / 4.f , 0));

		_w_msg->SetPosition(Vec2(target_x / 2, size_layout_btn.y + (target_y - header_size_h - size_layout_btn.y) / 2.f));
		p_text_title->SetPosition(Vec2(target_x / 2, target_y - (1.5f * delta_size)));
		hlbg->SetSize(Vec2(target_x / 1.25f, target_y / 1.25f));
		hlbg->SetPosition(Vec2(target_x / 2, 0));
	}
	else
	{
		int delta_size = 10 * delta_scale.x;
		float target_x = 500 * delta_scale.x;
		float target_y = 0;

		float header_size_h = delta_size;
		auto back_panel = p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0);
		auto sprite_frame = back_panel->GetFrameSource();

		Vec2 current_size_text = _w_msg->GetActualContainSize();
		Vec2 size_layout_btn = p_panel_1_btn->GetParentWidget()->GetSize();
        if(p_panel_fb_btn->Visible())
        {
            size_layout_btn.y += (60 *delta_scale.y);
        }
		if (sprite_frame)
		{
			header_size_h = (sprite_frame->getRect().size.height) * delta_scale.x;
		}
		/*if (GetGameSize().width < 1000)
		{
			header_size_h = delta_size;
		}*/

		target_y = size_layout_btn.y + delta_size + current_size_text.y + delta_size + header_size_h;
		if (target_y < 200.f * delta_scale.x)
		{
			target_y = 200.f * delta_scale.x;
		}
		else if (target_y > 400.f * delta_scale.x)
		{
			p_text_content->Visible(false);
			WidgetEntity * scroll_text = p_dialog_layout->GetWidgetChildByName(".layout_.scroll_content");
			scroll_text->Visible(true);
			WidgetEntity * text_contain = scroll_text->GetWidgetChildAtIdx(0);
			static_cast<TextWidget*>(text_contain)->SetText(current_string, true);
			scroll_text->SetPosition(Vec2(target_x / 2, size_layout_btn.y + delta_size));
			static_cast<ScrollView*>(scroll_text->GetResource())->setInnerContainerSize(cocos2d::Size(target_x, current_size_text.y));
			text_contain->SetPosition(Vec2(target_x / 2, current_size_text.y));
			target_y = 400.f * delta_scale.x;
		}

		p_dialog_layout->GetResource()->setContentSize(cocos2d::Size(target_x, target_y));
		p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0)->SetSize(Vec2(target_x, target_y));
		p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(0)->SetPosition(Vec2(target_x / 2, 0));
		auto widget_panel_btn = p_dialog_layout->GetWidgetChildAtIdx(0)->GetWidgetChildAtIdx(5);
		widget_panel_btn->SetPosition(Vec2(target_x / 2, 0));
//		_w_msg->SetTextAreaSize(Vec2(target_x * 3.f / 4.f , 0));
		_w_msg->SetPosition(Vec2(target_x / 2, size_layout_btn.y + ((target_y - header_size_h - size_layout_btn.y) / 2.f)));
		p_text_title->SetPosition(Vec2(target_x / 2, target_y - (1.5f * delta_size)));
		hlbg->SetSize(Vec2(target_x / 1.25f, target_y / 1.25f));
		hlbg->SetPosition(Vec2(target_x / 2, 0));
	}
    
    if(p_panel_fb_btn->Visible())
    {
        int loginType = -1;
        ScrMgr->GetDataSaveInt(STR_SAVE_TYPE_LOGIN, loginType);
        if(loginType == LOGIN_FACEBOOK)
        {
            static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildByName(".title_"))->SetText("logout_facebook");
        }
        else if (loginType == LOGIN_GOOGLE_PLAY)
        {
            auto text = LANG_STR("sign_out");
            text += " GOOGLE";
            static_cast<TextWidget*>(p_panel_fb_btn->GetWidgetChildByName(".title_"))->SetText(text , true);
        }
        static_cast<CheckBoxWidget*>(p_panel_fb_btn->GetWidgetChildByName(".check_logout"))->OnSetSelected(false);
    }
}
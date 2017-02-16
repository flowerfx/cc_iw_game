#include "TutorialLayer.h"
#include "UI/AdvanceUI/wText.h"

#include "Screens/MainScreen.h"

TutorialLayer::TutorialLayer()
{
	_tutorial_layer = nullptr;
	p_current_step_tutorial = 0;
}

TutorialLayer::~TutorialLayer()
{
	_tutorial_layer = nullptr;
}

bool TutorialLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;

	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "close_tutorial_btn")
		{
			CloseTutorial();
			return true;
		}
		else if (name == "next_tutorial_btn")
		{
			if (p_current_step_tutorial < 4)
			{
				MoveStepTutorial(p_current_step_tutorial + 1);
			}
			else
			{
				CloseTutorial();
				p_current_step_tutorial = 0;
			}
			return true;
		}
	}

	return false;
}

void TutorialLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	_tutorial_layer = screen->GetWidgetChildByName("common_screen.tutorial_layer");

	_tutorial_msg_layer = _tutorial_layer->GetWidgetChildByName(".msg_layer");
	_tutorial_txt = _tutorial_msg_layer->GetWidgetChildByName(".text");
	_tutorial_txt_bg = _tutorial_msg_layer->GetWidgetChildByName(".msg_bg");
	_show_item_tutorial = _tutorial_layer->GetWidgetChildByName(".show_item");
}
void TutorialLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	_tutorial_layer->Visible(true);
	_show_item_tutorial->ClearChild();
	MoveStepTutorial(0);
}

void TutorialLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	CloseTutorial(nullptr);
}

void TutorialLayer::CloseTutorial(CallFunc * call_back)
{
	_tutorial_layer->Visible(false);
}

bool TutorialLayer::IsVisibleLayer()
{
	return _tutorial_layer->Visible();
}

void TutorialLayer::MoveStepTutorial(int step)
{
	p_current_step_tutorial = step;

	WidgetEntity * object = nullptr;
	WidgetEntity * clone_object = nullptr;
	if (p_current_step_tutorial == 0)
	{
		_tutorial_txt->SetActionCommandWithName("STEP_0", CallFunc::create([this]() {
			_tutorial_txt->ForceFinishAction();
			auto size_text = static_cast<TextWidget*>(_tutorial_txt)->GetActualContainSize();
			_tutorial_txt_bg->GetWidgetChildByName(".img_bg")->SetSize(size_text);
		}));
		_tutorial_txt_bg->SetRotate(0);
		auto rect1 = static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->GetWorldObjectPosIntutorial(0, object);
		_tutorial_msg_layer->GetResource()->setAnchorPoint(Vec2(0.5, 1));
		_tutorial_msg_layer->SetPosition(Vec2(rect1.origin.x + rect1.size.width / 2, rect1.origin.y));
		if (object)
		{
			clone_object = object->Clone();
		}
	}
	else if (p_current_step_tutorial == 1)
	{
		_tutorial_txt->SetActionCommandWithName("STEP_1", CallFunc::create([this]() {
			_tutorial_txt->ForceFinishAction();
			auto size_text = static_cast<TextWidget*>(_tutorial_txt)->GetActualContainSize();
			_tutorial_txt_bg->GetWidgetChildByName(".img_bg")->SetSize(size_text);
		}));
		_tutorial_txt_bg->SetRotate(180);
		auto rect1 = static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->GetWorldObjectPosIntutorial(1, object);
		_tutorial_msg_layer->GetResource()->setAnchorPoint(Vec2(0.5, 0));
		_tutorial_msg_layer->SetPosition(Vec2(rect1.origin.x + rect1.size.width / 2, rect1.origin.y + rect1.size.height));

		if (object)
		{
			clone_object = object->Clone();
		}
	}
	else if (p_current_step_tutorial == 2)
	{
		_tutorial_txt->SetActionCommandWithName("STEP_2", CallFunc::create([this]() {
			_tutorial_txt->ForceFinishAction();
			auto size_text = static_cast<TextWidget*>(_tutorial_txt)->GetActualContainSize();
			_tutorial_txt_bg->GetWidgetChildByName(".img_bg")->SetSize(size_text);
		}));
		_tutorial_txt_bg->SetRotate(180);
		auto rect1 = static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->GetWorldObjectPosIntutorial(2, object);
		_tutorial_msg_layer->GetResource()->setAnchorPoint(Vec2(0.5, 0));
		_tutorial_msg_layer->SetPosition(Vec2(rect1.origin.x + rect1.size.width / 2, rect1.origin.y + rect1.size.height));

		if (object)
		{
			clone_object = object->Clone();
			clone_object->GetResource()->setAnchorPoint(Vec2(0, 0));
			clone_object->SetPosition(rect1.origin);
		}
	}
	else if (p_current_step_tutorial == 3)
	{
		auto rect = GetCommonScr->GetRectMiniBtn(object);
		if (rect.origin.x < GetGameSize().height / 2)
		{
			_tutorial_txt->SetActionCommandWithName("STEP_3_5", CallFunc::create([this]() {
				_tutorial_txt->ForceFinishAction();
				auto size_text = static_cast<TextWidget*>(_tutorial_txt)->GetActualContainSize();
				_tutorial_txt_bg->GetWidgetChildByName(".img_bg")->SetSize(size_text);
			}));
			_tutorial_txt_bg->SetRotate(0);
			_tutorial_msg_layer->GetResource()->setAnchorPoint(Vec2(0.5, 1));
			_tutorial_msg_layer->SetPosition(Vec2(rect.origin.x + rect.size.width / 2, rect.origin.y));
		}
		else
		{
			_tutorial_txt->SetActionCommandWithName("STEP_3", CallFunc::create([this]() {
				_tutorial_txt->ForceFinishAction();
				auto size_text = static_cast<TextWidget*>(_tutorial_txt)->GetActualContainSize();
				_tutorial_txt_bg->GetWidgetChildByName(".img_bg")->SetSize(size_text);
			}));
			_tutorial_txt_bg->SetRotate(180);
			_tutorial_msg_layer->GetResource()->setAnchorPoint(Vec2(0.5, 0));
			_tutorial_msg_layer->SetPosition(Vec2(rect.origin.x + rect.size.width / 2, rect.origin.y + rect.size.height));
		}

		if (object)
		{
			clone_object = object->Clone();
			clone_object->GetResource()->setAnchorPoint(Vec2(0, 0));
			clone_object->SetPosition(rect.origin);
		}

	}
	else if (p_current_step_tutorial == 4)
	{
		_tutorial_txt->SetActionCommandWithName("STEP_4", CallFunc::create([this]() {
			_tutorial_txt->ForceFinishAction();
			auto size_text = static_cast<TextWidget*>(_tutorial_txt)->GetActualContainSize();
			_tutorial_txt_bg->GetWidgetChildByName(".img_bg")->SetSize(size_text);
		}));
		_tutorial_txt_bg->SetRotate(180);
		auto rect1 = static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->GetWorldObjectPosIntutorial(4, object);
		_tutorial_msg_layer->GetResource()->setAnchorPoint(Vec2(0.5, 0));
		_tutorial_msg_layer->SetPosition(Vec2(rect1.origin.x + rect1.size.width / 2, rect1.origin.y + rect1.size.height));

		if (object)
		{
			clone_object = object->Clone();
			clone_object->GetResource()->setAnchorPoint(Vec2(0, 0));
			clone_object->SetPosition(rect1.origin);
		}
	}

	if (clone_object)
	{
		_show_item_tutorial->AddChildWidget(clone_object);
	}

	if (p_current_step_tutorial < 4)
	{
		auto txt = _tutorial_layer->GetWidgetChildByName(".next_tutorial_btn.txt");
		static_cast<TextWidget*>(txt)->SetText(
			StringUtils::format(LANG_STR("cont").GetString(), p_current_step_tutorial + 1, 5), true);

		auto size = static_cast<TextWidget*>(txt)->GetActualContainSize();
		auto curent_size = _tutorial_layer->GetWidgetChildByName(".next_tutorial_btn")->GetSize();
		_tutorial_layer->GetWidgetChildByName(".next_tutorial_btn")->SetSize(Vec2(size.x + 10, curent_size.y));

		txt->SetPosition(Vec2((size.x + 10) / 2, curent_size.y / 2));
	}
	else
	{
		auto txt = _tutorial_layer->GetWidgetChildByName(".next_tutorial_btn.txt");
		static_cast<TextWidget*>(txt)->SetText(LANG_STR("complete"));

		auto size = static_cast<TextWidget*>(txt)->GetActualContainSize();
		auto curent_size = _tutorial_layer->GetWidgetChildByName(".next_tutorial_btn")->GetSize();
		_tutorial_layer->GetWidgetChildByName(".next_tutorial_btn")->SetSize(Vec2(size.x + 20, curent_size.y));

		txt->SetPosition(Vec2((size.x + 20) / 2, curent_size.y / 2));
	}

	static_cast<TextWidget*>(_tutorial_layer->GetWidgetChildByName(".step_txt"))->SetText(std::to_string(p_current_step_tutorial + 1) + "/5", true);
	
}

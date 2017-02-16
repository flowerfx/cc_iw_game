#include "IwinTextField.h"
#include "UI/AdvanceUI/wButton.h"
#include "Platform/Platform.h"
using namespace Utility;
using namespace Utility::UI_Widget;

void IwinTextFieldHandle::IwinClearText() {
	this->ClearText();
}

RKString IwinTextFieldHandle::GetTFName()
{
	return this->GetName();
}

WidgetEntity * IwinTextFieldHandle::Clone()
{
	IwinTextFieldHandle * p_clone = new IwinTextFieldHandle();
	p_clone->CopyValue(this);
	//remove child remain
	p_clone->p_Resource->removeAllChildrenWithCleanup(true);
	//copy child
	CloneChild(p_clone);
	CloneThis(p_clone);

	p_clone->_btn_clear_text = _btn_clear_text;
	p_clone->_callback_attach = _callback_attach;
	p_clone->_callback_detach = _callback_detach;
	p_clone->_callback_insert = _callback_insert;
	p_clone->_callback_return = _callback_return;

	return p_clone;
}

void IwinEditBoxdHandle::IwinClearText() {
	this->ClearText();
}

RKString IwinEditBoxdHandle::GetTFName()
{
	return this->GetName();
}

WidgetEntity * IwinEditBoxdHandle::Clone() {
	IwinEditBoxdHandle * p_clone = new IwinEditBoxdHandle();
    p_clone->InitParam(this->GetName(), this->p_xml_dec);
    p_clone->SetValueFromXML(this->p_xml_dec);
	//p_clone->CopyValue(this);
	//remove child remain
	//p_clone->p_Resource->removeAllChildrenWithCleanup(true);
	//copy child
	//CloneChild(p_clone);
	//CloneThis(p_clone);

	p_clone->_btn_clear_text = _btn_clear_text;
	p_clone->_callback_attach = _callback_attach;
	p_clone->_callback_detach = _callback_detach;
	p_clone->_callback_insert = _callback_insert;
	p_clone->_callback_return = _callback_return;

	return p_clone;
}

IwinTextField::IwinTextField()
{
	_btn_clear_text = nullptr;

	_callback_attach = nullptr;
	_callback_detach = nullptr;
	_callback_insert = nullptr;
	_callback_delete = nullptr;
	_callback_return = nullptr;
}

IwinTextField::~IwinTextField()
{
	_btn_clear_text = nullptr;

	_callback_attach = nullptr;
	_callback_detach = nullptr;
	_callback_insert = nullptr;
	_callback_delete = nullptr;
	_callback_return = nullptr;
}

void IwinTextField::SetBtnClear(Utility::UI_Widget::WidgetEntity* btn)
{
	if (_btn_clear_text == btn)
		return;

	_btn_clear_text = btn;

	if (_btn_clear_text)
	{
		static_cast<ButtonWidget*>(_btn_clear_text)->SetActionBtn([this]() {
				_btn_clear_text->Visible(false);
				this->IwinClearText();
		});
	}
}

bool IwinTextField::HandleEventTextField(RKString name)
{
	auto plist = name.SplitLast("_", 2);
	RKString name_ac = plist[1];
	RKString name_w = plist[0];
	if (name_w != this->GetTFName())
	{
		return false;
	}
	if (name_ac == "DETACH")
	{
		if (_btn_clear_text)
		{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
			_btn_clear_text->Visible(false);
#else
			Platform::OnShowNativeClearTextBtn(false, -1, 0);
#endif
		}
		if (_callback_detach)
		{
			_callback_detach();
		}
	}
	else if (name_ac == "ATTACH")
	{
		if (_btn_clear_text)
		{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
			_btn_clear_text->Visible(true);
#else
			Vec2 pos_btn = GetWorldPosCloseBtn(Vec2(-1, 0));
			Platform::OnShowNativeClearTextBtn(true, pos_btn.x, pos_btn.y);
#endif
		}
		if (_callback_attach)
		{
			_callback_attach();
		}
	}
	else if (name_ac == "INSERT")
	{
		if (_callback_insert)
		{
			_callback_insert();
		}

		if (_btn_clear_text)
		{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
			_btn_clear_text->Visible(true);
#else
			Vec2 pos_btn = GetWorldPosCloseBtn(Vec2(-1, 0));
			Platform::OnShowNativeClearTextBtn(true, pos_btn.x, pos_btn.y);
#endif
		}
	}
	else if (name_ac == "DELETE")
	{
		if (_callback_delete)
		{
			_callback_delete();
		}
	}
	else if (name_ac == "ENTER")
	{
		if (_callback_return)
		{
			_callback_return();
		}
	}

	return true;
}

void IwinTextField::InitCallBack(
	const std::function<void(void)> & _call_attach ,
	const std::function<void(void)> & _call_detach ,
	const std::function<void(void)> & _call_insert ,
	const std::function<void(void)> & _call_delete ,
	const std::function<void(void)> & _call_return 
)
{
	_callback_attach = _call_attach;
	_callback_detach = _call_detach;
	_callback_insert = _call_insert;
	_callback_delete = _call_delete;
	_callback_return = _call_return;
}

Vec2 IwinTextField::GetWorldPosCloseBtn(Vec2 anchor)
{
	if (_btn_clear_text)
	{
		if (_btn_clear_text->GetResource()->getParent())
		{
			auto pos_anchor_child = _btn_clear_text->PositionWithVectorUnit(anchor);
			return _btn_clear_text->GetResource()->getParent()->convertToWorldSpace(pos_anchor_child);
		}
		else
		{
			return _btn_clear_text->PositionWithVectorUnit(anchor);
		}
	}

	return Vec2(-1, -1);
}
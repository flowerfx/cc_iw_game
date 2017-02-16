#ifndef __IWIN_TEXT_FIELD_H___
#define __IWIN_TEXT_FIELD_H___

#include "UI/AdvanceUI/wTextField.h"

using namespace Utility;
using namespace Utility::UI_Widget;

#if defined OS_IOS || defined OS_MAC || defined OS_ANDROID || defined OS_W10 || defined WP8 || defined OS_W8
#define IWIN_TF IwinEditBoxdHandle
#else
#define IWIN_TF IwinTextFieldHandle
#endif

class IwinTextField
{
protected:
	std::function<void(void)> _callback_attach;
	std::function<void(void)> _callback_detach;
	std::function<void(void)> _callback_insert;
	std::function<void(void)> _callback_delete;
	std::function<void(void)> _callback_return;

	Utility::UI_Widget::WidgetEntity* _btn_clear_text;
public:
	IwinTextField();
	~IwinTextField();


	void InitCallBack(
		const std::function<void(void)> & _call_attach,
		const std::function<void(void)> & _call_detach = nullptr,
		const std::function<void(void)> & _call_insert = nullptr,
		const std::function<void(void)> & _call_delete = nullptr,
		const std::function<void(void)> & _call_return = nullptr
	);
	void SetBtnClear(Utility::UI_Widget::WidgetEntity* btn);
	bool HandleEventTextField(RKString name);
	virtual void IwinClearText() {};
	virtual RKString GetTFName() { return ""; }

	//get the world pos close btn to set the native close btn (Wp/ios/android)
	Vec2 GetWorldPosCloseBtn(Vec2 anchor);
};

class IwinTextFieldHandle : public Utility::UI_Widget::TextFieldHandleWidget , public IwinTextField
{

public:
	IwinTextFieldHandle() {}
	virtual ~IwinTextFieldHandle() {}

	virtual void IwinClearText() override;
	virtual WidgetEntity * Clone() override;
	virtual RKString GetTFName() override;
};

class IwinEditBoxdHandle : public Utility::UI_Widget::EditBoxWidget, public IwinTextField
{

public:
	IwinEditBoxdHandle(){}
	virtual ~IwinEditBoxdHandle() {}

	virtual void IwinClearText() override;
	virtual WidgetEntity * Clone() override;
	virtual RKString GetTFName() override;
};


#endif // __IWIN_LIST_VIEW_H__
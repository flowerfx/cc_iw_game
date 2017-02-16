#ifndef __DIALOG_LAYER_H__
#define __DIALOG_LAYER_H__
#include "Screens/CommonScreens/CommonLayer.h"
using namespace Utility;
using namespace Utility::UI_Widget;
using namespace iwinmesage;

class DialogLayer : public CommonLayer
{
private:
	/*
	use : dialog screen here
	*/
	TYPE_DIALOG		p_current_dialog;
	RKString		p_nameBtnProcess;
	//for the call back

	std::function<void(const char * str_call_back, const char* btn_name)>		p_Function0;
	std::function<void(const char * str_call_back, const char* btn_name)>		p_Function1;
	std::function<void(const char * str_call_back, const char* btn_name)>		p_Function2;

	float		   p_time_show_button;

	WidgetEntity * p_dialog_layout;
	WidgetEntity * p_panel_1_btn;
	WidgetEntity * p_panel_2_btn;
	WidgetEntity * p_panel_0_btn;
	WidgetEntity * p_panel_fb_btn;

	WidgetEntity * p_text_title;
	WidgetEntity * p_text_content;

	//0 is hide , 1 is on appearing, 2 is on show, 3 is on hide
	int in_stage_show;
protected:
	void AligneSizePanel();
	void OnProcessBtnFunc();
public:
	DialogLayer();
	virtual ~DialogLayer();
	virtual void InitLayer(BaseScreen * screen)	override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual	int Update(float dt) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr)	override;
	virtual bool IsVisibleLayer() override;
	//call popup method
	void OnShowDialog(
		const RKString & str_title = "",
		const RKString & str_message = "",
		TYPE_DIALOG type = TYPE_DIALOG::DIALOG_NONE,
		const char * strCB = "",
		const std::function<void(const char * call_back, const char* btn_name)> & func = nullptr,
		const std::function<void(const char * call_back, const char* btn_name)> & func1 = nullptr,
		const std::function<void(const char * call_back, const char* btn_name)> & func2 = nullptr,
		const RKString & title_btn_0 = "", 
		const RKString & title_btn_1 = ""
	);
	void OnHideDialog();
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
};


#endif  //__DIALOG_LAYER_H__


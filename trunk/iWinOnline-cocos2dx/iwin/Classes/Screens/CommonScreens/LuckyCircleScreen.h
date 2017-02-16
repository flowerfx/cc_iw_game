#ifndef __LUCKY_CIRCLE_SCREEN_H__
#define __LUCKY_CIRCLE_SCREEN_H__
#include "Screens/CommonScreens/CommonLayer.h"
using namespace Utility;
using namespace Utility::UI_Widget;

namespace iwinmesage
{
	class LuckyCircleResultList;
	class LuckyCircleQuestList;
}
using namespace iwinmesage;

class LuckyCircleScreen : public CommonLayer
{

private:

	LuckyCircleQuestList*  p_lucky_quest_list;
	LuckyCircleResultList*  p_lucky_result;

	bool isRotation;
	int index_result;
	RKString _url_captcha;
	RKString _token_captcha;

	WidgetEntity* p_layout_main;
	WidgetEntity* p_arow_tick;
	WidgetEntity* p_list_view_result;
	WidgetEntity* p_btn_money_win;
	WidgetEntity* p_btn_money_rubby;
	
	WidgetEntity* p_popup_result;
	WidgetEntity* p_lb_money_win;
	WidgetEntity* p_lb_ticket;

	WidgetEntity*  p_img_circle;
	WidgetEntity* p_check_auto;
	WidgetEntity* p_anim_money;

	WidgetEntity* p_layout_auto_spin;
	WidgetEntity* p_layout_captcha;

	WidgetEntity * p_btn_help;

	WidgetEntity * p_hl_result;

	void RemoveImageFinish();
	void ShowResult();


public:

	LuckyCircleScreen();
	virtual ~LuckyCircleScreen();

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual int  Update(float dt)	override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr)		override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr)		override;
	virtual bool IsVisibleLayer() override;

	void SetVisible(bool isShow);
	void OnRecieveLuckyCircleResultList(LuckyCircleQuestList* lucky_quest_list);
	void OnRecieveLuckyCircleResultItem(LuckyCircleResultList* lucky_result_item);
	void ActionRotation(float angle);
	void OnRotationDone();
	void UpdateMoney();

	void SetLinkUrlCaptcha(RKString url);
	void SetTokenCaptcha(RKString token);

};

#endif 


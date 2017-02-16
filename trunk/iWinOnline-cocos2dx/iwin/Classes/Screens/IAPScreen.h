#ifndef __SCREEN_IAP_H__
#define __SCREEN_IAP_H__


#include "ScreenManager.h"
//#include "ui/UIScrollView.h"
using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;


class IwinListView;
class IwinScrollView;
class FocusScrollView;
class SpriteIap;

namespace iwinmesage
{
	class Payment;
	class CaptchaInfo;
	class CaptchaResult;
}


class IAPScreen : public BaseScreen
{
private:
	WidgetEntity * p_panel_iap_method;
	WidgetEntity * p_list_choose_iap_method;

	WidgetEntity * p_icon_arrow_backward;
	WidgetEntity * p_icon_arrow_forward;

	WidgetEntity*	p_scroll_item;
	WidgetEntity*	p_scroll_item_card_value;
	WidgetEntity*	p_scroll_item_telco;

	WidgetEntity*	p_scroll_view_iap;
	WidgetEntity*	p_scroll_view_banking;
	WidgetEntity*	p_scroll_view_sms;
	WidgetEntity*	p_scroll_view_value_card;
	WidgetEntity*	p_scroll_view_telco_sms;
	WidgetEntity*	p_scroll_view_telco_card;

	WidgetEntity*	p_textfield_bin;
	WidgetEntity*	p_textfield_serial;

	WidgetEntity*	p_captcha_text_field;

	WidgetEntity*	p_group_button_win;
	WidgetEntity*	p_group_button_ruby;

	WidgetEntity*	p_layout_btn_award;

	IwinListView*		_choose_payment_method;
	IwinScrollView*		_scroll_view_iap;
	IwinScrollView*		_scroll_view_banking;
	IwinScrollView*		_scroll_view_sms;
	IwinScrollView*		_scroll_view_value_card;
	FocusScrollView*	_scroll_view_telco_sms;
	FocusScrollView*	_scroll_view_telco_card;

	SpriteIap*			_spr_captcha;
	iwinmesage::CaptchaInfo	*		_cur_captcha;

	// parsed data
	std::vector<Payment*>	_payment_method_data;
	std::vector<Payment*>	_telco_data_sms;
	std::vector<Payment*>	_telco_data_card;
	std::vector<Payment*>	_telco_data_banking;
	std::vector<Payment*>	_telco_data_iap;

	Payment*				_payment_win;
	Payment*				_payment_ruby;

	Payment*				_payment;

	int						_cur_type_payment;
	int						_payment_err;
	bool					_check_downloading = false;

	int						_iap_style;

	void showSms();
	void showBanking();
	void showCards();
	void showIap();
	void showPayment(int idx);
	void onPaymentInfo();
	void parsePaymentInfo(Payment* payment, int level);

	void parseImageUrl(std::string ser_url, std::string& image_url, std::string& save_path);
	void unfocusPaymentList();
	void reloadTelcoListWithSms();
	void reloadTelcoListWithCard();
	void reloadBanking();
	void reloadIap();
	void reloadSms(int telco_idx);
	void reloadCard(int telco_idx);
	void setupScrollItem(WidgetEntity* entity);
	void updateCaptchaImage();
	void reloadPayment();
	void updateFocusPaymentList(int idx);
	void showWinBtn(bool visible);
	void showVerify(int error);
public:
	IAPScreen();
	virtual ~IAPScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnBeginFadeIn()															override;
	virtual void SetActionAfterShow(int action)												override;
	void StateShowArrow(int state);
	void OnShowArrowAsList(WidgetEntity * _list);
	virtual void OnKeyBack()																override;
	void reload();
	void updateItemPaymentMethod(int eventType, WidgetEntity* entity, int idx);
	void initItemScrollView(IwinScrollView* scrollview, WidgetEntity* entity, int idx);
	void updateItemScrollView(IwinScrollView* scrollview, WidgetEntity* entity, int idx);
	void updateItemScrollViewValueCard(IwinScrollView* scrollview, WidgetEntity* entity, int idx);
	void updateItemScrollViewCard(WidgetEntity* entity, int idx);
	void updateItemScrollViewTelco(FocusScrollView* scrollview, WidgetEntity* entity, int idx);
	void initItemScrollViewTelco(FocusScrollView* scrollview, WidgetEntity* entity, int idx);
	void updateChangeFocusTelco(FocusScrollView* scrollview, WidgetEntity* entity, bool focus, int focus_idx);
	void initScrollViewTelco(FocusScrollView* &view, WidgetEntity* box);

	void initFake();
	void updateScrolling(Ref* ref, ScrollView::EventType evType);


	void onReceiveCaptcha(iwinmesage::CaptchaInfo* info);
	void onReceiveCaptchaResult(iwinmesage::CaptchaResult* info);
	void onReceivePayment(iwinmesage::Payment* payment);

	int SetIAPStyle(int style);

};

#endif //__SCREEN_EVENT_H__


#ifndef __SCREEN_LOGIN_H__
#define __SCREEN_LOGIN_H__

#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
using namespace cocos2d;

class LoginLayer;
class LoginScreen : public BaseScreen
{
private:
	WidgetEntity * p_panel_first_choose_login;
    WidgetEntity * p_panel_logo_iwin;
	WidgetEntity * p_panel_choose_server;

	CommonLayer * _login_layer;
	CommonLayer * _register_layer;
	CommonLayer * _forget_pass_layer;
	CommonLayer * _otp_login_layer;
    

	WidgetEntity * p_list_server;
	WidgetEntity * p_btn_sound;
	WidgetEntity * p_layout_img_download;

public:
	LoginScreen();
	virtual ~LoginScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnBeginFadeIn()															override;
	virtual void OnFadeInComplete()															override;
	virtual void OnTouchMenuBegin(const cocos2d::Point & p)											override;
	virtual void SetActionAfterShow(int action)												override;
	void onSmsSyntaxInfo(s16 subId, std::string content, std::string address, std::string confirmMsg);
	
	void startWaitingDialog(LOGIN_TYPE login_type);

	void onCallBackCheckUserName(RKString msg);
    void onCallBackCheckEmail(RKString msg);
    void onCallBackRequestRegister(RKString msg);
	void onReceiveCaptcha(void* info);
	void onReceiveCaptchaResult(void* info);

	void onSetServer(int idx_server);
    
	RKString GetPassword();
    RKString GetUsername();

	int     GetStateLoginMenu();

	void OnSwitchServer(int idx);
    
    //void connect(cocos2d::Object *sender);
    //void disconnect(cocos2d::Object *sender);

	//otp
	void onReceiveRequireAuthenOTP(void * info);
	void onReceiveOTPStatus(void * list_data);
	void onReceiveAuthorizeOTP(void * data);
	bool IsOTPVisible();
	void HideOTPPage();
	//
	void	SwithToMainScreen(bool have_otp = false);

	//server
	void UpdateServerList();

	//clear text field pass when own user log-outS
	void OnClearPassInput();
};

#endif //__MENU_LOGIN_H__


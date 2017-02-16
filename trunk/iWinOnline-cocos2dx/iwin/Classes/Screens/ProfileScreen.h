#ifndef __PROFILE_SCREEN_H__
#define __PROFILE_SCREEN_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
class Achievement;
class Item;
class ProfileScreen : public BaseScreen
{
private:
	int p_previous_page_idx;

	WidgetEntity * p_top_panel;

	WidgetEntity * p_page_view_profile;
	WidgetEntity * p_page_view_achieve;


	WidgetEntity * p_layout_edit_profile;
	std::function<void(const char * buffer, unsigned int leng)> _callback_change_avatar;

	std::vector<Achievement*> p_list_achieve;

	//main user profile layer
	WidgetEntity * p_list_choose_sx;
	WidgetEntity * p_list_choose_day;
	WidgetEntity * p_list_choose_month;
	WidgetEntity * p_list_choose_year;
	CommonLayer * _main_user_profile_layer;
	//transaction history
	CommonLayer * _transaction_history_layer;
	//otp
	CommonLayer * _otp_page_layer;
	//transfer win/ruby dialog
	CommonLayer * _transfer_money_layout;
	//avatar shop
	CommonLayer * _avatar_shop_layout;
	//left user detail panel
	CommonLayer* _left_user_detail_panel;

	//
protected:
	
	bool p_receive_data_achievement;

	void OnShowIdxBtnTopPanel(int idx);

public:
	ProfileScreen();
	virtual ~ProfileScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnBeginFadeIn()															override;
	virtual void OnTouchMenuBegin(const cocos2d::Point & p)											override;
	virtual void ParseUserUI()																override;
	virtual void ClearComponent()															override;
	virtual void SetActionAfterShow(int action)												override;
	virtual void OnKeyBack()																override;

	void ShowLayoutState(int state);

	void RunThreadParseLB();

	void onSetAchievement(void * data);

	void OnParseAchievementIntoGame(WidgetEntity* widget , Achievement * achie, int idx);
	
	void onReceiveAvatarCatList(void * data);
	void onReceiveAvatarList(void * data);
	void onBuyingAvatarResult(void * data);

	void onChangeMyAvatar();
	void onMyAvatars(std::vector<Item*> items);
	void onMyItems(std::vector<Item*> items, int cateId, int totalPage, ubyte gender);
	void onEventItems(std::vector<Item*> items);

	bool IsShopAvatarVisible();

	void OnReceiveTransferInfo(int * arg_list);
	void OnRecieveTransactionHistory(void * trans_history);

	//otp
	void onReceiveAuthorizeOTP(void * data);
	void onReceiveOTPStatus(void * list_data);
	void onReceiveCHangeOtpDefault(int otpId, bool status, std::string msg);
	void onShowActiveOTPDialog(void * authen);

	int GetPageIdx();

	//action when open
};

#endif //__PROFILE_SCREEN_H__


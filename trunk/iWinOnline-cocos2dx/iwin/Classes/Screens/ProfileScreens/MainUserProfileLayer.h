#ifndef __MAIN_USER_PROFILE_LAYER_H__
#define __MAIN_USER_PROFILE_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class MainUserProfileLayer : public CommonLayer
{
public:
	MainUserProfileLayer();
	virtual ~MainUserProfileLayer();
private:
	WidgetEntity * p_page_profile_name;

	WidgetEntity * p_list_choose_sx;
	WidgetEntity * p_list_choose_day;
	WidgetEntity * p_list_choose_month;
	WidgetEntity * p_list_choose_year;


	ThreadWrapper * p_thread_check_time;

	bool p_user_dirty;
	bool p_is_create_list_choose;

	int p_cur_day;
	int p_cur_month;
	int p_cur_year;
protected:
	void InitTextFieldValue();


	void UpdatePlayerName_ToServer(const RKString & name);
	void UpdateSx_ToServer(int id);
	void UpdatePhoneNumber_ToServer(const RKString & number);
	void UpdateBirthDay_ToServer(int day, int month, int year);
	void UpdateAddress_ToServer(const RKString & address);
	void UpdateCMNDNumber_ToServer(int number);
	void UpdateEmail_ToServer(const RKString & email);

	void SetAccountName(const RKString & name);
	void SetPlayerName(const RKString & name);
	void SetSx(int id);
	void SetPhoneNumber(const RKString & number);
	void SetBirthDay(int day, int month, int year);
	void SetAddress(const RKString & address);
	void SetCMNDNumber(const RKString & number);
	void SetEmail(const RKString & email);

	bool CheckValidateDay(int &day, int &month, int &year, bool recorrect = false);
	void OnRecorrectDateTime();
	void RunThreadCheckDateTime();
public:

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual int  Update(float dt) override;

	void FuncWhenDeactiveLayer();
	void CreateListDate();
	void SetDetail(User * p_user);
	void OnShowOrHideListChoose(WidgetEntity * list, bool Show);

};

#endif //__MAIN_USER_PROFILE_LAYER_H__


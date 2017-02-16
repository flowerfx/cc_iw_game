#ifndef __LEFT_USER_PROFILE_LAYER_H__
#define __LEFT_USER_PROFILE_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class LeftUserProfileLayer : public CommonLayer
{
public:
	LeftUserProfileLayer();
	virtual ~LeftUserProfileLayer();
private:
	WidgetEntity * p_page_profile_left;
	WidgetEntity * p_panel_avatar;

	bool p_status_dirty;
protected:
	void UpdateStatus_ToServer(const RKString & status);
public:

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual int  Update(float dt) override;

	void SetAccountName(const RKString & name);
	void SetStatusPlayer(const RKString & status);
	void SetLevel(int level);
	void SetWinNumber(int number_win);
	void SetRubyNumber(int number_ruby);
	void SetHourPlayed(const RKString & hour);

	void ParseUserToLayer();
	void FuncWhenDeactiveLayer();
	WidgetEntity * GetStatusWidget();


};

#endif //__FORGET_PASS_LAYER_H__


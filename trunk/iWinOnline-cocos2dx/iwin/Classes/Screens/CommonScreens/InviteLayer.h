#ifndef __INVITE_LAYER_H__
#define __INVITE_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class InviteLayer : public CommonLayer
{
public:
	InviteLayer();
	virtual ~InviteLayer();
private:
	/*
	use : invite panel here
	*/
	int					_gameId;
	int					_roomId;
	int					_boardId;
	int					_avatarId;
	RKString			_password;
	RKString			_message;


	WidgetEntity *     p_panel_invite;
	WidgetEntity *     p_check_deny_invite_layout;
	WidgetEntity *     p_text_message_invite;
	WidgetEntity *     avatar_user_invite_layout;

	float			  _time_to_close;

protected:
	void OnHideInvite(CallFunc * call_back);
public:
	virtual void InitLayer(BaseScreen * screen)		override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual int Update(float dt) override;
	virtual bool IsVisibleLayer() override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;

	void OnSetDataInvite(int gameid, int roomid, int boardid, RKString pass, RKString message, int idavatar);
};

#endif //__INVITE_LAYER_H__


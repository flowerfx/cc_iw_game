#ifndef __ANNOUCE_SERVER_LAYER_H__
#define __ANNOUCE_SERVER_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class AnnouceServerLayer : public CommonLayer
{
public:
	AnnouceServerLayer();
	virtual ~AnnouceServerLayer();
private:
	/*
	handle announce server
	*/

	WidgetEntity * p_annouce_server_layout;
	bool		   p_have_annouce;
	RKString	   p_announce_text;
	bool		   p_annouce_is_running;

	float		   p_speed_move ; 
public:
	/*
	handle announce server
	*/
	virtual int Update(float dt) override;
	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual bool IsVisibleLayer() override;
	void OnReceiveAnnouncementFromServer(const RKString & text);
	void forceStopAnnouce(bool value);
};

#endif //__ANNOUCE_SERVER_LAYER_H__


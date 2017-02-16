#ifndef __SCREEN_DIALOG_H__
#define __SCREEN_DIALOG_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
//demo blank screen, seft add node/ui/touch in this screen !
//search ScrMgr->SwitchToMenu(CUSTOM_SCREEN, MAIN_SCREEN); to call 
class DialogScreen : public BaseScreen
{
private:
	

public:
	
	DialogScreen();
	virtual ~DialogScreen();

	virtual  int	Init()																override;
	virtual int		Update(float dt)													override;
	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()override;
    
};

#endif //__SCREEN_DIALOG_H__


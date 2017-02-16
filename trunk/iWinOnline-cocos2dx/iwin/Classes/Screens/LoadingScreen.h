#ifndef __SCREEN_LOADING_H__
#define __SCREEN_LOADING_H__


#include "ScreenManager.h"


class LoadingScreen : public BaseScreen
{
private:
	int p_current_state_loading;
	int p_state_load_resource;

	int p_state_handle_return_server; //0 is none , 1 is set game type lobby , 2 is set into game screen
	void * p_message_reconnect;
private:
	ThreadWrapper *		  p_thread_load_xml;
	void StartThreadLoadXML();
	bool finish_load_xml;

	int state_load;
	WidgetEntity * p_panel_loading_process;
	WidgetEntity * p_layout_img_download;
	void SetStateLoadIdx(int idx, bool load);
public:
	LoadingScreen();
	virtual ~LoadingScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
    virtual void OnFadeInComplete()                                                         override;

	int GetStateLoading();
	void SetStateLoading(int state);
	void SetStateServerReturn(int id) { p_state_handle_return_server = id; }
	void SetMessageHandle(void * msg);
    
    void LoadXMLFile();

};

#endif //__SCREEN_LOADING_H__


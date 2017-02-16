#ifndef __MENU_ENTITY_WRAPPER_H__
#define __MENU_ENTITY_WRAPPER_H__

#include "UI/AdvanceUI/Screen.h"
#include "Common/Common.h"
using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
namespace RKUtils
{
	class ThreadWrapper;
}
enum TYPE_DIALOG
{
	DIALOG_NONE = 0,
	DIALOG_ONE_BUTTON,
	DIALOG_TWO_BUTTON,
	DIALOG_ZERO_BUTTON,
	DIALOG_LOGOUT_ACC,
	DIALOG_WAITING_BUTTON,
	DIALOG_ZERO_BUTTON_WAIT,
	DIALOG_COUNT
};

enum MENU_LAYER
{
	MENU_NONE = 0,
	LOADING_SCREEN,
	DIALOG_SCREEN,
	LOGIN_SCREEN,
	MAIN_SCREEN,
	PROFILE_SCREEN,
	LOBBY_SCREEN,
	LEADERBOARD_SCREEN,
	EVENT_SCREEN,
	FRIEND_SCREEN,
	IAP_SCREEN,
	HUD_SCREEN,
	NEW_LOBBY_SCREEN,
	CHANGE_AWARD_SCREEN,
	COMMON_SCREEN,
	CUSTOM_SCREEN,
	MENU_COUNT
};
class BaseScreen;
#define TIME_ACTION 0.5f
struct EventQueue
{
    std::function<void(BaseScreen* scr)> event_action;
    float time_action;
	bool isDone;
public:
    EventQueue()
    {
        event_action = nullptr;
        time_action = 0.f;
		isDone = false;
    }
    EventQueue(const std::function<void(BaseScreen* scr)> & action, float time)
    {
        event_action = action;
        time_action = time;
		isDone = false;
    }
    virtual ~EventQueue()
    {
        event_action = nullptr;
        time_action = 0.f;
    }
    
};

class BaseScreen : public Screen
{
protected:

	MENU_LAYER p_menu_come_from;
    
    MENU_LAYER p_menu_show_next;

	bool	p_have_info_change;

	ThreadWrapper *		  p_thread_init_component;
	bool   p_init_component_complete;

	//event queue in game
	std::vector<EventQueue>	p_list_event;

	//event parallel in game
	std::vector<EventQueue> p_list_temp_event_parallel;
	std::vector<EventQueue> p_list_event_parallel;
public:
	BaseScreen();
	virtual ~BaseScreen();

	virtual  int	Init();
	virtual  int	InitComponent();
	virtual void	CallInitComponent(bool use_thread);
	virtual  int	Update(float dt) override;
	virtual void	VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags) override;
	virtual void	DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags) override;
	virtual void OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)					override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) = 0;
	virtual void OnDeactiveCurrentMenu();
	virtual void OnBeginFadeIn();
	virtual void OnFadeInComplete();

	virtual void ParseUserUI();
	virtual void onMyInfoChanged();
	virtual void ParseImgToNode(cocos2d::Node * node, void * data, void * url, int id, cocos2d::Texture2D::PixelFormat fmat = cocos2d::Texture2D::PixelFormat::RGB5A1, bool save_cache = true);
	virtual void ParseImgToPanel(WidgetEntity * panel, void * data, void * url , int id , cocos2d::Texture2D::PixelFormat fmat = cocos2d::Texture2D::PixelFormat::RGB5A1, bool save_cache = true);
	virtual void ParseSpriteToNode(cocos2d::Node * node , Sprite * sp, int z_order, Vec2 anchor_sp = Vec2(0.5, 0.5));
	virtual void ParseSpriteToPanel(WidgetEntity * panel, Sprite * sp, int z_order, bool keep_origin_size = false, Vec2 anchor_sp = Vec2(0.5,0.5));
	virtual void OnReceiveFriendList();

	void SetMenuComFrom(MENU_LAYER menu){ p_menu_come_from = menu; }
	MENU_LAYER GetMenuComeFrom() { return p_menu_come_from; }
    
    void SetMenuShowNext(MENU_LAYER menu) {p_menu_show_next = menu; }
    MENU_LAYER GetMenuShowNext() { return p_menu_show_next; }

	void OnCallBackProcess(const RKString & str);
	virtual void OnProcess(const RKString & str);

	virtual void CheckTouchOutSideWidget(const cocos2d::Point & p, WidgetEntity * panel, const std::function<void(void)> & func);
	virtual void SetInteractiveInsteadOf(bool value, WidgetEntity * exp);

	virtual void ClearComponent();
	virtual void OnKeyBack();

	void SetInteractiveWidget(WidgetEntity * widget, bool value);
	void RestoreAllWidgetHaveNone();

	//for the event
	void PushEvent(const std::function<void(BaseScreen* scr)> & event_action, float time=0.f , bool isParallel = false);

	//which action we want to do when screen show
	virtual void SetActionAfterShow(int action);

	//playmusic screen
	virtual void PlayMusic();
protected:
	virtual void OnHandleInfoChange();

	void HandleEvent(float dt);
};

#endif //__MENU_ENTITY_WRAPPER_H__


#ifndef __SCREEN_EVENT_H__
#define __SCREEN_EVENT_H__


#include "ScreenManager.h"
#include "Network/JsonObject/Facebook/FacebookFriend.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
namespace iwinmesage
{
	class QuestCategoryList;
	class QuestCategory;
	class Quest;
	class ConfirmBox;
	class ResponseQuestList;
}
class EventScreen : public BaseScreen
{
private:
	iwinmesage::QuestCategoryList * p_list_quest_category;
	iwinmesage::ResponseQuestList * p_current_quest_list;

	WidgetEntity * _list_quest_catalogue;
	WidgetEntity * _list_quest_sample;
	WidgetEntity * _list_quest_empty;
    WidgetEntity * _fb_invitation_dialog;
    WidgetEntity * p_top_title_invite;

	WidgetEntity * _title_name;
	int			   p_current_idx_quest;

	WidgetEntity * _dialog_captcha;

	iwinmesage::QuestCategory * _current_quest_category;
public:
	EventScreen();
	virtual ~EventScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnBeginFadeIn()															override;
    virtual void OnTouchMenuBegin(const cocos2d::Point & p)											override;
	virtual void OnKeyBack()																override;

	void OnReceiveListQuestCataloryJson(std::string str );
	void OnReceiveQuestByCalaloryJson(std::string str_json);
	void OnRequestDataJson();

	void doAction(Quest * q);
	void AcceptQuest(Quest* q);
	void SubmitQuest(Quest* q);
	void Refresh();
protected:
    void OnVisibleFBInviteDialog(bool value, const std::function<void(void)> & call_back);
    
	void ShowQuestAtId(iwinmesage::QuestCategory * qs);
	void ClearListAndDeleteTexture(WidgetEntity * list_clear,RKString path_panel, RKString name_delete);
	int GetTypeID(int action_type);
	int GetNumberLine(RKString str);
	void OnHideAllHLCatagoryList();

	void OnResizeAndInitQuestWidget(WidgetEntity * e, iwinmesage::Quest* q, int idx);
	int GetLineWithLongString(RKString str, WidgetEntity * _widget);

	void OnParseListQuest();

	void SetTitleName(RKString name);

	void OnShowDialogCaptcha();
	void OnHideDialogCaptcha();
    
    void OnParseFriendToInviteList(WidgetEntity * widget, int idx, bool isPlayer = true);
    void OnReceiveFBFriendList(std::vector<FacebookFriend*> fbFriendList);
    void CheckAllFBFriendList(bool isCheck);
    bool ModAllCheck(bool isCheck);
    void OnFilterFBFriendInviteDialog(RKString name);
    
private:
	float _time_waiting_reload;
	bool  _need_to_reload;
	void DoActionQuest(Quest q);
    std::vector<FacebookFriend*> p_list_fbFriend;
    std::vector<FacebookFriend*> p_list_fbFriend_filtered;
    bool isFiltered = false;
    cocos2d::Map<std::string, Sprite*>	p_list_avatar_download;
};

#endif //__SCREEN_EVENT_H__


#ifndef __SCREEN_FRIEND_H__
#define __SCREEN_FRIEND_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;

class iWinPlayer;
class ChatMessage;
class Player;
class FriendScreen : public BaseScreen
{
private:
	WidgetEntity * p_search_player_layout;
	WidgetEntity * p_list_friend_main;

	WidgetEntity * p_search_myfriend_layout;
	WidgetEntity * p_chat_friend_layout;
	WidgetEntity * p_main_friend_layout;

	WidgetEntity * p_chat_friend_input;
	WidgetEntity * p_count_friend_layout;

	WidgetEntity * p_top_panel;

	WidgetEntity * p_layout_list_friend_chat;
	WidgetEntity * p_list_friend_chat;

	WidgetEntity * p_list_chat;
	WidgetEntity * p_layout_chat;
	Player		 * p_current_friend_chat_to;
	//from newfriendscreen
	std::vector<ChatMessage*>	p_ADDFRIEND_MESSAGE ;
	std::vector<ChatMessage*>	p_ADMIN_MESSAGE ;
	std::vector<ChatMessage*>	p_UNREAD_MESSAGE ;

	std::vector<iWinPlayer*>	p_listRecentChat;
	std::vector<iWinPlayer*>	p_listFriendChat;
	//
	std::vector<Player*>		p_listPlayerFriend;
	//
	cocos2d::Map<int, Sprite*>	p_list_avatar_download;
	//
	int p_stage_get_list_friend;
	int p_current_idx_search_parse;
	int p_current_idx_search_friend_chat;
	//
	RKString p_current_friend_search;
	RKString p_current_friend_chat_search;

	// vari drag menu
	int				_state_press_layout_chat;
	Vec2			_previous_pos;
	LayerEntity *	_root_layer;
	Vec2			_delta_move;
	bool			_on_move_layer;

	bool			_have_list_friend_chat_appear;

	//vari drag cell
	Vec2			_previous_pos_cell;
	Vec2			_delta_move_cell;
	WidgetEntity *  _widget_cell_on_focus;
protected:
	void	OnParseListFriendMain(WidgetEntity* _widget, int idx , RKString name_fill);
	int		GetNumberFriend();

	void	ParseListFriend(WidgetEntity* _widget, Player * pl);
	void	ParseListFriendChat(WidgetEntity* _widget, iWinPlayer * pl);
	void	ParseListWaitingFriend(WidgetEntity* _widget, ChatMessage* player);
	size_t	GetNumberFriendContainName(RKString name);
	size_t	GetNumberFriendChat(RKString name);

	void	OnParseFriendList();
	void	InitTextFieldValue();

	void	OnParseListFriendChat(WidgetEntity* _widget, int idx, RKString name_fill);

	void	OnRootLayerMove(bool move_back = false);
	void	OnCellLayerMove(WidgetEntity * widget , bool move_back = false);

	std::vector<ChatMessage*>	GetUnreadMessageFromFriend(int iddb , bool remove_self = false);

	void ShowChatToFriend(Player * p);
public:
	FriendScreen();
	virtual ~FriendScreen();

	virtual  int	Init()																				override;
	virtual  int	InitComponent()																		override;
	virtual  int	Update(float dt)																	override;

	virtual void	OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)	override;
	virtual void	OnDeactiveCurrentMenu()																override;

	virtual void	OnReceiveFriendList()																override;

	virtual void	OnTouchMenuBegin(const cocos2d::Point & p)													override;
	virtual void	OnTouchMenuEnd(const cocos2d::Point & p)													override;
	virtual void	OnTouchMenuHold(const cocos2d::Point & p)													override;
	virtual void SetActionAfterShow(int action)															override;
	virtual void	ClearComponent()																	override;
public:
	std::vector<ChatMessage*> GET_MESSAGE(int state);
	void PUSH_MESSAGE(int state, std::vector<ChatMessage*> vec);
	void PUSH_SINGLE_MESSAGE(int sate, ChatMessage* msg);
	void REMOVE_MESSAGE(int state, int id);


	int GetStateFriendMenu();
	void onReceiveServerWarning(RKString msg);
	void handleChatEmotion(int idx);
	void onReceiveMessage(ChatMessage * msg);
	void OnParseIntoChat(RKString msg, int from);

	void ForceUpdateListFriendChat();
};

#endif //__SCREEN_FRIEND_H__


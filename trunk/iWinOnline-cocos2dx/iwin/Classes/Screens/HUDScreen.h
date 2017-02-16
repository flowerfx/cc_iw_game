#ifndef __HUD_SCREEN_H__
#define __HUD_SCREEN_H__


#include "ScreenManager.h"

using namespace RKUtils;
using namespace Utility;
using namespace Utility::UI_Widget;
class Player;
class PlayerGroup;
class ChatBoard;
class Category;
class Item;
class HUDScreen : public BaseScreen
{
protected:
	WidgetEntity*   p_hud_layout;
	WidgetEntity*	p_text_money;
	WidgetEntity*	p_text_name;
	WidgetEntity*	p_btn_purchase_iwin;
	WidgetEntity*	p_btn_purchase_ruby;
	WidgetEntity*	btn_quick_play;
	WidgetEntity*	p_text_wait;
	WidgetEntity*	p_btn_ingame_menu;
	WidgetEntity*	p_layout_gift_send_to;

	WidgetEntity * p_list_player_layout;
	WidgetEntity * p_panel_igm;
	WidgetEntity * p_panel_ig_setting;
	WidgetEntity * p_list_lang_choose;

	WidgetEntity * p_panel_bottom;

	WidgetEntity * p_panel_chat_root;
	WidgetEntity * p_btn_recent_chat;
	WidgetEntity * p_btn_emo_chat;
	WidgetEntity * p_list_recent_chat;
	WidgetEntity * p_list_emoticon_chat;
	WidgetEntity * p_text_input_chat;

	WidgetEntity * p_layout_invite_player;
	WidgetEntity * p_list_player_invite;
	WidgetEntity * p_top_title_invite;

	WidgetEntity * p_panel_custom;

	std::vector<PlayerGroup *>		p_list_player_group;
	std::vector<Player*>			p_list_player;
	//
	WidgetEntity *					p_panel_count_down;

	//WidgetEntity *					p_panet_time_bar;
	float							p_timer_count_down;
	bool							p_on_count_down;
	int								p_max_player;
	ChatBoard*						_chat_board;
	std::map<int, xml::BasicDec*>	p_list_pos_dec;
	cocos2d::Map<long, Sprite*>		p_list_avatar_download;

	//panel game screen
	WidgetEntity*			p_panel_game_screen;
	WidgetEntity*			p_text_min_bet;
	WidgetEntity*			p_text_gamename;
	WidgetEntity*			p_text_other1;

	//gift
	WidgetEntity * p_panel_send_gift;
	WidgetEntity * p_layout_title_gift;
	WidgetEntity * p_layout_list_gift;
	int								p_current_gift_layout_idx;
	int								p_current_id_items_select;
	int								p_current_iddb_player_gift;
	std::vector<Category*>			p_gift_categories;
	std::vector<Item*>				p_list_items;
	WidgetEntity * p_bg_table_win;
	WidgetEntity * p_bg_table_rubby;

	WidgetEntity * p_layout_effect_rank;
	//WidgetEntity * p_layout_player_group_com;
	//WidgetEntity * p_player_group_com_refer;

protected:
	xml::UILayerWidgetDec* xml_icon_dec;
	xml::UILayerWidgetDec* xml_chat_text_dec;
	ThreadWrapper *		  p_thread_parse_ui;
	void start_thread_parse_ui();

	struct PlayerWaiting
	{
		Player* player;
		bool is_check;

		PlayerWaiting(Player* p) {
			player = p;
			is_check = true;
		}
		virtual ~PlayerWaiting()
		{
			if (player)
			{
				delete player;
			}
			is_check = false;
		}
	};
	std::vector<PlayerWaiting*> p_list_player_waiting;
	void OnShowListChooseLang(bool visible);
public:
	HUDScreen();
	virtual ~HUDScreen();

	virtual  int	Init()																	override;
	virtual  int	InitComponent()															override;
	virtual  int	Update(float dt)														override;

	virtual void OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)override;
	virtual void OnDeactiveCurrentMenu()													override;
	virtual void OnBeginFadeIn()															override;
	virtual void OnFadeInComplete()															override;
	virtual void OnTouchMenuBegin(const cocos2d::Point & p)											override;
	virtual void OnShow()																	override;
	virtual void OnKeyBack()																override;
	virtual void PlayMusic()																override;

	virtual void DoBack();
	virtual void PushTextChat(RKString name, RKString msg);
	virtual void HandleButtonAddUser();
	PlayerGroup * GetPlayerGroupAtIdx(int idx);
	PlayerGroup* getPlayerGroup(int playerID);

	virtual void SetMaxPlayer(int max_player);
	
	void showGameScreen(s16 roomID, s16 boardID, int ownerID, int money, std::vector<Player*> players);
	void OnCreateBtnRecentChat(RKString text);
	void OnRefreshTextChatBtn();
	void OnSetTextBtn(WidgetEntity * w_text, RKString text);
	void OnCreateBtnEmoticonChat(int idx , xml::UILayerWidgetDec* xml_dec);

	void OnHideCountDown();
	virtual void OnShowCountDown(float time, float cur_time);
	void onSetData(void* data);

	void onReceiveAvatarCatList(void * data);
	void onReceiveAvatarList(void * data);
	void onBuyingAvatarResult(void * data);

	void OnParsePlayerToInviteList(WidgetEntity * widget, int idx, bool isPlayer = true);
	void OnReceiveWaitingList(std::vector<Player*> list);
	void CheckAllWaitingList(bool ischeck);
	bool ModAllCheck(bool is_check);
	void HandleChatText(RKString text, bool send_to_screen = true);

	void setGiftCategories(std::vector<Category*> categories);
	void setGiftItems(int categoryID, int totalpage, std::vector<Item*> items);
	void sendGiftTo(int from, std::vector<int> tos, int itemID);
	void openGiftWithIDPlayer(int IDDB);

	int GetStateHUDScreen();

	void OnShowMessageFromServer(RKString msg);

	void SendChatToScreen(RKString text_chat);

	void SetVisiblePlayerCom(bool visible, int idx);
	void ShowRankNumberAtPos(bool value,int idx_rank, Vec2 pos);
	void ShowTextChatAtIdx(bool value, int idx, RKString text = "" , int state = 0 , const std::function<void(void)> &  call_back = nullptr);
	bool IsTextChatVisible(int idx);
	void ShowTextBonusMoney(int idx , s64 money , Vec2 Pos);
	void ShowMessagePopup_New(std::string message);
protected:

	void LoadXMLEmotion(TiXmlDocument * p_objectXML);
	void ShowGameScreen(GameType game_type);

	void ShowListGiftAtIdx(int idx);
	void OnSelectItemGift(int id);

	void _showMessagePopUp_new(std::string text, float timeDelay, bool vibravate);

protected:
	xml::UILayerWidgetDec* p_sample_dec;

	int		_idx_load_btn;
	int		_idx_load_emo;
	void	LoadFunctionLoop(const std::function<bool(BaseScreen * scr)> & func, float delta_time);

	std::vector<RKString>	_recent_chat;
	void SaveTheRecentChat(RKString str_chat);
	void LoadTheRecentChat();
	virtual void PushChatToServer(RKString str) {};
	void InitTextFieldValue();
};

#endif //__HUD_SCREEN_H__


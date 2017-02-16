#include "EventScreen.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wTimeProcessWidget.h"
#include "UI/AdvanceUI/wTimeDigitalWidget.h"
#include "UI/AdvanceUI/wLoadingBar.h"

#include "Network/JsonObject/Quest/ConfirmBox.h"
#include "Network/JsonObject/Quest/Quest.h"
#include "Network/JsonObject/Quest/QuestCategory.h"
#include "Network/JsonObject/Quest/QuestCategoryList.h"
#include "Network/JsonObject/Quest/ResponseQuestList.h"
#include "Network/JsonObject/Quest/RequestQuest.h"

#include "Network/JsonObject/Facebook/FacebookFriendList.h"

#include "Network/Global/Globalservice.h"
#include "FileManager.h"
#include "MainGame.h"

#include "Common/IwinListViewTable.h"
#include "Common/IwinTextfield.h"

#include "Features/FacebookMgr.h"
#include "Platform/Platform.h"

#include "Network/JsonObject/Quest/ScreenID.h"
#include "Screens/MainScreen.h"
#include "Screens/CommonScreens/LuckyCircleScreen.h"

EventScreen::EventScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
	p_list_quest_category = nullptr;
	p_current_quest_list = nullptr;
	p_current_idx_quest = 0;

	_time_waiting_reload = 0.25f;
	_need_to_reload = false;

	_current_quest_category = nullptr;
}


EventScreen::~EventScreen()
{
	SAFE_DELETE(p_list_quest_category);
	SAFE_DELETE(p_current_quest_list);
    SAFE_DELETE_VECTOR(p_list_fbFriend);
    
	_current_quest_category = nullptr;
}

int EventScreen::Init()
{
	RKString _menu = "event_screen";
	float cur_ratio = GetGameSize().width / GetGameSize().height;
	if (GetGameSize().width <= 480) //ipad ratio
	{
		//_menu = "login_screen_small_size";
	}
	//override the list view table default with the iwinlistview table
	this->_func_action = [&](LayerEntity * layer) {
		layer->OverloadRegisterUIWidget<IwinListViewTable>(xml::WIDGET_LIST_VIEW_TABLE);
		layer->OverloadRegisterUIWidget<IwinTextFieldHandle>(xml::WIDGET_TEXT_FIELD);
		layer->OverloadRegisterUIWidget<IwinEditBoxdHandle>(xml::WIDGET_EDIT_BOX);
	};
	
	InitMenuWidgetEntity(_menu);
	CallInitComponent(true);
	return 1;
}

int	EventScreen::InitComponent()
{
	p_list_quest_category = new iwinmesage::QuestCategoryList();

	auto layout_quest_catalogue = GetWidgetChildByName("event_ui.quest_catalogue");
	_list_quest_catalogue = layout_quest_catalogue->GetWidgetChildByName(".list_quest_catalogue");

	auto quest_ = GetWidgetChildByName("event_ui.quest_");
	_list_quest_sample = quest_->GetWidgetChildByName(".list_quest_sample");
	_list_quest_empty = quest_->GetWidgetChildByName(".list_quest_");


	_title_name = GetWidgetChildByName("event_ui.top_panel.title_1");

	_dialog_captcha = GetWidgetChildByName("event_ui.capcha_layout.capcha_dialog");
    
    _fb_invitation_dialog = GetWidgetChildByName("event_ui.fb_invite_dialog.dialog_");
    p_top_title_invite = _fb_invitation_dialog->GetWidgetChildByName(".top_title");
    auto pListTableViewFriend = _fb_invitation_dialog->GetWidgetChildByName(".list_friend_invite");
    
    if (pListTableViewFriend)
    {
        auto list_player_lb = static_cast<IwinListViewTable*>(pListTableViewFriend);
        list_player_lb->InitWithParam(_fb_invitation_dialog->GetWidgetChildByName(".layout_pull_update"),
                                      nullptr,
                                      //action on set
                                      [this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
                                      {
                                          int current_idx_to = list_player_lb->GetCurrentIdxTo();
                                          OnParseFriendToInviteList(_widget, current_idx_to);
                                      },
                                      //action on touch
                                      [this, list_player_lb](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
                                      {
                                          size_t current_idx = list_player_lb->GetCurrentIdxSelected();
                                          if (current_idx < p_list_fbFriend.size())
                                          {
                                              FacebookFriend p = p_list_fbFriend[current_idx];
                                              p_list_fbFriend[current_idx]->setSelected(!p.getSelected());
                                              
                                              _widget->GetWidgetChildByName(".on_check")->Visible(p_list_fbFriend[current_idx]->getSelected());
                                              _widget->GetWidgetChildByName(".off_check")->Visible(!p_list_fbFriend[current_idx]->getSelected());
                                              
                                              ModAllCheck(p.getSelected());
                                          }
                                          else
                                          {
                                              PASSERT2(false, "have problem here!");
                                          }
                                      },
                                      //action callback top
                                      nullptr,
                                      //action call back bottom
                                      nullptr);
    }
    
    WidgetEntity* _widget = _fb_invitation_dialog->GetWidgetChildByName(".panel_search_input.text_input_search");
    auto w_tx = static_cast<IWIN_TF*>(_widget);
    w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".btn_clear_text"));
    w_tx->InitCallBack(
                       //action on attach
                       nullptr,
                       //action on detach
                       [this, _widget]()
                       {
                           RKString name = static_cast<TextFieldWidget*>(_widget)->GetText();
                           this->isFiltered = name != "";
                           //filter name and update display
                           this->OnFilterFBFriendInviteDialog(name);
                       },
                       //action insert
                       nullptr,
                       //action on delete
                       nullptr,
                       //action on return
                       nullptr
                       );
    
	return 1;
}

int EventScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (_need_to_reload)
	{
		_time_waiting_reload -= dt;
		if (_time_waiting_reload <= 0)
		{
			_time_waiting_reload = 1.f;
			_need_to_reload = false;

			_list_quest_sample->Visible(true);
			_list_quest_sample->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(false);
			OnParseListQuest();

		}
	}
    
    auto pListTableViewFriend = _fb_invitation_dialog->GetWidgetChildByName(".list_friend_invite");
    static_cast<IwinListViewTable*>(pListTableViewFriend)->UpdateReload(dt);
    
	return 1;
}


void EventScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_close")
		{
			ScrMgr->SwitchToMenu(p_menu_come_from, EVENT_SCREEN);
		}
		else if (name == "btn_show_quest")
		{
			if (!_widget->GetParentWidget()->GetWidgetChildAtIdx(0)->Visible())
			{
				OnHideAllHLCatagoryList();
				_widget->GetParentWidget()->GetWidgetChildAtIdx(0)->Visible(true);
				RKString name_list = _widget->GetParentWidget()->GetName();
				RKString idx = name_list.SplitLast("_").GetAt(1);
				auto list_category = p_list_quest_category->getCategoryList();
				auto detail_list = list_category[atoi(idx.GetString())];
				GlobalService::GetQuestByCategoryID(detail_list->getId());
				ShowQuestAtId(detail_list);
			}

		}

		else if (name == "btn_cancel_captcha")
		{
			OnHideDialogCaptcha();
		}
		else if (name == "btn_refesh_captcha")
		{

		}
        else if (name == "btn_check_invite_all_friend")
        {
            auto parent = _widget->GetParentWidget();
            if (parent->GetWidgetChildByName(".on_check")->Visible() == true)
            {
                parent->GetWidgetChildByName(".on_check")->Visible(false);
                parent->GetWidgetChildByName(".off_check")->Visible(true);
                CheckAllFBFriendList(false);
            }
            else
            {
                parent->GetWidgetChildByName(".on_check")->Visible(true);
                parent->GetWidgetChildByName(".off_check")->Visible(false);
                CheckAllFBFriendList(true);
            }
        }
        else if (name == "invite_btn")
        {
            _fb_invitation_dialog->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
                this->_fb_invitation_dialog->ForceFinishAction();
                _fb_invitation_dialog->GetParentWidget()->Visible(false);
                
                std::vector<FacebookFriend*> listFriendInvite = isFiltered ? p_list_fbFriend_filtered : p_list_fbFriend;
                std::vector<std::string> listIDInvite;
                for (auto p : listFriendInvite)
                {
                    if (p->getSelected())
                    {
                        listIDInvite.push_back(p->getId());
                    }
                }
                
                int id = p_list_quest_category->getCurrentCategory();
                auto list_quest = p_current_quest_list->getQuests();
                Quest* existQuest = nullptr;
                for(auto quest : list_quest)
                {
                    if(quest && quest->getActionType() == QuestType::TYPE_INVITE_FACEBOOK_FRIENDS)
                    {
                        existQuest = quest;
                        break;
                    }
                }
                
                this->PushEvent([this, existQuest, listIDInvite](BaseScreen* scr){
                    Platform::FBUtils::InviteFriend(listIDInvite, [this, listIDInvite, existQuest](bool res, void* data) {
                        if (res) {
                            if(existQuest != nullptr) {
                                this->AcceptQuest(existQuest);
                                //here send invited list to server
                                GlobalService::sendInviteFacebookList(listIDInvite);
                            }
                        }
                        else {
                            ScrMgr->OnShowErrorDialog("cannot_invite_fb");
                        }
                    });
                });
                
            }));
        }
	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		auto w_tx = static_cast<IWIN_TF*>(_widget);
		w_tx->HandleEventTextField(name);
	}
}

void EventScreen::OnBeginFadeIn()
{
	OnRequestDataJson();

	auto back_panel = GetWidgetChildByName("main_background.background_texture");
	GameMgr->HandleURLImgDownload(
		LINK_BACKGROUND_WINFREE, "", [this, back_panel](void * data, void * str, int tag)
	{
		this->ParseImgToPanel(back_panel, data, str, 100, Texture2D::PixelFormat::RGB888);
	},false);
}

void EventScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(EVENT_SCREEN);
	p_menu_show_next = MENU_NONE;
	_current_quest_category = nullptr;
}

void EventScreen::SetTitleName(RKString name)
{
	static_cast<TextWidget*>(_title_name)->SetText(name, true);
}



void EventScreen::OnReceiveListQuestCataloryJson(std::string str_json)
{
	int current_def_idx = 1;
	if (_current_quest_category)
	{
		current_def_idx = _current_quest_category->getId();
	}
	else if (p_current_idx_quest > 0)
	{
		current_def_idx = p_current_idx_quest;
	}

	if (!p_list_quest_category)
	{
		PASSERT2(p_list_quest_category != nullptr, "p_list_quest_category is null");
		return;
	}
	delete p_list_quest_category;
	p_list_quest_category = new iwinmesage::QuestCategoryList();
	_current_quest_category = nullptr;
	p_list_quest_category->toData(str_json);

	auto list_category = p_list_quest_category->getCategoryList();
	int current_number_child = _list_quest_catalogue->GetNumberChildWidget();


	int total_number_quest = 0;
	for (size_t i = 0; i < list_category.size(); i++)
	{
		WidgetEntity * _widget = nullptr;
		if (i < (size_t)current_number_child)
		{
			_widget = _list_quest_catalogue->GetWidgetChildAtIdx(i);
		}
		else
		{
			 _widget = static_cast<ListViewWidget*>(_list_quest_catalogue)->CreateWidgetRefer(i);
			 _list_quest_catalogue->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_BUTTON, _widget->GetWidgetChildByName(".btn_show_quest"));
		}
		auto ls = list_category[i];
		_widget->Visible(true);
		RKString previous_name = static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->GetText();
		if (previous_name != ls->getName())
		{
			static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title_id"))->SetText(ls->getName());
		}
		static_cast<TextWidget*>(_widget->GetWidgetChildByName(".layout_.number_quest"))->SetText(std::to_string(ls->getItemNo()));
		total_number_quest += ls->getItemNo();
		//if (previous_name != list_category[i].getName())
		{
			auto panel = _widget->GetWidgetChildByName(".layout_.panel_icon");
			panel->retain();
			GameMgr->HandleURLImgDownload(
				ls->getImgUrl(), ls->getName(), [this, panel, i](void * data, void * str, int tag)
			{
				this->ParseImgToPanel(panel, data, str, i);
			});
		}
		int current_id_cata = p_list_quest_category->getCurrentCategory();
		if (ls->getId() == ((current_id_cata > 0 )? current_id_cata : current_def_idx))
		{
			OnHideAllHLCatagoryList();
			GlobalService::GetQuestByCategoryID(ls->getId());
			_widget->GetWidgetChildAtIdx(0)->Visible(true);
			ShowQuestAtId(ls);
			this->PushEvent([i](BaseScreen* scr ) {
				EventScreen * event_scr = (EventScreen*)scr;
				static_cast<ListView*>(event_scr->_list_quest_catalogue->GetResource())->jumpToItem(i, Vec2(0.5, 0.5), Vec2(0.5, 0.5));
			});
		}
	}

	//hide the list if not use
	if (list_category.size() < (size_t)current_number_child)
	{
		for (size_t i = list_category.size(); i < (size_t)current_number_child; i++)
		{
			auto _widget = _list_quest_catalogue->GetWidgetChildAtIdx(i);
			_widget->Visible(false);
		}
	}

	//set number win free
	static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->SetNumWinfree(total_number_quest);

}

void EventScreen::OnRequestDataJson()
{
	GlobalService::GetQuestCategoryList();
}

void EventScreen::OnParseListQuest()
{
	p_current_idx_quest = p_current_quest_list->getCategoryId();
	int current_idx_q = p_current_idx_quest;
	auto _widget_list = _list_quest_empty;
	ClearListAndDeleteTexture(_widget_list, ".icon", "img_url");
	//int current_number_child = _widget_list->GetNumberChildWidget();
	auto list_quest = p_current_quest_list->getQuests();

	if (list_quest.size() > 0)
	{
		_list_quest_sample->GetParentWidget()->GetWidgetChildByName(".txt_none")->Visible(false);
		for (size_t i = 0; i < list_quest.size(); i++)
		{
			Quest * q = list_quest[i];
			//if (q.getActionType() <= iwinmesage::QuestType::TYPE_OPEN_URL)
			{
				auto _widget = static_cast<ListViewWidget*>(_list_quest_sample)->CreateWidgetReferNoPushBack(0);
				OnResizeAndInitQuestWidget(_widget, q , i);
				static_cast<ListViewWidget*>(_widget_list)->PushBackItem(_widget);
				//
				auto panel = _widget->GetWidgetChildByName(".icon");
				auto resource = panel->GetResource();
				GameMgr->HandleURLImgDownload(
					q->getUrlImage(), q->getId() + "_" + std::to_string(i), [this, i, current_idx_q , resource](void * data, void * str, int tag)
				{
					Image * img = (Image *)data;
					int ref_count = resource->getReferenceCount();
					if (ScrMgr->GetCurrentMenuUI() == EVENT_SCREEN && current_idx_q == this->p_current_idx_quest && ref_count > 0 && ref_count < 10)
					{
						this->ParseImgToNode(resource, data, str, i);
					}
					else
					{
						GameMgr->CreateAvatarFromImg(data, str, Texture2D::PixelFormat::RGB5A1, true);
					}
				});
			}
		}
	}
	else
	{
		_list_quest_sample->GetParentWidget()->GetWidgetChildByName(".txt_none")->Visible(true);
	}
}

void EventScreen::OnReceiveQuestByCalaloryJson(std::string str_json)
{
	if (p_current_quest_list)
	{
		delete p_current_quest_list;
	}
	p_current_quest_list = new iwinmesage::ResponseQuestList();

	p_current_quest_list->toData(str_json);

	_list_quest_sample->Visible(false);
	_list_quest_sample->GetParentWidget()->GetWidgetChildByName(".txt_none")->Visible(false);
	_list_quest_sample->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(true);
	_list_quest_sample->GetParentWidget()->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");

	_need_to_reload = true;
	
}

void EventScreen::ShowQuestAtId(iwinmesage::QuestCategory * qs)
{
	_current_quest_category = qs;
	SetTitleName(qs->getName());

	return;
}

int EventScreen::GetTypeID(int action_type)
{
	iwinmesage::QuestType type = (iwinmesage::QuestType)action_type;
	switch (type)
	{
	case iwinmesage::TYPE_ACCEPT_QUEST:
	case iwinmesage::TYPE_OPEN_URL:
	{
		return 0;
		break;
	}
	case iwinmesage::TYPE_INSTALL_APP:
		break;
	case iwinmesage::TYPE_SUBMIT_QUEST:
		break;
	case iwinmesage::TYPE_OPEN_SCREEN:
		break;
	case iwinmesage::TYPE_RATE_APP:
		break;
	case iwinmesage::TYPE_LIKE_FAN_PAGE:
		break;
	case iwinmesage::TYPE_POST_WALL:
		break;
	case iwinmesage::TYPE_WIN_FREE:
		break;
	case iwinmesage::TYPE_ENABLE_CONTACT:
		break;
	case iwinmesage::TYPE_ENABLE_NOTIFICATION:
		break;
	case iwinmesage::TYPE_OPEN_FACEBOOK_REQUEST:
		break;
	case iwinmesage::TYPE_SHARE_LINK_FACEBOOK:
		break;
	case iwinmesage::TYPE_OPEN_ADMIN_ROLE:
		break;
	case iwinmesage::TYPE_DICE:
		break;
	case iwinmesage::TYPE_LIKE_PAGE_GOOGLE:
		break;
	case iwinmesage::TYPE_MOON_FESTIVAL:
		break;
	case iwinmesage::TYPE_INDEPENDENCE:
		break;
	case iwinmesage::TYPE_HALLOWEEN:
		break;
	case iwinmesage::TYPE_SEND_USER_INFO:
		break;
	case iwinmesage::TYPE_INVITE_FACEBOOK_FRIENDS:
		break;
	case iwinmesage::TYPE_NEW_LUCKY_CIRCLE:
		break;
	case iwinmesage::TYPE_OPEN_NATIVE_URL:
		break;
	default:
		break;
	}
	return 1;
}


void EventScreen::ClearListAndDeleteTexture(WidgetEntity * list_clear, RKString path_panel, RKString name_delete)
{
	for (size_t i = 0; i < list_clear->GetNumberChildWidget(); i++)
	{
		auto _panel = list_clear->GetWidgetChildByName(path_panel);
		if (_panel)
		{
			auto child = _panel->GetResource()->getChildByName(name_delete.GetString());
			if (child)
			{
				_panel->GetResource()->removeChildByName(name_delete.GetString());
			}
		}
	}
	list_clear->ClearChild();
}

int EventScreen::GetNumberLine(RKString str)
{
	if (str == "")
		return 0;
	size_t number_end_line = Utils::countStringLine(std::string(str.GetString()));
	if (number_end_line < 1)
		return 1;
	else
		return number_end_line;
}

void EventScreen::OnHideAllHLCatagoryList()
{
	for (size_t i = 0; i < _list_quest_catalogue->GetNumberChildWidget(); i++)
	{
		auto _widget = _list_quest_catalogue->GetWidgetChildAtIdx(i);
		_widget->GetWidgetChildAtIdx(0)->Visible(false);
	}
}

void EventScreen::doAction(Quest * q)
{
	if (q->getActionType() == QuestType::TYPE_NEW_LUCKY_CIRCLE)
	{
		GlobalService::GetLuckyCircleInfo(q->getBonusEventId(), 1);
		//GetCommonScr->OnShowLayer(COMMONLAYER::LUCKY_CIRCLE_LAYER);
	}
	else if (q->getActionType() == QuestType::TYPE_ACCEPT_QUEST ||
		q->getActionType() == TYPE_ENABLE_CONTACT ||
		q->getActionType() == TYPE_ENABLE_NOTIFICATION)
	{
		AcceptQuest(q);
	}
	else if (q->getActionType() == TYPE_INSTALL_APP)
	{
		Platform::downloadAppAndSetup(q->getLink(), nullptr);
		AcceptQuest(q);
	}
	else if (q->getActionType() == TYPE_LIKE_FAN_PAGE)
	{
		Platform::FBUtils::likePage(q->getLink(), [q , this](bool res, void* data) {
			if (res)
			{
				AcceptQuest(q);
			}
		});
	}
	else if (q->getActionType() == TYPE_OPEN_SCREEN)
	{
		int _screen_ID = q->getScreenID();
		ScrMgr->OpenScreenByScreenID((iwinmesage::SCREEN_ID)_screen_ID, q->getGameId());
		if (_screen_ID == (int)iwinmesage::SCREEN_ID::SCREEN_CHARGE_MONEY)
		{
			if (q->getchargeType() != "")
			{
				PASSERT2(false, "not handle this");
			}
		}
		AcceptQuest(q);
	}
	else if (q->getActionType() == TYPE_OPEN_URL || q->getActionType() == TYPE_OPEN_NATIVE_URL)
	{
		OPEN_URL(q->getLink());
		if (q->getActionType() == TYPE_OPEN_URL)
		{
			AcceptQuest(q);
		}
	}
	else if (q->getActionType() == TYPE_POST_WALL)
	{
        ScrMgr->OnShowDialog("notice", StringUtils::format(LANG_STR("do_you_want_to_share").GetString(), q->getLink().c_str()),
			DIALOG_TWO_BUTTON, "",
                             [this, q](const char * str, const char * name) {
                                 this->PushEvent([this, q](BaseScreen* scr){
                                     Platform::FBUtils::postWall(q->getJsonData(), [this, q](bool res, void* data) {
                                         if (res) {
                                             this->AcceptQuest(q);
                                         }
                                         else {
                                             ScrMgr->OnShowErrorDialog("cannot_share_fb");
                                         }
                                     });
                                 });
                             });
	}
	else if (q->getActionType() == TYPE_RATE_APP)
	{
		Platform::RateApp(q->getLink());
		AcceptQuest(q);
	}
	else if (q->getActionType() == TYPE_SUBMIT_QUEST)
	{
		ScrMgr->OnShowWaitingDialog();
		SubmitQuest(q);
	}
	else if (q->getActionType() == TYPE_WIN_FREE)
	{
		ScrMgr->OnShowWaitingDialog();
		GlobalService::sendRequestToGetQuestion();
	}
	else if (q->getActionType() == TYPE_OPEN_FACEBOOK_REQUEST)
	{
		std::vector<std::string> list_gift;
		Platform::FBUtils::requestAskGift(list_gift, [](bool res, void* data)
		{
			ScrMgr->OnShowOKDialog("gift_request_sent");
		});
	}
	else if (q->getActionType() == TYPE_SHARE_LINK_FACEBOOK)
	{
        ScrMgr->OnShowDialog("notice", StringUtils::format(LANG_STR("do_you_want_to_share").GetString(), q->getLink().c_str()), DIALOG_TWO_BUTTON, "",
                             [this, q](const char * str, const char * name) {
                                 this->PushEvent([this, q](BaseScreen* scr){
                                     Platform::FBUtils::shareLink(q->getLink(), [this, q](bool res, void* data) {
                                         if (res) {
                                             this->AcceptQuest(q);
                                         }
                                         else {
                                             ScrMgr->OnShowErrorDialog("cannot_share_fb");
                                         }
                                     });
                                 });
                             });
	}
	else if (q->getActionType() == TYPE_SHARE_LINK_FACEBOOK_NEW)
	{
        ScrMgr->OnShowDialog("notice", StringUtils::format(LANG_STR("do_you_want_to_share").GetString(), q->getLink().c_str()), DIALOG_TWO_BUTTON, "",
                             [this, q](const char * str, const char * name) {
                                 this->PushEvent([this, q](BaseScreen* scr){
                                     Platform::FBUtils::shareLink(q->getLink(), [this, q](bool res, void* data) {
                                         if (res) {
                                             this->AcceptQuest(q);
                                         }
                                         else {
                                             ScrMgr->OnShowErrorDialog("cannot_share_fb");
                                         }
                                     });
                                 });
                             });
	}
	else if (q->getActionType() == TYPE_OPEN_ADMIN_ROLE)
	{
		
	}
	else if (q->getActionType() == TYPE_DICE)
	{
		//not use this anymore
	}
	else if (q->getActionType() == TYPE_LIKE_PAGE_GOOGLE)
	{
		OPEN_URL(q->getLink());
	}
	else if (q->getActionType() == TYPE_SEND_USER_INFO)
	{
		AcceptQuest(q);
	}
	else if (q->getActionType() == TYPE_INVITE_FACEBOOK_FRIENDS)
	{

        _fb_invitation_dialog->GetParentWidget()->Visible(true);
        _fb_invitation_dialog->SetActionCommandWithName("ON_SHOW");
        
        //loading circle rotate enable
        auto circle = _fb_invitation_dialog->GetWidgetChildByName(".loading_circle");
        circle->Visible(true);
        circle->SetActionCommandWithName("ROTATE");
        //txt visible false
        auto txtHasData = _fb_invitation_dialog->GetWidgetChildByName(".txt_have_data");
        txtHasData->Visible(false);
        //list view visble false
        auto pListTableViewFriend = _fb_invitation_dialog->GetWidgetChildByName(".list_friend_invite");
        pListTableViewFriend->Visible(false);
        
        //1. get invitable friend
        //2. open invite dialog
        //2. call invite with list friend ids
        //3. success invite
//        ScrMgr->OnShowWaitingDialog();
        
        
        Platform::FBUtils::getInvitableFriends([this](bool res, void* data){
            if(res) {
//                ScrMgr->OnHideDialog();
                FacebookFriendList* fbFriendList = static_cast<FacebookFriendList*>(data);
                this->OnReceiveFBFriendList(fbFriendList->getData());
            }
        });

        
        
//        EventScreen* eventScreen = this;
        
//        ScrMgr->OnShowDialog("notice",  StringUtils::format(LANG_STR("do_you_want_to_share").GetString(), q->getLink().c_str()), DIALOG_TWO_BUTTON, "",
//                             [this, q](const char * str, const char * name) {
//                                 this->PushEvent([this, q](BaseScreen* scr){
//                                     Platform::FBUtils::shareLink("", [this, q](bool res, void* data) {
//                                         if (res) {
//                                             this->AcceptQuest(q);
//                                         }
//                                         else {
//                                             ScrMgr->OnShowDialog("notice", "cannot_share_fb", DIALOG_ONE_BUTTON);
//                                         }
//                                     });
//                                 });
//                             });
        
//        ScrMgr->OnShowDialog("notice", StringUtils::format(LANG_STR("do_you_want_to_share").GetString(), q->getLink().c_str()), DIALOG_TWO_BUTTON, "",
//                             [this, q](const char * str, const char * name) {
//                                 this->PushEvent([this, q](BaseScreen* scr){
//                                     Platform::FBUtils::postWall(/*q->getJsonData()*/"", [this, q](bool res, void* data) {
//                                         if (res) {
//                                             this->AcceptQuest(q);
//                                         }
//                                         else {
//                                             ScrMgr->OnShowDialog("notice", "cannot_share_fb", DIALOG_ONE_BUTTON);
//                                         }
//                                     });
//                                 });
//                             });
	}
}

void EventScreen::AcceptQuest(Quest * q)
{
	RequestQuest*  obj = new RequestQuest();;
	obj->pushQuest(new Quest(q));
	GlobalService::requestAcceptQuest(obj);

	delete obj;
}

void EventScreen::SubmitQuest(Quest* q)
{
	RequestQuest * obj = new RequestQuest();
	obj->pushQuest(q);
	GlobalService::requestSubmitQuest(obj);

	delete obj;
}

void EventScreen::OnResizeAndInitQuestWidget(WidgetEntity * _widget, iwinmesage::Quest* q, int idx)
{
	cocos2d::Size current_size = _widget->GetResource()->getContentSize();
	float current_size_y = current_size.height;
	auto _text_dec = _widget->GetWidgetChildByName(".title_content");
	auto _text_title = _widget->GetWidgetChildByName(".title_");

	int type_widget = GetTypeID((int)q->getActionType());
	//int number_line = GetNumberLine(q.getDescription());//GetLineWithLongString(q.getDescription(), _text_dec);
	int origin_delta = 10;
	float size_delta = origin_delta * GetGameSize().width / this->GetDesignSize().x;


	static_cast<TextWidget*>(_text_title)->SetText(q->getTitle());
	static_cast<TextWidget*>(_text_dec)->SetText(q->getDescription());

	Vec2 pos_bot_left_text_title = _text_title->GetPosition_BottomLeft();
	_text_dec->SetPosition(Vec2(pos_bot_left_text_title.x, pos_bot_left_text_title.y - size_delta));

	auto _w_btn = _widget->GetWidgetChildByName(".btn_action");
	if (q->getButtonTitle() == "")
	{
		_w_btn->Visible(false);
	}
	else
	{
		static_cast<TextWidget*>(_w_btn->GetWidgetChildAtIdx(0))->SetText(q->getButtonTitle(), true);
		static_cast<ButtonWidget*>(_w_btn)->SetActionBtn(
			[idx , this , _w_btn]()
		{
				Quest* q = this->p_current_quest_list->getQuests()[idx];

				if (q->getIsCapchaRequire())
				{
					if (q->getActionType() == QuestType::TYPE_NEW_LUCKY_CIRCLE)
					{
						RKString link_captcha = q->getCaptchaLink();
						RKString token_captcha = q->getToken();
						auto luck_layer = static_cast<LuckyCircleScreen*>(GetCommonScr->GetLayer(COMMON_LAYER::LUCKY_CIRCLE_LAYER));
						luck_layer->SetLinkUrlCaptcha(link_captcha);
						luck_layer->SetTokenCaptcha(token_captcha);

						this->doAction(q);
					}
					else
					{
						OnShowDialogCaptcha();
						auto panel_cap = _dialog_captcha->GetWidgetChildByName(".panel_captcha");
						GameMgr->HandleURLImgDownload(
							q->getCaptchaLink(), "", [this, panel_cap](void * data, void * str, int tag)
						{
							this->ParseImgToPanel(panel_cap, data, str, 100, Texture2D::PixelFormat::RGB5A1, false);
						},false);

						auto w_btn_refresh = static_cast<ButtonWidget*>(_dialog_captcha->GetWidgetChildByName(".btn_refesh_captcha"));
						w_btn_refresh->SetActionBtn([this, q , panel_cap]() 
						{
								GameMgr->HandleURLImgDownload(
									q->getCaptchaLink(), "", [this, panel_cap](void * data, void * str, int tag)
								{
									this->ParseImgToPanel(panel_cap, data, str, 100, Texture2D::PixelFormat::RGB5A1,false);
								},false);
						});

						auto w_btn_ok = static_cast<ButtonWidget*>(_dialog_captcha->GetWidgetChildByName(".btn_ok_captcha"));
						w_btn_ok->SetActionBtn([this , q]()
						{
								//
								RKString text_capt_cha = static_cast<IWIN_TF*>(_dialog_captcha->GetWidgetChildByName(".panel_input.captcha_input"))->GetText();
								if (text_capt_cha != "")
								{
									static_cast<IWIN_TF*>(_dialog_captcha->GetWidgetChildByName(".panel_input.captcha_input"))->ClearText();
									q->setCaptchaChar(text_capt_cha.GetString());
									this->doAction(q);
									//
									OnHideDialogCaptcha();
									//
									Refresh();
								}
						});
					}
				}
				else if (q->getconfirmBox().getConfirmMsg() != "")
				{
					iwinmesage::ConfirmBox cofirm_box = q->getconfirmBox();
					ScrMgr->OnShowDialog(
						cofirm_box.getConfirmTitle(), cofirm_box.getConfirmMsg(), DIALOG_TWO_BUTTON, "str_call_back",
						[q , this](const char * call_back, const char * btn_name)
					{
						//function here
						this->doAction(q);
						OnRequestDataJson();

					} , nullptr,nullptr, cofirm_box.getYesButton() , cofirm_box.getNoButton());
				}
				else
				{
					doAction(q);
					OnRequestDataJson();
				}
		});
	}

	Vec2 pos_bot_left_text = _text_dec->GetPosition_BottomLeft();
	if(pos_bot_left_text.y < 0 )
	{
		current_size_y -= pos_bot_left_text.y;
	}
	current_size_y += size_delta;

	Vec2 size_layout_options = Vec2(0, 0);
	size_t number_option = q->getoptionList().getoptionList().size();
	if (number_option > 0)
	{
		for (size_t i = 0; i < number_option; i++)
		{
			auto option_detail = q->getoptionList().getoptionList()[i];
			int type_option = q->getoptionList().getListType();
			int type_widget = 2; // radio btn
			if (type_option == 1)
			{
				type_widget = 4; //check box
			}
			auto _widget_options = static_cast<ListViewWidget*>(_list_quest_sample)->CreateWidgetReferNoPushBack(type_widget);
			size_layout_options = _widget_options->GetSize();
			_widget_options->Visible(true);
			_widget_options->SetPosition(Vec2(pos_bot_left_text.x, pos_bot_left_text.y - size_delta - (i * size_layout_options.y)));
			current_size_y += size_layout_options.y;
			_widget->InsertChildWidget(_widget_options);
			static_cast<TextWidget*>(_widget_options->GetWidgetChildAtIdx(0))->SetText(option_detail.getText());
			_widget_options->GetWidgetChildAtIdx(2)->Visible(option_detail.getIsSelected());
			//add event for btn
			static_cast<ButtonWidget*>(_widget_options->GetWidgetChildAtIdx(1))->SetActionBtn(
				[_widget_options, type_option , q , i]()
			{	
					if (type_option == 0)
					{
						int idx_state = 0;
						for (size_t id = 0; id < _widget_options->GetParentWidget()->GetNumberChildWidget(); id++)
						{
							auto _w = _widget_options->GetParentWidget()->GetWidgetChildAtIdx(id);
							if (_w->GetName() == "layout_option_")
							{
								_w->GetWidgetChildAtIdx(2)->Visible(false);
								q->changeOptionsAtIdx(idx_state, false);
								idx_state++;
							}
						}
					}

					_widget_options->GetWidgetChildAtIdx(2)->Visible(true);
					q->changeOptionsAtIdx(i, true);
			});
		}
		current_size_y += size_delta;
	}

	if (q->getRuby() > 0 || q->getMoney() > 0)
	{
		//create money or ruby
		auto _widget_options = static_cast<ListViewWidget*>(_list_quest_sample)->CreateWidgetReferNoPushBack(5);
		_widget->InsertChildWidget(_widget_options);
		_widget_options->Visible(true);
		s64 money = 0;
		if (q->getRuby() > 0)
		{
			_widget_options->GetWidgetChildAtIdx(0)->Visible(false);
			_widget_options->GetWidgetChildAtIdx(1)->Visible(true);
			money = q->getRuby();
		}
		else
		{
			_widget_options->GetWidgetChildAtIdx(0)->Visible(true);
			_widget_options->GetWidgetChildAtIdx(1)->Visible(false);
			money = q->getMoney();
		}
		static_cast<TextWidget*>(_widget_options->GetWidgetChildByName(".icon_ruybang.txt"))->SetText(Utils::formatNumber_dot(money), true);
	}

	std::vector<WidgetEntity*> list_text_field;
	if (q->getTextboxInfo().size() > 0)
	{
		int i_tf = 0;
		for (auto str : q->getTextboxInfo())
		{
			std::string text_input = str;
			auto _widget_textfield = static_cast<ListViewWidget*>(_list_quest_sample)->CreateWidgetReferNoPushBack(6);
			_widget->InsertChildWidget(_widget_textfield);
			_widget_textfield->Visible(true);
			auto w_textfield = static_cast<IWIN_TF*>(_widget_textfield->GetWidgetChildByName(".panel_input.text_input"));

			w_textfield->SetBtnClear(_widget_textfield->GetWidgetChildByName(".panel_input.clear_text_input"));
			w_textfield->InitCallBack(nullptr, 
				[this, _widget , idx , i_tf , w_textfield]()
			{
				Quest * q = this->p_current_quest_list->getQuests()[idx];
				q->ReplaceTextBox(i_tf ,std::string(w_textfield->GetText().GetString()));
			}, nullptr);

			w_textfield->SetPlaceHolderText(text_input);

			current_size_y += (_widget_textfield->GetSize().y + (size_delta / 2));
			list_text_field.push_back(_widget_textfield);

			_list_quest_sample->GetParentLayer()->AddEventType(xml::TYPE_UI_WIDGET::WIDGET_TEXT_FIELD, w_textfield);
			i_tf++;
		}
	}

	_widget->GetResource()->setContentSize(cocos2d::Size(current_size.width, current_size_y));
	_widget->GetWidgetChildAtIdx(0)->GetResource()->setContentSize(cocos2d::Size(current_size.width, current_size_y));

	Vec2 pos_top_left_icon = _widget->GetWidgetChildAtIdx(1)->GetPosition_TopLeft();
	int delta = current_size_y - pos_top_left_icon.y - size_delta;

	if (delta >= 0)
	{
		for (size_t i = 1; i < _widget->GetNumberChildWidget(); i++)
		{
			Vec2 current_pos = _widget->GetWidgetChildAtIdx(i)->GetPosition();
			current_pos.y += delta;
			_widget->GetWidgetChildAtIdx(i)->SetPosition(current_pos);
		}
	}

	for (int i = 0 ; i < list_text_field.size() ; i ++)
	{
		list_text_field[i]->SetPosition(Vec2(size_delta, (size_delta / 2) + (i > 0 ? (list_text_field[i - 1]->GetPosition_TopLeft().y) : 0)));
	}

	_w_btn->SetPosition(Vec2(_w_btn->GetPosition().x, current_size_y / 2));

	int current_process = q->getCurrentProgress();
	int max_process = q->getMaxProgressIndex();
	if (q->getMaxProgressIndex() > 0)
	{
		auto _widget_process = static_cast<ListViewWidget*>(_list_quest_sample)->CreateWidgetReferNoPushBack(1);
		Vec2 pos_bot_left = _w_btn->GetPosition_BottomLeft();
		_widget_process->SetPosition(Vec2(pos_bot_left.x, pos_bot_left.y - size_delta));
		_widget_process->Visible(true);
		_widget->InsertChildWidget(_widget_process);
		auto widget_process_panel = _widget_process->GetWidgetChildAtIdx(0);
		static_cast<LoadingBarWidget*>(widget_process_panel->GetWidgetChildAtIdx(0))->SetPercent(((float)current_process / max_process) * 100);
		static_cast<TextWidget*>(widget_process_panel->GetWidgetChildAtIdx(1))->SetText(std::to_string(current_process) + "/" + std::to_string(max_process));
	}

	RKUtils::int64 total_time = q->getTotalTime();
	RKUtils::int64 remain_time = q->getRemainTime();
	if (total_time > 0 && remain_time > 0)
	{
		auto _widget_time = static_cast<ListViewWidget*>(_list_quest_sample)->CreateWidgetReferNoPushBack(3);
		_widget_time->Visible(true);
		_widget->InsertChildWidget(_widget_time);

		Vec2 pos_mid_btn = _w_btn->GetPosition_Middle();
		Vec2 pos_bot_left = _w_btn->GetPosition_BottomLeft();

		_widget_time->SetPosition(Vec2(pos_bot_left.x - size_delta, pos_mid_btn.y));

		RKUtils::int64 total_time_sec = total_time / 1000;
		RKUtils::int64 remain_time_sec = remain_time / 1000;
		static_cast<TimeProcessWidget*>(_widget_time->GetWidgetChildAtIdx(1))->SetTimer(total_time_sec, remain_time_sec);
		static_cast<TimeDigitalWidget*>(_widget_time->GetWidgetChildAtIdx(3))->SetTimer(total_time_sec, remain_time_sec);
	}
}

int EventScreen::GetLineWithLongString(RKString str, WidgetEntity * _widget)
{
	auto list_number_line = str.Split("\n", true);

	int width_size = static_cast<TextWidget*>(_widget)->GetTextAreaSize().x;
	int size_text = static_cast<ui::Text*>(_widget->GetResource())->getFontSize();

	int line_addition = 0;
	for (size_t i = 0; i < list_number_line.Size(); i++)
	{
		if ((float)(list_number_line[i].LengthUTF() * size_text) > width_size)
		{
			line_addition = (list_number_line[i].LengthUTF() * size_text) / width_size;
		}
	}

	return list_number_line.Size() + line_addition;

}

void EventScreen::OnShowDialogCaptcha()
{
	_dialog_captcha->GetParentWidget()->Visible(true);
	_dialog_captcha->SetActionCommandWithName("ON_SHOW");
}
void EventScreen::OnHideDialogCaptcha()
{
	_dialog_captcha->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
		this->_dialog_captcha->ForceFinishAction();
		this->_dialog_captcha->GetParentWidget()->Visible(false);
	}));
}

void EventScreen::Refresh()
{
	if (!_current_quest_category)
	{
		return;
	}
	OnRequestDataJson();
	GlobalService::GetQuestByCategoryID(_current_quest_category->getId());
}

void EventScreen::OnTouchMenuBegin(const cocos2d::Point & p)
{
    if (_fb_invitation_dialog->GetParentWidget()->Visible())
    {
        CheckTouchOutSideWidget(p, _fb_invitation_dialog,
                                [&](void){
                                    OnVisibleFBInviteDialog(false, nullptr);
                                });
    }
}


void EventScreen::OnKeyBack()
{
	if (_dialog_captcha->GetParentWidget()->Visible())
	{
		OnHideDialogCaptcha();
	}
	else if (_fb_invitation_dialog->GetParentWidget()->Visible())
	{
		OnVisibleFBInviteDialog(false, nullptr);
	}
	ScrMgr->SwitchToMenu(p_menu_come_from, EVENT_SCREEN);
}

void EventScreen::OnVisibleFBInviteDialog(bool value, const std::function<void(void)> & call_back)
{
    if (value)
    {
        this->_fb_invitation_dialog->GetParentWidget()->Visible(true);
        _fb_invitation_dialog->SetActionCommandWithName("ON_SHOW", CallFunc::create(
                                                                                    [this, call_back]()
                                                                                    {
                                                                                        this->_fb_invitation_dialog->ForceFinishAction();
                                                                                        
                                                                                        if (call_back)
                                                                                        {
                                                                                            call_back();
                                                                                        }
                                                                                    }));
    }
    else
    {
        _fb_invitation_dialog->SetActionCommandWithName("ON_HIDE", CallFunc::create(
                                                                                    [this, call_back]()
                                                                                    {
                                                                                        this->_fb_invitation_dialog->ForceFinishAction();
                                                                                        this->_fb_invitation_dialog->GetParentWidget()->Visible(false);
                                                                                        if (call_back)
                                                                                        {
                                                                                            call_back();
                                                                                        }
                                                                                    }));
    }
}

void EventScreen::OnParseFriendToInviteList(WidgetEntity * widget, int idx, bool isPlayer/* = true*/)
{
    if (idx < 0 || idx >= p_list_fbFriend.size())
        return;
    
    FacebookFriend fbFriend = !isFiltered ? p_list_fbFriend[idx] : p_list_fbFriend_filtered[idx];
    
//    if (!player)
//    {
//        PASSERT2(false, "player is null");
//        return;
//    }
    
    if (idx % 2 == 0)
    {
        widget->GetWidgetChildByName(".hl_panel")->Visible(true);
    }
    else
    {
        widget->GetWidgetChildByName(".hl_panel")->Visible(false);
    }
    
    static_cast<TextWidget*>(widget->GetWidgetChildByName(".title_id"))->SetText(fbFriend.getName(), true);
    
    widget->GetWidgetChildByName(".on_check")->Visible(fbFriend.getSelected());
    widget->GetWidgetChildByName(".off_check")->Visible(!fbFriend.getSelected());
    
    auto panel_avatar = widget->GetWidgetChildByName(".panel_avatar_clip");
    if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(fbFriend.getId()) != p_list_avatar_download.end())
    {
        this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(fbFriend.getId()), 100);
    }
}

void EventScreen::OnReceiveFBFriendList(std::vector<FacebookFriend*> fbFriendList)
{
    p_list_fbFriend_filtered.clear();
    SAFE_DELETE_VECTOR(p_list_fbFriend);
    for (auto p : fbFriendList)
    {
        FacebookFriend* fbFriend = new FacebookFriend(p);
        p_list_fbFriend.push_back(fbFriend);
        p_list_fbFriend_filtered.push_back(fbFriend);
    }
    
    WidgetEntity* dialog = _fb_invitation_dialog;
    
    //loading circle disable
    auto circle = dialog->GetWidgetChildByName(".loading_circle");
    circle->Visible(false);
    //list view visble true
    auto pListTableViewFriend = dialog->GetWidgetChildByName(".list_friend_invite");
    pListTableViewFriend->Visible(true);
    //txt visible false
    auto txtHasData = dialog->GetWidgetChildByName(".txt_have_data");
    txtHasData->Visible(fbFriendList.size() == 0);
    //txt button invite display number
    auto txtButton = dialog->GetWidgetChildByName(".invite_btn.title");
    static_cast<TextWidget*>(txtButton)->SetText(LangMgr->GetString("invite") + StringUtils::format(" (%lu)", p_list_fbFriend_filtered.size()));
    
    static_cast<IwinListViewTable*>(pListTableViewFriend)->SetHaveReload([this, dialog]() {
        auto pListTableViewFriend = dialog->GetWidgetChildByName(".list_friend_invite");
        if (p_list_fbFriend.size() > 0) {
            static_cast<IwinListViewTable*>(pListTableViewFriend)->SetNumberSizeOfCell(p_list_fbFriend.size());
            
            //download avatar in save into the list
            for (size_t i = 0; i < p_list_fbFriend.size(); i++)
            {
                auto idx_player = p_list_fbFriend[i];
                {
                    GameMgr->HandleURLImgDownload(idx_player->getPicture().getData().getUrl(), idx_player->getName(),
                                                  [this, dialog, idx_player, i](void * data, void * str, int tag){
                                                      auto pListTableViewFriend = dialog->GetWidgetChildByName(".list_friend_invite");
                                                      Image * img = (Image *)data;
                                                      if (p_list_fbFriend.size() > 0)
                                                      {
                                                          Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
                                                          this->p_list_avatar_download.insert(idx_player->getId(), sprite_img);
                                                          
                                                          auto widget_lb = static_cast<ListViewTableWidget*>(pListTableViewFriend);
                                                          if ((size_t)i < widget_lb->GetListWidgetSample().size())
                                                          {
                                                              widget_lb->UpdateCellAtIdx(i);
                                                          }
                                                      }
                                                  });
                }
            }
        }
    });
}

void EventScreen::CheckAllFBFriendList(bool isCheck)
{
    for (auto fbFriend : p_list_fbFriend)
    {
        fbFriend->setSelected(isCheck);
    }
    
    auto pListTableViewFriend = _fb_invitation_dialog->GetWidgetChildByName(".list_friend_invite");
    auto list_widget = static_cast<ListViewTableWidget*>(pListTableViewFriend)->GetListWidgetSample();
    
    for (auto widget : list_widget)
    {
        widget->GetWidgetChildByName(".on_check")->Visible(isCheck);
        widget->GetWidgetChildByName(".off_check")->Visible(!isCheck);
    }
    
    //txt button invite display number
    auto txtButton = _fb_invitation_dialog->GetWidgetChildByName(".invite_btn.title");
    static_cast<TextWidget*>(txtButton)->SetText(LangMgr->GetString("invite") + StringUtils::format(" (%lu)", isCheck ? p_list_fbFriend.size() : 0));
}

bool EventScreen::ModAllCheck(bool isCheck)
{
    if (isCheck)
    {
        bool have_un_check = false;
        for (auto p : p_list_fbFriend)
        {
            if (!p->getSelected())
            {
                have_un_check = true;
                break;
            }
        }
        
        p_top_title_invite->GetWidgetChildByName(".on_check")->Visible(!have_un_check);
        p_top_title_invite->GetWidgetChildByName(".off_check")->Visible(have_un_check);
    }
    else
    {
        bool have_check = false;
        for (auto p : p_list_fbFriend)
        {
            if (p->getSelected())
            {
                have_check = true;
                break;
            }
        }
        
        p_top_title_invite->GetWidgetChildByName(".on_check")->Visible(!have_check);
        p_top_title_invite->GetWidgetChildByName(".off_check")->Visible(have_check);
    }
    
    
    //txt button invite display number
    auto txtButton = _fb_invitation_dialog->GetWidgetChildByName(".invite_btn.title");
    static_cast<TextWidget*>(txtButton)->SetText(LangMgr->GetString("invite") + StringUtils::format(" (%lu)", isCheck ? p_list_fbFriend.size() : 0));
    
    return true;
}

void EventScreen::OnFilterFBFriendInviteDialog(RKString name)
{
//    isFiltered = true;
    p_list_fbFriend_filtered.clear();
    for(auto fbFriend : p_list_fbFriend)
    {
        if(fbFriend->getName().find(name.GetString()) != std::string::npos)
        {
            p_list_fbFriend_filtered.push_back(fbFriend);
        }
    }
    
    //update display filtered
    auto pListTableViewFriend = _fb_invitation_dialog->GetWidgetChildByName(".list_friend_invite");
    if(isFiltered)
        static_cast<IwinListViewTable*>(pListTableViewFriend)->SetNumberSizeOfCell(p_list_fbFriend_filtered.size(), true);
    else
        static_cast<IwinListViewTable*>(pListTableViewFriend)->SetNumberSizeOfCell(p_list_fbFriend.size(), true);
    
    //txt button invite display number
    auto txtButton = _fb_invitation_dialog->GetWidgetChildByName(".invite_btn.title");
    static_cast<TextWidget*>(txtButton)->SetText(LangMgr->GetString("invite") + StringUtils::format(" (%lu)", name != "" ? p_list_fbFriend_filtered.size() : p_list_fbFriend.size()));
}


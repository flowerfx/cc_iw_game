#include "ProfileScreen.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wPageView.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wListViewTable.h"
#include "UI/AdvanceUI/wCheckBox.h"

#include "MainScreen.h"

#include "Social/SocialManager.h"
#include "Network/Global/Globalservice.h"
#include "Models/Achievement.h"
#include "Models/Item.h"

#include "Common/IwinListViewTable.h"
#include "Common/IwinTextfield.h"

#include "Platform/Platform.h"

#include "ProfileScreens/TransferMoneyLayout.h"
#include "ProfileScreens/ShopAvatarLayer.h"
#include "ProfileScreens/LeftUserProfileLayer.h"
#include "ProfileScreens/MainUserProfileLayer.h"
#include "ProfileScreens/TransactionHistoryLayer.h"
#include "ProfileScreens/OTPPageLayer.h"

using namespace iwinmesage;

#define DISABLE_ANIM_SCROLL 1

ProfileScreen::ProfileScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;

	p_page_view_profile = nullptr;

	p_page_view_achieve = nullptr;
	p_previous_page_idx = 0;
	p_list_achieve.clear();

	p_layout_edit_profile = nullptr;
	p_top_panel = nullptr;
	_callback_change_avatar = nullptr;
}

ProfileScreen::~ProfileScreen()
{

	p_previous_page_idx = -1;

	_callback_change_avatar = nullptr;

	SAFE_DELETE_VECTOR(p_list_achieve);



	SAFE_DELETE(_otp_page_layer);
	SAFE_DELETE(_transaction_history_layer);
	SAFE_DELETE(_main_user_profile_layer);
	SAFE_DELETE(_transfer_money_layout);
	SAFE_DELETE(_avatar_shop_layout);
	SAFE_DELETE(_left_user_detail_panel);
}

int ProfileScreen::Init()
{
	//override the list view table default with the iwinlistview table
	this->_func_action = [&](LayerEntity * layer) {
		layer->OverloadRegisterUIWidget<IwinListViewTable>(xml::WIDGET_LIST_VIEW_TABLE);
		layer->OverloadRegisterUIWidget<IwinTextFieldHandle>(xml::WIDGET_TEXT_FIELD);
		layer->OverloadRegisterUIWidget<IwinEditBoxdHandle>(xml::WIDGET_EDIT_BOX);
	};
	//

	RKString _menu = "profile_screen";

	InitMenuWidgetEntity(_menu);
	
	CallInitComponent(true);

	return 1;
}

int	ProfileScreen::InitComponent()
{
	_transfer_money_layout = new TransferMoneyLayout();
	_transfer_money_layout->InitLayer(this);

	_avatar_shop_layout = new ShopAvatarLayer();
	_avatar_shop_layout->InitLayer(this);

	_left_user_detail_panel = new LeftUserProfileLayer();
	_left_user_detail_panel->InitLayer(this);

	_main_user_profile_layer = new MainUserProfileLayer();
	_main_user_profile_layer->InitLayer(this);

	_transaction_history_layer = new TransactionHistoryLayer();
	_transaction_history_layer->InitLayer(this);

	_otp_page_layer = new OTPPageLayer();
	_otp_page_layer->InitLayer(this);

	p_top_panel = GetWidgetChildByName("profile_screen.top_panel");
	p_page_view_profile = GetWidgetChildByName("profile_screen.main_profile_layout");

	auto page_profile_name = p_page_view_profile->GetWidgetChildByName(".page_profile_.page_profile_name");
	p_list_choose_sx = page_profile_name->GetWidgetChildByName(".title_input_sx.list_choose_sx");
	p_list_choose_day = page_profile_name->GetWidgetChildByName(".title_input_birth.list_choose_day");
	p_list_choose_month = page_profile_name->GetWidgetChildByName(".title_input_birth.list_choose_month");
	p_list_choose_year = page_profile_name->GetWidgetChildByName(".title_input_birth.list_choose_year");

	p_page_view_achieve = p_page_view_profile->GetWidgetChildByName(".page_profile_.page_profile_achieve.list_detail_achieve");

	p_layout_edit_profile = GetWidgetChildByName("profile_screen.layout_edit_profile");

	//init table_view
	static_cast<ListViewTableWidget*>(p_page_view_achieve)->InitTable();


	_callback_change_avatar = [](const char *buffer, unsigned int leng) {
		if (buffer)
		{
            cocos2d::Vec2 size_save = Vec2(100.f , 100.f);
            
            //we need to resize the buffer because some imge too large
            cocos2d::Image * img = new cocos2d::Image();
            img->initWithImageData((const unsigned char*)buffer, leng);
            cocos2d::Texture2D * tex = new cocos2d::Texture2D();
            tex->initWithImage(img);
            delete img;
            //delete buffer;
            
            auto sprite = cocos2d::Sprite::createWithTexture(tex);
            sprite->setScale(size_save.x / tex->getContentSizeInPixels().width, size_save.y / tex->getContentSizeInPixels().height);
            auto _renderTexture = RenderTexture::create(size_save.x,size_save.y,Texture2D::PixelFormat::RGBA8888);
            _renderTexture->begin();
            sprite->visit();
            _renderTexture->end();
            
            img =  _renderTexture->newImage();
			GlobalService::sendCaptureAvatar((char*)img->getData(), img->getDataLen());
            
            delete img;
           // tex->release();
        
		}
		else
		{
			ScrMgr->OnShowOKDialog("cannot_get_image");
		}
	
	};
	return 1;
}


int ProfileScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}

	_transfer_money_layout->Update(dt);
	_main_user_profile_layer->Update(dt);
	_transaction_history_layer->Update(dt);
	_otp_page_layer->Update(dt);

	if (p_receive_data_achievement && static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->GetGameList())
	{
		p_receive_data_achievement = false;
		RunThreadParseLB();
	}

	return 1;
}

void ProfileScreen::OnTouchMenuBegin(const cocos2d::Point & p)
{
	if (p_list_choose_sx->Visible())
	{
		CheckTouchOutSideWidget(p, p_list_choose_sx,
			[&](void) {
			static_cast<MainUserProfileLayer*>(_main_user_profile_layer)->OnShowOrHideListChoose(p_list_choose_sx, false);
		});
	}
	else if (p_list_choose_day->Visible())
	{
		CheckTouchOutSideWidget(p, p_list_choose_day,
			[&](void) {
			static_cast<MainUserProfileLayer*>(_main_user_profile_layer)->OnShowOrHideListChoose(p_list_choose_day, false);
		});
	}
	else if (p_list_choose_month->Visible())
	{
		CheckTouchOutSideWidget(p, p_list_choose_month,
			[&](void) {
			static_cast<MainUserProfileLayer*>(_main_user_profile_layer)->OnShowOrHideListChoose(p_list_choose_month, false);
		});
	}
	else if (p_list_choose_year->Visible())
	{
		CheckTouchOutSideWidget(p, p_list_choose_year,
			[&](void) {
			static_cast<MainUserProfileLayer*>(_main_user_profile_layer)->OnShowOrHideListChoose(p_list_choose_year, false);
		});
	}
	else if (p_layout_edit_profile->Visible())
	{
		CheckTouchOutSideWidget(p, p_layout_edit_profile,
			[this](void)
		{
			this->p_layout_edit_profile->SetActionCommandWithName("ON_HIDE");
		});
	}
	else if (_avatar_shop_layout->IsVisibleLayer())
	{
		CheckTouchOutSideWidget(p, GetWidgetChildByName("profile_screen.shop_avatar_layout"),
			[this](void)
		{
			_avatar_shop_layout->OnHideLayer();
		});
	}
}

void ProfileScreen::OnShowIdxBtnTopPanel(int idx)
{
	p_top_panel->GetWidgetChildByName(".bg_info_on")->Visible(false);
	p_top_panel->GetWidgetChildByName(".bg_achieve_on")->Visible(false);
	p_top_panel->GetWidgetChildByName(".bg_history_on")->Visible(false);
	p_top_panel->GetWidgetChildByName(".bg_otp_on")->Visible(false);

	if (idx == 0)
	{
		p_top_panel->GetWidgetChildByName(".bg_info_on")->Visible(true);
	}
	else if (idx == 1)
	{
		p_top_panel->GetWidgetChildByName(".bg_achieve_on")->Visible(true);
	}
	else if (idx == 2)
	{
		p_top_panel->GetWidgetChildByName(".bg_history_on")->Visible(true);
	}
	else if (idx == 3)
	{
		p_top_panel->GetWidgetChildByName(".bg_otp_on")->Visible(true);
	}
}

void ProfileScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	_left_user_detail_panel->ProcessWidget(name, type_widget, _widget);
	_transfer_money_layout->ProcessWidget(name, type_widget, _widget);
	_avatar_shop_layout->ProcessWidget(name, type_widget, _widget);
	_main_user_profile_layer->ProcessWidget(name, type_widget, _widget);
	_transaction_history_layer->ProcessWidget(name, type_widget, _widget);
	_otp_page_layer->ProcessWidget(name, type_widget, _widget);
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_info" && p_previous_page_idx != 0)
		{
			ShowLayoutState(0);
		}
		else if (name == "btn_achieve" && p_previous_page_idx != 1)
		{
			ShowLayoutState(1);
		}
		else if (name == "btn_history" && p_previous_page_idx != 2)
		{
			ShowLayoutState(2);
		}
		else if (name == "btn_otp" && p_previous_page_idx != 3)
		{
			ShowLayoutState(3);
		}

		else if (name == "btn_back")
		{
			ScrMgr->SwitchToMenu(p_menu_come_from, MENU_LAYER::PROFILE_SCREEN);
		}

		
		else if (name == "send_iwin_other")
		{
			_transfer_money_layout->OnShowLayer();
		}

		else if (name == "btn_profile")
		{
			p_layout_edit_profile->SetActionCommandWithName("ON_SHOW");
		}

		else if (name == "change_pass_btn")
		{
			p_layout_edit_profile->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_layout_edit_profile->ForceFinishAction();

				//scr_common->SetUser(this->p_user);
				GetCommonScr->OnShowLayer(COMMON_LAYER::CHANGE_PASS_LAYER);
			}));
		}
		else if (name == "take_avatar_gallery_btn")
		{
			if (_callback_change_avatar)
			{
				Platform::LoadImageFromFile(_callback_change_avatar);
			}
		}
		else if (name == "take_avatar_camera_btn")
		{
			if (_callback_change_avatar)
			{
				Platform::CaptureImageFromCamera(_callback_change_avatar);
			}
		}
		else if (name == "take_avatar_shop_btn")
		{
			p_layout_edit_profile->SetActionCommandWithName("ON_HIDE", CallFunc::create(
				[this]() {
				this->p_layout_edit_profile->ForceFinishAction();
				_avatar_shop_layout->OnShowLayer();
			}));
		}

	}
	else if (type_widget == UI_TYPE::UI_TEXT_FIELD)
	{
		auto w_tx = static_cast<IWIN_TF*>(_widget);
		w_tx->HandleEventTextField(name);
	}
	
}

void ProfileScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(PROFILE_SCREEN);

	p_menu_show_next = MENU_NONE;

	static_cast<MainUserProfileLayer*>(_main_user_profile_layer)->FuncWhenDeactiveLayer();
	static_cast<LeftUserProfileLayer*>(_left_user_detail_panel)->FuncWhenDeactiveLayer();

}

void ProfileScreen::OnBeginFadeIn()
{
	static_cast<MainUserProfileLayer*>(_main_user_profile_layer)->CreateListDate();

	GlobalService::getAllUserAchievement(GameController::myInfo->IDDB);

	ShowLayoutState(0);
}

void ProfileScreen::SetActionAfterShow(int idx)
{
	this->PushEvent([idx](BaseScreen * scr) {
		ProfileScreen * p_scr = (ProfileScreen*)scr;
		if (idx == 0)
		{
			p_scr->_avatar_shop_layout->OnShowLayer();
		}
		else if (idx == 1)
		{
			p_scr->ShowLayoutState(0);
		}
	}, TIME_ACTION);
}

void ProfileScreen::OnKeyBack()
{
	if (_transfer_money_layout->IsVisibleLayer())
	{
		_transfer_money_layout->OnHideLayer();
	}
	else if (_avatar_shop_layout->IsVisibleLayer())
	{
		if (static_cast<ShopAvatarLayer*>(_avatar_shop_layout)->IsConfirmBuyAvatarVisible())
		{
			static_cast<ShopAvatarLayer*>(_avatar_shop_layout)->OnCloseConfirmBuyAvatar();
		}
		else
		{
			_avatar_shop_layout->OnHideLayer();
		}
	}
	else if (p_layout_edit_profile->Visible())
	{
		p_layout_edit_profile->SetActionCommandWithName("ON_HIDE");
	}
	else if (static_cast<TransactionHistoryLayer*>(_transaction_history_layer)->isDetailHisPanelVisible())
	{
		static_cast<TransactionHistoryLayer*>(_transaction_history_layer)->CloseDetailHisPanel();
	}
	else if (static_cast<OTPPageLayer*>(_otp_page_layer)->isOTPInputVisible())
	{
		static_cast<OTPPageLayer*>(_otp_page_layer)->CloseOTPInput();
	}
	else
	{
		ScrMgr->SwitchToMenu(p_menu_come_from, MENU_LAYER::PROFILE_SCREEN);
	}
}

void ProfileScreen::ParseUserUI()
{
	User * p_user = GetUser;
	if (p_user == nullptr)
	{
		PASSERT2(false, "user is null");
		return;
	}

	static_cast<MainUserProfileLayer*>(_main_user_profile_layer)->SetDetail(p_user);

	static_cast<LeftUserProfileLayer*>(_left_user_detail_panel)->ParseUserToLayer();

	if (!p_user->isEmailValidate)
	{
		static_cast<OTPPageLayer*>(_otp_page_layer)->OnActiveShowOTP(false);
	}
}

void ProfileScreen::onMyAvatars(std::vector<Item*> items)
{
}

void ProfileScreen::onMyItems(std::vector<Item*> items, int cateId, int totalPage, ubyte gender) 
{
}

void ProfileScreen::onEventItems(std::vector<Item*> items) 
{
	//showLoadingCircle(false, userInfoScroll);
}

void ProfileScreen::onChangeMyAvatar()
{
	User * p_user = GetUser;
	if (p_user == nullptr)
	{
		PASSERT2(false, "user is null");
		return;
	}

	p_user->avatarID = GameController::myInfo->avatar;
	ScrMgr->OnShowOKDialog("you_have_successfully");
	if (p_previous_page_idx == 0)
	{
		GlobalService::getMyAvatars();
	}
}

void ProfileScreen::onSetAchievement(void * data)
{
	//p_page_view_achieve->ClearChild();
	for (auto a : p_list_achieve)
	{
		delete a;
	}
	p_list_achieve.clear();

	std::vector<Achievement*> p_achie = *(std::vector<Achievement*>*)data;
	p_list_achieve = p_achie;
	//
	PASSERT2(p_list_achieve.size() > 0 , "list achievement is zero!");
	if (p_list_achieve.size() < 0)
	{
		return;
	}
	//
	p_receive_data_achievement = true;

}

void ProfileScreen::RunThreadParseLB()
{
	auto _list_view_achieve = static_cast<ListViewTableWidget*>(p_page_view_achieve);

	_list_view_achieve->addEventListener(
		[this, _list_view_achieve](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{

			int current_idx_to = _list_view_achieve->GetCurrentIdxTo();
			if ((size_t)current_idx_to >= this->p_list_achieve.size())
			{
				PASSERT2(false, "problem here !");
				current_idx_to = this->p_list_achieve.size() - 1;
			}

			OnParseAchievementIntoGame(_widget, this->p_list_achieve[current_idx_to], current_idx_to);
		}
	});

	_list_view_achieve->SetNumberSizeOfCell(p_list_achieve.size());
}

void ProfileScreen::OnParseAchievementIntoGame(WidgetEntity* widget , Achievement * achie , int idx)
{
	PASSERT2(achie != nullptr, "incorrect data");
	//PASSERT2(idx < p_page_view_achieve->GetNumberChildWidget() && idx >= 0, "idx out of list achievement!");

	PASSERT2(widget != nullptr, "widget is nullptr !");

	auto detail = ScrMgr->GetGameDetailByType((GameType)(achie->gameId));

	static_cast<TextWidget*>(widget->GetWidgetChildByName(".title"))->SetText(detail.name);
	auto panel_layout = widget->GetWidgetChildByName(".layout_");
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".panel_exp_score.title"))->SetText(std::to_string(achie->exp), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_level_number"))->SetText(std::to_string(achie->eloNumber), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_level"))->SetText(achie->elo, true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_win_number"))->SetText(std::to_string(achie->win), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_lose_number"))->SetText(std::to_string(achie->lose), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_draw_number"))->SetText(std::to_string(achie->draw), true);
	static_cast<TextWidget*>(panel_layout->GetWidgetChildByName(".txt_leave_number"))->SetText(std::to_string(achie->givenUp), true);
}


void ProfileScreen::onReceiveAvatarCatList(void * data)
{
	static_cast<ShopAvatarLayer*>(_avatar_shop_layout)->onReceiveAvatarCatList(data);
}

void ProfileScreen::onReceiveAvatarList(void * data)
{
	static_cast<ShopAvatarLayer*>(_avatar_shop_layout)->onReceiveAvatarList(data);
}


void ProfileScreen::onBuyingAvatarResult(void * data)
{
	static_cast<ShopAvatarLayer*>(_avatar_shop_layout)->onBuyingAvatarResult(data);
}


void ProfileScreen::ShowLayoutState(int state)
{
	this->PushEvent([this, state](BaseScreen * scr) {

		
		auto w_left_panel = static_cast<LeftUserProfileLayer*>(_left_user_detail_panel);
		if (state == 0)
		{
#if !DISABLE_ANIM_SCROLL
			p_page_profile_name->SetActionCommandWithName("MOVE_MIDDLE", CallFunc::create(
				[this]() {
				this->p_page_profile_name->ForceFinishAction();
				this->p_page_profile_left->GetWidgetChildByName(".player_status")->Visible(true);
			}));
			if (p_previous_page_idx == 1)
			{
				p_page_view_achieve->GetParentWidget()->SetActionCommandWithName("MOVE_RIGHT");
			}
			else
			{
				p_page_trans_history->SetActionCommandWithName("MOVE_RIGHT");
			}
#else

			_main_user_profile_layer->OnShowLayer([this, w_left_panel]() {
				w_left_panel->OnShowLayer();
				w_left_panel->GetStatusWidget()->Visible(true);
			});

			p_page_view_achieve->GetParentWidget()->Visible(false);
			_transaction_history_layer->OnHideLayer();
			_otp_page_layer->OnHideLayer();
#endif
			p_previous_page_idx = 0;
			OnShowIdxBtnTopPanel(p_previous_page_idx);
		}
		else if (state == 1)
		{
#if ! DISABLE_ANIM_SCROLL
			this->p_page_profile_left->GetWidgetChildByName(".player_status")->Visible(false);
			if (p_previous_page_idx == 0)
			{
				p_page_profile_name->SetActionCommandWithName("MOVE_LEFT");
				p_page_view_achieve->GetParentWidget()->SetActionCommandWithName("MOVE_MIDDLE_FROM_RIGHT");
			}
			else
			{
				p_page_trans_history->SetActionCommandWithName("MOVE_RIGHT");
				p_page_view_achieve->GetParentWidget()->SetActionCommandWithName("MOVE_MIDDLE_FROM_LEFT");
			}
#else
			p_page_view_achieve->GetParentWidget()->SetActionCommandWithName("SET_POS", CallFunc::create(
				[this , w_left_panel]() {
				this->p_page_view_achieve->GetParentWidget()->ForceFinishAction();
				this->p_page_view_achieve->GetParentWidget()->Visible(true);
				w_left_panel->OnShowLayer();
				w_left_panel->GetStatusWidget()->Visible(false);
			}));

			_main_user_profile_layer->OnHideLayer();
			_transaction_history_layer->OnHideLayer();
			_otp_page_layer->OnHideLayer();
#endif
			p_previous_page_idx = 1;
			OnShowIdxBtnTopPanel(p_previous_page_idx);
		}
		else if (state == 2)
		{
#if ! DISABLE_ANIM_SCROLL
			this->p_page_profile_left->GetWidgetChildByName(".player_status")->Visible(false);
			p_page_trans_history->SetActionCommandWithName("MOVE_MIDDLE");
			if (p_previous_page_idx == 0)
			{
				p_page_profile_name->SetActionCommandWithName("MOVE_LEFT");
			}
			else
			{
				p_page_view_achieve->GetParentWidget()->SetActionCommandWithName("MOVE_LEFT");
			}
#else
			_transaction_history_layer->OnShowLayer([this ,w_left_panel]() {
				w_left_panel->OnHideLayer();
				w_left_panel->GetStatusWidget()->Visible(false);
			});

			_main_user_profile_layer->OnHideLayer();
			_otp_page_layer->OnHideLayer();
			p_page_view_achieve->GetParentWidget()->Visible(false);
#endif
			p_previous_page_idx = 2;
			OnShowIdxBtnTopPanel(p_previous_page_idx);
		}
		else if (state == 3)
		{
			_otp_page_layer->OnShowLayer([this, w_left_panel]() 
			{
				w_left_panel->OnShowLayer();
				w_left_panel->GetStatusWidget()->Visible(false);
			});
			
			p_page_view_achieve->GetParentWidget()->Visible(false);

			_transaction_history_layer->OnHideLayer();
			_main_user_profile_layer->OnHideLayer();

			p_previous_page_idx = 3;
			OnShowIdxBtnTopPanel(p_previous_page_idx);
		}

	});
}

bool ProfileScreen::IsShopAvatarVisible()
{
	return _avatar_shop_layout->IsVisibleLayer();
}

void ProfileScreen::ClearComponent()
{
	p_previous_page_idx = 0;

	for (auto a : p_list_achieve)
	{
		delete a;
	}
	p_list_achieve.clear();
	p_receive_data_achievement = false;

	_avatar_shop_layout->Reset();

	static_cast<ListViewTableWidget*>(p_page_view_achieve)->ResetListView();

	this->PushEvent([](BaseScreen * scr) 
	{
		ProfileScreen * new_scr = (ProfileScreen*)scr;
		new_scr->ShowLayoutState(0);
		new_scr->PushEvent([](BaseScreen * scr) 
		{
			ProfileScreen * new_scr = (ProfileScreen*)scr;
			static_cast<TransactionHistoryLayer*>(new_scr->_transaction_history_layer)->ResetStateHistory();
		});
	});
}

void ProfileScreen::OnReceiveTransferInfo(int * arg_list)
{
	static_cast<TransferMoneyLayout*>(_transfer_money_layout)->OnReceiveTransferInfo(arg_list);
}

void ProfileScreen::OnRecieveTransactionHistory(void * trans_history)
{
	static_cast<TransactionHistoryLayer*>(_transaction_history_layer)->OnRecieveTransactionHistory(trans_history);
}

void ProfileScreen::onReceiveAuthorizeOTP(void * data)
{
	static_cast<OTPPageLayer*>(_otp_page_layer)->onReceiveAuthorizeOTP(data);
}

void ProfileScreen::onReceiveOTPStatus(void * list_data)
{
	static_cast<OTPPageLayer*>(_otp_page_layer)->onReceiveOTPStatus(list_data);
}

void ProfileScreen::onReceiveCHangeOtpDefault(int otpId, bool status, std::string msg)
{
	static_cast<OTPPageLayer*>(_otp_page_layer)->onReceiveCHangeOtpDefault(otpId , status , msg);
}


void ProfileScreen::onShowActiveOTPDialog(void * data)
{
	static_cast<OTPPageLayer*>(_otp_page_layer)->onShowActiveOTPDialog(data);
}

int ProfileScreen::GetPageIdx()
{
	return p_previous_page_idx;
}

#include <math.h>
#include "PlayerGroup.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wTimeProcessWidget.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wButton.h"

#include "Screens/BaseScreen.h"
#include "Screens/MainScreen.h"
#include "Screens/GameScreens/GameScreens.h"
#include "Screens/GamePlayMgr.h"

#include "Common/GameController.h"
#include "Common/SpriteUrl.h"
#include "Models/Player.h"

#include "Utils.h"
#include "Network/Global/Globalservice.h"

PlayerGroup::PlayerGroup(WidgetEntity * widget)
{
	_widget = widget;

	xml_icon_dec = nullptr;
	xml_chat_text_dec = nullptr;

	p_current_rank = -1;
	p_current_seat = -1;
	p_current_game_screen = nullptr;
	p_is_owner = false;
	p_layout_timer = nullptr;
	if (_widget)
	{
		p_layout_player = _widget->GetWidgetChildByName(".layout_player");

		p_btn_user_info = p_layout_player->GetWidgetChildAtIdx(0);
		p_layout_user_control = p_layout_player->GetWidgetChildByName(".layout_player_controller");

		p_layout_timer = _widget->GetWidgetChildByName(".layout_timer");
		p_panel_info = p_layout_player->GetWidgetChildByName(".layout_info_player");
	}
	p_current_money = 0;
	p_pos_bonus_money = Vec2(0, 0);
}

PlayerGroup::PlayerGroup()
{
	_widget = nullptr;
	p_layout_player = nullptr;

	xml_dec_player = nullptr;
	p_layout_timer = nullptr;
	p_current_game_screen = nullptr;
	p_current_seat = -1;
	p_current_money = 0;
	p_pos_bonus_money = Vec2(0, 0);

	p_is_lock_chat_action = true;
	p_current_rank = -1;
}
PlayerGroup::~PlayerGroup()
{
	removePlayer();
	//weak pointer cannot be deteled
	_widget = nullptr;
	p_layout_player = nullptr;

	p_layout_timer = nullptr;
	if (xml_icon_dec)
	{
		delete xml_icon_dec;
		xml_icon_dec = nullptr;
	}
	if (xml_chat_text_dec)
	{
		delete xml_chat_text_dec;
		xml_chat_text_dec = nullptr;
	}
	if (xml_dec_player)
	{
		delete xml_dec_player;
		xml_dec_player = nullptr;
	}

	//
	p_current_game_screen = nullptr;
	p_current_rank = -1;
}


/// nếu p nó == null là ko có người nếu có người là nó viết sai
void PlayerGroup::setPlayer(Player* p, int idx)
{
	p_current_seat = idx;
	if (!p)
	{
		p_current_game_screen->SetVisiblePlayerCom(false, p_current_seat);
	}
	else
	{
		p_current_game_screen->SetVisiblePlayerCom(true, p_current_seat);
	}
	auto _player = p_current_game_screen->GetPlayerAtSeat(p_current_seat);
	//
	auto btn_add_player = _widget->GetWidgetChildByName(".btn_add_user_");

	p_layout_player->Visible(true);


	auto user_name_txt = p_panel_info->GetWidgetChildByName(".user_name_txt");
	auto number_iwin_txt = p_panel_info->GetWidgetChildByName(".number_iwin_txt");

	auto btn_show_info = p_layout_user_control->GetWidgetChildAtIdx(0);
	static_cast<ButtonWidget*>(btn_show_info)->SetActionBtn([this, idx]() 
	{
			if (this->p_layout_user_control->Visible())
			{
				this->p_layout_user_control->Visible(false);
				PASSERT2(this->p_current_game_screen != nullptr, "p_current_game_screen is null");
				GetCommonScr->OnShowPlayerInfo(this->p_current_game_screen->GetPlayerGroupAtIdx(idx)->getPlayer());
			}			
	});

	auto btn_add_friend = p_layout_user_control->GetWidgetChildAtIdx(2);
	static_cast<ButtonWidget*>(btn_add_friend)->SetActionBtn([this, idx]() 
	{
			if (this->p_layout_user_control->Visible())
			{
				this->p_layout_user_control->Visible(false);
				PASSERT2(this->p_current_game_screen != nullptr, "p_current_game_screen is null");
				auto current_player = this->p_current_game_screen->GetPlayerGroupAtIdx(idx)->getPlayer();
				if (current_player)
				{
					ScrMgr->OnShowDialog("notice", (LANG_STR("friend_adding")).GetString() + current_player->name, DIALOG_ZERO_BUTTON_WAIT);
					GlobalService::addFriend(current_player->IDDB, current_player->name);
				}
			}
	});

	auto btn_kick_info = p_layout_user_control->GetWidgetChildAtIdx(1);
	static_cast<ButtonWidget*>(btn_kick_info)->SetActionBtn([this, idx]() 
	{
			if (this->p_layout_user_control->Visible())
			{
				this->p_layout_user_control->Visible(false);
				GlobalService::kick(
					GameController::currentGameScreen->GetRoomID(),
					GameController::currentGameScreen->GetBoardID(),
					this->getPlayer()->IDDB);
			}
	});

	if (p_btn_user_info)
	{
		static_cast<ButtonWidget*>(p_btn_user_info)->SetActionBtn([this, idx]() 
		{
				if (this->getPlayer() && this->getPlayer()->IDDB == GameController::myInfo->IDDB)
				{
					//p_current_game_screen->OnShowProfileLayout();
					GetCommonScr->OnShowPlayerInfo(this->getPlayer());
				}
				else
				{

					if (!this->p_layout_user_control->Visible())
					{
						this->p_layout_user_control->Visible(true);
						this->p_layout_user_control->GetWidgetChildAtIdx(1)->Visible(this->p_current_game_screen->IsMyInfoRoomOwner());
						auto current_player = this->p_current_game_screen->GetPlayerGroupAtIdx(idx)->getPlayer();
						if (current_player)
						{
							bool isFriend = GameController::isFriend(current_player->IDDB);
							if (isFriend)
							{
								this->p_layout_user_control->GetWidgetChildAtIdx(2)->Visible(false);
							}
							else
							{
								this->p_layout_user_control->GetWidgetChildAtIdx(2)->Visible(true);
							}
						}
						else
						{
							this->p_layout_user_control->GetWidgetChildAtIdx(2)->Visible(true);
						}

						for (size_t i = 0; i < this->p_layout_user_control->GetNumberChildWidget(); i++)
						{
							this->p_layout_user_control->GetWidgetChildAtIdx(i)->SetActionCommandWithName("ON_SHOW");
						}
					}
					else
					{
						for (size_t i = 0; i < this->p_layout_user_control->GetNumberChildWidget(); i++)
						{
							auto w = this->p_layout_user_control->GetWidgetChildAtIdx(i);
							w->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, i, w]() {
								w->ForceFinishAction();
								if (i == this->p_layout_user_control->GetNumberChildWidget() - 1)
								{
									this->p_layout_user_control->Visible(false);
								}
							}));
						}
					}
				}
		});
	}

	if (!_player || (_player && _player->IDDB < 0))
	{
		btn_add_player->Visible(true);
		p_layout_player->Visible(false);

		static_cast<TextWidget*>(user_name_txt)->SetText("");
		static_cast<TextWidget*>(number_iwin_txt)->SetText("");

		p_panel_info->Visible(false);
		p_layout_player->Visible(false);

		p_current_game_screen->ShowTextChatAtIdx(false, p_current_seat);
		p_current_seat = -1;
		return ;
	}
	else
	{
		p_layout_player->Visible(true);
		btn_add_player->Visible(false);
		p_panel_info->Visible(true);
	}
		
	//set name 
	if (_player->IDDB == GameController::myInfo->IDDB &&
		p_current_game_screen && 
		p_current_game_screen->SetMyInfoToPanel(p->name, p->getMoney(), GameController::currentMoneyType))
	{
		p_panel_info->Visible(false);

		p_layout_player->GetWidgetChildByName(".avatar_user_panel")->SetActionCommandWithName("SCALE_BIG");
		p_layout_player->GetWidgetChildByName(".bg_avatar")->SetActionCommandWithName("SCALE_BIG");
		p_layout_timer->SetActionCommandWithName("SCALE_BIG");
		p_layout_player->GetWidgetChildByName(".btn_gift_user_")->Visible(false);
		p_layout_player->GetWidgetChildByName(".btn_gift_user_")->SetActionCommandWithName("MOVE_BIG");
		p_layout_player->GetWidgetChildByName(".icon_host")->SetActionCommandWithName("MOVE_BIG");
		p_layout_player->GetWidgetChildByName(".icon_quickplay")->SetActionCommandWithName("MOVE_BIG");
		p_layout_player->GetWidgetChildByName(".icon_deny")->SetActionCommandWithName("SCALE_BIG");

	}
	else
	{

		static_cast<TextWidget*>(user_name_txt)->SetText(_player->name);
		static_cast<TextWidget*>(number_iwin_txt)->SetText(Utils::formatNumber_dot(_player->getMoney()));
		p_panel_info->Visible(true);
		p_layout_player->GetWidgetChildByName(".avatar_user_panel")->SetActionCommandWithName("SCALE_NORMAL");
		p_layout_player->GetWidgetChildByName(".bg_avatar")->SetActionCommandWithName("SCALE_NORMAL");
		p_layout_timer->SetActionCommandWithName("SCALE_NORMAL");
		p_layout_player->GetWidgetChildByName(".btn_gift_user_")->Visible(true);
		static_cast<ButtonWidget*>(p_layout_player->GetWidgetChildByName(".btn_gift_user_"))->SetActionBtn([this]() 
		{
			this->p_current_game_screen->openGiftWithIDPlayer(((this->getPlayer() != nullptr) ? this->getPlayer()->IDDB : -1));
		});
		p_layout_player->GetWidgetChildByName(".icon_host")->SetActionCommandWithName("MOVE_NORMAL");
		p_layout_player->GetWidgetChildByName(".icon_quickplay")->SetActionCommandWithName("MOVE_NORMAL");
		p_layout_player->GetWidgetChildByName(".btn_gift_user_")->SetActionCommandWithName("MOVE_NORMAL");
		p_layout_player->GetWidgetChildByName(".icon_deny")->SetActionCommandWithName("SCALE_NORMAL");

		//resize the bg
		auto size_name = static_cast<TextWidget*>(user_name_txt)->GetActualContainSize();
		auto size_money = static_cast<TextWidget*>(number_iwin_txt)->GetActualContainSize();

		auto size_bg = p_panel_info->GetWidgetChildByName(".bg")->GetSize();
		p_panel_info->GetWidgetChildByName(".bg")->SetSize(Vec2((size_name.x > size_money.x ? size_name.x : size_money.x) + (50.f * GetGameSize().width / p_current_game_screen->GetDesignSize().x), size_bg.y));
	}
	p_layout_player->GetWidgetChildByName(".btn_gift_user_")->SetOpacity(255);
	//set avatar
	SetAvatar();
	HideRank();

	//
	return;
}


Player* PlayerGroup::getPlayer()
{
	return p_current_game_screen->GetPlayerAtSeat(p_current_seat);
}

void PlayerGroup::start(long totalTime, long currentTime)
{

}

void PlayerGroup::setOwner(bool param1)
{
	p_layout_player->GetWidgetChildByName(".icon_host")->Visible(param1);
	p_is_owner = param1;
}
void PlayerGroup::stopTime()
{
	p_layout_timer->Visible(false);
	static_cast<TimeProcessWidget*>(p_layout_timer->GetWidgetChildByName(".progress_1"))->StopTimeProcess();
	static_cast<TimeProcessWidget*>(p_layout_timer->GetWidgetChildByName(".progress_2"))->StopTimeProcess();
	static_cast<TimeProcessWidget*>(p_layout_timer->GetWidgetChildByName(".progress_3"))->StopTimeProcess();
	//GetSound->StopTheCurrentSound();
}
// đưa vào time count down cho từng thằng nó chơi bài
void PlayerGroup::startTime(int interval, int curentTime)
{
	//curentTime = interval - curentTime;

	//khi toi luot cua minh thi rung len
	if (this->GetIDDB() == GameController::myInfo->IDDB)
	{
		Utils::vibrateShort();
	}

	p_layout_timer->Visible(true);
	static_cast<TimeProcessWidget*>(p_layout_timer->GetWidgetChildByName(".progress_1"))->SetTimer(interval, curentTime);
	static_cast<TimeProcessWidget*>(p_layout_timer->GetWidgetChildByName(".progress_2"))->SetTimer(interval, curentTime);
	static_cast<TimeProcessWidget*>(p_layout_timer->GetWidgetChildByName(".progress_3"))->SetTimer(interval, curentTime);
	xml::BasicDec value;

	auto process_timer = p_layout_timer->GetWidgetChildByName(".progress_3");

	Vec2 current_pos = Vec2(process_timer->GetPosition_Middle().x, process_timer->GetPosition_TopLeft().y);
	Vec2 center_pos = process_timer->GetPosition_Middle();


	//current_pos.x = current_pos.x / process_timer->GetScale().x;
	//current_pos.y = current_pos.y / process_timer->GetScale().y;

	//center_pos.x = center_pos.x / process_timer->GetScale().x;
	//center_pos.y = center_pos.y / process_timer->GetScale().y;
	int value_rotate = 0;
	if (interval == 0)
	{
		value_rotate = 0;
	}
	else
	{
		value_rotate = ((interval - curentTime / interval) * 360.f);
	}


	float angle = (value_rotate) * (3.141f / 180); // Convert to radians

	float rotatedX = (std::cos(angle) * (current_pos.x - center_pos.x)) - (std::sin(angle) * (current_pos.y - center_pos.y)) + center_pos.x;
	float rotatedY = (std::sin(angle) * (current_pos.x - center_pos.x)) + (std::cos(angle) * (current_pos.y - center_pos.y)) + center_pos.y;

	if (value_rotate != 0 && value_rotate % 360 == 0)
	{
		value_rotate = 360;
	}
	else if (value_rotate > 360)
	{
		value_rotate = value_rotate % 360;
	}

	value.InsertDatafloat("TIME", interval - curentTime);
	value.InsertDataInt("ANGLE_ROTATE_AROUND", value_rotate);
//	value.InsertDataVector2("SET_POSITION", Vec2(rotatedX, rotatedY));
	value.InsertDataVector2("ROTATE_AROUND", center_pos);

	auto _red_dot_fly = p_layout_timer->GetWidgetChildByName(".red_dot_fly");
	_red_dot_fly->GetResource()->stopAllActions();

	_red_dot_fly->SetHookCommand("ROTATE", "act0", 0, 2, value, "TIME");
	_red_dot_fly->SetHookCommand("ROTATE", "act0", 0, 2, value, "ANGLE_ROTATE_AROUND");
	_red_dot_fly->SetHookCommand("ROTATE", "act0", 0, 2, value, "ROTATE_AROUND");
	_red_dot_fly->SetHookCommand("ROTATE", "act0", 0, 0, value, "SET_POSITION");

	_red_dot_fly->SetActionCommandWithName("ROTATE");

	_red_dot_fly->ClearHookCommand("ROTATE", "act0", 0, 2);
	_red_dot_fly->ClearHookCommand("ROTATE", "act0", 0, 0);
}


cocos2d::Rect PlayerGroup::GetRectFront()
{
	cocos2d::Rect rect(0, 0, 0, 0);
	Vec2 pos = _widget->GetPosition_Middle();
	Vec2 size = _widget->GetSize();
	auto layout_control = _widget->GetWidgetChildByName(".layout_player.layout_player_controller");
	float current_rotate = layout_control->GetRotate();
	float distance = 100.f * (GetGameSize().width / (p_current_game_screen != nullptr ? p_current_game_screen->GetDesignSize().x : GetGameSize().width));

	Vec2 start = Vec2(0, 0);
	float sin_value = float((int)(std::sinf(current_rotate * M_PI / 180.f) + 0.5f));
	float cos_value = float((int)(std::cosf(current_rotate * M_PI / 180.f) + 0.5f));

	if (sin_value == 0) //0 or 180 dess
	{
		start.x = pos.x - (distance / 2.f);
		if (cos_value == -1)
		{
			start.y = (pos.y - size.y);
		}
		else if (cos_value == 1)
		{
			start.y = (pos.y + size.y);
		}

		rect = cocos2d::Rect(start.x, start.y, pos.x + (distance / 2.f), cos_value);
	}
	if (cos_value == 0) //90 or 270 dess
	{
		start.y = pos.y - (distance / 2.f);
		if (sin_value == -1)
		{
			start.x = (pos.x + size.x);
		}
		else if (sin_value == 1)
		{
			start.x = (pos.x - size.x);
		}
		rect = cocos2d::Rect(start.x, start.y, sin_value, pos.y + (distance / 2.f));
	}
	
	return rect;
}

void PlayerGroup::setReady(bool isReady)
{
	PASSERT2(_widget != nullptr, "resource is null");
	p_layout_player->GetWidgetChildByName(".icon_ready")->Visible(isReady);
}
void PlayerGroup::setVisible(bool isVisible)
{
	_widget->Visible(isVisible);
}
void PlayerGroup::setPosition(Vec2 pos)
{
	_widget->SetPosition(pos);
}

void PlayerGroup::setRankNumber(int rank)
{
	p_current_rank = rank;
	if (rank == 0)
	{
		auto w_flare = _widget->GetWidgetChildByName(".fx_flare");
		w_flare->Visible(true);
		w_flare->SetActionCommandWithName("ROTATE");
	}
	p_current_game_screen->ShowRankNumberAtPos(true, p_current_rank, _widget->GetPosition_Middle());
	p_layout_player->Visible(false);


}

void PlayerGroup::HideRank()
{
	p_current_game_screen->ShowRankNumberAtPos(false, p_current_rank, _widget->GetPosition_Middle());
	ShowLayerPlayer(true);
	showFlare(false);
}

void PlayerGroup::ShowLayerPlayer(bool isShow)
{
	p_layout_player->Visible(isShow);
	_widget->GetWidgetChildByName(".btn_add_user_")->Visible(!isShow);
}

void PlayerGroup::setIsDenyTurn(bool isDeny)
{
	auto deny_icon = p_layout_player->GetWidgetChildByName(".icon_deny");
	deny_icon->Visible(isDeny);
	/*if (isDeny)
	{
		deny_icon->SetActionCommandWithName("ACTION");
	}
	else
	{
		deny_icon->ForceFinishAction();
	}*/
}


void PlayerGroup::SetXMLData(xml::UILayerWidgetDec* xml_dec, int id)
{
	if (id == 0)
	{
		xml_dec_player = xml_dec;
	}
	else if (id == 1)
	{
		xml_chat_text_dec = xml_dec;
	}
	else
	{
		xml_icon_dec = xml_dec;
	}
}

void PlayerGroup::Update(float dt)
{
	if (!p_is_lock_chat_action && p_current_game_screen)
	{
		p_is_lock_chat_action = true;
		if (p_current_game_screen->IsTextChatVisible(p_current_seat))
		{
			if (!_stack_text.empty())
			{
				ShowChat();
			}
			else
			{
				p_current_game_screen->ShowTextChatAtIdx(false, p_current_seat);
			}
		}
		else
		{
			p_current_game_screen->ShowTextChatAtIdx(true, p_current_seat,"",0);
			this->p_is_lock_chat_action = false;
		}
	}
}

void PlayerGroup::SetTextChat(RKString text,bool isChatBoard)
{
	if (text == "") 
		return;
	if (_stack_text.size() <= 0)
	{
		p_is_lock_chat_action = false;
	}
	ChatBoard b;
	b.isChatBoard = isChatBoard;
	b.text = Utils::chatConvert(text.GetString());
	_stack_text.push_back(b );

	if (!p_is_lock_chat_action)
	{
		p_current_game_screen->ShowTextChatAtIdx(true, p_current_seat, _stack_text[0].text, 1);
	}
}

void PlayerGroup::ShowChat()
{
	ChatBoard board = _stack_text.front();
	p_current_game_screen->ShowTextChatAtIdx(true, p_current_seat, board.text, 2, [this]() {
		this->p_is_lock_chat_action = false;
	});
	_stack_text.erase(_stack_text.begin());
	if (board.isChatBoard)
	{
		this->p_current_game_screen->PushTextChat(this->getPlayer() != nullptr ? this->getPlayer()->name : "", board.text);
	}
	

}

void PlayerGroup::SetAvatar()
{
	auto panel = p_layout_player->GetWidgetChildByName(".avatar_user_panel");
	auto _player = p_current_game_screen->GetPlayerAtSeat(p_current_seat);
	if (_player->userProfile == nullptr )
	{
		PASSERT2(false, "user profile of this player is null");
		return;
	}

	GameMgr->HandleAvatarDownload(
		_player->avatar, _player->name, [this, panel](void * data, void * str, int tag)
	{
		if (GamePlay == nullptr
			|| (GamePlay && GamePlay->GetCurrentGameScreen() == nullptr)
			|| (GamePlay && GamePlay->GetCurrentGameScreen() && GamePlay->GetCurrentGameScreen()->GetPlayerGroupAtIdx(0) == nullptr))
		{
			CC_SAFE_DELETE(data);
			return;
		}

		ScrMgr->GetMenuUI(ScrMgr->GetCurrentMenuUI())->ParseImgToPanel(panel, data , str, 100);
	});
}

void PlayerGroup::OnSetData(User* data)
{
	User *p_user =  ((User*)data);
	auto player = p_current_game_screen->GetPlayerAtSeat(p_current_seat);
	if (player)
	{
		player->setUserProfile(p_user);
	}
}

int PlayerGroup::GetIDDB()
{
	auto player = p_current_game_screen->GetPlayerAtSeat(p_current_seat);
	if (player)
	{
		return player->IDDB;
	}
	else
	{
		return -1;
	}
}


void PlayerGroup::setMoney(s64 money)
{
	static_cast<TextWidget*>(p_panel_info->GetWidgetChildByName(".number_iwin_txt"))->SetText(Utils::formatNumber_dot(money));
}

void PlayerGroup::SetCurrentGameScreen(GameScreen * gr)
{
	p_current_game_screen = gr;
}

int PlayerGroup::GetSeat()
{
	return p_current_seat;
}

bool PlayerGroup::IsOwner()
{
	return p_is_owner;
}

void PlayerGroup::showFlare(bool visible)
{
	auto w_flare = _widget->GetWidgetChildByName(".fx_flare");
	w_flare->Visible(visible);
	if (visible)
	{
		w_flare->SetActionCommandWithName("ROTATE");
	}
	else
	{
		w_flare->GetResource()->stopAllActions();
	}
}

void PlayerGroup::RunTurnEffect(CallFunc * call_back)
{
	if (p_layout_player->Visible())
	{
		p_layout_player->GetWidgetChildByName(".avatar_user_panel")->SetActionCommandWithName("TURN_EFFECT", call_back);
	}
}

void PlayerGroup::removePlayer()
{
	//remove avatar
	auto child = p_layout_player->GetWidgetChildByName(".avatar_user_panel")->GetResource()->getChildByTag(99);
	if (child)
	{
		p_layout_player->GetWidgetChildByName(".avatar_user_panel")->GetResource()->removeChildByTag(99);
	}
}

s64 PlayerGroup::GetCurrentMoney()
{
	return p_current_money;
}

void PlayerGroup::SetCurrentMoney(s64 value)
{
	p_current_money = value;
}

void PlayerGroup::SetActionMoneyRunTo(s64 toMoney)
{
	s64 money = toMoney;
	auto w_text = p_panel_info->GetWidgetChildByName(".number_iwin_txt");
	static_cast<TextWidget*>(w_text)->TextRunEffect(
		money, p_current_money, 1.f, Utils::formatNumber_dot, CallFunc::create([this, money]()
	{
		this->SetCurrentMoney(money);
	}));
}

void PlayerGroup::SetActionAppearBonusMoney(s64 bonus_money)
{
	p_current_game_screen->ShowTextBonusMoney(p_current_seat, bonus_money, p_pos_bonus_money);
}

void PlayerGroup::SetPosBonus(Vec2 pos)
{
	p_pos_bonus_money = pos;
}

void PlayerGroup::setQuickPlay(bool isQuickPlay)
{
	p_layout_player->GetWidgetChildByName(".icon_quickplay")->Visible(isQuickPlay);
}

void PlayerGroup::ShowIconWaiting(bool visible)
{
	_widget->GetWidgetChildByName(".icon_waiting")->Visible(visible);
}


#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wTextField.h"

#include "Network/Game/GameLogicHandler.h"
#include "Network/Game/GameMessageHandler.h"
#include "Network/Game/GameService.h"
#include "Network/Global/GlobalService.h"
#include "Common/FlyMoney.h"
#include "Constants.h"

#include "Screens/Object/ObjectEntity.h"
#include "Screens/Object/TimeBarProcess.h"
#include "Screens/Object/PlayerGroup.h"

#include "RKThread_Code/RKThreadWrapper.h"
#include "GameScreens.h"
#include "Screens/Object/Card.h"

#include "Screens/NewLobbyScreen.h"
#include "Screens/LobbyScreen.h"

#include "Network/Core/ServerMgr.h"
GameScreen::GameScreen()
{
	SetGameID(GameType::NULL_TYPE);
	p_max_player = GameController::getMaxPlayerOf(GameController::gameId);

	vector<int> ls1;
	ls1.resize(5);
	ls1.clear();
	vector<int> ls2(4,2);
	btn_bet_iwin = nullptr;
	p_text_money = nullptr;
	_myInfo = nullptr;

	_isStartGame = false;
	_isGameEnd = false;

	_roomID = -1;
	_boardID = -1;
	_isViewer = true;

	p_thread_load_xml_player_dec = nullptr;
	p_load_xml_state = 0;
	p_is_finish_load_player = false;
	p_list_msg_return.clear();
	p_list_id_card_on_hold.clear();

	p_touch_start = Vec2(0, 0);
	p_on_hold_card_selected = false;

}

GameScreen::~GameScreen()
{
	GameController::myInfo->isQuickPlay = false;
	seats.clear();
	//weak pointer cannot delete
	p_panel_game_screen_resource = nullptr;
	p_panel_card_hold = nullptr;
	//
	_myInfo = nullptr;
	if (p_timebar_process)
	{
		delete p_timebar_process;
		p_timebar_process = nullptr;
	}

	if (p_thread_load_xml_player_dec)
	{
		delete p_thread_load_xml_player_dec;
		p_thread_load_xml_player_dec = nullptr;
	}

	//weak point not delete
	p_thread_player_infos.clear();
	//free msg temp
	for (auto msg : p_list_msg_return)
	{
		delete msg;
	}
	p_list_msg_return.clear();
	p_is_finish_load_player = false;

	p_list_card_on_selected.clear();
	RemoveCardOnSelected();
	SetGameID(GameType::NULL_TYPE);
}


bool GameScreen::InitGame(int game_id)
{
	HUDScreen::Init();

	CallInitComponent(false);

	GlobalService::getBetRange();

	livePlayer();

	return true;
}

int	GameScreen::InitComponent()
{
	HUDScreen::InitComponent();
	p_panel_game_screen_resource = GetWidgetChildByName("HUD_screen.panel_game_card")->GetResource();
	p_panel_card_hold = GetWidgetChildByName("HUD_screen.layout_list_card_hold")->GetResource();

	auto panel_timebar = GetWidgetChildByName("HUD_screen.progress_bar_panel");
	p_timebar_process = new TimeBarProcess(panel_timebar);
	p_timebar_process->setVisible(false);

	btn_bet_iwin = GetWidgetChildByName("HUD_screen.top_right_layout.btn_bet_iwin");

	p_bonus_effect = GetWidgetChildByName("HUD_screen.animate_effect_bonus");

	p_panel_result = GetWidgetChildByName("HUD_screen.result_layout");

	p_effect_hl = GetWidgetChildByName("HUD_screen.hud_screen_layout.effect_hl");

	return 1;
}

int GameScreen::Update(float dt)
{
	if (HUDScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (p_timebar_process && p_timebar_process->getVisible()) 
	{
		p_timebar_process->update(dt);
	}

	if (p_load_xml_state > 0 && p_load_xml_state <= p_max_player)
	{
		parse_xml_player(p_load_xml_state - 1);
		p_load_xml_state++;
		if (p_load_xml_state > p_max_player) {
			HandleAfterInitPlayer();
		}
	}

	return 1;
}

GameType GameScreen::CurrentGameType()
{
	return p_current_game_type;
}

void GameScreen::SetGameID(GameType id)
{
	p_current_game_type = id;
	GameController::currentScreenId = id;
}


void GameScreen::addChildObject(Node * child, int z_order)
{
	child->setTag(13);
	child->removeFromParent();
	p_panel_game_screen_resource->addChild(child, z_order);
}

void GameScreen::removeChildObject(Node * child, bool isAll)
{
	if (!isAll)
	{
		p_panel_game_screen_resource->removeChild(child, true);
	}
	else
	{
		p_panel_game_screen_resource->removeChildByTag(13);
		//p_panel_game_screen_resource->removeAllChildrenWithCleanup(true);
	}
}

void GameScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	//abstract class not implement this function
}

bool GameScreen::SetMyInfoToPanel(RKString name, u64 number_money, int type_money)
{
	updateTextWithMoneyType();
	static_cast<TextWidget*>(p_text_name)->SetText(name, true);

	if (type_money == BASE_MONEY) // iwin
	{
		p_text_money = p_btn_purchase_iwin->GetWidgetChildByName(".title_");
		static_cast<TextWidget*>(p_text_money)->SetText(Utils::formatNumber_dot(number_money), true);
	}
	else //ruby
	{
		p_text_money = p_btn_purchase_ruby->GetWidgetChildByName(".title_");
		static_cast<TextWidget*>(p_text_money)->SetText(Utils::formatNumber_dot(number_money), true);
	}

	bool isBaseMoney = GameController::isBaseMoney();

	p_btn_purchase_iwin->SetVisible(isBaseMoney);
	p_bg_table_win->SetVisible(isBaseMoney);

	p_btn_purchase_ruby->SetVisible(!isBaseMoney);
	p_bg_table_rubby->SetVisible(!isBaseMoney);

	if (isBaseMoney)
	{
		p_text_min_bet->SetColor(Color3B::WHITE);
		p_text_gamename->SetColor(Color3B::WHITE);
	}
	else
	{
		p_text_min_bet->SetColor(Color3B::BLACK);
		p_text_gamename->SetColor(Color3B::BLACK);
	}

	return true;
}

std::map<RKString, xml::UILayerWidgetDec*> GameScreen::LoadWidgetCustom(TiXmlDocument * p_objectXML)
{
	std::map<RKString, xml::UILayerWidgetDec*> p_list_sample_dec;
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return p_list_sample_dec;
	}

	auto sample_ui_dec = Root->FirstChildElement("MenuWidget");
	if (!sample_ui_dec)
	{
		PASSERT2(false, "XML fail to load!");
		return p_list_sample_dec;
	}
	Vec2 design_size = XMLMgr->ConvertToVector2(sample_ui_dec->Attribute("designSize"));

	//read xml and parse into data
	auto sample_ui_dec_xml = sample_ui_dec->FirstChildElement("Widget");
	do
	{
		if (sample_ui_dec_xml)
		{
			auto _widget_root = XMLMgr->OnParseWidgetEntity1(sample_ui_dec_xml, nullptr, XMLMgr->GetUIWidgetByName(name_ui), nullptr);
			p_list_sample_dec.insert(std::pair<RKString, xml::UILayerWidgetDec*>(_widget_root->NameWidget, _widget_root));
			sample_ui_dec_xml = sample_ui_dec_xml->NextSiblingElement();
		}
	} while (sample_ui_dec_xml);

	return p_list_sample_dec;
}

void GameScreen::ParseIntoCustom(xml::UILayerWidgetDec* xml_dec, WidgetEntity * p_panel_custom, RKString name_panel_xml)
{
	auto menu_widget_xml = XMLMgr->GetUIWidgetByName(name_ui);
	auto layer_root = GetLayerChildByName("HUD_screen");

	xml_dec->p_menu_widget = menu_widget_xml;
	xml_dec->p_parent_layer = menu_widget_xml->GetElementChildByName("HUD_screen")->GetChildWidgetByName(name_panel_xml.GetString());

	auto widget = layer_root->InitWidgetEntity(xml_dec, layer_root, p_panel_custom);
	p_panel_custom->ResizeAndPosChildWidget(widget);

	widget->SetMenuParent(p_panel_custom->GetMenuParent());
	widget->SetParentWidget(p_panel_custom);
	widget->SetParentLayer(p_panel_custom->GetParentLayer());
	p_panel_custom->InsertChildWidget(widget);

}

void GameScreen::kicked(int roomId, int boardId, int playerID, std::string reason)
{
	if (roomId != GetRoomID() || boardId != GetBoardID()) 
		return;

	if (playerID == GameController::myInfo->IDDB)
	{
		RKString reason_correct = "";
		if (reason == "")
		{
			reason_correct = ((_ownerID == playerID) ? "timeout_you_are_kicked" : "you_are_kicked_by_room");
		}
		else
		{
			reason_correct = reason;
		}
		ScrMgr->OnShowDialog("notice", reason_correct, TYPE_DIALOG::DIALOG_ONE_BUTTON, "",
			
			[this](const char * str, const char* btn_name)
		{
			this->doQuit();
		});

	}
}


void GameScreen::DoBack()
{
	GameService::leaveBoard(_roomID, _boardID);
	clientQuit();
}

void GameScreen::clientQuit()
{
}

void GameScreen::doQuit()
{
	RKString game_name_server = GetServerMgr()->getCurServer()._name;
	GameType current_type = GameController::getCurrentGameType();
	if (GameController::isNewLobby(current_type))
	{
		ScrMgr->SwitchToMenu(MENU_LAYER::NEW_LOBBY_SCREEN, MENU_LAYER::HUD_SCREEN);
		auto ui_screen = ScrMgr->GetMenuUI(MENU_LAYER::NEW_LOBBY_SCREEN);
		static_cast<NewLobbyScreen*>(ui_screen)->setTitle((game_name_server + "-" + GameController::getCurrentGameName()).GetString());
		static_cast<NewLobbyScreen*>(ui_screen)->SetCurrentGameID(current_type);
		static_cast<NewLobbyScreen*>(ScrMgr->GetMenuUI(NEW_LOBBY_SCREEN))->SetHaveComeFromGame();
	}
	else
	{
		ScrMgr->SwitchToMenu(MENU_LAYER::LOBBY_SCREEN, MENU_LAYER::HUD_SCREEN);
		static_cast<LobbyScreen*>(ScrMgr->GetMenuUI(MENU_LAYER::LOBBY_SCREEN))->SetTextTitle(game_name_server + "-" + GameController::getCurrentGameName());
		static_cast<LobbyScreen*>(ScrMgr->GetMenuUI(MENU_LAYER::LOBBY_SCREEN))->SetCurrentGameID(current_type);
		static_cast<LobbyScreen*>(ScrMgr->GetMenuUI(LOBBY_SCREEN))->SetHaveComeFromGame();
	}
}

void GameScreen::doConfirmQuit(string msg)
{
	ScrMgr->OnShowDialog("notice", msg,
		TYPE_DIALOG::DIALOG_TWO_BUTTON,
		"",
		[& , this](const char * call_back, const char* btn_name)
	{
		//agree leave board
		GlobalService::confirmLeaveGame(this->_roomID,this->_boardID, (unsigned char)0);
		ScrMgr->OnShowWaitingDialog();
	},
		[&, this](const char * call_back, const char* btn_name)
	{
		//not agree leave board
		GlobalService::confirmLeaveGame(this->_roomID, this->_boardID, (unsigned char)1);
	});
}

/**
* Ham nay dung de kick 1 user ra khoi game.
*/
void GameScreen::doKick(int playerID)
{}
void GameScreen::getFriendListToInvite()
{}

void GameScreen::showProfileOf(int userID)
{}

/**
* Set cho user vao trang thai ready hay k?
*
* @param isReady
*/
void GameScreen::doReady()
{}

void GameScreen::doReadyForReady()
{}

void GameScreen::doQuickPlay()
{}

void GameScreen::resetReady()
{}

void GameScreen::doResetQuickPlay(bool isStart)
{
	btn_quick_play->Visible(isStart);
	int number_player = 0;
	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB >= 0)
		{
			onSomeOneQuickPlay(_PLAYER(p)->IDDB, false);
			number_player++;
		}
	}
	if (number_player > 1)
	{
		btn_quick_play->Visible(!isStart);
	}
}


/**
* Ham nay dung de set lai tien dat cuoc khi server tra ve.
*
* @param money
*/
void GameScreen::setBetMoney(s64 money, s64 max_money)
{
	GetSound->PlaySoundEffect("SFX_BETMONEY");
	Utils::vibrateShort();
	_money = money;

	if (btn_bet_iwin)
	{
		static_cast<ButtonWidget*>(btn_bet_iwin)->SetTitle(Utils::formatNumber_dot(money));
	}

	std::string string_money = "";
	std::string string_max_money = "";
	bool isBaseMoney = GameController::isBaseMoney();
	if (isBaseMoney)
	{
		string_money = Utils::formatNumber(money);
		string_max_money = Utils::formatNumber_win(max_money);
	}
	else
	{
		string_money = Utils::formatNumber(money);
		string_max_money = Utils::formatNumber_ruby(max_money);
	}

	if (money == 0 || money == max_money || 
		(CurrentGameType() != GAMEID_BAICAO && CurrentGameType() != GAMEID_BLACKJACK && CurrentGameType() != GAMEID_BAICAORUA))
	{
		static_cast<TextWidget*>(p_text_min_bet)->SetText(Utils::formatNumber_win(money), true);
	}
	else
	{
		static_cast<TextWidget*>(p_text_min_bet)->SetText(string_money + " / " + string_max_money , true);
	}
	auto w_front_text = p_text_min_bet->GetParentWidget()->GetWidgetChildByName(".min_bet_tx");
	auto size_text_bet = static_cast<TextWidget*>(w_front_text)->GetActualContainSize();
	p_text_min_bet->SetPosition(Vec2(w_front_text->GetPosition().x + size_text_bet.x + (5 * GetGameSize().width / this->GetDesignSize().x), w_front_text->GetPosition().y));
}
/**
* Remove the player when the player leaved the board. Khong nen override
* ham nay.
*
* @param leaveID
*/
void GameScreen::removePlayer(int leaveID)
{
	int idx_player = 0;
	for (auto p : p_list_player_group)
	{
		auto player = _PLAYER(p);
		if (p && player && player->IDDB == leaveID)
		{
			if (p != nullptr)
			{
				//remove gift at that player !!!
				p_layout_gift_send_to->GetResource()->removeChildByTag(leaveID);
				//
				p->setPlayer(nullptr , idx_player);
				p->setReady(false);
				p->stopTime();
				p->HideRank();
				p->ShowLayerPlayer(false);
			}
			seats[idx_player] = -1;
			player->IDDB = -1;
			player->setName("");
			player->setExp(0);
			player->isReady = false;
			player->reset();
			break;
		}
		idx_player++;
	}

}
/**
* Thêm player vào danh sách
*/
void GameScreen::addPlayer(int seat, Player * p)
{
	//PASSERT2((size_t)seat < p_list_player_group.size(), "wrong seat number !");
	//p_list_player_group[seat]->setPlayer(p , seat);
	delete p_list_player[seat];
	p_list_player[seat] = p;
}

/**
* Khoi tao thong tin ban khi minh moi vao game. Khi overide ham nay phai
* goi super truoc.
*
* @param roomID
* @param boardID
* @param ownerID
* @param money
* @param playerInfos
*/


/**
* cập nhật thông tin bàn
*/
void GameScreen::updateBoardInfo()
{}


void GameScreen::resetSeat() {

}

/**
* reset seat phuc vu cho viec join board khong nhay tien cua tat ca cac
* thang
*
*/
void GameScreen::resetSeatNotUserAction()
{}
/**
* Set chu phong cho game.
*
* @param newOwner
*/

void GameScreen::hideStartButton() {}
void GameScreen::hideReadyButton() {}

void GameScreen::showViewerLabel() {}

void GameScreen::hideViewerLabel() {}


bool GameScreen::hasOrtherPlayerInBoard()
{
	return false;
}
/**
* Show button continue game.
*/
void GameScreen::setContinueButton() {}

void GameScreen::hideContinueButton() {}
void GameScreen::setStartButton() {}



/**
* Tra ve player co id cu the
*
* @param userID
* @return null neu k tim thay.
*/

void GameScreen::onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney, RKString dec)
{
	//not handle this function
}

void GameScreen::OnMyMoneyChange(int value, PlayerGroup* p, s64 currMoney)
{
	int pre_money = p->GetCurrentMoney();
	if (p_text_money)
	{
		static_cast<TextWidget*>(p_text_money)->TextRunEffect(
			currMoney, pre_money, 1.f, Utils::formatNumber_dot, CallFunc::create([p, currMoney, this]()
		{
			p->SetCurrentMoney(currMoney);

			this->setMyMoney(currMoney);
		}));
		GameController::myInfo->setMoney(GameController::currentMoneyType, currMoney);
		GameController::myInfo->userProfile->money = currMoney;
		ScrMgr->MyInfoChanged();
	}
	else
	{
		PASSERT2(false, "some thing wrong here");
	}
	if (value > 0)
	{
		p_bonus_effect->Visible(true);
		static_cast<AnimWidget*>(p_bonus_effect)->RunAnim(CallFunc::create([this]() {
			this->p_bonus_effect->Visible(false);
		}));
		//playsound
		PLAY_SOUND("SFX_COIN_DROP");
		//
	}
}

void GameScreen::onBonusMoney(int playerID, int value, std::string description, s64 currMoney)
{
	//if (value == 0) return;
	//CCLOG("BONUS MONEY : %lld value %d with dec: %s ", currMoney, value, description.c_str());
	PlayerGroup* p = getPlayerGroup(playerID);
	if (p 
		&& p->getPlayer() && p->GetIDDB() >= 0)
	{
		if (p->GetIDDB() == GameController::myInfo->IDDB)
		{
			OnMyMoneyChange(value, p, currMoney);
		}
		else
		{
			p->SetActionMoneyRunTo(currMoney);
		}
		if (value != 0)
		{
			p->SetActionAppearBonusMoney(value);
		}

		onHandleBonusMoney(p, value, currMoney , description);

		if (description != "")
		{
			p->SetTextChat(description,false);
		}

	}
}

PlayerGroup*  GameScreen::getSeatGroup(int seat)
{
	return p_list_player_group[seat];
}

void GameScreen::setInviteOrtherPeopleButton()
{

}

void GameScreen::onSomeOneReady(int playerID, bool isReady)
{
	ScrMgr->OnHideDialog();
	if (p_list_player_group.size() <= 0)
	{
		PASSERT2(false, "wrong !");
		return;
	}

	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB == playerID)
		{
			_PLAYER(p)->isReady = isReady;

			if (playerID == GameController::myInfo->IDDB) 
			{
				if (!_isViewer && !_isStartGame && !_isGameEnd)
				{
					//setReadyButton(isReady);
				}

				if (_myInfo && _PLAYER(_myInfo))
				{
					_PLAYER(_myInfo)->isReady = isReady;
				}
			}
			if (isReady)
			{
				GetSound->PlaySoundEffect("SFX_READY");
			}
			else
			{
				GetSound->PlaySoundEffect("SFX_UNREADY");
			}
			p->setReady(isReady);
			break;
		}
	}
	if (_isViewer && (!_myInfo || !_PLAYER(_myInfo))) {
		hideReadyButton();
		hideStartButton();
		showViewerLabel();
	}
}

void GameScreen::onSomeOneQuickPlay(int playerID, bool isQuickPlay)
{
	ScrMgr->OnHideDialog();
	if (p_list_player_group.size() <= 0) 
	{
		PASSERT2(false, "wrong !");
		return;
	}

	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB == playerID)
		{

			_PLAYER(p)->isQuickPlay = isQuickPlay;

			if (playerID == GameController::myInfo->IDDB) 
			{
				// if (!isViewer && !isStartGame && !isGameEnd)
				// setReadyButton(isReady);
				if (_myInfo && _PLAYER(_myInfo))
				{
					_PLAYER(_myInfo)->isQuickPlay = isQuickPlay;
				}
			}
			if (isQuickPlay)
			{
				GetSound->PlaySoundEffect("SFX_READY");
			}
			/*else
			{
				GetSound->PlaySoundEffect("SFX_UNREADY");
			}*/
			p->setQuickPlay(isQuickPlay);
			break;
		}
	}

	if (_isViewer && _myInfo == nullptr)
	{
		hideReadyButton();
		hideStartButton();
		showViewerLabel();
	}

	if (playerID == GameController::myInfo->IDDB && isQuickPlay)
	{
		btn_quick_play->Visible(false);
		GameController::myInfo->isQuickPlay = true;
	}
}

void GameScreen::livePlayer()
{
	int livePlayer = 0;
	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB >= 0)
		{
			livePlayer++;
		}
	}

	if (_myInfo && _PLAYER(_myInfo) && !_PLAYER(_myInfo)->isQuickPlay && !_isStartGame)
	{
		if (livePlayer < 2)
		{
			onSomeOneQuickPlay(_PLAYER(_myInfo)->IDDB, false);
			btn_quick_play->Visible(false);
		}
		else
		{
			btn_quick_play->Visible(true);
		}
	}
	if (_isViewer)
	{
		CCLOG("livePlayer() _isViewer: btn_quick_play visible : FALSE");
		btn_quick_play->Visible(false);
	}
	// return livePlayer;
}

void GameScreen::OnShowTxtWait(int value)
{
	if (value == -1)
	{
		if (getNumberPlayer() <= 1)
		{
            static_cast<TextWidget*>(p_text_wait)->SetText("wait_others");
			p_text_wait->Visible(true);
			p_text_wait->SetActionCommandWithName("ACTION");
		}
		else
		{
            static_cast<TextWidget*>(p_text_wait)->SetText("watching");
			p_text_wait->Visible(true);
			p_text_wait->SetActionCommandWithName("ACTION");
		}
	}
	else if (value == 0)
	{
		p_text_wait->Visible(false);
		p_text_wait->ForceFinishAction();
	}
	else
	{
		if (getNumberPlayer() <= 1)
		{
            static_cast<TextWidget*>(p_text_wait)->SetText("wait_others");
			p_text_wait->Visible(true);
			p_text_wait->SetActionCommandWithName("ACTION");
		}
	}

}

void GameScreen::OnShowCountDown(float time, float cur_time)
{
    static_cast<TextWidget*>(p_panel_count_down->GetWidgetChildByName(".timer_text"))->SetText(LangMgr->GetString("start_after"));
	HUDScreen::OnShowCountDown(time, cur_time);
	OnShowTxtWait(0);
	livePlayer();
}



/**
* Implement ham nay de goi khi reset toan bo game cho van moi.
*/
void GameScreen::resetGame()
{
	livePlayer();
}

void GameScreen::doFinishGame()
{
	_isGameEnd = true;
	_isStartGame = false;

}

/**
* thuc hien ban phao bong
*
* @param userID
*/


/**
* Goi ham nay de start game.
*/
bool GameScreen::doStartGame()
{
	Utils::vibrateShort();
	doResetQuickPlay(true);
	_isGameEnd = false;
	_isStartGame = true;
	//btn_quick_play->Visible(false);
	return false;
}

bool GameScreen::doStopGame()
{
	Utils::vibrateShort();
	doResetQuickPlay(false);
	_isGameEnd = true;
	_isStartGame = false;
	if(!_isViewer) _PLAYER(_myInfo)->isQuickPlay = false;
	return false;
}

void GameScreen::onReconnect()
{
	Utils::vibrateMedium();
	GetSound->PlaySoundEffect("SFX_BUZZ");
	OnShowTxtWait(0);
	_isStartGame = true;
	_isGameEnd = false;
}

void GameScreen::showDialogSendGiftTo(int userID)
{

}



///thanh count down dem khi moi vao ban
void GameScreen::stopCountDownStart()
{
	OnHideCountDown();
}

// stop time bar phia duoi
void GameScreen::stopTimeBar()
{
	if (!p_timebar_process)
		return;

	p_timebar_process->onStop();
}
void GameScreen::startTimeBar(int interval, int remain)
{
	if (!p_timebar_process)
		return;

	p_timebar_process->setVisible(true);
	p_timebar_process->onStartime(interval,remain);

}
// nhìn chữ visibale thì biết dùng để ẩn count downtime rồi
void GameScreen::setVisibleCountDownTime(bool param1)
{

}

void GameScreen::clearSeat()
{
	for (int i = 0; i < p_max_player; i++)
	{
		seats[i] = -1;
	}
}
void GameScreen::setSeat(std::vector<Player*> l_players)
{
	if (p_list_player_group.size() > 0)
	{
		int myIndex = findMyIndex(l_players);

		size_t count = 0;

		while (count < l_players.size())
		{
			int _myseat = 0;//_myInfo ? _myInfo->GetSeat() : 0;
			int seat =  (_myseat + count) % p_max_player;
			int index = abs(myIndex - (int)count) % l_players.size();

			if (myIndex < (int)count)
				index *= -1;

			if (index < 0)
			{
				index = l_players.size() + index;
			}

			if (index < (int)l_players.size()
				&& l_players[index] != nullptr
				&& l_players[index]->IDDB != -1)
			{
				seats[seat] = index;
			}
			else
			{
				seats[seat] = -1;
			}
			count++;
		}

	}
}

void GameScreen::p_set_player_after_init(std::vector<Player*> playerInfos)
{
	//after create player finish, set the player 
	//alway remove old player
	if (p_list_player_group.size() > 0)
	{
		for (auto p : p_list_player_group)
		{
			p->removePlayer();
		}
	}
	//
	CreatePLayer(playerInfos);
	UpdateInfoPlayer(_roomID, _boardID, _ownerID, _money, playerInfos);
}

void GameScreen::setPlayers(ubyte roomID, ubyte boardID, int ownerID, s64 money, std::vector<Player*> playerInfos)
{	
	this->_roomID = roomID;
	this->_boardID = boardID;
	this->_ownerID = ownerID;
	_money = money;
	p_thread_player_infos.clear();
	p_thread_player_infos = playerInfos;

	if (p_load_xml_state > p_max_player)
	{
		p_set_player_after_init(p_thread_player_infos);
	}

}

void GameScreen::CreatePLayer(std::vector<Player*> playerInfos)
{
	//do not call to this function
	//PASSERT2(false, "child class dont not call this function, just override its !");
}

void GameScreen::UpdateInfoPlayer(ubyte roomID, ubyte boardID, int ownerID, s64 money ,std::vector<Player*> playerInfos)
{
	p_list_player = playerInfos;

	if (p_list_player_group.size() <= 0)
	{
		PASSERT2(false, "something wrong !");
		return;
	}

	if (playerInfos.size() > p_list_player_group.size())
	{
		PASSERT2(false, "size of player must be smaller or equal with size of design");
		return;
	}

	/*for (size_t i = playerInfos.size(); i < p_list_player_group.size(); i++)
	{
		_WIDGET(p_list_player_group[i])->Visible(false);
	}*/

	//
	this->_roomID = roomID;
	this->_boardID = boardID;
	this->_ownerID = ownerID;
	for (auto p : playerInfos)
	{
		if (p && GameController::myInfo->IDDB == p->IDDB)
		{
			_isViewer = false;
		}
		if (!GameController::isBaseMoney() && GameController::listPlayerRubyInGame.size()>0)
		{
			for(Player* p1 : GameController::listPlayerRubyInGame)
			{
				if (p1->IDDB == p->IDDB)
				{
					p->setMoney(RUBY_MONEY,p1->getMoney());
				}
			}
		}
	}
	clearSeat();
	setSeat(playerInfos);
	ResetReady();
	setPlayerSeat(playerInfos);
	//CheckReady();
	setOwner(ownerID);
//	setBetMoney(money);

	//if have one player show text waiting, otherwise is none
	OnShowTxtWait();
}

void GameScreen::setPlayerSeat(std::vector<Player*> l_players)
{
	if (l_players.size() > 0)
	{
		_myInfo = nullptr;
		for (size_t i = 0; i < l_players.size(); i++)
		{
			if (i >= (size_t)p_max_player)
			{
				return;
			}
			int seat = seats[i];
			PlayerGroup* pg = p_list_player_group[i];
			if (seat != -1)
			{
				if (l_players[seat] && 
					l_players[seat]->IDDB == GameController::myInfo->IDDB) // nếu là mình thì run action tiền ở khung my info
				{
					_myInfo = pg;
				}

				pg->setPlayer(l_players[seat], i);
			}
			else
			{
				pg->setPlayer(nullptr , i);
			}
		}
		_isViewer = (_myInfo == nullptr);
	}
}

int GameScreen::findMyIndex(std::vector<Player*> l_players)
{
	int idx = 0;
	for (auto p : l_players)
	{
		if (p != nullptr && 
			GameController::myInfo->IDDB == p->IDDB)
		{
			break;
		}

		idx++;

	}
	return idx;
}

void GameScreen::onSomeOneJoinBoard(int seat, Player * player)
{
	PLAY_SOUND("SFX_JOINBOARD");
	addPlayer(seat, player);
	setSeat(p_list_player);
	setPlayerSeat(p_list_player);

	
	//if (p_list_player_group.size() > 0)
	//{
	//	int myIndex = findMyIndex(l_players);

	//	size_t count = 0;

	//	while (count < l_players.size())
	//	{
	//		int _myseat = 0;//_myInfo ? _myInfo->GetSeat() : 0;
	//		int s = (_myseat + count) % p_max_player;
	//		int index = abs(myIndex - (int)count) % l_players.size();
	//		if (myIndex < (int)count)
	//			index *= -1;

	//		if (index < 0)
	//		{
	//			index = l_players.size() + index;
	//		}

	//		if (index < (int)l_players.size()
	//			&& l_players[index] != nullptr
	//			&& l_players[index]->IDDB != -1 && index == seat)
	//		{
	//			addPlayer(s, player);
	//		}
	//	
	//		count++;
	//	}

	//}

	
	
	if (_myInfo != nullptr)
	{
		setOwner(_ownerID);
	}
	//run effect money
	auto player_group = getPlayerGroup(player->IDDB);
	if (player_group && player)
	{
		player_group->SetActionMoneyRunTo(player->getMoney());
	}

	livePlayer();

	if (p_text_wait->Visible())
	{
		p_text_wait->Visible(false);
		p_text_wait->ForceFinishAction();
	}
}

void GameScreen::onSomeOneLeaveBoard(int leave, int newOwner)
{
	PLAY_SOUND("SFX_LEAVEBOARD");
	_ownerID = newOwner;
	removePlayer(leave);
	setOwner(newOwner);
	//ResetSeat();

	if (getNumberPlayer() <= 1)
	{
		if (_isViewer && (GameController::getCurrentGameType() == GAMEID_CO_UP ||
			GameController::getCurrentGameType() == GAMEID_CHESS
			))
		{
			static_cast<TextWidget*>(p_text_wait)->SetText("watching");
		}
		else
		{
			static_cast<TextWidget*>(p_text_wait)->SetText("wait_others");
		}
		p_text_wait->Visible(true);
		p_text_wait->SetActionCommandWithName("ACTION");
		OnHideCountDown();
	}


}

PlayerGroup* GameScreen::getUserGroup(int playerID)
{
	int seat = GetSeatFromPlayerID(playerID);
	return GetSeatGroup(seat);
}

int GameScreen::GetSeatFromPlayerID(int playerID)
{
	int idx = 0;
	for (auto p :  p_list_player_group)
	{
		if (p != nullptr &&
			p->getPlayer() != nullptr &&
			p->getPlayer()->IDDB > 0 &&
			p->getPlayer()->IDDB == playerID)
		{
			return idx;
		}
		idx++;
	}
	return -1;

}

PlayerGroup* GameScreen::GetSeatGroup(int seat)
{
	if (seat > -1 && seat < p_max_player)
	{
		return p_list_player_group.at(seat);
	}
	else
	{
		return nullptr;
	}
}

Player* GameScreen::getMyPlayer()
{
	for (auto p : p_list_player_group)
	{
		if (p != nullptr
			&& _PLAYER(p)
			&& _PLAYER(p)->IDDB == GameController::myInfo->IDDB)
		{
			return _PLAYER(p);
		}
	}

	return nullptr;
}


void GameScreen::OnBeginFadeIn()
{
	static_cast<TextWidget*>(p_text_gamename)->SetText(GameController::getCurrentGameName() , true);
	OnShowTxtWait(1);
}

void GameScreen::OnFadeInComplete()
{
	HUDScreen::OnFadeInComplete();
	if (!_isStartGame)
	{
		resetGame();
	}
}

void GameScreen::setOwner(int newOwner)
{
	_ownerID = newOwner;
	for( PlayerGroup* g : p_list_player_group)
	{
		if (g != nullptr)
		{
			g->setOwner(false);
		}
	}

	PlayerGroup* pg = getUserGroup(newOwner);
	if (pg != nullptr)
	{
		Player * p = getPlayer(newOwner);
		if (p != nullptr)
		{
			p->isReady = false;
		}
		pg->setOwner(true);

	}
}

Player* GameScreen::GetPlayerAtSeat(int seat)
{
	if (seat >= 0 && seat < (int)seats.size())
	{
		int s = seats[seat];
		if (p_list_player.size() > 0 && s >= 0 && s < (int)p_list_player.size() && p_list_player[s])
		{
			return p_list_player[s];
		}
	}
	return nullptr;
}

Player* GameScreen::getPlayer(int userID)
{
	for (auto p : p_list_player_group)
	{
		if (p != nullptr
			&& _PLAYER(p)
			&& _PLAYER(p)->IDDB == userID)
		{
			return _PLAYER(p);
		}
	}
	return nullptr;
}

int	GameScreen::getNumberPlayer()
{
	int number = 0;
	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB > -1)
		{
			number++;
		}
	}
	return number;
}

void GameScreen::stopAllTimePlayer()
{
	for (PlayerGroup* p : p_list_player_group)
	{
		if (p != nullptr)
		{
			p->stopTime();
		}
	}
}
bool GameScreen::IsShow()
{
	return ScrMgr->GetCurrentMenuUI() == MENU_LAYER::HUD_SCREEN;
}

void GameScreen::flyMoney(s64 money, int userID)
{
	PlayerGroup* group = getPlayerGroup(userID);
	GetFlyMoney()->fly(money, getPlayerPos(group), this);
}

Vec2 GameScreen::getPlayerPos(PlayerGroup* pg)
{
	Vec2 pos;
	Vec2 group_pos = _WIDGET(pg)->GetPosition();
	Vec2 group_size = _WIDGET(pg)->GetSize();
	bool horizontal = false;
	if (pg->GetSeat() == 0) return Vec2(group_pos.x + group_size.x / 2, group_pos.y + group_size.y / 2);
	if (pg->GetSeat() == 1)
	{
		pos = group_pos + Vec2(group_size.x, group_size.y);
	}
	else if (pg->GetSeat() == 2)
	{
		pos = group_pos + Vec2(group_size.x, .0f);
		horizontal = true;
	}
	else if (pg->GetSeat() == 3)
	{
		pos = group_pos - Vec2(group_size.x, -group_size.y / 2);
	}
	return pos;
}


void GameScreen::updateTextWithMoneyType()
{
	p_btn_purchase_ruby->SetVisible(false);
	p_btn_purchase_iwin->SetVisible(false);

	if (GameController::currentMoneyType == RUBY_MONEY)
	{
		p_btn_purchase_ruby->SetVisible(true);
		p_text_money = static_cast<TextWidget*>(p_btn_purchase_ruby->GetWidgetChildByName(".title_"));
		p_panel_game_screen->GetWidgetChildByName(".panel_detail.game_logo_win")->SetVisible(false);
		p_panel_game_screen->GetWidgetChildByName(".panel_detail.game_logo_ruby")->SetVisible(true);
	}
	else
	{
		p_btn_purchase_iwin->SetVisible(true);
		p_text_money = static_cast<TextWidget*>(p_btn_purchase_iwin->GetWidgetChildByName(".title_"));
		p_panel_game_screen->GetWidgetChildByName(".panel_detail.game_logo_win")->SetVisible(true);
		p_panel_game_screen->GetWidgetChildByName(".panel_detail.game_logo_ruby")->SetVisible(false);
	}
	//auto current_size = p_panel_game_screen->GetSize();
	//p_panel_game_screen->GetWidgetChildByName(".panel_detail")->SetPosition(current_size / 2);
}

void GameScreen::setMyMoney(s64 money)
{
	if (p_text_money)
	{
		static_cast<TextWidget*>(p_text_money)->SetText(Utils::formatNumber_dot(money));
	}
}

void GameScreen::setMyUsername(std::string name)
{
	static_cast<TextWidget*>(p_text_name)->SetText(name);
}

void GameScreen::setRubyMoney(std::vector<Player*> lsPlayer)
{
	for (Player* p : lsPlayer)
	{

		if (p->IDDB == GameController::myInfo->IDDB)
		{
			setMyMoney(p->getMoney());
		}
		else
		{
			PlayerGroup* pg = getPlayerGroup(p->IDDB);
			if (pg)
			{
				pg->setMoney(p->getMoney());
			}
		}
	}
}

void GameScreen::HandleAfterInitPlayer()
{
	//PASSERT2(false, "every class must override this function ???");
	//not handle this
	p_is_finish_load_player = true;
	for (PlayerGroup* gp: p_list_player_group)
	{
		gp->getWidget()->SetActionCommandWithName("ON_SHOW", nullptr);
	}
}

void GameScreen::start_thread_load_xml_player(RKString name)
{
	if (p_thread_load_xml_player_dec)
	{
		delete p_thread_load_xml_player_dec;
		p_thread_load_xml_player_dec = nullptr;
	}

	p_thread_load_xml_player_dec = new ThreadWrapper();
	p_game_name_xml = name;
	p_thread_load_xml_player_dec->CreateThreadWrapper("thread_parse_ui",
		([](void * data) -> uint32
	{
		GameScreen * scr = (GameScreen*)data;
		scr->p_thread_load_xml_player_dec->OnCheckUpdateThreadWrapper
		([scr]()
		{
			XMLMgr->OnLoadXMLData<GameScreen>(scr->p_game_name_xml, std::mem_fn(&GameScreen::LoadXMLListPosPlayer), scr);
			XMLMgr->OnLoadXMLData<GameScreen>("ListPlayer", std::mem_fn(&GameScreen::LoadXMLListPlayer), scr);
			scr->p_load_xml_state = 1;
		});
		return 1;
	}), (void*)this);
}

void GameScreen::parse_xml_player(int idx)
{
	CCLOG("Create Player Group %d", idx);
	if(idx < 0 || idx >= p_list_pos_dec.size())
	{
		PASSERT2(false, "have bug occur!");
		return;
	}

	InitPlayerChild(p_list_sample_dec.at("layout_user_"), p_list_pos_dec.at(idx), idx);
	if (idx == p_max_player - 1)
	{

		for (auto it : p_list_sample_dec)
		{
			delete it.second;
		}
		p_list_sample_dec.clear();

		if (p_thread_player_infos.size() > 0)
		{
			p_set_player_after_init(p_thread_player_infos);
		}
	}
}

void GameScreen::onCreateListPlayer(RKString name_game_xml)
{
	if (p_list_player_group.size() > 0)
	{
		p_list_player_group.clear();
		p_list_player_layout->ClearChild();
	}
	//XMLMgr->OnLoadXMLData<GameScreen>(name_game_xml, std::mem_fn(&GameScreen::LoadXMLListPosPlayer), this);
	//XMLMgr->OnLoadXMLData<GameScreen>("ListPlayer", std::mem_fn(&GameScreen::LoadXMLListPlayer), this);

	start_thread_load_xml_player(name_game_xml);
}

void GameScreen::LoadXMLListPosPlayer(TiXmlDocument * p_objectXML)
{
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return;
	}
	GameType current_gameplay = GameController::getCurrentGameType();

	Vec2 design_size = Vec2(0, 0);

	auto sample_game_type_dec = Root->FirstChildElement("game");

	if (sample_game_type_dec)
	{
		int id = atoi(sample_game_type_dec->Attribute("id"));
		//if (id == current_gameplay)
		{
			design_size = XMLMgr->ConvertToVector2(sample_game_type_dec->Attribute("design"));
			auto _player_xml = sample_game_type_dec->FirstChildElement("player");
			do
			{
				if (_player_xml)
				{
					int id = atoi(_player_xml->Attribute("id"));

					xml::BasicDec * dec = new xml::BasicDec();

					auto common_obj = _player_xml->FirstChildElement("common");
					do
					{
						if (common_obj)
						{
							RKString name_common_value = common_obj->Attribute("name");
							RKString common_value = common_obj->Attribute("value");

							bool res = XMLMgr->OnParseValueIntoXMLDec(name_common_value, common_value, dec, nullptr);
							

							common_obj = common_obj->NextSiblingElement();
						}
					} while (common_obj);
			
					dec->InsertDataVector2("design_size", design_size);
					p_list_pos_dec.insert(std::pair<int, xml::BasicDec* >(id, dec));
					_player_xml = _player_xml->NextSiblingElement();
				}
			} while (_player_xml);
		}
	}
}

void GameScreen::LoadXMLListPlayer(TiXmlDocument * p_objectXML)
{
	auto Root = p_objectXML->FirstChildElement("GAME");
	if (!Root)
	{
		PASSERT2(false, "XML fail to load!");
		return;
	}

	auto sample_ui_dec = Root->FirstChildElement("list_widget");
	if (!sample_ui_dec)
	{
		PASSERT2(false, "XML loader fail to load!");
		return;
	}

	auto sample_ui_dec_xml = sample_ui_dec->FirstChildElement("Widget");
	do
	{
		if (sample_ui_dec_xml)
		{
			auto _widget_root = XMLMgr->OnParseWidgetEntity1(sample_ui_dec_xml, nullptr, XMLMgr->GetUIWidgetByName(name_ui), nullptr);
			p_list_sample_dec.insert(std::pair<RKString, xml::UILayerWidgetDec*>(_widget_root->NameWidget, _widget_root));
			sample_ui_dec_xml = sample_ui_dec_xml->NextSiblingElement();
		}
	} while (sample_ui_dec_xml);

}

void GameScreen::InitPlayerChild(xml::UILayerWidgetDec* xml_dec, xml::BasicDec * dec, int idx)
{
	auto menu_widget_xml = XMLMgr->GetUIWidgetByName(name_ui);
	xml_dec->p_menu_widget = menu_widget_xml;
	xml_dec->p_parent_layer = menu_widget_xml->GetElementChildByName("HUD_screen")->GetChildWidgetByName("layout_list_user");
	auto layer_root = GetLayerChildByName("HUD_screen");

	Vec2 position = dec->GetDataVector2("value_pos");

	Vec2 pos_user = dec->GetDataVector2("pos_tex1");
	Vec2 pos_iwin = dec->GetDataVector2("pos_tex2");

	Vec2 pos_chat = dec->GetDataVector2("pos_chat");
	Vec2 pos_text_bonus = dec->GetDataVector2("pos_text_bonus");

	Vec2 design_size = dec->GetDataVector2("design_size");
	Vec2 ratio_pos = Vec2(position.x / design_size.x, position.y / design_size.y);
	Vec2 ratio_pos_user = Vec2(pos_user.x / design_size.x, pos_user.y / design_size.y);
	Vec2 ratio_pos_chat = Vec2(pos_chat.x / design_size.x, pos_chat.y / design_size.y);
	Vec2 ratio_text_bonus  = Vec2(
		pos_text_bonus.x * GetGameSize().width / design_size.x, 
		pos_text_bonus.y * GetGameSize().height / design_size.y);

	Vec2 anchor_point = dec->GetDataVector2("anchor_point");

	xml_dec->GeneralValue->ReplaceDataVector2("anchor_point", anchor_point);
	xml_dec->OriginValue->ReplaceDataVector2("pos_ratio", ratio_pos);

	auto layout_player = xml_dec->GetChildWidgetByName("layout_player");

	auto user_name_panel = layout_player->GetChildWidgetByName("layout_info_player");
	user_name_panel->GeneralValue->ReplaceDataVector2("anchor_point", Vec2(anchor_point.x, 0.5));
	user_name_panel->OriginValue->ReplaceDataVector2("pos_ratio", ratio_pos_user);
	if (anchor_point.x == 1)
	{
		Vec2 ratio_pos_name = Vec2(125 / design_size.x, 25/ design_size.y);
		auto name = user_name_panel->GetChildWidgetByName("user_name_txt");
		name->GeneralValue->ReplaceDataVector2("anchor_point", Vec2(anchor_point.x, 0));
		name->OriginValue->ReplaceDataVector2("pos_ratio", ratio_pos_name);

		Vec2 ratio_pos_money = Vec2(120 / design_size.x, 20 / design_size.y);
		auto money = user_name_panel->GetChildWidgetByName("number_iwin_txt");
		money->GeneralValue->ReplaceDataVector2("anchor_point", Vec2(anchor_point.x, 1));
		money->OriginValue->ReplaceDataVector2("pos_ratio", ratio_pos_money);

		Vec2 ratio_pos_bg = Vec2(160 / design_size.x, 0 / design_size.y);
		auto bg = user_name_panel->GetChildWidgetByName("bg");
		bg->GeneralValue->ReplaceDataVector2("anchor_point", Vec2(anchor_point.x, 0));
		bg->OriginValue->ReplaceDataVector2("pos_ratio", ratio_pos_bg);
	}

	WidgetEntity * w_player_group_com = nullptr;
	auto p_player_group_com_refer = GetWidgetChildByName("HUD_screen.layout_list_user_com")->GetWidgetChildAtIdx(0);
	if (idx > 0)
	{
		auto p_layout_player_group_com = GetWidgetChildByName("HUD_screen.layout_list_user_com");
		w_player_group_com = p_player_group_com_refer->Clone();
		p_layout_player_group_com->InsertChildWidget(w_player_group_com);
	}
	else
	{
		w_player_group_com = p_player_group_com_refer;
	}

	if (w_player_group_com)
	{
		w_player_group_com->SetAnchorPoint(anchor_point);
		w_player_group_com->SetPosition(Vec2(ratio_pos.x * GetGameSize().width, ratio_pos.y * GetGameSize().height));
		auto display_chat = w_player_group_com->GetWidgetChildByName(".panel_display_chat");
		display_chat->SetAnchorPoint(Vec2(ratio_pos.x , 0.5));
		display_chat->SetPosition(Vec2(ratio_pos_chat.x * GetGameSize().width , ratio_pos_chat.y * GetGameSize().height));
	}

	auto widget = layer_root->InitWidgetEntity(xml_dec, layer_root, p_list_player_layout);
	p_list_player_layout->ResizeAndPosChildWidget(widget);

	widget->SetName("layout_user_" + std::to_string(idx));

	widget->GetWidgetChildByName(".layout_player.btn_info_user_")->SetName("btn_info_user_" + std::to_string(idx));

	auto layout_control = widget->GetWidgetChildByName(".layout_player.layout_player_controller");
	if (layout_control && dec->HaveDataInt("rotate_control"))
	{
		int rotate = dec->GetDataInt("rotate_control");
		layout_control->SetRotate(rotate);
		for (size_t i = 0; i < layout_control->GetNumberChildWidget(); i++)
		{
			layout_control->GetWidgetChildAtIdx(i)->SetRotate(-rotate);
		}
	}

	widget->SetMenuParent(p_list_player_layout->GetMenuParent());
	widget->SetParentWidget(p_list_player_layout);
	widget->SetParentLayer(p_list_player_layout->GetParentLayer());

	//set for owner
	if (idx == 0)
	{
		widget->GetWidgetChildByName(".btn_add_user_")->Visible(false);
		widget->GetWidgetChildByName(".layout_player.layout_info_player.user_name_txt")->Visible(false);
		widget->GetWidgetChildByName(".layout_player.layout_info_player.number_iwin_txt")->Visible(false);
	}

	auto group_player = new PlayerGroup(widget);

	group_player->SetPosBonus(ratio_text_bonus);
	group_player->SetCurrentGameScreen(this);
	group_player->SetXMLData(xml_dec->Clone(), 0);
	OnLoadDecPrivatePlayer(dec, group_player);
	p_list_player_group.push_back(group_player);
	p_list_player_layout->InsertChildWidget(widget);
	
}

int GameScreen::GetRoomID()
{
	return _roomID;
}

int GameScreen::GetBoardID()
{
	return _boardID;
}

bool GameScreen::IsMyInfoRoomOwner()
{
	if (_myInfo)
	{
		return _myInfo->IsOwner();
	}
	return false;
}

int GameScreen::GetOwnerID()
{
	return _ownerID;
}


void GameScreen::ActionTouchBeginObject(Utility::wObject::wBasicObject * obj)
{
	//not handle
}
void GameScreen::ActionTouchMoveObject(Utility::wObject::wBasicObject * obj)
{
	//not handle
}
void GameScreen::ActionTouchEndObject(Utility::wObject::wBasicObject * obj)
{
	//not handle
}

void GameScreen::onBuzz(int fromId, std::string message)
{
	auto p = getPlayerGroup(fromId);
	if (p && p->getPlayer())
	{
		p->SetTextChat(message);
	}
	Utils::vibrateShort();
}

void GameScreen::onChat(int fromId, std::string message)
{
	getPlayerGroup(fromId)->SetTextChat(message);
}

size_t GameScreen::GetAvailableNumberPlayers()
{
	size_t num = 0;

	for (auto p : p_list_player_group)
	{
		if (p 
			&& _PLAYER(p)
			&& _PLAYER(p)->IDDB > -1 
			&& _PLAYER(p)->name != "")
		{
			num++;
		}
	}
	return num;
}


void GameScreen::SetMaxPlayer(int max_player)
{
	p_max_player = max_player;
	for (int i = 0; i < p_max_player; i++)
	{
		seats.push_back(-1);
	}
}

void GameScreen::OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player)
{
	//not handle this function
}

void GameScreen::OnHandleMsg(iwincore::Message* msg)
{
	//not handle this function
	if (GameLogicHandler::gameHandlers)
	{
		CCLOG("Handle late message");
		//GameLogicHandler::gameHandlers->onMessage(msg);
		GetGameMessageHandler()->onMessage(msg);
	}
	else
	{
		PASSERT2(false, "");
	}
}

int GameScreen::PushReturnMsg(iwincore::Message* msg)
{
	if (!p_is_finish_load_player)
	{
		p_list_msg_return.push_back(new iwincore::Message(msg));
		return 1;
	}

	return 0;
}



void GameScreen::onBetRange(int min, s64  max, s64 maxMoneyRoom)
{
	s64 min_max_money = maxMoneyRoom;

	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB > -1)
		{
			s64 money = _PLAYER(p)->money;

			if (money < min_max_money)
			{
				min_max_money = money;
			}
		}
	}

	max = min_max_money;

	setBetMoney(min , maxMoneyRoom);
}

void GameScreen::doSetMoney(int money)
{
	GameService::setMoney(_roomID, _boardID, money);
}

void GameScreen::ResetReady()
{
	for (auto p : p_list_player_group) 
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB != -1 && _PLAYER(p)->IDDB != _ownerID) 
		{
			_PLAYER(p)->isReady = false;
			p->setReady(false);
		}
	}

	if (_myInfo &&  _PLAYER(_myInfo) && _PLAYER(_myInfo)->IDDB != _ownerID)
	{
		_PLAYER(_myInfo)->isReady = false;
		_myInfo->setReady(false);
	}
}

void GameScreen::PushChatToServer(RKString str)
{
	GameService::chatToBoard(_roomID, _boardID, str.GetString());
}

void GameScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_quick_play")
	{
		GameService::quickPlay(_roomID, _boardID);
	}
	if (type_widget == UI_TYPE::UI_BUTTON && name == "quit_ui_btn")
	{	
		p_panel_igm->SetActionCommandWithName("ON_HIDE", CallFunc::create(
			[this]()
		{
			p_panel_igm->ForceFinishAction();
			this->DoBack();
		}));
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_ok")
	{
		hideResult();
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_close_gift")
	{
		p_panel_send_gift->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
			this->p_panel_send_gift->ForceFinishAction();
			this->p_panel_send_gift->GetParentWidget()->Visible(false);
		}));
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_gift_one")
	{
		p_panel_send_gift->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
			this->p_panel_send_gift->ForceFinishAction();
			this->p_panel_send_gift->GetParentWidget()->Visible(false);
			this->PushEvent([](BaseScreen* scr) {
				GameScreen * game_scr = (GameScreen *)scr;
				std::vector<int> list_player;
				for (auto p : game_scr->p_list_player_group)
				{
					if (p && _PLAYER(p) && _PLAYER(p)->IDDB >= 0 && _PLAYER(p)->IDDB == game_scr->p_current_iddb_player_gift)
					{
						list_player.push_back(game_scr->p_current_iddb_player_gift);
					}
				}
				if (game_scr->p_current_id_items_select >= 0 && list_player.size() > 0)
				{
					GameService::giveItem(game_scr->GetRoomID(), game_scr->GetBoardID(), game_scr->p_current_id_items_select, list_player);
				}
				game_scr->p_current_iddb_player_gift = -1;
			});
		}));
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_gift_all")
	{
		p_panel_send_gift->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
			this->p_panel_send_gift->ForceFinishAction();
			this->p_panel_send_gift->GetParentWidget()->Visible(false);
			this->PushEvent([](BaseScreen* scr) {
				GameScreen * game_scr = (GameScreen *)scr;
				std::vector<int> list_player;
				for (auto p : game_scr->p_list_player_group)
				{
					if (p && _PLAYER(p) && _PLAYER(p)->IDDB >= 0)
					{
						list_player.push_back(_PLAYER(p)->IDDB);
					}
				}
				if (game_scr->p_current_id_items_select >= 0 && list_player.size() > 0)
				{
					GameService::giveItem(game_scr->GetRoomID(), game_scr->GetBoardID(), game_scr->p_current_id_items_select, list_player);
				}
				game_scr->p_current_iddb_player_gift = -1;
			});
		}));
	}

	else if (name.Contains("btn_select_gift"))
	{
		auto idx_str = name.SplitLast("_").GetAt(1);
		int idx_to = atoi(idx_str.GetString());
		ShowListGiftAtIdx(idx_to);
	}
	else
	{
		HUDScreen::OnProcessWidgetChild(name, type_widget, _widget);
	}
}

void GameScreen::OnChatInBoard(int fromID, RKString message)
{
	auto p = getPlayerGroup(fromID);
	if (p)
	{
		p->SetTextChat(message);
	}
}

void GameScreen::OnSystemChat(RKString message)
{
	if (_myInfo)
	{
		message = "[SYSTEM] : " + message;
		_myInfo->SetTextChat(message);
	}
}

void GameScreen::setResultInfo(int slot, int iddb, std::string name, int avatar, int money, int cur_exp, int extra_exp)
{
	WidgetEntity* user_widget = p_panel_result->GetWidgetChildByName(StringUtils::format(".result_panel.layout_user%d", slot).c_str());
	std::string win = (money > 0) ? LangMgr->GetString("won").GetString() : LangMgr->GetString("lose").GetString();
	if (money > 0)
	{
		win = LangMgr->GetString("won").GetString();
	}
	else if (money < 0)
	{
		win = LangMgr->GetString("lose").GetString();
	}
	else
	{
		win = LangMgr->GetString("tie").GetString();
	}

	static_cast<TextWidget*>(user_widget->GetWidgetChildByName(".username"))->SetText(StringUtils::format("%s: %s", name.c_str(), win.c_str()));
	static_cast<TextWidget*>(user_widget->GetWidgetChildByName(".money"))->SetText(StringUtils::format("win: %d %s", money, GameController::getMoneyType().c_str()));
	static_cast<TextWidget*>(user_widget->GetWidgetChildByName(".level"))->SetText(StringUtils::format("%s: %d(%d)", LangMgr->GetString("leve").GetString(), cur_exp, extra_exp));
	user_widget->GetWidgetChildByName(".ico_cup")->Visible(money > 0);
	if (p_list_avatar_download.find(iddb) != p_list_avatar_download.end())
	{
		auto panel_avatar = user_widget->GetWidgetChildByName(".panel_avatar");
		this->ParseSpriteToPanel(panel_avatar, p_list_avatar_download.at(iddb), 100);
	}
	else
	{
		GameMgr->HandleAvatarDownload(iddb, name,
			[this, iddb, user_widget](void * data, void * str, int tag)
		{
			Image * img = (Image *)data;
			Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
			this->p_list_avatar_download.insert(iddb, sprite_img);

			auto panel_avatar = user_widget->GetWidgetChildByName(".panel_avatar");
			this->ParseSpriteToPanel(panel_avatar, sprite_img, 100);
		});
	}
}

void GameScreen::OnMatchResult(std::vector<GameResult*> result)
{
	for(int i = 0; i < result.size(); i++)
	{
		GameResult* m = result.at(i);
		PlayerGroup * g = getPlayerGroup(m->ID);

		setResultInfo(i, m->ID, m->name, m->avatar, m->d_money, m->d_sumexp, m->d_addexp);
		if (g && _PLAYER(g))
		{
			//m.name = Util.computeVisibleTextAndTruncated(GameController.game.commonAsset.defaultFont, p.name,
			//	getWidth() / 4);

			m->avatar = _PLAYER(g)->avatar;
			// m.d_exp_percent = (m.d_exp * 100) / p.level.expRequire;
			// Update new data for wait screen
			int newExp = _PLAYER(g)->getExp() + m->d_exp;

			if (newExp < 0) 
			{
				newExp = 0;
			}

			_PLAYER(g)->setExp(newExp);
			_PLAYER(g)->addMoney(m->d_money);

			if (m->ID == GameController::myInfo->IDDB) 
			{
				GameController::myInfo->addMoney(m->d_money);
				//flyMoney(m->d_money , m->ID);
				GameController::myInfo->setExp(newExp);
				if (GameController::getCurrentGameType() == GameType::GAMEID_CHESS
					|| GameController::getCurrentGameType() == GameType::GAMEID_CO_UP
					|| GameController::getCurrentGameType() == GameType::GAMEID_GIAI_DAU_CO_UP
					|| GameController::getCurrentGameType() == GameType::GAMEID_COVUA) {
					switch (m->result) 
					{
					case -1:
						doSomethingWhenILose();
						PLAY_SOUND("SFX_GAMECO_LOSE");
						//shareCaption = L.gL(843);
						break;
					case 0:
						doSomethingWhenIRaw();
						PLAY_SOUND("SFX_GAMECO_DRAW");
						//shareCaption = L.gL(844);
						break;
					case 1:
						doSomethingWhenIWin();
						PLAY_SOUND("SFX_GAMECO_WIN");
						//shareCaption = L.gL(842);
						break;
					}
				}
				else if (
					GameController::getCurrentGameType() == GameType::GAMEID_CARO
					|| GameController::getCurrentGameType() == GameType::GAMEID_TETRIS) 
				{
					switch (m->result) 
					{
					case -1:
						doSomethingWhenILose();
						PLAY_SOUND("SFX_GAMEBAI_THUABAI");
						//shareCaption = L.gL(846);
						break;
					case 0:
						doSomethingWhenIRaw();
						PLAY_SOUND("SFX_VOTAY");
						//shareCaption = L.gL(847);
						break;
					case 1:
						doSomethingWhenIWin();
						PLAY_SOUND("SFX_GAMEBAI_THANGBAI");
						//shareCaption = L.gL(845);
						break;
					}

				}
			}
		}
	}

	showResult();

	//delete after use

}

void GameScreen::OnReceivedWaitTimeInfo(int sec)
{
	PASSERT2(false, "handle action here!!!!");
}

/**
* thuc hien ban phao bong
*
* @param userID
*/
void GameScreen::doFireWork(int userID) 
{
	PlayerGroup * g = getPlayerGroup(userID);
	if (g != nullptr && g->getPlayer() != nullptr) 
	{
		auto position = _WIDGET(g)->GetPosition_Middle();
		PASSERT2(false, "not handle effect firework");
		//fireWorkGroup.toFront();
		//fireWorkGroup.fire();
	}
}

/**
* lam mot so hieu ung, am thanh gi do khi minh thang , thuc hien cac hieu
* ung chung cho tat ca game
*/
void GameScreen::doSomethingWhenIWin() 
{
}

/**
* lam mot so hieu ung, am thanh gi do khi minh thua, thuc hien cac hieu ung
* chung cho tat ca game
*/
void GameScreen::doSomethingWhenILose() 
{
}

/**
* lam mot so hieu ung, am thanh gi do khi minh hoa, thuc hien cac hieu ung
* chung cho tat ca game
*/
void GameScreen::doSomethingWhenIRaw() 
{
}

void GameScreen::showResult()
{
	p_panel_result->SetVisible(true);
	p_panel_result->SetActionCommandWithName("ON_SHOW");
	
}

void GameScreen::hideResult()
{
	if (p_panel_result->IsVisible()) {
		p_panel_result->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]()
		{
			p_panel_result->SetVisible(false);
		}));
	}
}

std::chrono::steady_clock::time_point	GameScreen::GetTimeNow()
{
	return std::chrono::high_resolution_clock::now();
}

u64 GameScreen::GetDeltaTimeHold(std::chrono::steady_clock::time_point start_time)
{
	auto time_now = GetTimeNow();
	return std::chrono::duration_cast<std::chrono::microseconds>(time_now - start_time).count();
}

bool GameScreen::IsHoldOverTime(float time)
{
	auto delta_time = GetDeltaTimeHold(p_start_time_hold_card);
	float second = (float)delta_time / (1000.f*1000.f);
	if (second < time)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void GameScreen::MarkTimeHere(bool reset_mark_time)
{
	static bool already_mark = false;
	if (!already_mark)
	{
		p_start_time_hold_card = GetTimeNow();
		already_mark = true;
	}
	if (reset_mark_time)
	{
		already_mark = false;
	}
}

bool GameScreen::InListSelected(int id)
{
	for (auto i : p_list_id_card_on_hold)
	{
		if (i == id)
		{
			return true;
		}
	}
	return false;
}

void GameScreen::RemoveCardOnSelected()
{
	removeCardHold();
}

void GameScreen::addCardHold(Card * child, int z_order)
{
	p_list_card_on_selected.push_back(child);
	p_panel_card_hold->addChild(NODE(child), z_order);
	p_panel_card_hold->setVisible(true);
}
void GameScreen::removeCardHold()
{
	for (Card *c : p_list_card_on_selected)
	{
		NODE(c)->stopAllActions();
		p_panel_card_hold->removeChild(NODE(c), true);
		delete c;
	}
	p_list_card_on_selected.clear();
	if (p_panel_card_hold)
	{
		p_panel_card_hold->setVisible(false);
		p_panel_card_hold->setPosition(Vec2(0, 0));
	}
}

void GameScreen::OnShowEffectHL(int start_card_idx, int end_card_idx, bool isShow)
{
	//do nothing
}


bool GameScreen::isViewer()
{
	return _isViewer;
}

void GameScreen::onLanguageChanged(LANGUAGE_TYPE lang)
{
	Screen::onLanguageChanged(lang);

	static_cast<TextWidget*>(p_text_gamename)->SetText(GameController::getCurrentGameName(), true);
}
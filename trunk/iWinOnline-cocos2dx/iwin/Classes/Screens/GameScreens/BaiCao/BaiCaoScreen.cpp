

#include "UI/AdvanceUI/wSlider.h"
#include "UI/AdvanceUI/wTextField.h"
#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wText.h"

#include "Screens/Object/PlayerGroup.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/SliderBet.h"

#include "Screens/GamePlayMgr.h"

#include "Network/Game/GameService.h"
#include "Network/IwinProtocol.h"
#include "Network/Global/Globalservice.h"

#include "BaiCaoScreen.h"
#include "Utils.h"
#include "AnimUtils.h"
#include "BaiCaoPlayer.h"
#include "FlopCardUI.h"


void BaiCaoScreen::DealCardSeat(int s)
{
	BaiCaoPlayer* p = static_cast<BaiCaoPlayer*>(GetPlayerAtSeat(s));
	if (!p)
		return;
	p->CreateCard(ls_players_card[s]->GetWidgetChildByName(".contain_card") , Vec2(GetGameSize().width / 2, GetGameSize().height / 2));
	p->DealCard();
	ls_players_card[s]->Visible(true);

}

void BaiCaoScreen::ShowButtonSetBet(bool param1)
{
	btn_bet_money->Visible(param1);
}

void BaiCaoScreen::flipCard(int seat, vector<char> ids)
{

}

BaiCaoScreen* BaiCaoScreen::getInstance()
{
	return static_cast<BaiCaoScreen*>(GamePlay->GetCurrentGameScreen());
}

BaiCaoScreen::BaiCaoScreen()
{
	SetGameID(GameType::GAMEID_BAICAO);
	p_panel_slider = nullptr;
	SetMaxPlayer(6);
	ls_cards.resize(16);
	_list_pos_layer_card.clear();
	_list_card_flip.clear();
}

BaiCaoScreen::~BaiCaoScreen()
{
	//p_panel_slider->ClearChild();
	p_panel_slider = nullptr;
	ls_players_card.clear();

	for (auto p : p_list_player_group)
	{
		if (p && p->getPlayer())
		{
			auto bc_player = static_cast<BaiCaoPlayer*>(p->getPlayer());
			bc_player->ClearCard();
		}
	}

	_list_pos_layer_card.clear();

	if (p_slider_bet)
	{
		delete p_slider_bet;
		p_slider_bet = nullptr;
	}
	_list_card_flip.clear();
}


bool BaiCaoScreen::InitGame(int game_id)
{
	bool res = GameScreen::InitGame(game_id);
	
	XMLMgr->OnLoadXMLData<BaiCaoScreen>("BaiCao_Screen", std::mem_fn(&BaiCaoScreen::LoadXMLGameScreenCustom), this);
	onCreateListPlayer("BaiCao_Screen");

	p_panel_bottom = p_panel_custom->GetWidgetChildByName(".back_panel_card");
	PASSERT2(p_panel_bottom != nullptr, "p_panel_bottom is null , recheck the path");

	flopCard = new FlopCardUI();
	flopCard->init();
	flopCard->setVisible(false);
	addChild(flopCard , 100);
	

	btn_bet_money = static_cast<ButtonWidget*>( p_panel_bottom->GetWidgetChildByName(".layout_btn_game_baicao.btn_bet_money"));
	btn_flop_card = static_cast<ButtonWidget*>(p_panel_bottom->GetWidgetChildByName(".layout_btn_game_baicao.btn_rub_card"));
	btn_quick_play->SetPosition(Vec2(btn_quick_play->GetPosition().x - btn_bet_money->getWidth(), btn_quick_play->GetPosition().y));

	_isReceiveMyScore = false;
	_myResult = 0;
	return true;
}

void BaiCaoScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_bet_money")
	{
		if (!p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->Visible())
		{
			OnShowBetSlider(true);
		}
		else if (!_isStartGame)// dat cuoc
		{
			OnShowBetSlider(false);
			s64 money = p_slider_bet->getCurrentBet();
			if (money > GameController::myInfo->getMoney())
			{
				money = GameController::myInfo->getMoney();
			}
			doSetMoney(money);
		}

	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_rub_card")
	{
	
		Player *p = getPlayer(GameController::myInfo->IDDB);
		if (p)
		{
			flopCard->setVisible(true);
			std::vector<unsigned char> ids;
			int i = 0;
			for (Card* c :((BaiCaoPlayer*)p)->cards)
			{
				if (c->getID() == -1)
				{
					ids.push_back(((BaiCaoPlayer*)p)->cardValue[i]);
				}
				i++;
			}
			if (ids.size() > 0)
			{
				flopCard->setValueCards(ids);
				flopCard->SetAction(
					([this, p](bool value)
				{
					if (value)
					{
						std::vector<char> ids;
						int i = 0;
						bool havecard = false;
						for (Card* c : ((BaiCaoPlayer*)p)->cards)
						{
							if (c->getID() == -1 && !havecard)
							{
								havecard = true;
								ids.push_back(((BaiCaoPlayer*)p)->cardValue[i]);
							}
							else
							{
								ids.push_back(c->getID());
							}
							i++;
						}
						GlobalService::latBaiBaiCao(_roomID, _boardID, ids);
					}
					else
					{
						this->flopCard->setVisible(false);
						this->btn_flop_card->Visible(false);
						this->_list_card_flip.clear();
					}
				}));
			}
		}
		
	}
	else if (type_widget == UI_TYPE::UI_SLIDER && name.Contains("slider_bet_money") )
	{
		if (p_slider_bet)
		{
			p_slider_bet->onChangeValue(name == "slider_bet_money_PERCENTAGE_CHANGED" ? true : false);
		}

		//string text = "Cược (" +Utils::formatNumber(p_slider_bet->getValue())+")";
		//static_cast<ButtonWidget*>(btn_bet_money)->SetTitle(text);
		
	}
	else if (type_widget == UI_TYPE::UI_BUTTON  && name.Contains("btn_bet_rand_value_min"))
	{
		if (p_slider_bet)
		{
			p_slider_bet->setValue(p_slider_bet->getMinValue());
		}
	}
	else if (type_widget == UI_TYPE::UI_BUTTON  && name.Contains("btn_bet_rand_value_14"))
	{
		if (p_slider_bet)
		{
			p_slider_bet->setDeltaPercent(25);
		}
	}
	else if (type_widget == UI_TYPE::UI_BUTTON  && name.Contains("btn_bet_rand_value_12"))
	{
		if (p_slider_bet)
		{
			p_slider_bet->setDeltaPercent(50);
		}
	}
	else if (type_widget == UI_TYPE::UI_BUTTON  && name.Contains("btn_bet_rand_value_34"))
	{
		if (p_slider_bet)
		{
			p_slider_bet->setDeltaPercent(75);
		}
	}
	else if (type_widget == UI_TYPE::UI_BUTTON  && name.Contains("btn_bet_rand_value_max"))
	{
		if (p_slider_bet)
		{
			p_slider_bet->setValue(p_slider_bet->getMaxValue());
		}
	}
	else
	{
		GameScreen::OnProcessWidgetChild(name, type_widget, _widget);
	}
}

int BaiCaoScreen::Update(float dt)
{
	if (GameScreen::Update(dt) == 0)
	{
		return 0;
	}

	//update action all cards;
	for (auto p : p_list_player_group)
	{
		if (p && p->getPlayer())
		{
			auto bc_player = static_cast<BaiCaoPlayer*>(p->getPlayer());
			for (auto c : bc_player->cards)
			{
				c->Update(dt);
			}
		}
	}
	
	if (flopCard)
	{
		flopCard->update(dt);
	}

	return 1;
}

void BaiCaoScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	auto p_list_sample_dec = LoadWidgetCustom(p_objectXML);
	PASSERT2(p_list_sample_dec.size() > 0, "no widget loaded!");
	//insert into custom menu

	p_panel_slider = GetWidgetChildByName("HUD_screen.panel_custom_slider_bet");

	ParseIntoCustom(p_list_sample_dec.at("back_panel_card"), p_panel_custom, "panel_custom_menu");
	auto w_layout_card = GetWidgetChildByName("HUD_screen.panel_game_card");
	ParseIntoCustom(p_list_sample_dec.at("layout_card"), w_layout_card, "panel_game_card");
	
	//ParseIntoCustom(p_list_sample_dec.at("progress_bar_panel"), p_panel_custom, "panel_custom_menu");

	if (!p_panel_custom)
	{
		PASSERT2(p_panel_custom != nullptr, "panel custom not set!");
		return;
	}
	auto widget_layout_card = w_layout_card->GetWidgetChildByName(".layout_card");
	
	widget_layout_card->GetWidgetChildByName(".contain_card")->SetActionCommandWithName("SCALE_NORMAL");
	widget_layout_card->SetZOrder(1);
	ls_players_card.push_back(widget_layout_card);
	createDynamicCardLayout(widget_layout_card, 5 , [&](Utility::UI_Widget::WidgetEntity * _widget)
	{
	});
	ParseIntoCustom(p_list_sample_dec.at("sp_slider_bet"), p_panel_slider, "panel_custom_slider_bet");


	ls_players_bet_info.clear();
	for (int i = 0; i <= 3; i++)
	{
		ParseIntoCustom(p_list_sample_dec.at("lb_bet_win_"+std::to_string(i)), w_layout_card, "panel_game_card");
		auto widget_bet_money = w_layout_card->GetWidgetChildByName(".lb_bet_win_" + std::to_string(i));
		if (GameController::currentMoneyType == RUBY_MONEY)
		{
			widget_bet_money->GetWidgetChildByName(".icon_bet.icon_win")->Visible(false);
			widget_bet_money->GetWidgetChildByName(".icon_bet.icon_ruby")->Visible(true);
		}
		else
		{
			widget_bet_money->GetWidgetChildByName(".icon_bet.icon_win")->Visible(true);
			widget_bet_money->GetWidgetChildByName(".icon_bet.icon_ruby")->Visible(false);
		}

		ls_players_bet_info.push_back(widget_bet_money);
	}

	for (auto it = p_list_sample_dec.begin(); it != p_list_sample_dec.end(); it++)
	{
		delete it->second;
	}
	p_list_sample_dec.clear();

	p_slider_bet = new SliderBet(p_panel_slider->GetWidgetChildByName(".sp_slider_bet"));

}

void BaiCaoScreen::createDynamicCardLayout(WidgetEntity* layerCard,int numClone, const std::function<void(Utility::UI_Widget::WidgetEntity*)> & func)
{
	if (!layerCard)
	{
		PASSERT2(layerCard != nullptr, "layout card not set!");
		return;
	}
	auto w_layout_card = GetWidgetChildByName("HUD_screen.panel_game_card");
	for (int idx = 1; idx <= numClone; idx++)
	{
		auto _widget = layerCard->Clone();
		_widget->SetName("layout_card_" + std::to_string(idx));
		_widget->SetParentWidget(w_layout_card);
		_widget->SetParentLayer(w_layout_card->GetParentLayer());
		_widget->SetMenuParent(w_layout_card->GetMenuParent());
		//
		w_layout_card->InsertChildWidget(_widget);
		ls_players_card.push_back(_widget);
		_widget->GetWidgetChildByName(".contain_card")->ForceFinishAction();
		_widget->GetWidgetChildByName(".contain_card")->SetActionCommandWithName("SCALE_SMALL");
	}

}

void BaiCaoScreen::layoutGame()
{
	if (GameController::gameId == GameType::GAMEID_BAICAO)
	{
		ls_players_card[4]->Visible(false);
		ls_players_card[5]->Visible(false);
		p_list_player_group[4]->setVisible(false);
		p_list_player_group[5]->setVisible(false);
		//set pos layercard
		for (size_t i = 0; i < 4; i++)
		{
			auto _w = _WIDGET(p_list_player_group[i]);
			auto pos = NODE(_w)->convertToWorldSpace(_list_pos_layer_card[i]);
			ls_players_card[i]->SetPosition(pos);
		} 
		//set pos label money 
	}
	else
	{

	}
	
}

void BaiCaoScreen::OnTouchMenuBegin(const cocos2d::Point & p)
{
	HUDScreen::OnTouchMenuBegin(p);
	if (p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->Visible())
	{
		CheckTouchOutSideWidget(p, p_panel_slider->GetWidgetChildByName(".sp_slider_bet"),
			[&](void)
		{
			p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->SetActionCommandWithName("ON_HIDE");
		});
	}
}

void BaiCaoScreen::OnBetMoney(int money, int id)
{
	GetSound->PlaySoundEffect("SFX_BETMONEY");
	int seat = GetSeatFromPlayerID(id);
	if (seat == -1 || ls_players_bet_info.size() <= 0)
		return;
	if (id == _ownerID)
	{
		static_cast<TextWidget*>(ls_players_bet_info[seat])->Visible(false);
		return;
	}

	auto bet_seat = ls_players_bet_info[seat];
	auto bet_txt_seat = bet_seat->GetWidgetChildByName(".icon_bet.lb_win_value");
	static_cast<TextWidget*>(bet_txt_seat)->TextRunEffect(money, 0, 0.3f, Utils::formatNumber_dot, CallFunc::create([bet_txt_seat , bet_seat, this]() {
		auto size_text = static_cast<TextWidget*>(bet_txt_seat)->GetActualContainSize();
		Vec2 size = Vec2(size_text.x + (60 * GetGameSize().width / this->GetDesignSize().x), bet_seat->GetSize().y);
		bet_seat->SetSize(size);
		bet_seat->GetWidgetChildByName(".icon_bet.icon_win")->SetSize(size);
		bet_seat->GetWidgetChildByName(".icon_bet.icon_ruby")->SetSize(size);
		bet_seat->SetPosition(bet_seat->GetPosition());

	}));
	static_cast<TextWidget*>(ls_players_bet_info[seat])->Visible(true);
}

void BaiCaoScreen::ChangeBetMoney(int money, ubyte roomID, ubyte boardID)
{
	
}

void BaiCaoScreen::OnStopGame(ubyte numPlayer,vector<vector<int>> cards)
{
	_list_card_flip.clear();
	doStopGame();

	_isViewer = false;
	/*if (dealCards == null)
	{
		CreateListCardUp();
	}*/
	stopTimeBar();
	for (int i = 0; i < numPlayer; i++)
	{
		int seat = GetSeatFromPlayerID(cards[i][0]);
		if (seat == -1) continue;
		vector<char> ids(3);

		for (int j = 1; j < 4; j++)
		{
			ids[j - 1] = cards[i][j];
		}
//		flipCard(seat, ids);
		BaiCaoPlayer* p =(BaiCaoPlayer*) getPlayer(cards[i][0]);
		p->FlipCard(ids);
	}

	needReset = true;
	/*StopAllTime();
	ShowStartButton();*/
	//ShowButtonRubCard(false);
	//HideRubCard();

	//Game bài cào thì hiển thị nút sẵng sàng luôn
	//SetReadyButton(false);
	//ShowBetButton();
	ShowButtonAction();

	flopCard->setVisible(false);
	GlobalService::getBetRange();
	ResetReady();

	if (_myResult > 0)
	{
		doSomethingWhenIWin();
	}
	else if (_myResult < 0)
	{
		doSomethingWhenILose();
	}
}

void BaiCaoScreen::ShowButtonAction()
{
	if (_isStartGame)
	{
		btn_flop_card->Visible(true);
		btn_quick_play->Visible(false);
		btn_bet_money->Visible(false);
	}
	else
	{
		btn_flop_card->Visible(false);
		int currentNumber_player = this->getNumberPlayer();
		if (currentNumber_player > 1)
		{
			btn_quick_play->Visible(true);
		}
		else
		{
			btn_quick_play->Visible(false);
		}
		if (_ownerID != GameController::myInfo->IDDB) 
		{
			btn_bet_money->Visible(true);
		}
		else
		{
			btn_bet_money->Visible(false);
		}
	}


}

void BaiCaoScreen::SetTextAndAligneResult(RKString text, WidgetEntity * wpanel)
{
	auto w_text = wpanel->GetWidgetChildByName(".panel_info.text_info");
	//aligne center the text info
	auto res = wpanel->GetWidgetChildAtIdx(0)->GetResource();

	auto list_child = res->getChildren();
	auto w_panel = wpanel->GetWidgetChildByName(".panel_info");
	w_panel->Visible(true);

	float distance = 0;
	Vec2 pos_0 = Vec2(0, 0);
	Vec2 pos_last = Vec2(0, 0);
	if (list_child.size() > 0)
	{
		pos_0 = Utils::getPositionAtAnchorPoint(Vec2(0, 0), list_child.at(0));
		pos_last = Utils::getPositionAtAnchorPoint(Vec2(1, 0), list_child.back());

		pos_0 = res->convertToWorldSpace(pos_0);
		pos_last = res->convertToWorldSpace(pos_last);

		pos_0 = wpanel->GetResource()->convertToNodeSpace(pos_0);
		pos_last = wpanel->GetResource()->convertToNodeSpace(pos_last);
		distance = abs(pos_last.x - pos_0.x);
		w_panel->SetPosition(Vec2(pos_0.x + distance / 2, 0));
	}

	static_cast<TextWidget*>(w_text)->SetText(text);
}

void BaiCaoScreen::OnScores(int who1, string des1)
{
	int s = GetSeatFromPlayerID(who1);
	if (s < 0)
		return;
	SetTextAndAligneResult(des1, ls_players_card[s]);

	if (who1 == _myInfo->GetIDDB() && !_isReceiveMyScore)
	{
		PLAY_SOUND("SFX_GAMEBAI_BAICAO_BAOHIEUNUT");
		_isReceiveMyScore = true;
	}
}

void BaiCaoScreen::OnPlaying(ubyte interval, ubyte numPlayer, int cards[6][4])
{
	_isViewer = true;

	_isGameEnd = false;
	_isStartGame = true;

	OnShowBetSlider(false);
	for (int i = 0; i < numPlayer; i++)
	{
		int s = GetSeatFromPlayerID(cards[i][0]);
		if (s<=-1 || p_list_player_group[s]->getPlayer() == nullptr || p_list_player_group[s]->getPlayer()->IDDB == -1) continue;
		/*if (_isViewer)
		{
			continue;
		}*/
		//seatsDeal[s] = 1;

		DealCardSeat(s);
	}
	ShowButtonSetBet(false);
	livePlayer();
	//startTimeBar(interval, 0);
}

void BaiCaoScreen::OnStartGame(vector<ubyte> myCards, ubyte interval1)
{
	_list_card_flip.clear();
	doStartGame();
	stopTimeBar();
	stopCountDownStart();
	resetGame();

	//CreateListCardUp();

	ShowButtonSetBet(false);
	OnShowBetSlider(false);
	//GameController.Vibrate(VIBRATE_STARTGAME_TIME);
	PLAY_SOUND("SFX_GAMEBAI_START");

	for (auto p : p_list_player_group)
	{
		if (p && p->getPlayer() && GetSeatFromPlayerID(p->getPlayer()->IDDB) > -1)
		{
			auto _player = p->getPlayer();
			auto current_seat = GetSeatFromPlayerID(_player->IDDB);
			auto bc_player = static_cast<BaiCaoPlayer*>(_player);
			bc_player->ClearCard();
			if (_player->IDDB == GameController::myInfo->IDDB)
			{
				bc_player->cardValue = myCards;
			}
			DealCardSeat(current_seat);
		}
	}
	if (GameController::gameId == GameType::GAMEID_BAICAO)
	{
		ShowButtonSetBet(false);
	}
	ShowButtonAction();
	startTimeBar(interval1, 0);
	//hide all text status
	for (auto t : ls_players_card)
	{
		t->GetWidgetChildByName(".panel_info")->Visible(false);
	}
	btn_flop_card->Visible(true);
}

void BaiCaoScreen::HandleAfterInitPlayer()
{
	GameScreen::HandleAfterInitPlayer();
	layoutGame();
	resetGame();
	//check if reconnect
	if (p_list_msg_return.size() > 0)
	{
		for (auto msg : p_list_msg_return)
		{
			OnHandleMsg(msg);
			delete msg;
		}
	}
	p_list_msg_return.clear();
}

void BaiCaoScreen::OnReconnect(
	int limitTime, 
	int remainTime, 
	std::vector< std::vector<int>> 
	playerCards, std::vector<int> playerBetMoneys, 
	int nPlayer, ubyte myCards[], 
	int totalMoney, 
	bool isPlaying)
{
	//GameController.Vibrate(1);
	//SoundManager.Instance.PlayEffect(Sounds.BUZZ);
	onReconnect();
	resetGame();
	int seat = -1;
	if (GameController::gameId == GameType::GAMEID_BAICAO)
	{
		for (int i = 0; i < nPlayer; i++)
		{
			seat = GetSeatFromPlayerID(playerCards[i][0]);
			if (seat <= 0) continue;
			static_cast<TextWidget*>(ls_players_bet_info[seat])->TextRunEffect(playerBetMoneys[i], 0 , 0.3f , Utils::formatNumber_dot);
		/*	_listLayerBetMoney[seat].Money = playerBetMoneys[i];
			_playerBetMoney[seat] = playerBetMoneys[i];*/
		}
	}
	_isStartGame = isPlaying;
	if (isPlaying)
	{
		// set screen start game
		//CreateListCardUp();
		/*this.myCards = myCards;
		HideStartButton();
		var seatsDeal = new int[NUM_SEATS];*/
		OnShowBetSlider(false);
		ResetReady();
		/*
		ShowButtonRubCard(true);
		*/
		if (GameController::getCurrentGameType() == GameType::GAMEID_BAICAO)
		{
			ShowButtonSetBet(false);
		}
		ShowButtonAction();
		startTimeBar(limitTime, remainTime);

		seat = -1;
		bool isShowFlopCard= false;
		for (int i = 0; i < nPlayer; i++)
		{
			seat = GetSeatFromPlayerID(playerCards[i][0]);
			if (seat <= -1) continue;
			
			std::vector<char> ids;

			for (int j = 1; j < 4; j++)
			{
				ids.push_back( (char)playerCards[i][j]);
				if (!isShowFlopCard &&playerCards[i][0] == GameController::myInfo->IDDB && playerCards[i][j] <=-1)
				{
					isShowFlopCard = true;
				}
			}
			DealCardSeat(seat);
			OnFlipCard(playerCards[i][0], ids);
			btn_flop_card->Visible(isShowFlopCard);
			
		}
	}
}

void BaiCaoScreen::OnFlipCard(int id, std::vector<char> cardID)
{
	BaiCaoPlayer* p = (BaiCaoPlayer*)getPlayer(id);
	if (!p)
		return;
	CCLOG("ACTUAL FLIP BAI CAO id: %d , %d , %d", cardID[0], cardID[1], cardID[2]);
	p->FlipCard(cardID);
	if (id == GameController::myInfo->IDDB)
	{
		bool isFullFlop = true;
		for (auto c : _list_card_flip)
		{
			if (c < 0)
			{
				isFullFlop = false;
				break;
			}
		}
		if (_list_card_flip.size() <= 0)
		{
			isFullFlop = false;
		}

		if (isFullFlop)
		{
			_list_card_flip.clear();
			flopCard->setVisible(false);
			btn_flop_card->Visible(false);
		}
	}

}

void BaiCaoScreen::doFlopCard(std::vector<char> ids)
{
	//SoundManager.Instance.PlayEffect(Sounds.LATBAI);
	CCLOG("PRESS BAI CAO id :  %d , %d , %d", ids[0], ids[1], ids[2]);
	if (_list_card_flip.size() == 0)
	{
		_list_card_flip = ids;
	}
	else
	{
		int idx = 0;
		for (auto c : _list_card_flip)
		{
			if (c >= 0)
			{
				ids[idx] = c;
			}
			else if (c < 0 && ids[idx] >= 0)
			{
				_list_card_flip[idx] = ids[idx];
			}
			idx++;
		}
	}

	GameService::latBaiBaiCao(_roomID, _boardID, ids);
}

void BaiCaoScreen::resetGame()
{
	if (_isStartGame)
		return;
	GameScreen::resetGame();
	ShowButtonAction();
	flopCard->setVisible(false);
	showLbInfoBet();
	hideAllCard();
	hideAllFlare();

	_list_card_flip.clear();
	_isViewer = false;

	//hide all text status
	for (auto t : ls_players_card)
	{
		t->GetWidgetChildByName(".panel_info")->Visible(false);
	}
	_isReceiveMyScore = false;
	_myResult = 0;
	//demo show het thong tin len
	//ShowButtonSetBet(true);
	/*for (int i = 1; i < 4; i++)
	{
		auto player = new BaiCaoPlayer();
		p_list_player_group[i]->setPlayer(player);
		DealCardSeat(i);
		static_cast<TextWidget*>(ls_players_bet_info[i])->TextRunEffect(1000000000, 50000, 0.3f, Utils::formatNumber_dot);
		static_cast<TextWidget*>(ls_players_bet_info[i])->Visible(true);
		auto w_text = ls_players_card[i]->GetWidgetChildByName(".panel_info.text_info");
		w_text->GetParentWidget()->Visible(true);
		static_cast<TextWidget*>(w_text)->SetText("9 diem");
	}
	static_cast<TextWidget*>(ls_players_bet_info[0])->TextRunEffect(1000000000, 50000, 0.3f, Utils::formatNumber_dot);
	static_cast<TextWidget*>(ls_players_bet_info[0])->Visible(true);
	DealCardSeat(0);
	startTimeBar(0, 20);
	auto w_text = ls_players_card[0]->GetWidgetChildByName(".panel_info.text_info");
	w_text->GetParentWidget()->Visible(true);
	static_cast<TextWidget*>(w_text)->SetText("9 diem");*/
}

void BaiCaoScreen::hideAllCard()
{
	for (auto p : p_list_player_group)
	{
		if (p 
			&& _PLAYER(p)
			&& _PLAYER(p)->IDDB != -1)
		{
			static_cast<BaiCaoPlayer*>(_PLAYER(p))->HideAllCard();
		}
	}
}

void BaiCaoScreen::showLbInfoBet()
{
	size_t i = 0;
	for (auto lb :ls_players_bet_info)
	{
		if(!lb)
			continue;
		if (i < p_list_player_group.size()&& 
			p_list_player_group[i] && 
			p_list_player_group[i]->getPlayer() && 
			p_list_player_group[i]->getPlayer()->IDDB != _ownerID)
		{
			lb->Visible(true);
		}
		else
		{
			lb->Visible(false);
		}
		i++;
	}
}

void BaiCaoScreen::onSomeOneJoinBoard(int seat, Player * player)
{
	if (seat >= 0)
	{
		ls_players_card[seat]->Visible(true);
	}
	GameScreen::onSomeOneJoinBoard(seat, player);
}

void BaiCaoScreen::onSomeOneLeaveBoard(int leave, int newOwner)
{
	int s = GetSeatFromPlayerID(leave);
	if (!_isStartGame && s >= 0)
	{
		ls_players_bet_info[s]->Visible(false);
	}
	if (s >= 0)
	{
		ls_players_card[s]->Visible(false);
	}

	if (newOwner == (_myInfo != nullptr ? _myInfo->GetIDDB() : GameController::myInfo->IDDB))
	{
		btn_bet_money->Visible(false);
	}
	else
	{
		btn_bet_money->Visible(true);
	}

	GameScreen::onSomeOneLeaveBoard(leave, newOwner);
}

void BaiCaoScreen::OnTouchMenuEnd(const cocos2d::Point & p)
{
	if (_isGameEnd)
	{
		resetGame();
	}
	else if (GetMe())
	{
		bool isFlop = false;
		vector<char> lsCardSelects;
		int idx = 0;
		for (Card *c : GetMe()->cards)
		{
			if (NODE(c)->getTag() == 1)
			{
				lsCardSelects.push_back(GetMe()->GetCardValueIdx(idx));
				isFlop = true;
			}
			else
			{
				lsCardSelects.push_back(c->getID());
			}
			NODE(c)->setTag(0);
			idx++;
		}

		if (lsCardSelects.size() > 0 && isFlop)
		{
			this->doFlopCard(lsCardSelects);
		}
	}
}


void BaiCaoScreen::ActionTouchBeginObject(Utility::wObject::wBasicObject * obj)
{
	auto c = (Card*)obj;
	if (c->getID() == -1)
	{
		PLAY_SOUND("SFX_GAMEBAI_CHONBAI");
		NODE(c)->setTag(1);
	}
}
void BaiCaoScreen::ActionTouchMoveObject(Utility::wObject::wBasicObject * obj)
{
	auto c = (Card*)obj;
	if (c->getID() == -1)
	{
		NODE(c)->setTag(1);
	}
}

BaiCaoPlayer* BaiCaoScreen::GetMe()
{
	return static_cast<BaiCaoPlayer*>(getMyPlayer());
}

void BaiCaoScreen::OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player)
{
	Vec2 pos = dec->GetDataVector2("pos_contain_card");
	pos.x = pos.x * GetGameSize().width / this->GetDesignSize().x;
	pos.y = pos.y * GetGameSize().height / this->GetDesignSize().y;
	_list_pos_layer_card.push_back(pos);
}

void BaiCaoScreen::onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney, RKString dec)
{
	if (player &&player->GetIDDB() == _myInfo->GetIDDB())
	{
		_myResult = value;
		if (_isGameEnd)
		{
			if (_myResult > 0)
			{
				doSomethingWhenIWin();
				_myResult = 0;
			}
			else if (_myResult < 0)
			{
				doSomethingWhenILose();
				_myResult = 0;
			}
		}
	}
	if (value > 0)
	{
		player->showFlare(true);
	}
}

void BaiCaoScreen::hideAllFlare()
{
	for (auto p : p_list_player_group)
	{
		p->showFlare(false);
	}
}

void BaiCaoScreen::onBetRange(int min, s64  max, s64 maxMoneyRoom)
{
	GameScreen::onBetRange(min, max, maxMoneyRoom);
	if (p_slider_bet)
	{
		if (max > GameController::myInfo->getMoney())
		{
			max = GameController::myInfo->getMoney();
		}
		p_slider_bet->setMinMax(min, max);

	}
	else
	{
		PASSERT2(false, "wrong");
	}
}

void BaiCaoScreen::onSomeOneQuickPlay(int playerID, bool isQuickPlay)
{
	GameScreen::onSomeOneQuickPlay(playerID, isQuickPlay);
	if (playerID == GameController::myInfo->IDDB && isQuickPlay) 
	{
		//canClearCard = false;
		OnShowBetSlider(false);
		if (isQuickPlay 
			&& _myInfo 
			&& _PLAYER(_myInfo)
			&& _PLAYER(_myInfo)->IDDB != _ownerID)
		{
			btn_bet_money->Visible(false);
		}
	}
}

void BaiCaoScreen::OnShowBetSlider(bool show)
{
	if (!show && p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->Visible())
	{
		p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->SetActionCommandWithName("ON_HIDE");
	}
	else if (show && !p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->Visible())
	{
		p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->SetActionCommandWithName("ON_SHOW", CallFunc::create(
			[this]()
		{
			this->p_panel_slider->GetWidgetChildByName(".sp_slider_bet")->ForceFinishAction();
			//request bet range from server !!!!
			GlobalService::getBetRange();
		}));
	}

}

void BaiCaoScreen::doSetMoney(int money)
{
	GameService::setCommandinGametoServer(
		(GameController::getCurrentGameType() == GameType::GAMEID_BAICAO) ? IwinProtocol::WANT_DRAW : IwinProtocol::SET_MONEY, 
		_roomID, 
		_boardID, 
		money);
}

void BaiCaoScreen::onGetTimeLeft(float timeLeft)
{
	OnShowCountDown(timeLeft, 0);
}

void BaiCaoScreen::doSomethingWhenIWin()
{
	GameScreen::doSomethingWhenIWin();
	PLAY_SOUND("SFX_GAMEBAI_THANGBAI");
}
void BaiCaoScreen::doSomethingWhenILose()
{
	GameScreen::doSomethingWhenIWin();
	PLAY_SOUND("SFX_GAMEBAI_THUABAI");
}
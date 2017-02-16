#include <algorithm>

#include "Screens/GamePlayMgr.h"
#include "AnimUtils.h"
#include "TienLenService.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/Object/RectProcessTime.h"
#include "Screens/Object/PlayerGroup.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wButton.h"

#include "TienLenScreen.h"
#include "TienLenPlayer.h"



#define CARD_SELECT_OP  100
#define TIME_SKIP  5
TienLenScreen* TienLenScreen::getInstance()
{
	if (!GamePlay)
		return nullptr;

	return static_cast<TienLenScreen*>(GamePlay->GetCurrentGameScreen());
}

TienLenScreen::TienLenScreen()
{
	SetGameID(GameType::GAMEID_TIENLEN);
	p_rect_process = nullptr;

	SetMaxPlayer(4);
	_interval = 0;
	
	IsAutoSelect = false;
	isClearCurrentCard = false;

	p_btn_skip = nullptr;
	p_btn_fire = nullptr;
	p_btn_sort = nullptr;
	_list_fire_card.clear();
	_list_last_card.clear();
	_list_end_card.clear();

	_currentPlayer = -1;
	_currentCardsType = -1;
	_autoSkip = false;
	_selectedCardIDs.clear();

	_forceMove3Bich = false;
	_forceMoveSmallest = false;
	_isFirstMatch = true;
	p_layout_fire_card = nullptr;
	_delta_aligne = Vec2(0, 0);
	_load_card_finish = false;
	_number_card_remain_after_game = 0;
	p_layout_force_finish = nullptr;
	_list_card_finish.clear();
}

TienLenScreen::~TienLenScreen()
{
	CleanLastCard();
	CleanFireCard();
	CleanEndCard();
	SAFE_DELETE(p_rect_process);

	CleanMyCard(true);

	removeChildObject(nullptr, true);
	_list_fire_card.clear();
	_list_last_card.clear();
	_list_end_card.clear();
	_list_pos_swap_group.clear();

	_list_card_finish.clear();
}

void TienLenScreen::createUI()
{
	p_rect_process = new RectProcessTime(p_btn_skip);
	
}

void TienLenScreen::CleanMyCard(bool cleanup)
{
	if (getMe() == nullptr || getMe()->cards.size() <= 0)
	{
		return;
	}

	//do-not clear my card
	if (cleanup)
	{
		for (Card *c : getMe()->cards)
		{
			NODE(c)->stopAllActions();
			removeChildObject(NODE(c));
			delete c;
		}
		getMe()->cards.clear();
	}
	else
	{
		for (Card *c : getMe()->cards)
		{
			c->SetVisible(false);
		}
		getMe()->remain_cards.clear();
	}
	
}

void TienLenScreen::CleanFireCard()
{
	for (auto c : _list_fire_card)
	{
		NODE(c)->stopAllActions();
		removeChildObject(NODE(c));
		delete c;
	}

	_list_fire_card.clear();
}

void TienLenScreen::CleanLastCard()
{
	for (auto c : _list_last_card)
	{
		NODE(c)->stopAllActions();
		removeChildObject(NODE(c));
		delete c;
	}
	_list_last_card.clear();
}

void TienLenScreen::CleanEndCard()
{
	for (auto c : _list_end_card)
	{
		NODE(c)->stopAllActions();
		removeChildObject(NODE(c));
		delete c;
	}

	_list_end_card.clear();
}

void TienLenScreen::addLastCard(std::vector<Card*> lsNode)
{
	int idx = 0;
	for (Card * c : lsNode)
	{
		NODE(c)->setZOrder(175 + idx);
		c->SetActionCommandWithName("MOVE_CENTER_BACK",nullptr,true);
		_list_last_card.push_back(c);
		//c->showTrans();
		idx++;
	}
}

void TienLenScreen::runCardFire(std::vector<unsigned char> cards, int whoMove)
{
	auto card_type = CardUtils::getType(cards);
	bool have_chat_heo = false;
	if (card_type == CardUtils::CardCombo::TYPE_3DOITHONG ||
		card_type == CardUtils::CardCombo::TYPE_4DOITHONG ||
		card_type == CardUtils::CardCombo::TYPE_TUQUY ||
		card_type == CardUtils::CardCombo::TYPE_2TUQUY)
	{
		have_chat_heo = true;
	}

	if (whoMove == GameController::myInfo->IDDB)//my self move
	{
		if (getMe() != nullptr &&  getMe()->cards.size() > 0)
		{
			float y =		p_layout_fire_card->GetPosition_BottomLeft().y  + _delta_aligne.y;
			float firstX =	p_layout_fire_card->GetPosition_BottomLeft().x  + _delta_aligne.x;
			float endX =	p_layout_fire_card->GetPosition_BottomRight().x + _delta_aligne.x;
			

			std::vector<Card*> list_card_move = CardUtils::getCardId(getMe()->cards, cards);		
			std::vector<Vec2> list_pos_move_to = CardUtils::AligneCardPosition(firstX, y, endX, list_card_move.size(), list_card_move.size() > 0 ? list_card_move[0]->GetSize() : getMe()->cards[0]->GetSize());

			if (list_pos_move_to.size() != list_card_move.size())
			{
				PASSERT2(false, "wrong here!");
			}	
			int idx = 0;
			int size_list_card_move = list_card_move.size();

			PLAY_SOUND("SFX_GAMEBAI_BAYLEN");
			for (Card* c : list_card_move)
			{
				getMe()->cards.erase(std::remove(
					getMe()->cards.begin(),
					getMe()->cards.end(), c), 
					getMe()->cards.end());

				NODE(c)->setAnchorPoint(Vec2(0.5, 0.5));
				NODE(c)->setZOrder(250 + idx);

				AnimUtils::RunEffectMoveToFire(c, c->GetPosition(), list_pos_move_to[idx], [c , idx , size_list_card_move, this , have_chat_heo]() {
					c->EventActionFinish();
					if (idx == 0)
					{
						if (size_list_card_move < 2)
						{
							auto anim = this->p_animate_1;
							this->p_animate_2->Visible(false);
							anim->Visible(true);
							anim->SetPosition(c->GetPosition());
							static_cast<AnimWidget*>(anim)->RunAnim(CallFunc::create([anim]() {anim->Visible(false); }));
						}
						else
						{
							auto anim = this->p_animate_2;
							this->p_animate_1->Visible(false);
							anim->Visible(true);
							anim->SetPosition(c->GetPosition());
							static_cast<AnimWidget*>(anim)->RunAnim(CallFunc::create([anim]() {anim->Visible(false); }));
						}
					}
					if (idx == size_list_card_move - 1)
					{
						CheckCardSpecial();

						if (size_list_card_move == 1)
						{
							PLAY_SOUND("SFX_GAMEBAI_1QUAN");
						}
						else if (size_list_card_move > 1)
						{
							PLAY_SOUND("SFX_GAMEBAI_NHIEUQUAN");
						}
						if (have_chat_heo)
						{
							PLAY_SOUND("SFX_GAMEBAI_CHACHEO");
						}
					}
				});
				c->showHighlight(false);
				c->SetTouchEnable(false);
				_list_fire_card.push_back(c);		
				idx++;
			}

			layoutMyCard();
		}
	}
	else
	{
		float y = p_layout_fire_card->GetPosition_BottomLeft().y      + _delta_aligne.y;
		float firstX = p_layout_fire_card->GetPosition_BottomLeft().x + _delta_aligne.x;
		float endX = p_layout_fire_card->GetPosition_BottomRight().x  + _delta_aligne.x;
		int idx = 0;
		for (auto id : cards)
		{
			Card* c = Card::NewInstance(id, DECK_TYPE::DECK_TIENLEN);
			addChildObject(NODE(c), 250 + idx);
			NODE(c)->setAnchorPoint(Vec2(0.5, 0.5));
			c->showHighlight(false);
			c->SetTouchEnable(false);
			_list_fire_card.push_back(c);
			idx++;
		}

		auto seat_id = GetSeatFromPlayerID(whoMove);
		if (seat_id > -1)
		{
			auto pl = GetPlayerGroupAtIdx(seat_id);
			if (pl)
			{
				WidgetEntity* userG = _WIDGET(pl);
				PASSERT2(userG != nullptr, "userG is nullptr");
				if (userG && _list_fire_card.size() > 0)
				{
					std::vector<Vec2> list_pos_move_to = CardUtils::AligneCardPosition(firstX, y, endX, _list_fire_card.size(), _list_fire_card[0]->GetSize());
					idx = 0;

					int size_list_card_move = _list_fire_card.size();

					PLAY_SOUND("SFX_GAMEBAI_BAYLEN");
					for (Card* c : _list_fire_card)
					{
						AnimUtils::RunEffectMoveToFire(c, userG->GetPosition_Middle(), list_pos_move_to[idx], [c, idx, size_list_card_move, this , have_chat_heo]() {
							c->EventActionFinish();
							if (idx == 0)
							{
								if (size_list_card_move < 2)
								{
									auto anim = this->p_animate_1;
									this->p_animate_2->Visible(false);
									anim->Visible(true);
									anim->SetPosition(c->GetPosition());
									static_cast<AnimWidget*>(anim)->RunAnim(CallFunc::create([anim]() {anim->Visible(false); }));
								}
								else
								{
									auto anim = this->p_animate_2;
									this->p_animate_1->Visible(false);
									anim->Visible(true);
									anim->SetPosition(c->GetPosition());
									static_cast<AnimWidget*>(anim)->RunAnim(CallFunc::create([anim]() {anim->Visible(false); }));
								}
							}

							if (idx == size_list_card_move - 1)
							{
								if (size_list_card_move == 1)
								{
									PLAY_SOUND("SFX_GAMEBAI_1QUAN");
								}
								else if (size_list_card_move > 1)
								{
									PLAY_SOUND("SFX_GAMEBAI_NHIEUQUAN");
								}
								if (have_chat_heo)
								{
									PLAY_SOUND("SFX_GAMEBAI_CHACHEO");
								}
							}
						});
						idx++;
					}
				}
			}
		}
	}
}

void TienLenScreen::OnTouchMenuEnd(const cocos2d::Point & p)
{

	if (!getMe() || getMe()->cards.size() <= 0 || !_isStartGame)
	{
		if (needReset)
		{
			resetGame();
		}
		return;
	}

	//reset all cache card

	if (_isGameEnd)
	{
		CleanMyCard();
		CleanFireCard();
		CleanLastCard();
		CleanEndCard();
		hideAllRank();
	}
	else
	{
		if (!IsAutoSelect)
		{
			bool isNotHaveSelect = true;
			for (int i = getMe()->cards.size() - 1; i >= 0; i--)
			{
				auto c = getMe()->cards[i];
				if (c->isSelected())
				{
					isNotHaveSelect = false;
					//break;;
				}
			
			}
			IsAutoSelect = isNotHaveSelect;
		}
		if (p_list_id_card_on_hold.size() > 0)
		{
			bool have_fire_card = false;
			//
			for (auto i : p_list_id_card_on_hold)
			{
				getMe()->cards[i]->SetOpacity(255);
				NODE(getMe()->cards[i])->setTag(0);
			}

			if (p_panel_card_hold->getPosition().y > 0)
			{
				//fire card
				have_fire_card = true;

				_selectedCardIDs.clear();
				for (auto c : p_list_id_card_on_hold) {
					_selectedCardIDs.push_back((char)getMe()->cards[c]->getID());
				}
				TienLenService::move(_roomID, _boardID, _selectedCardIDs);
			}
			RemoveCardOnSelected();
			//
			p_list_id_card_on_hold.clear();
			p_on_hold_card_selected = false;
			if (have_fire_card)
			{
				return;
			}
		}
		else
		{
			if (!getMe())
				return;

			int numAciton = 0;
			vector<unsigned char> lsCardSelects;
			bool isCheckEffec = false;
			bool isSelectAll = true;
			for (int i = getMe()->cards.size() - 1; i >= 0; i--)
			{
				auto c = getMe()->cards[i];
				if (NODE(c)->getTag() == 1 && c->isSelected())
				{
					isSelectAll = false;
				}
			}
			for (int i = getMe()->cards.size() - 1; i >= 0; i--)
			{
				auto c = getMe()->cards[i];
				if (NODE(c)->getTag() == 1)
				{
					c->SelectCardAction(isSelectAll);
					isCheckEffec = true;
				}
				if (c->isSelected()) {
					lsCardSelects.push_back(c->getID());
					numAciton++;
				}
				c->showHighlight(false);
				NODE(c)->setTag(0);
			}
			if (isCheckEffec)
			{
				CheckCardSpecial();
			}
			if (!getMe()  || !IsAutoSelect || lsCardSelects.size() <=0) {
				return;
			}

			if ( GameController::gameId != GameType::GAMEID_TIENLEN_MB)
			{
				vector<Card*> listCards;
				if (lsCardSelects.size() > 1 && lsCardSelects[0]/4 != lsCardSelects[1]/4)
				{
					if (_list_card_select_action.size() > 0 && _list_card_select_action[0] != lsCardSelects[0])// giam dan
					{
						for (Card* c : getMe()->cards)
						{
							if (lsCardSelects[0] >= c->getID()
								&& c->GetCardValue() != lsCardSelects[0] / 4 && c->GetCardValue() != lsCardSelects[1] / 4
								|| (c->getID() == lsCardSelects[0] || c->getID() == lsCardSelects[1]))
							{
								Card* c1 = new Card();
								c1->SetID(c->getID(), DECK_TIENLEN);
								listCards.push_back(c1);
							}

						}
					}
					else  if (_list_card_select_action.size() > 0  && _list_card_select_action[0] == lsCardSelects[0])// tang dan
					{
						for (Card* c : getMe()->cards)
						{
							if (lsCardSelects[lsCardSelects.size() - 1] <= c->getID()
								&& c->GetCardValue() != lsCardSelects[0] / 4 && c->GetCardValue() != lsCardSelects[1] / 4
								|| (c->getID() == lsCardSelects[0] || c->getID() == lsCardSelects[1]))
							{
								Card* c1 = new Card();
								c1->SetID(c->getID(), DECK_TIENLEN);
								listCards.push_back(c1);
							}

						}
					}
				}
				else
				{
					for (Card* c : getMe()->cards)
					{
						Card* c1 = new Card();
						c1->SetID(c->getID(), DECK_TIENLEN);
						listCards.push_back(c1);

					}
				}
			
				TienLenPlayer* playerTmp = new TienLenPlayer();
				playerTmp->cards = listCards;
				vector<unsigned char> selectAuto;
				if (numAciton == 1) {
					selectAuto = CardUtils::autoSelecWhenTouchLTMN(playerTmp, lsCardSelects, getcardFire());
				}
				else if (numAciton > 1) {
					if (lsCardSelects.size() >= 3)
					{
						TienLenPlayer* playerCardSelect = new TienLenPlayer();
						for (unsigned char id : lsCardSelects)
						{
							Card* c1 = new Card();
							c1->SetID(id, DECK_TIENLEN);
							playerCardSelect->cards.push_back(c1);
						}
						selectAuto = CardUtils::autoWhenMultiSelectTLMN(playerCardSelect, lsCardSelects, getcardFire());
						for (Card* c : playerCardSelect->cards)
						{
							delete c;
						}
						delete playerCardSelect;
					}
					if (selectAuto.size() <= 0)
					{
						selectAuto = CardUtils::autoWhenMultiSelectTLMN(playerTmp, lsCardSelects, getcardFire());
					}
					
				}
				for (Card* c : listCards)
				{
					delete c;
				}
				delete playerTmp;
				for (Card* c : getMe()->cards) {
					for (unsigned char id : lsCardSelects) {
						if (id == c->getID()) {
							c->SelectCardAction(true, false);
							break;
						}
					}

				}

				if (selectAuto.size() >0) {
					for (Card* c : getMe()->cards) {
						c->SelectCardAction(false, false);
					}
					for (auto s : selectAuto) {
						for (Card* c : getMe()->cards) {
							if (!c->isSelected()) {
								if (s == c->getID()) {
									c->SelectCardAction(true, false);
									break;
								}
							}
						}

					}
					_list_card_select_action.clear();
					IsAutoSelect = false;
				}

			}
		}
	}


}

void TienLenScreen::OnTouchMenuHold(const cocos2d::Point & p)
{
	if (!getMe())
		return;
	//khong phai luoc di cua minh kok dc move
	if (_currentPlayer != GameController::myInfo->IDDB)
		return;

	for (int j = 0; j < getMe()->cards.size(); j++)
	{
		auto p = getMe()->cards[j];
		if ((p->isSelected() || p->isHighlight()) && !InListSelected(j))
		{
			p_list_id_card_on_hold.push_back(j);
		}
	}

	if (getMe()->cards.size() > 0 && p_list_id_card_on_hold.size() > 0)
	{
		if (!p_on_hold_card_selected)
		{
			float distance = p.distance(p_touch_start);
			if (distance > (getMe()->cards[0]->GetSize().y / 2.f) 
				&& p.y > p_current_pos_touch.y 
				&& p.y > getMe()->cards[0]->GetPosition().y * 3.f / 2.f)
			{
				p_on_hold_card_selected = true;
				for (auto c : p_list_id_card_on_hold)
				{
					auto cc = getMe()->cards[c];
					cc->SetOpacity(128);

					auto card = Card::NewInstance(cc->getID(), DECK_TYPE::DECK_TIENLEN);
					addCardHold(card);
				}

				float leftX = p.x - ((p_list_id_card_on_hold.size() / 2)*p_list_card_on_selected[0]->GetSize().x);
				float RightX = p.x + ((p_list_id_card_on_hold.size() / 2)*p_list_card_on_selected[0]->GetSize().x);
				auto list_pos = CardUtils::AligneCardPosition(leftX, getMe()->cards[p_list_id_card_on_hold[0]]->GetPosition().y, RightX, p_list_id_card_on_hold.size(), p_list_card_on_selected[0]->GetSize());


				for (int i = 0 ; i < p_list_card_on_selected.size() ; i ++)
				{
					p_list_card_on_selected[i]->SetPosition(list_pos[i]);
				}
				
			}
			else
			{
				p_list_id_card_on_hold.clear();
			}
		}
		else
		{
			auto delta_move = p_current_pos_touch - p;
			auto current_pos = p_panel_card_hold->getPosition();
			current_pos.x = current_pos.x - delta_move.x;
			current_pos.y = current_pos.y - delta_move.y;
			p_panel_card_hold->setPosition(current_pos);
		}
	}

	
}

void TienLenScreen::ActionTouchBeginObject(Utility::wObject::wBasicObject * obj)
{
	Card * c = (Card*)obj;

	if (NODE(c)->getTag() != 1)
	{
		c->showHighlight(true);
		NODE(c)->setTag(1);
		PLAY_SOUND("SFX_GAMEBAI_CHONBAI");
	}
	if (!c->isSelected())
	{
		_list_card_select_action.clear();
		_list_card_select_action.push_back(c->getID());
	}

}

void TienLenScreen::ActionTouchMoveObject(Utility::wObject::wBasicObject * obj)
{

	if (p_list_id_card_on_hold.size() > 0)
	{
		return;
	}

	Card * c = (Card*)obj;
	if (NODE(c)->getTag() != 1)
	{
		c->showHighlight(true);
		NODE(c)->setTag(1);
	}
}


std::vector<unsigned char> TienLenScreen::doAutoSelect(bool auto_skip)
{
	vector<unsigned char> curentFire = getcardFire();
	/*vector<Card*> listCards;
	for (Card* c : getMe()->cards)
	{
		Card* c1 = new Card();
		c1->SetID(c->getID(), DECK_TIENLEN);
		listCards.push_back(c1);
	}
	TienLenPlayer* playerTmp = new TienLenPlayer();
	playerTmp->cards = listCards;
	std::vector<unsigned char> foundAt = CardUtils::autoSelect(curentFire, playerTmp);
	for (Card* c : listCards)
	{
		delete c;
	}
	delete playerTmp;*/
	std::vector<unsigned char> foundAt = CardUtils::autoSelect(curentFire, getMe());
	if (foundAt.size() <= 0 && GameController::myInfo->IDDB == _currentPlayer)
	{
		
		for (Card* c : getMe()->cards)
		{
			c->SelectCardAction(false, false);
		}
		// ko chon duoc la nao thi se hien bo luot
		/*_currentPlayer = -1;
		_forceMoveSmallest = false;
		auto w_panel = p_btn_skip->GetWidgetChildByName(".eat_card_panel");
		if (!p_rect_process->isShowTimeAction()) {
			p_rect_process->startTime(TIME_SKIP, ([this , w_panel, auto_skip]() {
				if (auto_skip)
				{
					w_panel->Visible(false);
					this->p_rect_process->stop();
					TienLenService::skip(_roomID, _boardID);
				}
				
			}));
		}

		if (auto_skip)
		{		
			w_panel->ForceFinishAction();
			w_panel->Visible(true);
			w_panel->SetActionCommandWithName("FOCUS_ACTION");
		}*/
		/*else
		{
			ActionSkip(L.Get(1055) + ". " + L.Get(90));
		}*/
	}

	/*if (!notify)
	{
		for (var i = 0; i < GetMe().cards.Count; i++)
		{
			CardUtil.SetSelect(GetMe().cards[i], false);
		}
	}*/
	return foundAt;
}

std::vector<unsigned char> TienLenScreen::getcardFire()
{
	vector<unsigned char> curentFire;

	for (auto c : _list_fire_card)
	{
		curentFire.push_back(c->getID());
	}
	return curentFire;
}

bool TienLenScreen::InitGame(int game_id)
{
	bool res = GameScreen::InitGame(game_id);

	XMLMgr->OnLoadXMLData<TienLenScreen>("TLMN_Screen", std::mem_fn(&TienLenScreen::LoadXMLGameScreenCustom), this);
	onCreateListPlayer("TLMN_Screen");

	p_panel_bottom = p_panel_custom->GetWidgetChildByName(".back_panel_card");
	PASSERT2(p_panel_bottom != nullptr, "p_panel_bottom is null , recheck the path");
	
	auto lay_btn_tl = p_panel_bottom->GetWidgetChildByName(".layout_btn_game_tlmn");
	p_btn_fire = lay_btn_tl->GetWidgetChildByName(".btn_deal_turn");
	p_btn_skip = lay_btn_tl->GetWidgetChildByName(".btn_leave_turn");
	p_btn_sort = lay_btn_tl->GetWidgetChildByName(".btn_quick_choose_turn");


	p_layout_fire_card = p_panel_custom->GetWidgetChildByName(".layout_fire_card");
	p_animate_1 = p_panel_custom->GetWidgetChildByName(".animate_effect_1");
	p_animate_2 = p_panel_custom->GetWidgetChildByName(".animate_effect_2");

	p_layout_force_finish = GetWidgetChildByName("HUD_screen.panel_custom_slider_bet.layout_show_toi_trang");

	createUI();

	_myResult = 0;
	_pos_y_hl = -1;
	return true;
}

void TienLenScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_leave_turn")
	{
		if (p_rect_process&& p_rect_process->isShowTimeAction())
		{
			doSkip();
			p_btn_skip->GetWidgetChildByName(".eat_card_panel")->Visible(false);
			p_rect_process->stop();
		}
		else
		{
			p_rect_process->startTime(TIME_SKIP);
		}
		
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_deal_turn")
	{
		doFireWork();
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_quick_choose_turn")
	{
		auto list_card = doAutoSelect();

		int idx = 0;
		for (auto c : getMe()->cards)
		{
			for (auto l : list_card)
			{
				if (idx == l)
				{
					c->SelectCardAction(true,false);
					break;
				}
				else
				{
					c->SelectCardAction(false,false);
				}
			}
			idx++;
		}
	}
	else
	{
		GameScreen::OnProcessWidgetChild(name, type_widget, _widget);
		
	}
}

int TienLenScreen::Update(float dt)
{
	if (GameScreen::Update(dt) == 0)
	{
		return 0;
	}

	if (!_load_card_finish)
	{
		if(_list_card_create.size() > 0)
		{
			Card * c = Card::NewInstance(_list_card_create[0], DECK_TYPE::DECK_TIENLEN);
			c->SetTouchEnable(true);
			c->SetVisible(false);
			getMe()->cards.push_back(c);
			_list_card_create.erase(_list_card_create.begin());
			if (_list_card_create.size() == 0 && _number_card_remain_after_game <= 0)
			{
				_load_card_finish = true;
				p_is_finish_load_player = true;
			}
		}
		else if (_list_card_create.size() <= 0)
		{
			if (_number_card_remain_after_game > 0)
			{
				int idx = _number_card_remain_after_game - 1;
				(getMe()->cards[idx])->ResetID(-1, DECK_TYPE::DECK_TIENLEN);
				getMe()->cards[idx]->SetVisible(false);
				_number_card_remain_after_game--;
			}
			else
			{
				_load_card_finish = true;
			}
		}
		return 0;
	}

	if (p_is_finish_load_player)
	{
		//handle msg return after init player group
		if (p_list_msg_return.size() > 0)
		{
			OnHandleMsg(p_list_msg_return[0]);
			delete p_list_msg_return[0];
			p_list_msg_return.erase(p_list_msg_return.begin());
		}
	}
	else
	{
		return 0;
	}

	if (getMe())
	{
		for (auto c : getMe()->cards) {
			c->Update(dt);
		}
	}

	for (auto c : _list_fire_card) {
		c->Update(dt);
	}

	for (auto c : _list_last_card) {
		c->Update(dt);
	}

	for (auto c : _list_end_card) {
		c->Update(dt);
	}

	return 1;
}

void TienLenScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	auto p_list_sample_dec = LoadWidgetCustom(p_objectXML);
	PASSERT2(p_list_sample_dec.size() > 0, "no widget loaded!");
	//
	ParseIntoCustom(p_list_sample_dec.at("back_panel_card"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("layout_fire_card"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("animate_effect_1"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("animate_effect_2"), p_panel_custom, "panel_custom_menu");

	ParseIntoCustom(p_list_sample_dec.at("layout_show_toi_trang"), GetWidgetChildByName("HUD_screen.panel_custom_slider_bet"), "panel_custom_slider_bet");

	for (auto it : p_list_sample_dec)
	{
		delete it.second;
	}
	p_list_sample_dec.clear();
}

/// để tạo cho thằng choi nó ngồi ấy mà
void TienLenScreen::resetSeat()
{

}

void TienLenScreen::start(int whoMoveFirst, char interval, std::vector<unsigned char> myCards)
{
	removeChildObject(nullptr, true);
	doStartGame();
	OnHideCountDown();
	resetGame();
	//GameController.Vibrate(VIBRATE_STARTGAME_TIME);
	//RemoveAllImageFinish();
	CleanMyCard();
	CleanFireCard();
	CleanLastCard();
	CleanEndCard();
	ResetReady();
	ResetAllSkip();
	//ShowAllUserGroupOpacity();

	//TienLenModel.Instance.ShowHeighBetMoney(false);

	_forceMove3Bich = false;

	int idx = 0;
	
	PASSERT2(getMe()->cards.size() > 0, "my list card must be init first");
	if (getMe()->cards.size() < myCards.size())
	{
		int delta = myCards.size() - getMe()->cards.size();
		for (int i = 0; i < delta; i++)
		{
			Card * c = Card::NewInstance(-1, DECK_TYPE::DECK_TIENLEN);
			c->SetTouchEnable(true);
			c->SetVisible(false);
			getMe()->cards.push_back(c);
		}
	}

	getMe()->remain_cards = myCards;
	for (auto mc : myCards){
		Card * c = getMe()->cards[idx];
		c->ResetID(mc, DECK_TYPE::DECK_TIENLEN);
		c->SetVisible(true);
		addChildObject(NODE(c), 50 + idx);
		NODE(c)->setAnchorPoint(Vec2(0.5, 0.5));
		c->SetStatus(false);
		idx++;
	}
	_delta_aligne = getMe()->cards[0]->GetSize() / 2.f;
	CardUtils::sortByOrder(getMe()->cards);

	dealCards();

	if (_isFirstMatch 
		&& whoMoveFirst == GameController::myInfo->IDDB 
		&& getMe()->cards[0]->getID() == 0)
	{
		_forceMove3Bich = true;
	}
	this->_interval = interval;
	_currentPlayer = whoMoveFirst;

	//layoutMyCard();

	stopTimeBar();
	PlayerGroup* pg = getUserGroup(whoMoveFirst);
	if (pg != nullptr)
	{
		pg->startTime(interval, 0);
	}
	if (myCards.size() > 0)
	{
		if (_currentPlayer == GameController::myInfo->IDDB)
		{
			EnableButton(BTN_TIENLEN::BTN_COUNT, true);
			EnableButton(BTN_TIENLEN::BTN_SKIP_CARD, false);
			//IsAutoSelect = true;
		}
		else
		{
			EnableButton(BTN_TIENLEN::BTN_COUNT, false);
		}
	}

	pg->RunTurnEffect();

}

TienLenPlayer* TienLenScreen::getMe()
{
	return static_cast<TienLenPlayer*>(getMyPlayer());
}

void TienLenScreen::resetGame()
{
	CleanMyCard();
	CleanFireCard();
	CleanLastCard();
	CleanEndCard();

	EnableButton(BTN_TIENLEN::BTN_COUNT, false);

	stopTimeBar();
	IsAutoSelect = false;
	stopAllTimePlayer();
	hideAllRank();
	OnShowEffectHL(-1,-1, false);
	p_layout_force_finish->Visible(false);
	_list_card_select_action.clear();
	needReset = false;

}

void TienLenScreen::layoutMyCard()
{
	if (!getMe() || getMe()->cards.size() <= 0)
		return;

	auto w_avatar = _WIDGET(_myInfo);
	auto size_avatar = w_avatar->GetWidgetChildByName(".layout_player.bg_avatar")->GetSize();
	int idx = 0;
	auto list_pos = CardUtils::AligneCardPosition(
		w_avatar->GetPosition_Middle().x + (size_avatar.x / 2)+ 5 + _delta_aligne.x,
		p_panel_bottom->GetSize().y + 5 + _delta_aligne.y,
		GetGameSize().width - 10 + _delta_aligne.x,
		getMe()->cards.size(),
		getMe()->cards[0]->GetSize());

	for (Card* c : getMe()->cards)
	{
		NODE(c)->setAnchorPoint(Vec2(0.5,0.5));
		c->SelectCardAction(false, false);
		CallFunc *  func = nullptr;
		AnimUtils::RunEffectLayout(c, list_pos[idx] , func);
		idx++;
	}

}

void TienLenScreen::Move(int whoMove, std::vector< unsigned char> cards, int nextMove)
{
	/*
	if (_list_last_card.size() > 0){
		CleanLastCard();
	}*/
	int seat = GetSeatFromPlayerID(whoMove);
	CCLOG("MOVE: who move skip id : %d", whoMove);
	seat = GetSeatFromPlayerID(nextMove);
	CCLOG("MOVE: nextMove skip id : %d", nextMove);

	if (_interval <= 0){
		_interval = 20;
	}

	if (_list_fire_card.size() > 0){
		CleanLastCard();
		addLastCard(_list_fire_card);
		_list_fire_card.clear();
	}

	if (_myInfo && whoMove == _myInfo->getPlayer()->IDDB){
	//	layoutMyCard();
		EnableButton(BTN_TIENLEN::BTN_COUNT, false);
		//StopTimeBar();
		OnHideCountDown();
		
	}
	else
	{
		int p = GetSeatFromPlayerID(whoMove);
	}

	if (whoMove == nextMove)
	{
		ResetAllSkip();
	}
	
	if (_myInfo)
	{
		runCardFire(cards, whoMove);
	}
	//ShowLastCard();
	//ShowFireCard(false);
	_forceMove3Bich = false;
	_currentPlayer = nextMove;
	//SetCurrentCards(fireCards);

	PlayerGroup* group = getUserGroup(whoMove);
	if (group){
		group->stopTime();
	}

	auto groupNext = getUserGroup(nextMove);
	if (groupNext){
		groupNext->startTime((_interval == 0 ? 30 : _interval), 0);
		groupNext->RunTurnEffect();
	}
	int myID = (_myInfo ? _myInfo->getPlayer()->IDDB : -1);
	CCLOG("next move ID: %d , myID: %d", nextMove, myID);
	if (_myInfo && nextMove == myID)
	{
		Utils::vibrateVeryShort();
		EnableButton(BTN_TIENLEN::BTN_COUNT, true);
		//GameController.Vibrate(1000);
		// IsAutoSelect = true;

		 bool have_card_available = CheckCardAviableFire();
		if (!have_card_available)
		{
			_autoSkip = true;
			EnableButton(BTN_TIENLEN::BTN_SKIP_CARD, true);
			EnableButton(BTN_TIENLEN::BTN_SORT_CARD, false);
			EnableButton(BTN_TIENLEN::BTN_FIRE_CARD, false);
			auto w_panel = p_btn_skip->GetWidgetChildByName(".eat_card_panel");
			if (!p_rect_process->isShowTimeAction()) {
				p_rect_process->startTime(TIME_SKIP, ([this, w_panel]() {
					w_panel->Visible(false);
					this->p_rect_process->stop();
					TienLenService::skip(_roomID, _boardID);

				}));
			}
		}
	}
	else
	{
		if (_myInfo && whoMove == myID)
		{
			stopTimeBar();
			if (_isStartGame)
			{
				EnableButton(BTN_TIENLEN::BTN_COUNT, false);
			}
			
		}
	}
	IsAutoSelect = true;
	//HideStartButton();
	//ShowButtonSoftCard(cards != null && cards.Count > 2);
	//IsAutoSelect = true;
}

void TienLenScreen::doFireWork()
{
	_selectedCardIDs.clear();
	for (Card* c : getMe()->cards){
		if (c->isSelected()){
			_selectedCardIDs.push_back((char)c->getID());
		}
	}
	TienLenService::move(_roomID, _boardID, _selectedCardIDs);


}

void TienLenScreen::doSkip(){
	TienLenService::skip(_roomID, _boardID);
}

void TienLenScreen::skip(int whoSkip, int nextMove, bool clearCard)
{
	_currentPlayer = nextMove;
	if (clearCard)
	{
		if (whoSkip == GameController::myInfo->IDDB )
		{
			this->isClearCurrentCard = clearCard;

		}
		else
		{
			stopTimeBar();
		}
		CleanLastCard();
		CleanFireCard();
		ResetReady();
		ResetAllSkip();
	}
	
	int seat = GetSeatFromPlayerID(whoSkip);
	CCLOG("SKIP: skip seat pos : %d , ID whoSkip : %d , MyID: %d", seat, whoSkip, GameController::myInfo->IDDB);
	PlayerGroup* pg = GetSeatGroup(seat);
	if (pg && !clearCard)
	{
		pg->stopTime();
		pg->setIsDenyTurn(true);
	}
	seat = GetSeatFromPlayerID(nextMove);

	CCLOG("SKIP: next move seat id : %d , ID moveNext: %d", seat , nextMove);

	PlayerGroup* pgnext = GetSeatGroup(seat);
	if (pgnext)
	{
		pgnext->startTime(_interval, 0);
	}
	if (seat == 0)
	{
		stopTimeBar();
	}
	if (_isStartGame && getMe() && getMe()->cards.size() > 0)
	{
		if (_currentPlayer == GameController::myInfo->IDDB)
		{
			EnableButton(BTN_TIENLEN::BTN_COUNT, true);
			
			if(clearCard && nextMove == GameController::myInfo->IDDB)
			{
				EnableButton(BTN_TIENLEN::BTN_SKIP_CARD, false);
			}
		}
		else
		{
			EnableButton(BTN_TIENLEN::BTN_COUNT, false);
		}
	}
	if (whoSkip == GameController::myInfo->IDDB)
	{
		if (getMe())
		{
			getMe()->unSelectAllCards();
		}
		//Nếu đang đếm ngược bỏ lượt thì stop
		//if (isCountingSkip)
		//{
		//	OnHideCountDown();
		//}
	}
	else if (nextMove == GameController::myInfo->IDDB)
	{
		//check auto skip this state
		auto have_card_available = doAutoSelect(true);
		if (have_card_available.size() <= 0)
		{
			EnableButton(BTN_TIENLEN::BTN_SKIP_CARD, true);
			EnableButton(BTN_TIENLEN::BTN_SORT_CARD, false);
			EnableButton(BTN_TIENLEN::BTN_FIRE_CARD, false);
		}
	}
	
	if (clearCard)
	{
		for (PlayerGroup* pg : p_list_player_group)
		{
			pg->setIsDenyTurn(false);
		}
	}

}

void TienLenScreen::onGetTimeLeftAutoStart(char timeStart)
{
	OnShowCountDown(timeStart, 0);
}

void TienLenScreen::OnClearCardAndStartNewLoad()
{
	//reset all card , only call once
	_list_card_create.clear();
	_number_card_remain_after_game = getMe()->cards.size();
	for (int i = 0; i < NUMBER_CARD_TL; i++)
	{
		if (i < _number_card_remain_after_game)
		{
			auto c = getMe()->cards[i];
			NODE(c)->stopAllActions();
			removeChildObject(NODE(c));
		}
		else
		{
			_list_card_create.push_back(-1);
		}
	}
	_load_card_finish = false;
	_number_card_remain_after_game = getMe()->cards.size();
}

bool TienLenScreen::CheckCardAviableFire()
{
	vector<unsigned char> curentFire = getcardFire();
	vector<Card*> listCards;
	for (Card* c : getMe()->cards)
	{
	Card* c1 = new Card();
	c1->SetID(c->getID(), DECK_TIENLEN);
	listCards.push_back(c1);
	}
	TienLenPlayer* playerTmp = new TienLenPlayer();
	playerTmp->cards = listCards;
	std::vector<unsigned char> foundAt = CardUtils::autoSelect(curentFire, playerTmp);
	for (Card* c : listCards)
	{
		delete c;
	}
	delete playerTmp;
	listCards.clear();
	playerTmp = nullptr;
	return foundAt.size() > 0;
}

void TienLenScreen::Finish(int whoFinish, char finishPos, int money, int exp)
{
	//ShowContentChat(whoFinish, L.Get(359) + " " + (finishPosition + 1));
	//doFinishGame();
	if (whoFinish == GameController::myInfo->IDDB && GameController::gameId != GameType::GAMEID_TIENLEN_SOLO)
	{
		EnableButton(BTN_TIENLEN::BTN_COUNT, false);
		//reset all card , only call once
		OnClearCardAndStartNewLoad();
	}
	int s = GetSeatFromPlayerID(whoFinish);
	if (s >= 0 && GameController::gameId != GameType::GAMEID_TIENLEN_SOLO)
	{
		p_list_player_group[s]->setRankNumber(finishPos);
		//p_list_player_group[s]->SetTextChat( "Thắng "  + std::to_string(finishPos + 1));
	}
	//RemoveAllImageFinish();

}

void TienLenScreen::ForceFinish(int whoFinish, std::vector<char> list_card, int typeForceFinish)
{
	_typeForceFinish = typeForceFinish;
	_id_whoForceFinish = whoFinish;
	_list_card_finish.clear();
	_list_card_finish = list_card;
}

void TienLenScreen::StopGame(int whoShow, std::vector<char> cardsShow)
{
	/*if (GameController::gameId != GameType::GAMEID_TIENLEN_SOLO)
	{
		ShowStartButton();
	}*/
	if (cardsShow.size() == 0)
	{
		OnClearCardAndStartNewLoad();
	}
	OnHideCountDown();
	EnableButton(BTN_TIENLEN::BTN_COUNT, false);
	ResetReady();
	doStopGame();
	needReset = true;
	p_effect_hl->Visible(false);
	bool have_force_finish = false;
	if (_list_card_finish.size() != 0)
	{
		setTextAndShowForceFinish(getNameForceFinish(_typeForceFinish), _id_whoForceFinish);
		if (_myInfo->GetIDDB() == _id_whoForceFinish && getMe() && getMe()->cards.size() > 0)
		{
			CleanMyCard();
			have_force_finish = true;
		}
		_list_card_finish.clear();
	}

	if (whoShow != -1)
	{
		this->PushEvent([this, cardsShow , have_force_finish](BaseScreen * scr) {
			CleanLastCard();
			if (!have_force_finish && GameController::gameId != GameType::GAMEID_TIENLEN_SOLO)
			{
				CleanMyCard();
			}

			float y = p_layout_fire_card->GetPosition_BottomLeft().y;
			float y1 = p_layout_fire_card->GetPosition_TopLeft().y;
			float firstX = p_layout_fire_card->GetPosition_BottomLeft().x - _delta_aligne.x;
			float endX = p_layout_fire_card->GetPosition_BottomRight().x + _delta_aligne.x;

			int idx = 0;
			for (auto s : cardsShow)
			{
				Card* c = Card::NewInstance(s, DECK_TYPE::DECK_TIENLEN);
				_list_end_card.push_back(c);
				idx++;
			}

			this->PushEvent([this, firstX, y, endX, y1](BaseScreen * scr) {
				if (_list_end_card.size() <= 0)
					return;
				int idx = 0;

				Vec2 size_card = Vec2(75.f * GetGameSize().width / this->GetDesignSize().x, 100.f *GetGameSize().width / this->GetDesignSize().x);

				if (_list_end_card.size() > 0)
				{
					size_card = _list_end_card[0]->GetSize();
				}
				else if (_list_fire_card.size() > 0)
				{
					size_card = _list_fire_card[0]->GetSize();
				}
				else if (getMe() && getMe()->cards.size() > 0)
				{
					size_card = getMe()->cards[0]->GetSize();
				}

				std::vector<Vec2> list_pos_move_to = CardUtils::AligneCardPosition(firstX, y, endX, _list_end_card.size(), size_card);
				for (auto c : _list_end_card)
				{
					addChildObject(NODE(c), 250 + idx);
					c->SetPosition(list_pos_move_to[idx]);
					idx++;
				}
				this->PushEvent([this, firstX, endX, y1 , size_card](BaseScreen * scr) {
					int idx = 0;
					auto list_pos_move_to1 = CardUtils::AligneCardPosition(firstX, y1, endX, _list_fire_card.size(), size_card);
					for (auto c : _list_fire_card)
					{
						Card* c1 = Card::NewInstance(c->getID(), DECK_TYPE::DECK_TIENLEN);
						addChildObject(NODE(c1), 150 + idx);
						c1->SetPosition(list_pos_move_to1[idx]);
						_list_end_card.push_back(c1);
						idx++;
					}

					CleanFireCard();


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
					else
					{
						PLAY_SOUND("SFX_VOTAY");
					}

				},0.1f);
			},0.1f);
		},0.1f);
	}

	_isFirstMatch = false;

	//TienLenModel.Instance.ShowHeighBetMoney(true);
	stopAllTimePlayer();
}

void TienLenScreen::HandleAfterInitPlayer()
{
	GameScreen::HandleAfterInitPlayer();
	if (_list_pos_swap_group.size()<=0)
	{
		_list_pos_swap_group.push_back(_WIDGET(p_list_player_group[1])->GetPosition());
		_list_pos_swap_group.push_back(_WIDGET(p_list_player_group[2])->GetPosition());
	}
	if (GameController::gameId == GameType::GAMEID_TIENLEN_SOLO)
	{
		p_list_player_group[1]->setVisible(false);
		p_list_player_group[3]->setVisible(false);
		p_list_player_group[1]->setPosition(_list_pos_swap_group[1]);
	}
	else
	{
		p_list_player_group[1]->setVisible(true);
		p_list_player_group[3]->setVisible(true);
		p_list_player_group[1]->setPosition(_list_pos_swap_group[0]);
	}
	for (int i = 0; i < NUMBER_CARD_TL; i++)
	{
		_list_card_create.push_back(-1);
	}
	_load_card_finish = false;
}

void TienLenScreen::OnReconnect(
	int interval, 
	int playerId, 
	std::vector<unsigned char> movedCards, 
	int next_user_id, 
	std::vector<unsigned char> myCards, 
	int timeLimit, 
	std::vector<int> userRank, 
	std::vector<unsigned char> posRank, 
	std::vector<bool> userSkip)
{

	onReconnect();

	_interval = interval;
	_delta_aligne = getMe()->cards[0]->GetSize() / 2.f;
	
	int delta_card = getMe()->cards.size() - myCards.size();
	PASSERT2(delta_card >= 0, "size 2 list error!");
	for (int i = 0; i < (int)myCards.size(); i++)
	{
		auto c = getMe()->cards[i];
		addChildObject(NODE(c));
		c->ResetID(myCards[i], DECK_TYPE::DECK_TIENLEN);
		c->SetVisible(true);
	}
	if (delta_card > 0)
	{
		for (auto i = myCards.size(); i < getMe()->cards.size(); i++)
		{
			auto c = getMe()->cards[i];
			NODE(c)->stopAllActions();
			removeChildObject(NODE(c));
			delete c;
		}
		getMe()->cards.erase(getMe()->cards.begin() + myCards.size(), getMe()->cards.begin() + myCards.size() + delta_card);
	}


	Move(playerId, movedCards, next_user_id);
	layoutMyCard();
	PlayerGroup* pg = getUserGroup(_currentPlayer);
	if (pg)
	{
		pg->startTime(interval, timeLimit);
	}
	
	// 1 ai do ve truoc
	for (auto p : p_list_player_group)
	{
		if (!(p 
			&& _PLAYER(p) 
			&& _PLAYER(p)->IDDB >= 0))
		{
			continue;
		}

		auto tl_p = _PLAYER(p);

		TienLenPlayer* ptl = static_cast<TienLenPlayer*>(tl_p);
		if (ptl->IDDB != -1)
		{
			for (size_t i = 0; i < userRank.size(); i++)
			{
				if (ptl->IDDB == userRank[i] && userRank[i] != -1)
				{
					ptl->finishPosition = posRank[i];
					//SetFinishStatus(GetSeatFromPlayerID(p->IDDB), p->finishPosition);
					ptl->isReady = false;
					//p->bSkip = userSkip[j];
					//finish[i] = userRank[j];
					break;
				}
			}
		}
	}
	if (getMe()->cards.size() > 0)
	{
		EnableButton(BTN_TIENLEN::BTN_COUNT, _currentPlayer == GameController::myInfo->IDDB);
	}

	if (getMe()->cards.size() <= 0)
	{
		_isViewer = true;
		showViewerLabel();
	}
	//ShowButtonSoftCard(true);
}

void TienLenScreen::EnableButton(BTN_TIENLEN btn_type, bool value)
{
	if (btn_type == BTN_TIENLEN::BTN_FIRE_CARD || btn_type == BTN_TIENLEN::BTN_COUNT)
	{
		p_btn_fire->Visible(value);
	}

	if (btn_type == BTN_TIENLEN::BTN_SORT_CARD || btn_type == BTN_TIENLEN::BTN_COUNT)
	{
		p_btn_sort->Visible(value);
	}

	if (btn_type == BTN_TIENLEN::BTN_SKIP_CARD || btn_type == BTN_TIENLEN::BTN_COUNT)
	{
		if (btn_type == BTN_TIENLEN::BTN_SKIP_CARD && value == false)
		{
			CCLOG("btn skip is hide!");
		}
		p_btn_skip->Visible(value);
		if (!value && p_rect_process)
		{
			p_btn_skip->GetWidgetChildByName(".eat_card_panel")->Visible(false);
			p_rect_process->stop();
		}
		else
		{

		}
	}
}

void TienLenScreen::dealCards()
{
	std::vector<cocos2d::Rect> lsPointOther;
	std::vector<Vec2>		list_end_point;
	for (PlayerGroup* pg : p_list_player_group)
	{
		if (pg != nullptr && 
			pg->getPlayer() != nullptr && 
			pg->getPlayer()->IDDB > 0 &&
			pg->getPlayer()->IDDB != GameController::myInfo->IDDB)
		{
			lsPointOther.push_back(pg->GetRectFront());
			list_end_point.push_back(_WIDGET(pg)->GetPosition_Middle());
		}
	}

	int idx = 0;
	float delay_time = 0.1f;
	auto w_avatar = _WIDGET(_myInfo);
	auto size_avatar = w_avatar->GetWidgetChildByName(".layout_player.bg_avatar")->GetSize();
	auto list_pos_move_to = CardUtils::AligneCardPosition(
		w_avatar->GetPosition_Middle().x + (size_avatar.x / 2) + 5  + _delta_aligne.x,
		p_panel_bottom->GetSize().y		   + 5  + _delta_aligne.y,
		GetGameSize().width				   - 10 + _delta_aligne.x,
		getMe()->cards.size(),
		getMe()->cards[0]->GetSize());

	PASSERT2(getMe()->cards.size() == list_pos_move_to.size(), "size of 2 list must be equal");


	std::map<int , std::vector<Vec2> > list_move_other;
	for (size_t i = 0 ; i < lsPointOther.size() ; i ++)
	{
		auto recta = lsPointOther[i];
		cocos2d::Size size_rect = recta.size;
		cocos2d::Vec2 origin_rect = recta.origin;
		bool isAligneX = (size_rect.width > size_rect.height);
		if (isAligneX)
		{
			auto list_pos_move_other = CardUtils::AligneCardPosition(
				origin_rect.x + _delta_aligne.x,
				origin_rect.y - (size_rect.height * getMe()->cards[0]->GetSize().y),
				size_rect.width + _delta_aligne.x,
				getMe()->cards.size(), getMe()->cards[0]->GetSize(), true);
			list_move_other.insert({ i ,list_pos_move_other });
		}
		else
		{
			auto list_pos_move_other = CardUtils::AligneCardPosition(
				origin_rect.y,
				origin_rect.x - (size_rect.width * getMe()->cards[0]->GetSize().x),
				size_rect.height,
				getMe()->cards.size(), getMe()->cards[0]->GetSize(), false);
			list_move_other.insert({ i ,list_pos_move_other });
				
		}
	}

	for (Card * c : getMe()->cards)
	{
		c->SetStatus(false);
		if (idx < getMe()->cards.size() - 1)
		{
			this->PushEvent([](BaseScreen * scr) {
				GetSound->PlaySoundEffect("SFX_GAMEBAI_CHIABAI", 0.f, 0, true);
			}, 0.1);
		}
		AnimUtils::RunEffectChiaBai(c , list_pos_move_to[idx] , delay_time * (idx + 1) , [this, c , idx]() 
		{
			if (this->getMe()->cards.size() > 0)
			{
				c->EventActionFinish();
				c->SetStatus(true);
				if (idx == getMe()->cards.size() - 1)
				{
					CheckCardSpecial();
					PLAY_SOUND("SFX_GAMEBAI_START");
				}	
			}
		});

		for (size_t i = 0; i < lsPointOther.size(); i++)
		{
			Card* cOther = Card::NewInstance(-2, DECK_TYPE::DECK_TIENLEN);
			addChildObject(NODE(cOther), 50);

			AnimUtils::RunEffectChiaBai1(
				cOther, 
				list_move_other.at(i)[idx], 
				delay_time * (idx + 1),
				list_end_point[i],
				delay_time * (getMe()->cards.size() - idx), CallFunc::create([cOther, this]()
			{
				cOther->EventActionFinish();
				this->removeChildObject(NODE(cOther));
				cOther->release();
			}));
		}
		idx++;
	}
}

void TienLenScreen::hideAllRank()
{
	for (PlayerGroup* p : p_list_player_group)
	{
		if (p ) 
		{
			if (p->getPlayer() != nullptr && p->GetIDDB() != -1)
			{
				p->HideRank();
			}
			else
			{
				p->ShowLayerPlayer(false);
			}
		}
	}
}

void TienLenScreen::ResetReady()
{
	GameScreen::ResetReady();
}

void TienLenScreen::setSeat(std::vector<Player*> l_players)
{
	if (GameController::gameId != GAMEID_TIENLEN_SOLO)
	{
		GameScreen::setSeat(l_players);
	}
	else
	{
		for (int i=0 ;i<seats.size() ;i++)
		{
			seats[i] = -1;
		}
		int myIndex = findMyIndex(l_players);
		seats[0] = myIndex;
		int i = 0;
		for (auto p : l_players)
		{
			if (p && !isMe(p->IDDB))
			{
				seats[2] = i;
				break;
			}
			i++;
		}
	}
}

void TienLenScreen::setPlayerSeat(std::vector<Player*> l_players)
{
	if (GameController::gameId != GAMEID_TIENLEN_SOLO)
	{
		GameScreen::setPlayerSeat(l_players);
	}
	else
	{
		_myInfo = p_list_player_group[0];
		for (int i = 0; i < seats.size(); i++)
		{
			PlayerGroup* pg = p_list_player_group[i];
			if (seats[i] != -1 && l_players[seats[i]]->IDDB >-1)
			{
				pg->setPlayer(l_players[seats[i]], i);
			}
			else
			{
				pg->setPlayer(nullptr, seats[i]);
			}
		}
	}
}

void TienLenScreen::ResetAllSkip()
{
	for (auto g : p_list_player_group)
	{
		g->stopTime();
		g->setIsDenyTurn(false);
	}
}

void TienLenScreen::CheckCardSpecial()
{
	OnShowEffectHL(-1,-1, false);
	this->PushEvent([this](BaseScreen * scr) {
		if (getMe()->cards.size() < 4 || !_isStartGame)
			return;

		vector<unsigned char> arrCard = CardUtils::findDoubleSequence(0, getMe()->cards, 4);
		if (arrCard.size() <= 0)
		{
			arrCard = CardUtils::findDoubleSequence(0, getMe()->cards, 3);
			if (arrCard.size() <= 0)
			{
				arrCard = CardUtils::findAndSelectQuart(0, getMe()->cards);
			}
		}
		if (arrCard.size() > 0)
		{
			CardUtils::sort(arrCard);
			for (int i = 0; i < arrCard.size(); i++)
			{
				if (getMe()->cards[arrCard[i]]->isSelected())
				{
					return;
				}
			}
			OnShowEffectHL(arrCard[0], arrCard[arrCard.size() - 1]);
		}
	});
	
	
}

void TienLenScreen::onSomeOneJoinBoard(int seat, Player * p)
{
	if (GameController::gameId != GAMEID_TIENLEN_SOLO)
	{
		GameScreen::onSomeOneJoinBoard(seat, p);
	}
	else
	{
		int otherSeat = 2;
		PLAY_SOUND("SFX_JOINBOARD");
		seats[otherSeat] = seat;
		addPlayer(seats[otherSeat], p);
		setPlayerSeat(p_list_player);
		if (_myInfo != nullptr)
		{
			setOwner(_ownerID);
		}
		//run effect money
		auto player_group = getPlayerGroup(p->IDDB);
		if (player_group && p)
		{
			player_group->SetActionMoneyRunTo(p->getMoney());
		}
		livePlayer();
		if (p_text_wait->Visible())
		{
			p_text_wait->Visible(false);
			p_text_wait->ForceFinishAction();
		}
	}
}
void TienLenScreen::onSomeOneLeaveBoard(int leave, int newOwner)
{
	GameScreen::onSomeOneLeaveBoard(leave, newOwner);
}

void  TienLenScreen::OnShowEffectHL(int start_card_idx, int end_card_idx , bool isShow)
{
	if (!isShow)
	{
		p_effect_hl->Visible(false);
		return;
	}

	if (!getMe())
		return;

	if (start_card_idx < 0 || start_card_idx >= getMe()->cards.size()
		|| end_card_idx < 0 || start_card_idx >= getMe()->cards.size()
		|| start_card_idx >= end_card_idx)
	{
		return;
	}

	float ratio_scale = GetGameSize().width / this->p_design_size.x;

	p_effect_hl->Visible(true);
	auto c = getMe()->cards[start_card_idx];
	Vec2 pos_card = getMe()->cards[start_card_idx]->GetPosition();
	Vec2 anchor_point = c->GetResource()->getAnchorPoint();
	Vec2 size_card = c->GetSize();
	if (_pos_y_hl <= 0)
	{
		_pos_y_hl = pos_card.y;
	}

	Vec2 pos_card_start = Vec2(pos_card.x - (size_card.x*anchor_point.x), _pos_y_hl);

	pos_card = getMe()->cards[end_card_idx]->GetPosition();
	Vec2 pos_card_end = Vec2(pos_card.x + (size_card.x*(1-anchor_point.x)), _pos_y_hl);

	p_effect_hl->GetResource()->setAnchorPoint(Vec2(0.5,0.5));
//	p_effect_hl->SetZOrder(getMe()->cards[end_card_idx]->GetResource()->getZOrder() + 1);
	float delta_distance = abs(pos_card_end.x - pos_card_start.x);
	Vec2 size_effect = Vec2(delta_distance, c->GetSize().y );
	p_effect_hl->SetPosition(Vec2(pos_card_start.x + size_effect.x / 2 , pos_card_start.y));
	p_effect_hl->SetSize(size_effect);

	auto w_hl_0 = p_effect_hl->GetWidgetChildAtIdx(0);
	w_hl_0->SetPosition(Vec2(size_effect.x / 2, size_effect.y / 2));
	w_hl_0->SetSize(Vec2(size_effect.x+size_card.x/5, size_effect.y*1.2f));

	auto w_start_grow =  p_effect_hl->GetWidgetChildAtIdx(3);
	w_start_grow->SetPosition(Vec2(10 , size_effect.y - (10 * ratio_scale)));
	w_start_grow->SetActionCommandWithName("ROTATE");

	auto w_top_line = p_effect_hl->GetWidgetChildAtIdx(1);
	w_top_line->ForceFinishAction();
	xml::BasicDec dec;
	dec.InsertDataVector2("SET_POSITION",Vec2(-(size_effect.x / 3), size_effect.y));
	dec.InsertDataVector2("POSITION", Vec2(size_effect.x, size_effect.y ));

	w_top_line->SetHookCommand("MOVE", "act0", 2, 0, dec, "SET_POSITION");
	w_top_line->SetHookCommand("MOVE", "act0", 2, 1, dec, "POSITION");
	w_top_line->SetActionCommandWithName("MOVE");
	w_top_line->ClearHookCommand("MOVE", "act0", 2);

	auto w_bot_line = p_effect_hl->GetWidgetChildAtIdx(2);
	w_bot_line->ForceFinishAction();
	dec.ReplaceDataVector2("SET_POSITION", Vec2(size_effect.x + (size_effect.x / 3), 0));
	dec.ReplaceDataVector2("POSITION", Vec2(0, 0));

	w_bot_line->SetHookCommand("MOVE", "act0", 2, 0, dec, "SET_POSITION");
	w_bot_line->SetHookCommand("MOVE", "act0", 2, 1, dec, "POSITION");
	w_bot_line->SetActionCommandWithName("MOVE");
	w_bot_line->ClearHookCommand("MOVE", "act0", 2);

}

RKString TienLenScreen::getNameForceFinish(int type)
{
	auto current_lang = LangMgr->GetCurLanguage();
	switch (current_lang)
	{
	case Utility::LANG_NONE:
		break;
	case Utility::LANG_VI:
	{
		switch ((TYPE_FORCE_FINISH)type)
		{
		case STRAIGHT_DRAGON:
			return "Sảnh Rồng";
		case SIX_PAIRS:
			return "6 Đôi";
		case FIVE_PAIRS_CONT:
			return "5 Đôi thông";
		case FOUR_HEO:
			return "Tứ Quý Heo";
		case FOUR_TRIPLE:
			return "4 Sám Cô";
		case THREE_PAIRS_CONT_WITH_3SPADE:
			return "3 Đôi Thông Có 3 Bích";
		case FOUR_THREE_CARDS:
			return "Tứ Quý 3";
		default:
			return "";
			break;
		}
	}
		break;
	case Utility::LANG_EN:
	{
		switch ((TYPE_FORCE_FINISH)type)
		{
		case STRAIGHT_DRAGON:
			return "Dragon's Head";
		case SIX_PAIRS:
			return "Six Pairs";
		case FIVE_PAIRS_CONT:
			return "Five Pairs Cont";
		case FOUR_HEO:
			return "Four 2s";
		case FOUR_TRIPLE:
			return "Four Triples";
		case THREE_PAIRS_CONT_WITH_3SPADE:
			return "3 Pairs Cont Have 3 Spade";
		case FOUR_THREE_CARDS:
			return "Four 3";
		default:
			return "";
			break;
		}
	}
		break;
	case Utility::LANG_COUNT:
		break;
	default:
		break;
	}
	return "";
}

void TienLenScreen::setTextAndShowForceFinish(RKString force_msg, int id)
{
	auto w_text = static_cast<TextWidget*>(p_layout_force_finish->GetWidgetChildByName(".title"));
	w_text->SetText(force_msg, true);

	int seat = GetSeatFromPlayerID(id);
	RKString name_action = "SET_POS_0";
	if (seat == 0)
	{
		name_action = "SET_POS_0";
	}
	else if (seat == 1)
	{
		name_action = "SET_POS_1";
	}
	else if (seat == 2)
	{
		name_action = "SET_POS_2";
	}
	else if (seat == 3)
	{
		name_action = "SET_POS_3";
	}

	p_layout_force_finish->SetActionCommandWithName(name_action, CallFunc::create([this]() {
		//p_layout_force_finish->ForceFinishAction();
		p_layout_force_finish->SetActionCommandWithName("ON_SHOW");
	}));
}

void TienLenScreen::doSomethingWhenIWin()
{
	GameScreen::doSomethingWhenIWin();
	PLAY_SOUND("SFX_GAMEBAI_THANGBAI");
}
void TienLenScreen::doSomethingWhenILose()
{
	GameScreen::doSomethingWhenIWin();
	PLAY_SOUND("SFX_GAMEBAI_THUABAI");
}

void TienLenScreen::doSomethingWhenIRaw()
{
	GameScreen::doSomethingWhenIRaw();
	PLAY_SOUND("SFX_VOTAY");
}

void TienLenScreen::onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney, RKString dec)
{
	if (player && player->GetIDDB() == GameController::myInfo->IDDB)
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
			else
			{
				PLAY_SOUND("SFX_VOTAY");
			}
		}
		//GetCommonScr->OnShowBonusMoney(value, false, 1);
	}
}
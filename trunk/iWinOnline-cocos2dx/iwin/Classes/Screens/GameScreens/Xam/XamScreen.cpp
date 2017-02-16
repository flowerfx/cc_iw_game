#include <algorithm>

#include "Screens/GamePlayMgr.h"
#include "AnimUtils.h"
#include "XamService.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/Object/RectProcessTime.h"
#include "Screens/Object/PlayerGroup.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wButton.h"

#include "XamScreen.h"
#include "../TienLen/TienLenPlayer.h"
#include "Screens/Object/TimeBarProcess.h"


#define CARD_SELECT_OP  100
#define TIME_SKIP  5
XamScreen* XamScreen::getInstance()
{
	if (!GamePlay)
		return nullptr;

	return static_cast<XamScreen*>(GamePlay->GetCurrentGameScreen());
}

XamScreen::XamScreen()
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
	_forceMoveSmallest = false;
	_isFirstMatch = true;
	p_layout_fire_card = nullptr;
	_delta_aligne = Vec2(0, 0);
	_load_card_finish = false;
	_number_card_remain_after_game = 0;
	_list_card_finish.clear();
}

XamScreen::~XamScreen()
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
	if (p_timebar_dialog_xam)
		delete p_timebar_dialog_xam;
	p_timebar_dialog_xam = nullptr;
	clearDenBai();
}

void XamScreen::createUI()
{
	p_rect_process = new RectProcessTime(p_btn_skip);

}

void XamScreen::CleanMyCard(bool cleanup)
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

void XamScreen::CleanFireCard()
{
	for (auto c : _list_fire_card)
	{
		NODE(c)->stopAllActions();
		removeChildObject(NODE(c));
		delete c;
	}

	_list_fire_card.clear();
}

void XamScreen::CleanLastCard()
{
	for (auto c : _list_last_card)
	{
		NODE(c)->stopAllActions();
		removeChildObject(NODE(c));
		delete c;
	}
	_list_last_card.clear();
}

void XamScreen::CleanEndCard()
{
	for (auto c : _list_end_card)
	{
		NODE(c)->stopAllActions();
		removeChildObject(NODE(c));
		delete c;
	}

	_list_end_card.clear();
}

void XamScreen::doFireWork()
{
	_selectedCardIDs.clear();
	for (Card* c : getMe()->cards) {
		if (c->isSelected()) {
			_selectedCardIDs.push_back((char)c->getID());
		}
	}
	XamService::Move(_roomID, _boardID, _selectedCardIDs);

}

void XamScreen::addLastCard(std::vector<Card*> lsNode)
{
	int idx = 0;
	for (Card * c : lsNode)
	{
		NODE(c)->setZOrder(175 + idx);
		c->SetActionCommandWithName("MOVE_CENTER_BACK", nullptr, true);
		_list_last_card.push_back(c);
		//c->showTrans();
		idx++;
	}
}

void XamScreen::runCardFire(std::vector<unsigned char> cards, int whoMove)
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
			float y = p_layout_fire_card->GetPosition_BottomLeft().y + _delta_aligne.y;
			float firstX = p_layout_fire_card->GetPosition_BottomLeft().x + _delta_aligne.x;
			float endX = p_layout_fire_card->GetPosition_BottomRight().x + _delta_aligne.x;


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

				AnimUtils::RunEffectMoveToFire(c, c->GetPosition(), list_pos_move_to[idx], [c, idx, size_list_card_move, this, have_chat_heo]() {
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
		float y = p_layout_fire_card->GetPosition_BottomLeft().y + _delta_aligne.y;
		float firstX = p_layout_fire_card->GetPosition_BottomLeft().x + _delta_aligne.x;
		float endX = p_layout_fire_card->GetPosition_BottomRight().x + _delta_aligne.x;
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
						AnimUtils::RunEffectMoveToFire(c, userG->GetPosition_Middle(), list_pos_move_to[idx], [c, idx, size_list_card_move, this, have_chat_heo]() {
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



void XamScreen::showResult(vector<int> userIds, vector<int> penalty, vector<vector<byte>> cardsShow)
{

}

void XamScreen::clearDenBai()
{
	for (DenBai* d : _list_den_bai)
	{
		delete d;
	}
	_list_den_bai.clear();
}

void XamScreen::OnTouchMenuEnd(const cocos2d::Point & p)
{

	if (!getMe() || getMe()->cards.size() <= 0 || !_isStartGame)
		return;

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
				//TienLenService::move(_roomID, _boardID, _selectedCardIDs);
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
			if (!getMe() || !IsAutoSelect || lsCardSelects.size() <= 0) {
				return;
			}

			if (GameController::gameId != GameType::GAMEID_TIENLEN_MB)
			{
				vector<Card*> listCards;
				if (_list_card_select_action.size() > 0 && _list_card_select_action[0] == lsCardSelects[0])// giam dan
				{
					for (Card* c : getMe()->cards)
					{
						if (_list_card_select_action[0] >= c->getID())
						{
							Card* c1 = new Card();
							c1->SetID(c->getID(), DECK_TIENLEN);
							listCards.push_back(c1);
						}

					}
				}
				else  if (_list_card_select_action.size() > 0 && _list_card_select_action[0] == lsCardSelects[lsCardSelects.size() - 1])// tang dan
				{
					for (Card* c : getMe()->cards)
					{
						if (_list_card_select_action[0] <= c->getID())
						{
							Card* c1 = new Card();
							c1->SetID(c->getID(), DECK_TIENLEN);
							listCards.push_back(c1);
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
					selectAuto = CardUtils::autoWhenMultiSelectTLMN(playerTmp, lsCardSelects, getcardFire());

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

				if (selectAuto.size() > 0) {
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

void XamScreen::OnTouchMenuHold(const cocos2d::Point & p)
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


				for (int i = 0; i < p_list_card_on_selected.size(); i++)
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

void XamScreen::ActionTouchBeginObject(Utility::wObject::wBasicObject * obj)
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
		_list_card_select_action.push_back(c->getID());
	}
	else
	{
		_list_card_select_action.clear();
	}
}

void XamScreen::ActionTouchMoveObject(Utility::wObject::wBasicObject * obj)
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


std::vector<unsigned char> XamScreen::doAutoSelect(bool auto_skip)
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

		// ko chon duoc la nao thi se hien bo luot
		_currentPlayer = -1;
		_forceMoveSmallest = false;
		auto w_panel = p_btn_skip->GetWidgetChildByName(".eat_card_panel");
		if (!p_rect_process->isShowTimeAction()) {
			p_rect_process->startTime(TIME_SKIP, ([this, w_panel, auto_skip]() {
				if (auto_skip)
				{
					w_panel->Visible(false);
					this->p_rect_process->stop();
					//TienLenService::skip(_roomID, _boardID);
				}

			}));
		}

		if (auto_skip)
		{
			w_panel->ForceFinishAction();
			w_panel->Visible(true);
			w_panel->SetActionCommandWithName("FOCUS_ACTION");
		}
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

std::vector<unsigned char> XamScreen::getcardFire()
{
	vector<unsigned char> curentFire;

	for (auto c : _list_fire_card)
	{
		curentFire.push_back(c->getID());
	}
	return curentFire;
}

void XamScreen::startPlay()
{
	p_dialog_xam->Visible(false);
	if (isMe(_currentPlayer))
	{
		EnableButton(BTN_XAM::BTN_FIRE_CARD, true);
		EnableButton(BTN_XAM::BTN_SKIP_CARD, false);
		EnableButton(BTN_XAM::BTN_SORT_CARD, true);
	}
	else
	{
		EnableButton(BTN_XAM::BTN_COUNT, false);

	}
	/*UserGroup pg = GetUserGroup(currentPlayer);
	if (pg != null)
	{
		pg.StartTime(interval, 0);
	}
	ShowWaitingPlayerXam(false);*/
}

bool XamScreen::InitGame(int game_id)
{
	bool res = GameScreen::InitGame(game_id);

	XMLMgr->OnLoadXMLData<XamScreen>("Xam_Screen", std::mem_fn(&XamScreen::LoadXMLGameScreenCustom), this);
	onCreateListPlayer("Xam_Screen");

	p_panel_bottom = p_panel_custom->GetWidgetChildByName(".back_panel_card");
	PASSERT2(p_panel_bottom != nullptr, "p_panel_bottom is null , recheck the path");

	auto lay_btn_tl = p_panel_bottom->GetWidgetChildByName(".layout_btn_game_tlmn");
	p_btn_fire = lay_btn_tl->GetWidgetChildByName(".btn_deal_turn");
	p_btn_skip = lay_btn_tl->GetWidgetChildByName(".btn_leave_turn");
	p_btn_sort = lay_btn_tl->GetWidgetChildByName(".btn_quick_choose_turn");


	p_layout_fire_card = p_panel_custom->GetWidgetChildByName(".layout_fire_card");
	p_animate_1 = p_panel_custom->GetWidgetChildByName(".animate_effect_1");
	p_animate_2 = p_panel_custom->GetWidgetChildByName(".animate_effect_2");
	createUI();
	p_img_xam = p_panel_custom->GetWidgetChildByName(".image_xam");
	p_dialog_xam = p_panel_custom->GetWidgetChildByName(".dialog_xam");
	auto pr = p_dialog_xam->GetWidgetChildByName(".panel_contain.layout_.progress_bar_panel.progress_bar_dialog_xam");
	p_timebar_dialog_xam = new TimeBarProcess(pr);
	p_label_waiting_xam= p_panel_custom->GetWidgetChildByName(".title_wait_xam");
	p_img_one_card = p_panel_custom->GetWidgetChildByName(".image_one_card");
	_myResult = 0;
	_pos_y_hl = -1;
	return true;
}

void XamScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON && name == "btn_leave_turn")
	{
		if (p_rect_process&& p_rect_process->isShowTimeAction())
		{
			XamService::Skip(_roomID, _boardID);
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
					c->SelectCardAction(true, false);
					break;
				}
				else
				{
					c->SelectCardAction(false, false);
				}
			}
			idx++;
		}
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "ok_2_btn")
	{
		XamService::baoXam(_roomID, _boardID);
	}
	else if (type_widget == UI_TYPE::UI_BUTTON && name == "cancel_2_btn")
	{
		XamService::skipXam(_roomID, _boardID);
	}
	else
	{
		GameScreen::OnProcessWidgetChild(name, type_widget, _widget);
	}
}

int XamScreen::Update(float dt)
{
	if (GameScreen::Update(dt) == 0)
	{
		return 0;
	}
	if (p_timebar_dialog_xam && p_dialog_xam->IsVisible())
	{
		p_timebar_dialog_xam->update(dt);
	}
	if (!_load_card_finish)
	{
		if (_list_card_create.size() > 0)
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

void XamScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	auto p_list_sample_dec = LoadWidgetCustom(p_objectXML);
	PASSERT2(p_list_sample_dec.size() > 0, "no widget loaded!");
	//
	ParseIntoCustom(p_list_sample_dec.at("back_panel_card"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("layout_fire_card"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("animate_effect_1"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("animate_effect_2"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("dialog_xam"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("image_xam"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("dialog_xam"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("title_wait_xam"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("image_one_card"), p_panel_custom, "panel_custom_menu");
	ParseIntoCustom(p_list_sample_dec.at("layout_money_win"), p_panel_custom, "panel_custom_menu");
	for (auto it : p_list_sample_dec)
	{
		delete it.second;
	}
	p_list_sample_dec.clear();
}

/// để tạo cho thằng choi nó ngồi ấy mà
void XamScreen::resetSeat()
{

}

void XamScreen::start(int whoMoveFirst, char interval, std::vector<unsigned char> myCards)
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
	for (auto mc : myCards) {
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
	this->_interval = interval;
	_currentPlayer = whoMoveFirst;
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
			EnableButton(BTN_XAM::BTN_COUNT, true);
			EnableButton(BTN_XAM::BTN_SKIP_CARD, false);
			//IsAutoSelect = true;
		}
		else
		{
			EnableButton(BTN_XAM::BTN_COUNT, false);
		}
	}

	pg->RunTurnEffect();

}

TienLenPlayer* XamScreen::getMe()
{
	return static_cast<TienLenPlayer*>(getMyPlayer());
}

void XamScreen::resetGame()
{
	CleanMyCard();
	CleanFireCard();
	CleanLastCard();
	CleanEndCard();

	EnableButton(BTN_XAM::BTN_COUNT, false);

	stopTimeBar();
	IsAutoSelect = false;
	stopAllTimePlayer();
	hideAllRank();
	OnShowEffectHL(-1, -1, false);
	_list_card_select_action.clear();
	p_img_xam->SetVisible(false);
	p_dialog_xam->SetVisible(false);
	p_img_one_card->SetVisible(false);
	clearDenBai();
	//demo
	//EnableButton(BTN_XAM::BTN_FIRE_CARD, true);
}

void XamScreen::InitPlayerChild(xml::UILayerWidgetDec* xml_dec, xml::BasicDec * dec, int idx)
{
	GameScreen::InitPlayerChild(xml_dec, dec, idx);
	
}

void XamScreen::OnLoadDecPrivatePlayer(xml::BasicDec * dec, PlayerGroup * _player)
{
	Vec2 pos = dec->GetDataVector2("pos_img_xam");
	pos.x = pos.x * GetGameSize().width / this->GetDesignSize().x;
	pos.y = pos.y * GetGameSize().height / this->GetDesignSize().y;
	Vec2 pos_one = dec->GetDataVector2("pos_img_one_car");
	pos_one.x = pos_one.x * GetGameSize().width / this->GetDesignSize().x;
	pos_one.y = pos_one.y * GetGameSize().height / this->GetDesignSize().y;
	_list_pos_img_one_card.push_back(pos_one);
	_list_pos_img_xam.push_back(pos);
}

void XamScreen::layoutMyCard()
{
	if (!getMe() || getMe()->cards.size() <= 0)
		return;

	auto w_avatar = _WIDGET(_myInfo);
	auto size_avatar = w_avatar->GetWidgetChildByName(".layout_player.bg_avatar")->GetSize();
	int idx = 0;
	auto list_pos = CardUtils::AligneCardPosition(
		w_avatar->GetPosition_Middle().x + (size_avatar.x / 2) + 5 + _delta_aligne.x,
		p_panel_bottom->GetSize().y + 5 + _delta_aligne.y,
		GetGameSize().width - 10 + _delta_aligne.x,
		getMe()->cards.size(),
		getMe()->cards[0]->GetSize());

	for (Card* c : getMe()->cards)
	{
		NODE(c)->setAnchorPoint(Vec2(0.5, 0.5));
		c->SelectCardAction(false, false);
		CallFunc *  func = nullptr;
		AnimUtils::RunEffectLayout(c, list_pos[idx], func);
		idx++;
	}

}
void XamScreen::onGetTimeLeftAutoStart(char timeStart)
{
	OnShowCountDown(timeStart, 0);
}

void XamScreen::OnClearCardAndStartNewLoad()
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

bool XamScreen::CheckCardAviableFire()
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

void XamScreen::EnableButton(BTN_XAM btn_type, bool value)
{
	if (btn_type == BTN_XAM::BTN_FIRE_CARD || btn_type == BTN_XAM::BTN_COUNT)
	{
		p_btn_fire->Visible(value);
	}

	if (btn_type == BTN_XAM::BTN_SORT_CARD || btn_type == BTN_XAM::BTN_COUNT)
	{
		p_btn_sort->Visible(value);
	}

	if (btn_type == BTN_XAM::BTN_SKIP_CARD || btn_type == BTN_XAM::BTN_COUNT)
	{
		if (btn_type == BTN_XAM::BTN_SKIP_CARD && value == false)
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

void XamScreen::dealCards()
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
		w_avatar->GetPosition_Middle().x + (size_avatar.x / 2) + 5 + _delta_aligne.x,
		p_panel_bottom->GetSize().y + 5 + _delta_aligne.y,
		GetGameSize().width - 10 + _delta_aligne.x,
		getMe()->cards.size(),
		getMe()->cards[0]->GetSize());

	PASSERT2(getMe()->cards.size() == list_pos_move_to.size(), "size of 2 list must be equal");


	std::map<int, std::vector<Vec2> > list_move_other;
	for (size_t i = 0; i < lsPointOther.size(); i++)
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
		AnimUtils::RunEffectChiaBai(c, list_pos_move_to[idx], delay_time * (idx + 1), [this, c, idx]()
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

void XamScreen::hideAllRank()
{
	for (PlayerGroup* p : p_list_player_group)
	{
		if (p)
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

void XamScreen::ResetReady()
{
	GameScreen::ResetReady();
}

void XamScreen::setSeat(std::vector<Player*> l_players)
{
	if (GameController::gameId != GAMEID_TIENLEN_SOLO)
	{
		GameScreen::setSeat(l_players);
	}
	else
	{
		for (int i = 0; i < seats.size(); i++)
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

void XamScreen::setPlayerSeat(std::vector<Player*> l_players)
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
			if (seats[i] != -1 && l_players[seats[i]]->IDDB > -1)
			{
				pg->setPlayer(l_players[seats[i]], i);
			}
			else
			{
				pg->setPlayer(nullptr, i);
			}
		}
	}
}

void XamScreen::ResetAllSkip()
{
	for (auto g : p_list_player_group)
	{
		g->stopTime();
		g->setIsDenyTurn(false);
	}
}

void XamScreen::CheckCardSpecial()
{
	OnShowEffectHL(-1, -1, false);
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

void XamScreen::onSomeOneJoinBoard(int seat, Player * p)
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
		addPlayer(otherSeat, p);
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
void XamScreen::onSomeOneLeaveBoard(int leave, int newOwner)
{
	GameScreen::onSomeOneLeaveBoard(leave, newOwner);
}

void  XamScreen::OnShowEffectHL(int start_card_idx, int end_card_idx, bool isShow)
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
	Vec2 pos_card_end = Vec2(pos_card.x + (size_card.x*(1 - anchor_point.x)), _pos_y_hl);

	p_effect_hl->GetResource()->setAnchorPoint(Vec2(0.5, 0.5));
	//	p_effect_hl->SetZOrder(getMe()->cards[end_card_idx]->GetResource()->getZOrder() + 1);
	float delta_distance = abs(pos_card_end.x - pos_card_start.x);
	Vec2 size_effect = Vec2(delta_distance, c->GetSize().y);
	p_effect_hl->SetPosition(Vec2(pos_card_start.x + size_effect.x / 2, pos_card_start.y));
	p_effect_hl->SetSize(size_effect);

	auto w_hl_0 = p_effect_hl->GetWidgetChildAtIdx(0);
	w_hl_0->SetPosition(Vec2(size_effect.x / 2, size_effect.y / 2));
	w_hl_0->SetSize(Vec2(size_effect.x + size_card.x / 5, size_effect.y*1.2f));

	auto w_start_grow = p_effect_hl->GetWidgetChildAtIdx(3);
	w_start_grow->SetPosition(Vec2(10, size_effect.y - (10 * ratio_scale)));
	w_start_grow->SetActionCommandWithName("ROTATE");

	auto w_top_line = p_effect_hl->GetWidgetChildAtIdx(1);
	w_top_line->ForceFinishAction();
	xml::BasicDec dec;
	dec.InsertDataVector2("SET_POSITION", Vec2(-(size_effect.x / 3), size_effect.y));
	dec.InsertDataVector2("POSITION", Vec2(size_effect.x, size_effect.y));

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

RKString XamScreen::getNameForceFinish(int type)
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


void XamScreen::doSomethingWhenIWin()
{
	GameScreen::doSomethingWhenIWin();
	PLAY_SOUND("SFX_GAMEBAI_THANGBAI");
}
void XamScreen::doSomethingWhenILose()
{
	GameScreen::doSomethingWhenIWin();
	PLAY_SOUND("SFX_GAMEBAI_THUABAI");
}

void XamScreen::doSomethingWhenIRaw()
{
	GameScreen::doSomethingWhenIRaw();
	PLAY_SOUND("SFX_VOTAY");
}

void XamScreen::onHandleBonusMoney(PlayerGroup * player, int value, s64 currMoney, RKString dec)
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
void XamScreen::HandleAfterInitPlayer()
{
	GameScreen::HandleAfterInitPlayer();
	if (_list_pos_swap_group.size() <= 0)
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
#pragma region all process client - server






void XamScreen::Start(int whoMoveFirst, byte interval, vector<unsigned char> myCards)
{
	removeChildObject(nullptr, true);
	doStartGame();
	OnHideCountDown();
	resetGame();
	CleanMyCard();
	CleanFireCard();
	CleanLastCard();
	CleanEndCard();
	ResetReady();
	ResetAllSkip();
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

	//getMe()->remain_cards = myCards;
	for (auto mc : myCards) {
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
	_isStartGame = true;
	_interval = interval;
	_currentPlayer = whoMoveFirst;

	stopTimeBar();

	if (getMe()->cards.size() > 0)
	{
		if (isMe(_currentPlayer))
		{
			EnableButton(BTN_FIRE_CARD, true);
			EnableButton(BTN_COUNT, true);
			EnableButton(BTN_SKIP_CARD, true);
		
		}
		else
		{
			EnableButton(BTN_FIRE_CARD, false);
			EnableButton(BTN_COUNT, true);
			EnableButton(BTN_SKIP_CARD, false);
		}
	}
	onChat(_currentPlayer, getPlayer(_currentPlayer)->name + LangMgr->GetString("first_fire").GetString());
	/*HideAllFlareWin();
	ResetAllReady();
	HideStartButton();
	ShowContentChat(currentPlayer, GetPlayer(currentPlayer).name + L.Get(117));*/

}
void XamScreen::OnReconnect(int interval, int playerId, vector<byte> movedCards, int next_user_id, vector<byte> myCards, int timeLimit, vector<int> userRank, vector<bool> userSkip, int xamUserId, byte timeXamRemain)
{

}


void XamScreen::onMove(int whoMove, vector<unsigned char> cards, int nextMove)
{
	
	int seat = GetSeatFromPlayerID(whoMove);
	CCLOG("MOVE: who move skip id : %d", whoMove);
	seat = GetSeatFromPlayerID(nextMove);
	CCLOG("MOVE: nextMove skip id : %d", nextMove);

	if (_interval <= 0) {
		_interval = 20;
	}

	if (_list_fire_card.size() > 0) {
		CleanLastCard();
		addLastCard(_list_fire_card);
		_list_fire_card.clear();
	}

	if (_myInfo && whoMove == _myInfo->getPlayer()->IDDB) {
		//	layoutMyCard();
		EnableButton(BTN_XAM::BTN_COUNT, false);
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
	_currentPlayer = nextMove;
	PlayerGroup* group = getUserGroup(whoMove);
	if (group) {
		group->stopTime();
	}

	auto groupNext = getUserGroup(nextMove);
	if (groupNext) {
		groupNext->startTime((_interval == 0 ? 30 : _interval), 0);
		groupNext->RunTurnEffect();
	}
	int myID = (_myInfo ? _myInfo->getPlayer()->IDDB : -1);
	CCLOG("next move ID: %d , myID: %d", nextMove, myID);
	if (_myInfo && nextMove == myID)
	{
		EnableButton(BTN_XAM::BTN_COUNT, true);
		Utils::vibrateVeryShort();
		// IsAutoSelect = true;

		bool have_card_available = CheckCardAviableFire();
		if (!have_card_available)
		{
			_autoSkip = true;
			EnableButton(BTN_XAM::BTN_SKIP_CARD, true);
			EnableButton(BTN_XAM::BTN_SORT_CARD, false);
			EnableButton(BTN_XAM::BTN_FIRE_CARD, false);
			auto w_panel = p_btn_skip->GetWidgetChildByName(".eat_card_panel");
			if (!p_rect_process->isShowTimeAction()) {
				p_rect_process->startTime(TIME_SKIP, ([this, w_panel]() {
					w_panel->Visible(false);
					this->p_rect_process->stop();
					XamService::Skip(_roomID, _boardID);

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
				EnableButton(BTN_XAM::BTN_COUNT, false);
			}

		}
	}
	IsAutoSelect = true;
}

void XamScreen::OnHaveOneCard(int idUser)
{
	int s = GetSeatFromPlayerID(idUser);
	if (s < 0)
		return;
	p_img_one_card->SetPosition(_list_pos_img_one_card[s]);
	p_img_one_card->SetVisible(true);

}

void XamScreen::OnDenBai(int idUser, vector<ubyte> lsCardIds)
{
	clearDenBai();
	_list_den_bai.push_back(new DenBai(idUser, lsCardIds));
}

void XamScreen::OnSkip(int whoSkip, int nextMove, bool clearCard)
{
	_currentPlayer = nextMove;
	if (clearCard)
	{
		if (whoSkip == GameController::myInfo->IDDB)
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

	CCLOG("SKIP: next move seat id : %d , ID moveNext: %d", seat, nextMove);

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
			EnableButton(BTN_XAM::BTN_COUNT, true);

			if (clearCard && nextMove == GameController::myInfo->IDDB)
			{
				EnableButton(BTN_XAM::BTN_SKIP_CARD, false);
			}
		}
		else
		{
			EnableButton(BTN_XAM::BTN_COUNT, false);
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
			EnableButton(BTN_XAM::BTN_SKIP_CARD, true);
			EnableButton(BTN_XAM::BTN_SORT_CARD, false);
			EnableButton(BTN_XAM::BTN_FIRE_CARD, false);
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

void XamScreen::OnStopGame(byte roomID16, byte boardID16, int winnerID, vector<int> userIds, vector<int> penalty, vector<vector<byte>> cardsShow)
{
	
	EnableButton(BTN_COUNT,false);
	/*HideAutoButton();
	HideSkipButton();
	ShowFlareWinner(whoShow);
	XamModel.Instance.ShowHeighBetMoney(true);
	if (forceFinishInfo != null)
	{
		ShowContentChat(forceFinishInfo.owner, L.Get(120));
		forceFinishInfo = null;
	}*/
	_isGameEnd = true;
	if (isMe(_currentPlayer))
	{
		stopTimeBar();
	}
	PlayerGroup* p1 = GetSeatGroup(_currentPlayer);
	if (p1)
	{
		p1->stopTime();
	}
	/*if (myResult > 0)
	{
		SoundManager.Instance.PlayEffect(Sounds.THANGBAI);
		myResult = 0;
	}
	else if (myResult < 0)
	{
		SoundManager.Instance.PlayEffect(Sounds.THUABAI);
		myResult = 0;
	}*/

	//vector<vector<byte>> listCard;
	//vector<Player> listUser = new vector<Player>();
	//for (int i = 0; i < userIds.Length; i++)
	//{
	//	Player p = GetPlayer(userIds[i]);
	//	if (p != null)
	//	{
	//		listUser.Add(p);
	//		listCard.Add(cards[i]);
	//	}
	//}
	//
	//ShowDialogResult(listUser, penalty, listCard);
	//// show dialog ket thuc
	p_img_one_card->SetVisible(false);
	p_img_xam->SetVisible(false);
}

void XamScreen::ShowDialogConfirmXam(byte timeRemain, byte timeremain)
{
	p_dialog_xam->SetVisible(true);
	p_timebar_dialog_xam->onStartime(timeRemain, timeremain);
}

void XamScreen::OnGetTimeLeft(byte timeRemain)
{
	if (timeRemain > 0)
	{
		OnShowCountDown(timeRemain, 0);
	}
	else
	{
		OnHideCountDown();
	}
	_isStartGame = false;
	_isGameEnd = false;
}

void XamScreen::Xam(int whoXam)
{
	int s = GetSeatFromPlayerID(whoXam);
	if (s < 0 || _list_pos_img_xam.size() <= s)
		return;

	p_img_xam->SetPosition(_list_pos_img_xam[s]);
	p_img_xam->SetVisible(true);
	p_dialog_xam->SetVisible(false);
}

void XamScreen::SkipXam(int whoMoveFirst)
{
	startPlay();
}

#pragma endregion

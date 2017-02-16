#include "CardPhomBox.h"
#include "PhomScreen.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/Object/SpecialCardsBound.h"
#include "Screens/GameScreens/DealCardLayer.h"
#include "Screens/GameScreens/MauBinh/LabelBG.h"

#include "UI/AdvanceUI/wText.h"
extern int s_card_h;
extern int s_card_w;

#define PHOM_FIRE_CARD_SCALE 0.8f
#define PHOM_FREE_CARD_SCALE 0.9f

#define PHOM_FIRE_CARD_OVERLAP 0.5f
#define PHOM_FIRE_CARD_OVERLAP 0.5f

#define ZODER_FREE_CARD 300
#define ZODER_PHOM_CARD 200
#define ZODER_FIRE_CARD 100

void CardPhomBox::init(PhomScreen* game_screen, int seat)
{
	_game_screen = game_screen;

	setSeat(seat);
	if (seat == 0) _card_size = Vec2(s_card_w, s_card_h);
	else _card_size = Vec2(s_card_w * PHOM_FREE_CARD_SCALE, s_card_h * PHOM_FREE_CARD_SCALE);

	_result_info = nullptr;
	_result_score = nullptr;
}

void CardPhomBox::addFreeCard(int cardId)
{
	if (checkExist(_freeCards, cardId)) return;
    
//    if(cardId == 255)
//    {
//        //here crash incase tai vong B eat A, A eat B and B eat again
//        int i = 0;
//    }
    
	Card* c = _game_screen->getCard(cardId);
	c->SetSize(_card_size);
	//handle this to make free card stand front all
	if (_seat == 2)
	{
		c->GetResource()->setLocalZOrder(_freeCards.size() + ZODER_PHOM_CARD);
	}
	else
	{
		c->GetResource()->setLocalZOrder(_freeCards.size() + ZODER_FREE_CARD);
	}
    c->SetTouchEnable(true);
	_freeCards.push_back(c);
    
//    Card* c = _game_screen->getCard(cardId);
//    c->SetTouchEnable(true);
//    addCard(cardId, FLOOR_FREE);
}

void CardPhomBox::addPhomCard(int cardId)
{
	if (checkExist(_phomCards, cardId)) return;
    
	Card* c = _game_screen->getCard(cardId);
	c->SetSize(_card_size);
	if (_seat == 2)
	{
		c->GetResource()->setLocalZOrder(_phomCards.size() + ZODER_FREE_CARD);
	}
	else
	{
		c->GetResource()->setLocalZOrder(_phomCards.size() + ZODER_PHOM_CARD);
	}
	_phomCards.push_back(c);
    
//    addCard(cardId, FLOOR_PHOM);
}

void CardPhomBox::addEatCard(int cardId)
{
	if (checkExist(_eatCards, cardId)) return;
    
	Card* c = _game_screen->getCard(cardId);
	if (_seat == 0)
        c->SetSize(Vec2(s_card_w, s_card_h));
	else
        c->SetSize(Vec2(s_card_w * PHOM_FREE_CARD_SCALE, s_card_h * PHOM_FREE_CARD_SCALE));
	c->GetResource()->setLocalZOrder(_eatCards.size() + 10);
	_eatCards.push_back(c);
}

void CardPhomBox::addFireCard(int cardId)
{
	if (checkExist(_firedCards, cardId)) return;

	Card* c = _game_screen->getCard(cardId);
	c->SetSize(Vec2(s_card_w * PHOM_FIRE_CARD_SCALE, s_card_h * PHOM_FIRE_CARD_SCALE));
	c->GetResource()->setLocalZOrder(_firedCards.size() + ZODER_FIRE_CARD);
	_firedCards.push_back(_game_screen->getCard(cardId));
    
//    addCard(cardId, FLOOR_FIRED);
}

void CardPhomBox::addCard(int cardID, int floor)
{
    float scale = floor == FLOOR_FIRED ? PHOM_FIRE_CARD_SCALE : 1.0;
    if(checkExist(getList(floor), cardID)) return;
    
    Card* c = _game_screen->getCard(cardID);
    c->SetSize(Vec2(s_card_w * scale, s_card_h * scale));
    c->GetResource()->setLocalZOrder((int)getList(floor).size());
    //need reset card ???
    c->showHighlight(false);
    c->GetResource()->setColor(Color3B::WHITE);
    NODE(c)->setTag(0);
    c->SetVisible(true);
    
    K_VEC_REMOVE(_freeCards, c);
    K_VEC_REMOVE(_firedCards, c);
    K_VEC_REMOVE(_phomCards, c);
    
    getList(floor).push_back(c);
    int test = 0;
}

void CardPhomBox::layoutFreeCard()
{
    for (Card* c : _freeCards) {
		if (c)
		{
			c->SetVisible(true);
			c->SelectCardAction(false, false);
		}
    }
    
	if (_seat == 0)
	{
		CardUtils::layoutListCardCenter(_freeCards, _posFree, .0f);
	}
	else if (_seat == 2)
	{
		CardUtils::layoutListCardCenter(_freeCards, _posFree, .5f);
	}
	else
	{
		CardUtils::layoutListCardCenter_Vertical(_freeCards, _posFree, .6f);
	}
    
}

void CardPhomBox::layoutFireCard()
{
	CardUtils::layoutListCardCenter(_firedCards, _posFire, .5f);
	for (Card* c : _firedCards) {
		c->SetVisible(true);
	}
}

void CardPhomBox::layoutPhomCard()
{
	if (_phomCards.empty()) return;
    
    std::string tempStr = "[phom card list] : ";
    for (auto card : _phomCards)
    {
        tempStr.append(StringUtils::format("%d, ", card->GetCardName()));
    }
    CCLOG("layoutPhomCard for seat %d before sorted %s", _seat, tempStr.c_str());
    
	Vec2 posPhom;
	Vec2 card_size = _card_size;
	if (_seat == 0)
	{
		posPhom = Vec2(_posFree.x, _posFree.y + card_size.y / 2);
		CardUtils::layoutListCardCenter(_phomCards, posPhom, .0f);
		sortCards(_phomCards, ZODER_PHOM_CARD);
		int phom_size = _phomCards.size();
		int idx = 0;
		for(auto c : _freeCards)
		{
			NODE(c)->setLocalZOrder(phom_size + idx + ZODER_FREE_CARD);
			idx++;
		}
	}
	else if (_seat == 2)
	{
		posPhom = Vec2(_posFree.x, _posFree.y - (card_size.y * .5f));
		CardUtils::layoutListCardCenter(_phomCards, posPhom, .5f);
		//on the top seat, the zorder of free and phom card must swap to make the card phome and free look correct
        sortCards(_phomCards, ZODER_FREE_CARD);
//		for (size_t i = 0; i < _phomCards.size(); i++)
//		{
//			NODE(_phomCards[i])->setZOrder(i);
//		}
		for (size_t i = 0; i < _freeCards.size(); i++)
		{
			NODE(_freeCards[i])->setLocalZOrder(_phomCards.size() + i + 1 + ZODER_PHOM_CARD);
		}
	}
	else if (_seat == 1)
	{
		posPhom = Vec2(_posFree.x + card_size.x, _posFree.y);
		CardUtils::layoutListCardCenter_Vertical(_phomCards, posPhom, .6f);
        sortCards(_phomCards, ZODER_PHOM_CARD);
	}
	else
	{
		posPhom = Vec2(_posFree.x - card_size.x, _posFree.y);
		CardUtils::layoutListCardCenter_Vertical(_phomCards, posPhom, .6f);
        sortCards(_phomCards, ZODER_PHOM_CARD);
		
	}

    tempStr = "";
    for (auto card : _phomCards)
    {
        tempStr.append(StringUtils::format("%d, ", card->GetCardName()));
    }
    CCLOG("layoutPhomCard for seat %d after sorted %s", _seat, tempStr.c_str());
    
	for (Card* c : _phomCards) {
		c->SetVisible(true);
	}

}

void CardPhomBox::layoutEatCard()
{
	Vec2 posEatCard;
	Vec2 card_size = _card_size;
	if (_seat == 0)
	{
		posEatCard = Vec2(_posFree.x, _posFree.y /*+ card_size.y / 2*/);
		CardUtils::layoutListCardCenter(_eatCards, posEatCard, .0f);
	}
	else if (_seat == 2)
	{
		posEatCard = Vec2(_posFree.x, _posFree.y - (card_size.y * .5f));
		CardUtils::layoutListCardCenter(_eatCards, posEatCard, .5f);
	}
	else if (_seat == 1)
	{
		posEatCard = Vec2(_posFree.x + card_size.x, _posFree.y);
		CardUtils::layoutListCardCenter_Vertical(_eatCards, posEatCard, .6f);
	}
	else
	{
		posEatCard = Vec2(_posFree.x - card_size.x, _posFree.y);
		CardUtils::layoutListCardCenter_Vertical(_eatCards, posEatCard, .6f);
	}

	for (Card* c : _eatCards) {
		c->SetVisible(true);
	}
}

void CardPhomBox::layoutBox()
{
	if (_seat == 0) {
		_game_screen->clearAllCardBound();
	}
	
    //unselectFreeCards();
	layoutFreeCard();
	layoutFireCard();
	layoutPhomCard();
    
	if (_seat == 0) {
		for (size_t i = 0; i < _myPhoms.size(); i++)
		{
			for (size_t ii = 0; ii < _freeCards.size(); ii++)
			{
				if (_myPhoms[i]._cards[0] == _freeCards[ii]->getID())
				{
					Vec2 card_size = _freeCards[ii]->GetSize();
					Vec2 origin = _freeCards[ii]->GetPosition();
					cocos2d::Rect bound = cocos2d::Rect(origin, cocos2d::Size(card_size.x * _myPhoms[i]._cards.size(), card_size.y));
					SpecialCardsBound* card_bound = _game_screen->getFreeSpecialCardBound();
                    card_bound->setSpecialType(SpecialType(i));
					card_bound->setVisible(true);
					card_bound->setBound(bound);
					card_bound->setPosition(origin);
					break;
				}
			}
		}
	}
}

void CardPhomBox::addPhomToFreeCard(Card* c)
{
	K_VEC_REMOVE(_freeCards,c);
	K_VEC_REMOVE(_firedCards, c);
	K_VEC_REMOVE(_phomCards, c);
	if (c)
	{
		if (_seat == 0)
		{
			c->SetSize(Vec2(s_card_w, s_card_h));
		}
		else 
		{ 
			c->SetSize(Vec2(s_card_w * PHOM_FREE_CARD_SCALE, s_card_h * PHOM_FREE_CARD_SCALE)); 
		}
		_freeCards.insert(_freeCards.begin(), c);

		if (_seat == 2)
		{
			sortCards(_freeCards, ZODER_PHOM_CARD);
		}
		else
		{
			sortCards(_freeCards, ZODER_FREE_CARD);
		}
	}
}

/**
 * dùng để thay thế phỏm đã gửi bài vào phỏm hiện tại trong box
 *
 * @param addPhomCards
 *            : phỏm mới sau khi gửi (>=4 lá)
 */
void CardPhomBox::addSendCardToPhom(VecCard addPhomCards)
{
    if (addPhomCards.size() > 3)
    {
        CardUtils::sortByValue_CRChB_Cards(true, addPhomCards);
        for (int i = 0; i < addPhomCards.size(); i++)
        {
            Card* card = addPhomCards.at(i);
            if (card)
            {
                //resetCardAction(card);
                K_VEC_REMOVE(_freeCards, card);
                K_VEC_REMOVE(_firedCards, card);
                K_VEC_REMOVE(_phomCards, card);
            }
        }
        K_VEC_INSERT(_phomCards, addPhomCards);
    }
}

void CardPhomBox::removeCard(int cardId, int floor)
{
    Card* c = _game_screen->getCard(cardId);
    if(c != NULL)
    {
        //need reset card ???
        c->showHighlight(false);
        K_VEC_REMOVE(getList(floor), c);
    }
}

int CardPhomBox::getLastFireCardID()
{
    if (_firedCards.size() > 0)
    {
        return _firedCards.back()->getID();
    }
    else
    {
        return -1;
    }
}

VecCard CardPhomBox::getList(int floor)
{
    switch(floor)
    {
        case FLOOR_FIRED:
            return _firedCards;
        case FLOOR_PHOM:
            return _phomCards;
        case FLOOR_FREE:
        default:
            break;
    }
    
    return _freeCards;
}

/**
 * xóa hết bài trong box
 */
void CardPhomBox::clearBox()
{
    for (Card* c : _firedCards) {
        c->SetVisible(false);
    }
    for (Card* c : _phomCards) {
        c->SetVisible(false);
    }
    for (Card* c : _freeCards) {
        c->SetVisible(false);
    }
    for (Card* c : _eatCards) {
        c->SetVisible(false);
    }
    _firedCards.clear();
    _phomCards.clear();
    _freeCards.clear();
    _eatCards.clear();
}

void CardPhomBox::sortCards(std::vector<Card*> list_card, int default_z_order)
{
	for (size_t i = 0; i < list_card.size(); i++)
	{
        if(NODE(list_card[i]))
        {
			NODE(list_card[i])->setLocalZOrder(i + default_z_order);
        }
	}
}

void CardPhomBox::setMyPhomEat(std::vector<Phom> myPhoms, VecByte myEatCards)
{
	_myPhoms.clear();
	_myPhoms.insert(_myPhoms.begin(), myPhoms.begin(), myPhoms.end());

	_myEatCards.clear();
	_myEatCards.insert(_myEatCards.begin(), myEatCards.begin(), myEatCards.end());
}

void CardPhomBox::clearFreeCard()
{
	_freeCards.clear();
}

void CardPhomBox::addFreeCard(VecCard list_card)
{
	K_VEC_INSERT(_freeCards, list_card);
}

void CardPhomBox::removeFreeCard(int cardId) 
{
	if (_seat != 0) return;
	for (size_t i = 0; i < _freeCards.size(); i++)
	{
		if (_freeCards[i]->getID() == cardId) {
            _game_screen->getCard(cardId)->SetTouchEnable(false);
			_freeCards.erase(_freeCards.begin() + i);
		}
	}
}

void CardPhomBox::removeFiredCard(int cardId)
{
	for(size_t i = 0; i < _firedCards.size(); i++)
	{
		if (_firedCards[i]->getID() == cardId)
		{
			_firedCards.erase(_firedCards.begin() + i);
		}
	}
}

void CardPhomBox::selectFreeCard(int cardId) 
{
	if (_seat != 0) return;
	for (Card* c: _freeCards) {
		if (c->getID() == cardId) {
			c->SelectCardAction(true);
		}
	}
}

void CardPhomBox::selectAllCardPhom( std::vector<Phom> myPhoms)
{
	unselectFreeCards();
//    selected(_fireCards, false);
//    selected(_phomCards, false);
//    selected(_freeCards, false);
    layoutBox();
    
	for (size_t i = 0; i < myPhoms.size(); i++)
	{
		for (size_t ii = 0; ii < myPhoms[i]._cards.size(); ii++)
		{
			selectFreeCard(myPhoms[i]._cards[ii]);
		}
	}
    //_isSelectedCardPhom = true;
}

void CardPhomBox::unselectFreeCards()
{
	if (_seat != 0)
        return;
	for (Card* c : _freeCards) {
		c->SelectCardAction(false);
	}
}

bool CardPhomBox::hasFireCard(int cardId)
{
	for (Card* c: _firedCards) {
		if (c->getID() == cardId) {
			return true;
		}
	}
	return false;
}

void CardPhomBox::fireCard(int cardId) 
{
	Card* fire_card = _game_screen->getCard(cardId);
	fire_card->SetVisible(false);
	if (_seat == 0)
	{
		fire_card->SetSize(Vec2(s_card_w, s_card_h));
	}
	else
	{
		fire_card->SetSize(Vec2(s_card_w * PHOM_FREE_CARD_SCALE, s_card_h * PHOM_FREE_CARD_SCALE));
		fire_card->SetPosition(_posFree);
	}

	Vec2 fire_card_size = Vec2(s_card_w * PHOM_FIRE_CARD_SCALE, s_card_h * PHOM_FIRE_CARD_SCALE);
	Vec2 new_pos = CardUtils::calculateCardPosCenter(fire_card_size, _firedCards.size() + 1, _posFire, PHOM_FIRE_CARD_OVERLAP);
	_game_screen->getDealLayer()->moveCard(fire_card, new_pos, fire_card_size, [this, cardId]() {
		this->addFireCard(cardId);
		this->removeFreeCard(cardId);
		this->layoutBox();
	});

}

VecByte CardPhomBox::getCardSelected()
{
	VecByte rs;
	for (Card* c:_freeCards)
	{
		if (c->isSelected())
		{
			rs.push_back(c->getID());
		}
	}
	return rs;
}

void CardPhomBox::layoutFreeCardsWithAnim(VecByte cards)
{
	if (_seat != 0) return;
	Vec2 card_size = _card_size;
	VecByte old_ids = CardUtils::convertCardsToIds(_freeCards);
	std::vector<Vec2> old_pos = CardUtils::calculateCardListPosCenter(card_size, old_ids.size(), _posFree, .0f);
	mapByteToCard(_freeCards, cards);
	layoutFreeCard();


	std::vector<Vec2> list_pos_next = CardUtils::calculateCardListPosCenter(card_size, cards.size(), _posFree, .0f);
	for (size_t i = 0; i < _freeCards.size(); i++)
	{
		for (size_t ii = 0; ii < old_ids.size(); ii++)
		{
			if (_freeCards[i]->getID() == old_ids[ii])
			{
				_freeCards[i]->SetPosition(old_pos[ii]);
				NODE(_freeCards[i])->runAction(MoveTo::create(.5f, list_pos_next[ii]));
			}
		}
	}
}

void CardPhomBox::mapByteToCard(VecCard& card_array, VecByte byte_array)
{
	card_array.clear();
	for (size_t i = 0; i < byte_array.size(); i++)
	{
		card_array.push_back( _game_screen->getCard(byte_array[i]));
	}
}

void CardPhomBox::setupFreeCard(Card* c)
{
	if(_seat == 0) c->SetSize(Vec2(s_card_w, s_card_h));
	else c->SetSize(Vec2(s_card_w * PHOM_FREE_CARD_SCALE, s_card_h * PHOM_FREE_CARD_SCALE));
}

void CardPhomBox::getCard(int cardId, VecByte newFreeCard)
{
	size_t i = 0;
	for(i = 0; i < newFreeCard.size(); i++)
	{
		if (newFreeCard[i] == cardId)
		{
			break;
		}
	}

	layoutFreeCardsWithAnim(newFreeCard);

	Card* card_new = _freeCards[i];
	_game_screen->getDealLayer()->dealCard(0, _freeCards[i], [card_new]() {
		card_new->SetVisible(true);
	});
}

void CardPhomBox::eatCard(int cardId, VecByte newFreeCard)
{
	size_t i;
	for (i = 0; i < newFreeCard.size(); i++)
	{
		if (newFreeCard[i] == cardId)
		{
			break;
		}
	}

	layoutFreeCardsWithAnim(newFreeCard);

	Card* card_new = _freeCards[i];
	_game_screen->getDealLayer()->dealCard(0, _freeCards[i], [card_new]() {
		card_new->SetVisible(true);
	});
}

VecCard CardPhomBox::getFreeCards()
{
	return _freeCards;
}

void CardPhomBox::reset()
{
	resetListCard(_firedCards);
	resetListCard(_phomCards);
	resetListCard(_freeCards);
	resetListCard(_eatCards);
	_firedCards.clear();
	_phomCards.clear();
	_freeCards.clear();
	_eatCards.clear();
	_result_score->Visible(false);
	_result_info->Visible(false);
}

void CardPhomBox::resetListCard(VecCard cards)
{
	for (Card* c : cards)
	{
		c->SetVisible(false);
        c->GetResource()->setColor(Color3B::WHITE);
        c->showHighlight(false);
        c->SetTouchEnable(false);
	}
}

void CardPhomBox::hideFireCards()
{
	for (Card* c : _firedCards)
	{
		c->SetVisible(false);
	}
}

void CardPhomBox::showScore(int score)
{
	static_cast<TextWidget*>(_result_score->GetWidgetChildByName(".title_"))->SetText(StringUtils::format("%d", score), true);
	_result_score->Visible(true);
}

void CardPhomBox::showInfo(int type)
{
	auto text_result = static_cast<TextWidget*>(_result_info->GetWidgetChildByName(".title_"));
	bool isWin = false;
	switch (type)
	{
	case 0:
		text_result->SetText("phom_u");
		isWin = true;
		break;
	case 1:
		text_result->SetText("phom_u_khan");
		isWin = true;
		break;
	case 2:
		text_result->SetText("phom_u_den");
		isWin = true;
		break;
	case -1:
		text_result->SetText("damaged");
		break;
	case 3:
		text_result->SetText("maubinh_lose");
		break;
	case 4:
		text_result->SetText("maubinh_win");
		isWin = true;
		break;
	}
	_result_info->Visible(true);
	Color3B color_text = Color3B::YELLOW;
	if (isWin)
	{
		_result_info->GetWidgetChildByName(".bg_lose")->Visible(false);
		_result_info->GetWidgetChildByName(".bg_win")->Visible(true);
	}
	else
	{
		_result_info->GetWidgetChildByName(".bg_lose")->Visible(true);
		_result_info->GetWidgetChildByName(".bg_win")->Visible(false);
		color_text = Color3B::RED;
	}
	_result_info->GetWidgetChildByName(".title_")->SetColor(color_text);
}

void CardPhomBox::clearPhomCard()
{
	_phomCards.clear();
}

bool CardPhomBox::checkExist(VecCard cards, byte cardId)
{
	for (Card* c : cards)
	{
		if (c->getID() == cardId)
		{
			return true;
		}
	}
	return false;
}

void CardPhomBox::replacePhom(VecByte new_phom)
{
//    std::string phom = "replace new phom: ";
//    for(auto it : new_phom)
//    {
//        std::string cardStr = StringUtils::format("%d == ", (int)it);
//        phom.append(cardStr);
//    }
//    CCLOG("%s", phom.c_str());
    
    //always sort before replace
    CardUtils::sortByValue_CRChB_Cards(true, _phomCards);
    
	int last_idx = 0;
	for (size_t i = 0; i < new_phom.size(); i++)
	{
		bool exist = false;
		for (size_t ii = 0; ii < _phomCards.size(); ii++)
		{
			if (new_phom[i] == _phomCards[ii]->getID())
			{
				last_idx = ii;
				exist = true;
				break;
			}
		}
		if (!exist)
		{
//            CCLOG("idx to insert from replace %d to current at %d", i, last_idx);
//            phom = StringUtils::format("before card phom %d: ", (int)i);
//            for(auto it : _phomCards)
//            {
//                std::string cardStr = StringUtils::format("%d == ", (int)it->getID());
//                phom.append(cardStr);
//            }
//            CCLOG("%s", phom.c_str());
            
			Card* c = _game_screen->getCard(new_phom[i]);
			c->SetSize(_card_size);
			_phomCards.insert(_phomCards.begin() + last_idx, c);
			last_idx++;
            
//            phom = StringUtils::format("after card phom %d: ", (int)i);
//            for(auto it : _phomCards)
//            {
//                std::string cardStr = StringUtils::format("%d == ", (int)it->getID());
//                phom.append(cardStr);
//            }
//            CCLOG("%s", phom.c_str());
		}
	}
    
    //always sort before replace
    CardUtils::sortByValue_CRChB_Cards(true, _phomCards);
    
	if (_seat == 2)
	{
		sortCards(_phomCards, ZODER_FREE_CARD);
	}
	else
	{
		sortCards(_phomCards, ZODER_PHOM_CARD);
	}
}

void CardPhomBox::hideHightLightFiredCards()
{
    for(size_t i = 0; i < _firedCards.size(); i++)
    {
        _firedCards.at(i)->showHighlight(false);
    }
}

void CardPhomBox::unselectAllCardSelected()
{
    for(size_t i = 0; i < _freeCards.size(); i++)
    {
        if(_freeCards.at(i)->isSelected())
        {
            _freeCards.at(i)->SelectCardAction(false);
        }
    }
}

void CardPhomBox::SetWidget(WidgetEntity * _panel)
{
	if (_panel)
	{
		_result_info = _panel->GetWidgetChildByName(".result_layout.result");
		_result_info->Visible(false);

		_result_score = _panel->GetWidgetChildByName(".score_layout.score");
		_result_score->Visible(false);
	}
}

void CardPhomBox::selected(VecCard listCards, bool selected)
{
    for (auto card : listCards)
    {
        card->SelectCardAction(selected);
    }
}

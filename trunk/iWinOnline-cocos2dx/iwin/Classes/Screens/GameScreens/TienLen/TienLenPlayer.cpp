#include "TienLenPlayer.h"
#include "../../Object/Card.h"
#include "../../Object/CardUtils.h"

TienLenPlayer::TienLenPlayer()
{
	cards.clear();
	remain_cards.clear();
}

TienLenPlayer::~TienLenPlayer()
{
	cards.clear();
	remain_cards.clear();
}

TienLenPlayer::TienLenPlayer(Player * p)
{
	CopyValue(p);
	cards.clear();
}

void TienLenPlayer::unSelectAllCards()
{
	if (cards.size() <= 0)
		return;
	for (Card* c :cards)
	{
		c->SelectCardAction(false);
	}
}

std::vector<unsigned char> TienLenPlayer::getSelectedCardsValue()
{
	std::vector<unsigned char> selectedCardValues;
	for (size_t i = 0; i < cards.size(); i++)
	{
		Card* c = cards.at(i);
		if (c->isSelected())
		{
			selectedCardValues.push_back(c->getID());
		}
	}
	CardUtils::sort(selectedCardValues);
	return selectedCardValues;
}

std::vector<unsigned char> TienLenPlayer::getRemainCard()
{
	std::vector<unsigned char> remain_card;
	for (auto c : cards)
	{
		//if (c->IsVisible())
		{
			remain_card.push_back(c->getID());
		}
	}
	CardUtils::sort(remain_card);
	return remain_card;
}


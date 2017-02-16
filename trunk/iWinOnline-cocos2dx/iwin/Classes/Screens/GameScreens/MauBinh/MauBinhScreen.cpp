#include "MauBinhScreen.h"
#include "Screens/Object/Card.h"
#include "Screens/Object/CardUtils.h"
#include "Screens/Object/PlayerGroup.h"
#include "Screens/Object/TimeBarProcess.h"

#include "MauBinhPlayer.h"
#include "MauBinhService.h"
#include "Screens/GameScreens/TienLen/TienLenPlayer.h"

#include "Result.h"
#include "Common/IwinScale9Sprite.h"

#include "Screens/GamePlayMgr.h"
#include "Network/Game/GameService.h"
#include "Screens/GameScreens/DealCardLayer.h"
#include "Screens/GameScreens/MauBinh/LabelBG.h"
#include "Common/FlyMoney.h"

#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wButton.h"

USING_NS_CC;

int s_card_w = 50;
int s_card_h = 100;
float s_card_scale = 1.0f;
#define GROUP_SCALE_FINAL 1.1f
#define GROUP_SCALE_DRAG 0.8f
#define MAUBINH_LAYOUT_ORDER_ACE 4 
#define MAUBINH_LAYOUT_ORDER_HL MAUBINH_LAYOUT_ORDER_ACE + 1 
#define MAUBINH_LAYOUT_ORDER_LABEL_RESULT MAUBINH_LAYOUT_ORDER_HL + 1 


#define LAYER_ORDER_TRANS 1 
#define LAYER_ORDER_GROUP 2 
#define LAYER_ORDER_MY_CARD 3 
#define LAYER_ORDER_GROUP_DRAG 4 
#define LAYER_ORDER_DEAL 5 

#define TAG_LAST_CARD 9999

Vec2 s_offset_bound = Vec2(15, 19);

static const char TYPE_NONE = -1;
/**
* Thung Pha Sanh
*/
static	const char TYPE_FLUSH_STRAIGHT = 8;
/**
* Tu Quy
*/
static	const char TYPE_FOUR_OF_KIND = 7;
/**
* Cu Lu
*/
static const char TYPE_FULL_HOUSE = 6;
/**
* Thung
*/
static const char TYPE_FLUSH = 5;
/**
* Sanh
*/
static const char TYPE_STRAIGHT = 4;
/**
* Xam
*/
static const char TYPE_THREE_OF_KIND = 3;
/**
* Thu
*/
static const char TYPE_TWO_PAIRS = 2;
/**
* Dach
*/
static const char TYPE_ONE_PAIR = 1;
/**
* Mau Thau
*/
static const char TYPE_HIGH_CARDS = 0;
/**
* Get hand type.
*
* @param cards
*            must have 5 or 3 elements.
* @param isSort
*            true - sort input cards, false - otherwise.
* @return type of hand.
*/
ubyte getHandType(std::vector<Card*> cards, bool isSort)
{
	// Check cards.
	if (cards.size() == 0 || (cards.size() != 5 && cards.size() != 3)) {
		return TYPE_NONE;
	}
	int size = cards.size();
	if (isSort) {
		CardUtils::sortByOrder(cards);
	}

	std::vector<ubyte> cardIDs;
	for (int i = 0; i < size; i++) {
		cardIDs.push_back(cards.at(i)->getID());
	}
	CardUtils::sort(cardIDs);
	// isFlush: (Thung), cards have the same type.
	bool isFlush = true;
	int pairNo = 0;
	int threeNo = 0;
	int sameCardNo = 0;
	for (int i = 1; i < size; i++) {
		// if 2 continuous cards have different types.
		if (isFlush && cardIDs[i] % 4 != cardIDs[i - 1] % 4) {
			isFlush = false;
		}

		// Check pair, three or four of a kind.
		if (cardIDs[i] / 4 == cardIDs[i - 1] / 4) {
			sameCardNo++;
		}
		else {
			switch (sameCardNo) {
			case 0:
				break;
			case 1: // a pair.
				if (size == 3) {
					return TYPE_ONE_PAIR;
				}
				else {
					pairNo++;
				}
				break;
			case 2: // Three of a kind.
				threeNo++;
				break;
			case 3: // Four of a Kind.
					// this.type = SetType.FOUR_OF_KIND;
				return TYPE_FOUR_OF_KIND;
			default:
				break;
			}
			sameCardNo = 0;
		}
	}

	switch (sameCardNo) {
	case 0:
		if (size == 3) {
			return TYPE_HIGH_CARDS;
		}
		break;
	case 1: // a pair.
		if (size == 3) {
			return TYPE_ONE_PAIR;
		}
		else {
			pairNo++;
		}
		break;
	case 2: // Three of a kind.
		if (size == 3) {
			return TYPE_THREE_OF_KIND;
		}
		else {
			threeNo++;
		}
		break;
	case 3: // Four of a Kind.
		return TYPE_FOUR_OF_KIND;
	default:
		break;
	}

	// There is a three.
	if (threeNo > 0) {
		// Cu lu.
		if (pairNo > 0) {
			return TYPE_FULL_HOUSE;
		}
		else {
			// Xam chi.
			return TYPE_THREE_OF_KIND;
		}
	}

	// There is NOT any three.
	// 2 dao.
	if (pairNo == 2) {
		// this.type = SetType.TWO_PAIR;
		return TYPE_TWO_PAIRS;
	}
	else if (pairNo == 1) {
		// 1 dao.
		// this.type = SetType.ONE_PAIR;
		return TYPE_ONE_PAIR;
	}
	// There is NOT any pair.
	int firstCardNumber = cardIDs[0] / 4;
	int lastCardNumber = cardIDs[cardIDs.size() - 1] / 4;
	// Sanh.
	if (lastCardNumber - firstCardNumber == size - 1
		|| (cardIDs[cardIDs.size() - 1] / 4 == 12 && cardIDs[cardIDs.size() - 2] / 4 == 3)) {
		if (isFlush) {
			// Thung pha sanh.
			// this.type = SetType.STRAIGHT_FLUSH;
			return TYPE_FLUSH_STRAIGHT;
		}
		else {
			// Sanh.
			// this.type = SetType.STRAIGHT;
			return TYPE_STRAIGHT;
		}
	}

	// Thung. Plus ret to compare between flush.
	if (isFlush) {
		// this.type = SetType.FLUSH;
		return TYPE_FLUSH;
	}
	// Mau thau.
	return TYPE_HIGH_CARDS;

}

/**
* Get mau binh Name according the type parameter.
*
* @param type
* @return null if the type is incorrect.
*/
std::string getMaubinhName(char type) {
	switch (type) {
	case Result::FOUR_OF_THREE:
		return LangMgr->GetString("four_three_of_a_kind").GetString();
	case Result::SAME_COLOR_12:
		return LangMgr->GetString("twelve_cards_me_color").GetString();
	case Result::SAME_COLOR_13:
		return LangMgr->GetString("full_cards_me_color").GetString();
	case Result::SIX_PAIR:
		return LangMgr->GetString("six_pairs").GetString();
	case Result::SIX_PAIR_WITH_THREE:
		return LangMgr->GetString("six_pairs_with_three_of").GetString();
	case Result::STRAIGHT_13:
		return LangMgr->GetString("three_sequence_straights").GetString();
	case Result::THREE_FLUSH:
		return LangMgr->GetString("three_flushes").GetString();
	case Result::THREE_STRAIGHT:
		return LangMgr->GetString("three_straights").GetString();
	}
	return "";
}

std::string getHandInfo(std::vector<Card*> cards, bool isSort) {
	if ( cards.size() == 0) {
		return "";
	}
	switch (getHandType(cards, isSort)) {
	case TYPE_FULL_HOUSE:
		return LangMgr->GetString("full_house").GetString();
	case TYPE_HIGH_CARDS:
		return LangMgr->GetString("high_card").GetString();
	case TYPE_STRAIGHT:
		return LangMgr->GetString("straight").GetString();
	case TYPE_TWO_PAIRS:
		return LangMgr->GetString("two_pairs").GetString();
	case TYPE_FLUSH:
		return LangMgr->GetString("flush").GetString();
	case TYPE_FLUSH_STRAIGHT:
		return LangMgr->GetString("straight_flush").GetString();
	case TYPE_FOUR_OF_KIND:
		return LangMgr->GetString("four_of_kind").GetString();
	case TYPE_THREE_OF_KIND:
		return LangMgr->GetString("three_of_kind").GetString();
	case TYPE_ONE_PAIR:
		return LangMgr->GetString("one_pair").GetString();
	}
	return "";
}

Card* createCardMauBinh()
{
	Card* card = Card::NewInstance(0, DECK_TYPE::DECK_MAUBINH);
	card->SetSize(Vec2(s_card_w, s_card_h));
	NODE(card)->setPosition(Vec2::ZERO);
	NODE(card)->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	return card;
}

GroupCardDrag* GroupCardDrag::create()
{
	GroupCardDrag* rs = new GroupCardDrag();
	rs->init();
	rs->autorelease();
	return rs;
}

bool GroupCardDrag::init()
{
	Layout::init();
	for (size_t i = 0; i < MAUBINH_MAX_CARD; i++)
	{
		_cards.push_back(createCardMauBinh());
		addChild(NODE(_cards[i]));
		NODE(_cards[i])->setVisible(false);
	}
	setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	return true;
}



void GroupCardDrag::setCards(VecCard cards)
{
	if (cards.size() <= 0)
	{
		return;
	}
	_drag_cards = cards;
	if (cards[0] == NULL)
		return;
	Vec2 card_size = cards[0]->GetSize();
	for (size_t i = 0; i < cards.size(); i++)
	{
		if (cards[i] == NULL)
			continue;
		_cards[i]->SetSize(cards[i]->GetSize());
		_cards[i]->ResetID(cards[i]->getID(), DECK_TYPE::DECK_MAUBINH);
		_cards[i]->SetVisible(true);
		_cards[i]->SetX(card_size.x * i);
	}
	int group_w = card_size.x * cards.size();
	setSize(cocos2d::Size(group_w, card_size.y));
}

VecCard GroupCardDrag::getCards()
{
	return _drag_cards;
}

void GroupCardDrag::reset()
{
	setVisible(false);
	for (Card* c : _drag_cards) 
	{
		c->SetOpacity(255);
	}
	_drag_cards.clear();
	for (Card* c : _cards)
	{
		c->SetVisible(false);
	}
}

int GroupCardDrag::getNumCardDrag()
{
	return _drag_cards.size();
}

MauBinhResult* MauBinhResult::create()
{
	MauBinhResult* rs = new MauBinhResult();
	rs->init();
	rs->autorelease();
	return rs;
}

MauBinhResult::~MauBinhResult()
{
	_lbl_chi->release();
}

bool MauBinhResult::init()
{
	Layout::init();

	for (size_t i = 0; i < MAUBINH_MAX_CARD; i++)
	{
		_cards.push_back(createCardMauBinh());
	}

	for (size_t i = 0; i < MAUBINH_NUM_ACES; i++)
	{
		_aces[i] = createCardMauBinh();
	}

	_layoutCards();

	setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_card_hl_win = Scale9Sprite::createWithSpriteFrameName("gamescreen-binh-screen/vien_thang.9.png");
	_card_hl_win->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_card_hl_win->setZOrder(MAUBINH_LAYOUT_ORDER_HL);
	_card_hl_win->setVisible(false);
	_card_hl_lose = Scale9Sprite::createWithSpriteFrameName("gamescreen-binh-screen/vien_thua.9.png");
	_card_hl_lose->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_card_hl_lose->setZOrder(MAUBINH_LAYOUT_ORDER_HL);
	_card_hl_lose->setVisible(false);
	addChild(_card_hl_win);
	addChild(_card_hl_lose);

	_lbl_chi = TextBMFont::create();
	_lbl_chi->setFntFile(File::ResolveNamePath(TextureMgr->GetFontBmpByIdx(16)->path_fnt).GetString());
	_lbl_chi->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	_lbl_chi->retain();

	_lbl_bg = Sprite::createWithSpriteFrameName("gamescreen-binh-screen/nen_thang.png");
	_lbl_bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_lbl_bg->retain();


	_lbl_result = TextBMFont::create();
	_lbl_result->setFntFile(File::ResolveNamePath(TextureMgr->GetFontBmpByIdx(16)->path_fnt).GetString());
	_lbl_result->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

	_lbl_result_bg = Sprite::createWithSpriteFrameName("gamescreen-binh-screen/nen_thang.png");
	_lbl_result_bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_lbl_result_bg->addChild(_lbl_result);
	addChild(_lbl_result_bg, MAUBINH_LAYOUT_ORDER_HL + 1);
	_lbl_result_bg->setVisible(false);
	_lbl_result->setPosition(Vec2(_lbl_result_bg->getContentSize().width / 2, _lbl_result_bg->getContentSize().height / 2));

	//_label_result = new LabelBG();
	//_label_result->init("otherscreen/friend-screen/btn_green.9.png", 4);
	//addChild(_label_result, MAUBINH_LAYOUT_ORDER_LABEL_RESULT);
	//_label_result->setVisible(false);

	return true;
}

void MauBinhResult::updateTextHL(int chi)
{
	std::string text_chi;
	_lbl_chi->removeFromParent();
	_lbl_bg->removeFromParent();
	if (chi > 0)
	{
		text_chi += "+";
	}

	text_chi += StringUtils::format("%d %s", chi, LangMgr->GetString("hands").GetString());

	if (chi == 0)
	{
		text_chi = LangMgr->GetString("maubinh_tie").GetString();
	}

	_lbl_chi->setString(text_chi);

	if (chi > 0)
	{
		_lbl_bg->setSpriteFrame("gamescreen-binh-screen/nen_thang.png");
		_lbl_chi->setColor(Color3B(156, 237, 50));
	}
	else if (chi < 0)
	{
		_lbl_bg->setSpriteFrame("gamescreen-binh-screen/nen_thua.png");
		_lbl_chi->setColor(Color3B(251, 173, 70));
	}
	else
	{
		_lbl_bg->setSpriteFrame("gamescreen-binh-screen/nen_hoa.png");
		_lbl_chi->setColor(Color3B::WHITE);
	}
}

void MauBinhResult::showHL(int chi, Vec2 origin, int hl_w, bool behind_hl)
{
	Vec2 card_size = _cards[0]->GetSize();
	std::string text_chi;
	Scale9Sprite* hl;
	_card_hl_win->setVisible(chi >= 0);
	_card_hl_lose->setVisible(chi < 0);
	if (chi >= 0) hl = _card_hl_win;
	else hl = _card_hl_lose;
	updateTextHL(chi);
	hl->setContentSize(cocos2d::Size(hl_w + s_offset_bound.x * 2, card_size.y + s_offset_bound.y * 2));
	hl->addChild(_lbl_bg);
	hl->addChild(_lbl_chi);

	hl->setPosition(origin - s_offset_bound);
	cocos2d::Size text_size = _lbl_chi->getContentSize();

	if (!behind_hl)
	{
		_lbl_chi->setPosition(Vec2((hl->getContentSize().width - text_size.width) / 2, s_offset_bound.y + _lbl_bg->getContentSize().height / 2));
		_lbl_bg->setPosition(Vec2(hl->getContentSize().width / 2, s_offset_bound.y + _lbl_bg->getContentSize().height / 2 - (_lbl_bg->getContentSize().height - text_size.height) / 2));
	}
	else
	{
		_lbl_bg->setPosition(Vec2(hl->getContentSize().width / 2, -_lbl_bg->getContentSize().height / 2));
		_lbl_chi->setPosition(Vec2((hl->getContentSize().width - text_size.width) / 2, _lbl_bg->getPosition().y));
	}

}

void MauBinhResult::showHLChiThuong(int startIdx, int endIdx, int chi, int hl_w)
{
	for (int i = startIdx; i < endIdx;i++)
	{
		NODE(_cards[i])->setZOrder(4);
		_cards[i]->hideTrans();
	}
	showHL(chi, _list_pos[startIdx], hl_w);
}
void MauBinhResult::setCards(std::vector<Card*> cards)
{
	for (size_t i = 0; i < cards.size(); i++)
	{
		_cards[i]->ResetID(cards[i]->getID(), DECK_TYPE::DECK_MAUBINH);
	}
}

void MauBinhResult::setCardsState(bool state)
{
	for (auto c : _cards)
	{
		if(state) c->ResetID(-1, DECK_TYPE::DECK_MAUBINH);
	}
}

void MauBinhResult::showResultUp()
{
	for (auto c : _cards)
	{
		c->showTrans(true);
	}
}

void MauBinhResult::reset()
{
	for (size_t i = 0; i < MAUBINH_NUM_ACES; i++)
	{
		NODE(_aces[i])->removeFromParent();
	}
	_card_hl_win->setVisible(false);
	_card_hl_lose->setVisible(false);
	_lbl_result_bg->setVisible(false);
	setVisible(false);
}

void MauBinhResult::_sortZ()
{
	int z_order = 3;
	for (size_t i = 0; i < _cards.size(); i++)
	{
		NODE(_cards[i])->setZOrder(z_order);
		_cards[i]->showTrans();
		if (i == 4 || i == 9) z_order--;
	}
}

void MauBinhResult::showCard()
{
	for(int i = 0; i < _cards.size(); i++)
	{
		_cards.at(i)->SetPosition(_center_pos);
		_cards.at(i)->GetResource()->runAction(MoveTo::create(0.1f, _list_pos.at(i)));
	}
}

void MauBinhResult::showChi1()
{
    if (!_result)
    {
        PASSERT2(false, "result is null ?");
        return;
    }
    
	if (_result->mauBinhType >= 0)
	{
		showTotal();
		return;
	}

	_sortZ();
	showHLChiThuong(10, 13, _result->winChi01, _size_3);
}

void MauBinhResult::showChi2()
{
    if (!_result)
    {
        PASSERT2(false, "result is null ?");
        return;
    }
    
	if (_result->mauBinhType >= 0) return;
	_sortZ();
	showHLChiThuong(5, 10, _result->winChi02, _size_5);
}

void MauBinhResult::showChi3()
{
    if (!_result)
    {
        PASSERT2(false, "result is null ?");
        return;
    }
    
	if (_result->mauBinhType >= 0) return;
	_sortZ();
	showHLChiThuong(0, 5, _result->winChi03, _size_5);
}

void MauBinhResult::showChiAce()
{
	_sortZ();
	if (!_result)
	{
		PASSERT2(false, "result is null ?");
		return;
	}

	int chi = _result->winChiAce;
	cocos2d::Size content_size = getContentSize();
	Vec2 center(content_size.width / 2, content_size.height / 2);
	int num_aces = 0;
	std::vector<Card*> cards;
	for (size_t i = 0; i < MAUBINH_NUM_ACES; i++)
	{
		NODE(_aces[i])->removeFromParent();
	}
	for (size_t i = 0; i < MAUBINH_MAX_CARD; i++)
	{
		if (_cards[i]->GetCardValue() == 12)
		{
			Card* card = _aces[num_aces++];
			card->ResetID(_cards[i]->getID(), DECK_TYPE::DECK_MAUBINH);
			addChild(NODE(card), MAUBINH_LAYOUT_ORDER_ACE);
			cards.push_back(card);
		}
	}
	int width = CardUtils::setCardPosition(center.x, center.y - s_card_h / 2, _size_3, cards);
	if (!cards.empty())
	{
		showHL(chi, cards[0]->GetPosition(), width, true);
	}
}

void MauBinhResult::showTotal()
{
	for (size_t i = 0; i < _cards.size(); i++)
	{
		_cards[i]->hideTrans();
	}

	for (size_t i = 0; i < MAUBINH_NUM_ACES; i++)
	{
		NODE(_aces[i])->removeFromParent();
	}

	bool isSupHam = false;
	isSupHam = _result->isLoseThreeSet();

	_card_hl_win->setVisible(false);
	_card_hl_lose->setVisible(false);
	int chi = _result->winChi;
	cocos2d::Size layout_size = getContentSize();
	std::string left_str = "";
	std::string sign = chi > 0 ? "+" : "";
	if(_result->mauBinhType >= 0)
	{
		//_label_result->reset("otherscreen/friend-screen/btn_orange.9.png", 3);
		left_str = LangMgr->GetString("natural").GetString();
	}
	else if (_result->winChi > 0)
	{
		//_lbl_result->reset("otherscreen/friend-screen/btn_green.9.png", 3);
		left_str = LangMgr->GetString("maubinh_win").GetString();
	}
	else if (_result->winChi < 0)
	{
		//_label_result->reset("otherscreen/friend-screen/btn_red.9.png", 12);
		left_str = LangMgr->GetString("maubinh_lose").GetString();
	}
	else if (_result->winChi == 0)
	{
		//_label_result->reset("otherscreen/friend-screen/btn_gray.9.png", 4);
		left_str = LangMgr->GetString("maubinh_tie").GetString();
	}
	else if(_result->mauBinhType == -2)
	{
		//_label_result->reset("otherscreen/friend-screen/btn_black.9.png", 12);
		left_str = LangMgr->GetString("error").GetString();
	}
	else
	{
		left_str = LangMgr->GetString("maubinh_lose").GetString();
	}
	std::string result = StringUtils::format("%s %s %d", left_str.c_str(), sign.c_str(), chi);

	if (isSupHam)
	{
		result = StringUtils::format("%s %s", LangMgr->GetString("maubinh_lose_for_all").GetString(), result.c_str());
	}

	_lbl_result->setString(result);

	if (chi > 0)
	{
		_lbl_result_bg->setSpriteFrame("gamescreen-binh-screen/nen_thang.png");
		_lbl_result->setColor(Color3B(156, 237, 50));
	}
	else if (chi < 0)
	{
		_lbl_result_bg->setSpriteFrame("gamescreen-binh-screen/nen_thua.png");
		_lbl_result->setColor(Color3B(251, 173, 70));
	}
	else
	{
		_lbl_result_bg->setSpriteFrame("gamescreen-binh-screen/nen_hoa.png");
		_lbl_result->setColor(Color3B::WHITE);
	}
	_lbl_result_bg->setVisible(true);
	_lbl_result_bg->setPosition(Vec2(layout_size.width / 2, 0));
}

void MauBinhResult::showMauBinh()
{

}

void MauBinhResult::_layoutCards()
{
	const float overlap_percent_x = 0.35f;
	const float overlap_percent_y = 0.5f;

	std::vector<Card*> list_cards = _cards;
	for (Card* c : list_cards)
	{
		NODE(c)->removeFromParent();
	}
	int z_order = 3;
	for (size_t i = 0; i < list_cards.size(); i++)
	{
		addChild(NODE(list_cards[i]));
		NODE(list_cards[i])->setZOrder(z_order);
		if (i == 4 || i == 9) z_order--;
	}
	Vec2 card_size = list_cards[0]->GetSize();
	int overlap_x = card_size.x * overlap_percent_x;
	int list_card_w = (card_size.x - overlap_x) * 4 + card_size.x;
	_size_5 = list_card_w;
	int mid_layout = list_card_w / 2;

	int start_y = 0;

	int start_x = 0;
	CardUtils::doAlignLeftListCard(list_cards, card_size.x - overlap_x, Vec2(start_x, start_y), 0, 5);

	start_y += card_size.y * overlap_percent_y;
	CardUtils::doAlignLeftListCard(list_cards, card_size.x - overlap_x, Vec2(start_x, start_y), 5, 10);

	list_card_w = (card_size.x - overlap_x) * 2 + card_size.x;
	start_x = mid_layout - list_card_w / 2;
	start_y += card_size.y * overlap_percent_y;
	_size_3 = list_card_w;
	CardUtils::doAlignLeftListCard(list_cards, card_size.x - overlap_x, Vec2(start_x, start_y), 10, 13);

	setSize(cocos2d::Size((card_size.x - overlap_x) * 4 + card_size.x, card_size.y + card_size.y * overlap_percent_y * 2));
	for (Card* c : _cards)
	{
		_list_pos.push_back(c->GetPosition());
	}

	_center_pos = Vec2(this->getSize().width / 2, this->getSize().height / 2);
}

GroupCard::GroupCard(const char* name, int num_cards, cocos2d::Node* gamescreen)
{
	_gamescreen = gamescreen;
	init(name, num_cards);
}

GroupCard::~GroupCard()
{
}

void GroupCard::init(const char* name, int num_cards)
{
	_bg_group_hl = Scale9Sprite::createWithSpriteFrameName("loginscreen/nine/maubinh_viengroup_ngoai.9.png");
	_bg_group_hl->setCapInsets(_bg_group_hl->getSprite()->getSpriteFrame()->GetCapInsert());
	_bg_group_hl->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_bg_group_hl->setVisible(false);
	((MauBinhScreen*)_gamescreen)->addToMyDealLayer(_bg_group_hl);

	_offset_card = Vec2(_bg_group_hl->getInsetLeft(), _bg_group_hl->getInsetBottom());

	_bg_group = Scale9Sprite::createWithSpriteFrameName("loginscreen/nine/maubinh_viengroup_trong.9.png");
	_bg_group->setCapInsets(_bg_group->getSprite()->getSpriteFrame()->GetCapInsert());
	_bg_group->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_offset_border = Vec2(_offset_card.x - _bg_group->getInsetLeft(), _offset_card.y - _bg_group->getInsetBottom());

	((MauBinhScreen*)_gamescreen)->addToMyDealLayer(_bg_group);

	_real_size = cocos2d::Size(s_card_w * num_cards + _offset_card.x, s_card_h + _offset_card.y);
	_limit_num_card = num_cards;

	_lbl_chi_bg = Sprite::createWithSpriteFrameName("gamescreen-binh-screen/bg_loaimaubinh.png");
	((MauBinhScreen*)_gamescreen)->addChild(_lbl_chi_bg);

	_lbl_type_chi = TextBMFont::create();
	_lbl_type_chi->setFntFile(File::ResolveNamePath(TextureMgr->GetFontBmpByIdx(0)->path_fnt).GetString());
	_lbl_type_chi->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_lbl_type_chi->setString(LangMgr->GetString("straight_flush").GetString());
	_lbl_type_chi->setPosition(Vec2(_lbl_chi_bg->getContentSize().width / 2, _lbl_chi_bg->getContentSize().height / 2));
	_lbl_chi_bg->addChild(_lbl_type_chi);
	_lbl_chi_bg->setVisible(false);

	_focus = false;
	setCanTap(false);
}

cocos2d::Size GroupCard::getRealSize()
{
	return _real_size;
}

cocos2d::Rect GroupCard::getTouchZone()
{
	return _bg_group->getBoundingBox();
}

Card* GroupCard::getCardHL(Card* card)
{
	for (auto c : _cards)
	{
		if (c != card && c->isHighlight()) return c;
	}
	return nullptr;
}

Vec2 GroupCard::getCardPosition()
{
	return _bg_group_hl->getPosition();
}

Vec2 GroupCard::getTopRightCard()
{
	return _bg_group_hl->getPosition() + Vec2(_bg_group_hl->getContentSize().width, _bg_group_hl->getContentSize().height );
}

void GroupCard::layoutGroupStart()
{
	_bg_group->setVisible(true);
	_bg_group_hl->setVisible(true);
}

void GroupCard::doLayout(float card_scale, Vec2 bot_center)
{
	_real_size = cocos2d::Size(s_card_w * _limit_num_card * card_scale + _offset_card.x * 2, s_card_h * card_scale + _offset_card.y * 2);
	_bg_group_hl->setContentSize(_real_size);
	_bg_group_hl->setPosition(Vec2(bot_center.x - _real_size.width / 2, bot_center.y));


	_pos = _bg_group_hl->getPosition();
	_bg_group->setContentSize(cocos2d::Size(_real_size.width - _offset_border.x * 2, _real_size.height - _offset_border.y * 2));
	_bg_group->setPosition(_bg_group_hl->getPosition() + _offset_border);

	_lbl_chi_bg->setPosition(Vec2((bot_center.x - (s_card_w * 5 * card_scale + _offset_card.x * 2) / 2) / 2, bot_center.y + _real_size.height / 2));

	_start_card_pos = Vec2(bot_center.x - _real_size.width / 2 + _offset_card.x, bot_center.y + _offset_card.y);
	_top_right_card_pos = _start_card_pos + Vec2(_real_size.width - _offset_card.x * 2, s_card_h * card_scale);
	if (!_cards.empty())
	{

		for (Card* c : _cards) {
			c->SetSize(Vec2(s_card_w * card_scale, s_card_h * card_scale));
		}

		CardUtils::layoutListCardFromLeft(_cards, _cards[0]->GetSize().x, _start_card_pos);

		if (getAvailableSlot() == 0)
		{
			_lbl_chi_bg->setVisible(true);
			_lbl_type_chi->setString(getHandInfo(_cards, false));
			showHighlight(false);
		}
		else
		{
			_lbl_chi_bg->setVisible(false);
		}


	}
	else
	{
		_lbl_chi_bg->setVisible(false);
	}

}

bool GroupCard::isFull()
{
	if (_cards.size() >= (size_t)_limit_num_card)
	{
		return true;
	}
	return false;
}

bool GroupCard::contain(Card* c)
{
	for (Card* card : _cards) {
		if (c == card) {
			return true;
		}
	}
	return false;
}

int GroupCard::getAvailableSlot()
{
	return _limit_num_card - _cards.size();
}

void GroupCard::addCard(Card* card)
{
	_cards.push_back(card);
}
void GroupCard::removeCard(Card* card)
{
	std::vector<Card*>::iterator it = std::find(_cards.begin(), _cards.end(), card);
	if (it != _cards.end()) _cards.erase(it);
}

void GroupCard::addCards(std::vector<Card*> cards)
{
	_cards.insert(_cards.end(), cards.begin(), cards.end());
}

void GroupCard::empty()
{
	_cards.clear();
}

std::vector<Card*> GroupCard::getCards()
{
	return _cards;
}

void GroupCard::setFocus(bool focus)
{
	_focus = focus;
}

bool GroupCard::isFocus()
{
	return _focus;
}

void GroupCard::replace(Card* card1, Card* card2)
{
    for (size_t i = 0; i < _cards.size(); i++)
    {
        if (_cards[i] == card1)
        {
            _cards[i] = card2;
            return;
        }
    }
}

void GroupCard::swap(Card* card1, Card* card2)
{
    for (size_t i = 0; i < _cards.size(); i++)
    {
        if (_cards[i] == card1)
        {
            _cards[i] = card2;
        }
        else if (_cards[i] == card2)
        {
            _cards[i] = card1;
        }
    }
}

void GroupCard::showGroup(bool visible)
{
	_bg_group->setVisible(visible);
	_bg_group_hl->setVisible(visible);
	_lbl_chi_bg->setVisible(visible);
	if (getAvailableSlot() != 0) 
	{
		_lbl_chi_bg->setVisible(false);
	}

}

bool GroupCard::isVisible()
{
	return _bg_group->isVisible();
}

Card* GroupCard::getCardHL()
{
	for (Card* c : _cards)
	{
		if (c->isHighlight()) return c;
	}
	return nullptr;
}

Vec2 GroupCard::getPosition()
{
	return _pos;
}

void GroupCard::setPosition(Vec2 pos)
{
	_pos = pos;
	_bg_group->setPosition(pos);
}

void GroupCard::setScale(float scale)
{
	_scale = scale;
}

float GroupCard::getScale()
{
	return _scale;
}

void GroupCard::showHighlight(bool visible)
{
	_bg_group_hl->setVisible(visible);
	
}

MauBinhScreen* MauBinhScreen::_instance = nullptr;

MauBinhScreen* MauBinhScreen::getInstance()
{
	return static_cast<MauBinhScreen*>(GamePlay->GetCurrentGameScreen());
}

MauBinhScreen::MauBinhScreen()
{
	SetGameID(GameType::GAMEID_BINH);
	SetMaxPlayer(4);
	_is_finish_update_ui = false;
	
}

MauBinhScreen::~MauBinhScreen()
{
	CC_SAFE_DELETE(_group1);
	CC_SAFE_DELETE(_group2);
	CC_SAFE_DELETE(_group3);
	for (size_t i = 0; i < _pool_player_ui.size(); i++)
	{
		CC_SAFE_DELETE(_pool_player_ui[i]);
	}
	CC_SAFE_DELETE(_binh_timebar);
	for (size_t i = 0; i < _list_mesage_delay.size(); i++)
	{
		CC_SAFE_DELETE(_list_mesage_delay[i]);
	}
}

bool MauBinhScreen::InitGame(int game_id)
{

	GameScreen::InitGame(game_id);
	cocos2d::Size game_size = GetGameSize();
	_deal_card_layer = new DealCardLayer();
	_deal_card_layer->init();
	_deal_card_layer->autorelease();
	addChild(_deal_card_layer, LAYER_ORDER_DEAL);



	_img_arrow = Sprite::createWithSpriteFrameName("gamescreen-tienlen-screen/button_eatcard.png");
	addChild(_img_arrow, LAYER_ORDER_GROUP);
	_img_arrow->setVisible(false);
	_img_lung = Sprite::createWithSpriteFrameName("gamescreen-binh-screen/ico_lung.png");
	addChild(_img_lung, LAYER_ORDER_GROUP);
	_img_lung->setVisible(false);



	XMLMgr->OnLoadXMLData<MauBinhScreen>("MauBinh_Screen", std::mem_fn(&MauBinhScreen::LoadXMLGameScreenCustom), this);
	onCreateListPlayer("MauBinh_Screen");

	p_panel_bottom = p_panel_custom->GetWidgetChildByName(".custom_root.panel_custom");
	PASSERT2(p_panel_bottom != nullptr, "p_panel_bottom is null , recheck the path");
	_panel_trans_0 = p_panel_custom->GetWidgetChildByName(".custom_root.panel_trans_0");
	_panel_trans_0->SetVisible(false);	
	_panel_trans_1 = p_panel_custom->GetWidgetChildByName(".custom_root.panel_trans_1");
	_panel_trans_1->SetVisible(false);	

	_other_deal_layer = p_panel_custom->GetWidgetChildByName(".custom_root.other_deal_layer");
	_my_deal_layer = p_panel_custom->GetWidgetChildByName(".custom_root.my_deal_layer");

	_binh_timebar = new TimeBarProcess(p_panel_custom->GetWidgetChildByName(".custom_root.progress_bar_panel"));
	_binh_timebar->setVisible(false);
	_binh_timebar->setCustomLabel([](int cur_time, int interval)->std::string {
		return "";//StringUtils::format("%d / %d s", cur_time, interval);
	});

	Card* temp_card = Card::NewInstance(0, DECK_MAUBINH);
	s_card_w = temp_card->GetSize().x;
	s_card_h = temp_card->GetSize().y;
	s_card_scale = temp_card->GetScale().x;
	_deal_card_layer->setCardSize(Vec2(s_card_w, s_card_h));
	_btnAutoBinh = p_panel_bottom->GetWidgetChildByName(".layout_btn_game_maubinh.btn_auto_binh");
	_btnFinishBinh = p_panel_bottom->GetWidgetChildByName(".layout_btn_game_maubinh.btn_finish_binh");
	_btnSort = p_panel_bottom->GetWidgetChildByName(".layout_btn_game_maubinh.btn_sort_again");
	//_text_waiting = p_panel_bottom->GetWidgetChildByName(".txt_wait_player");
	_group1 = new GroupCard("group1", 5, this);
	_group2 = new GroupCard("group2", 5, this);
	_group3 = new GroupCard("group3", 3, this);
	
	showLayoutGroup(false);
	/*for (int i = 0; i < p_max_player; i++)
	{
		MauBinhPlayerUI* ui = new MauBinhPlayerUI();
		for (size_t j = 0; j < MAUBINH_MAX_CARD; j++)
		{
			ui->cards.push_back(createCardMauBinh());
		}

		ui->layout_result = MauBinhResult::create();
		ui->layout_result->retain();
		_pool_player_ui.push_back(ui);
	}*/


	_group_drag = GroupCardDrag::create();
	addChild(_group_drag, LAYER_ORDER_GROUP_DRAG);
	_group_drag->setVisible(false);

	_spr_lung_up = Scale9Sprite::createWithSpriteFrameName("gamescreen-binh-screen/vien_lung.9.png");
	_spr_lung_up->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_spr_lung_up->setVisible(false);

	_spr_lung_down = Scale9Sprite::createWithSpriteFrameName("gamescreen-binh-screen/vien_lung.9.png");
	_spr_lung_down->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_spr_lung_down->setVisible(false);
	addChild(_spr_lung_up);
	addChild(_spr_lung_down);

	Vec2 countdown_size = p_panel_count_down->GetSize();
	Vec2 countdown_pos = p_panel_count_down->GetPosition();
	p_panel_count_down->SetPosition(Vec2(countdown_pos.x, countdown_pos.y + countdown_size.y / 2));

    //initFake();

	/*Sprite* test = Sprite::create("test.png");
	addChild(test);
	test->setPosition(Vec2(0.0f, 0.0f));*/
	return true;
}


void MauBinhScreen::OnBeginFadeIn()
{
	GameScreen::OnBeginFadeIn();
	showGroupButton(false, false, false);
	_binh_timebar->setVisible(false);
	//_text_waiting->SetVisible(true);
	//_text_waiting->SetActionCommandWithName("ACTION");
	/*Card* c = Card::NewInstance(0, DECK_TYPE::DECK_MAUBINH);
	addChild(NODE(c));
	for (int i = 0; i < 50; i++)
	{
		c->ResetID(i, DECK_TYPE::DECK_MAUBINH);
	}*/

}

void MauBinhScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (name == "btn_auto_binh" && type_widget == UI_BUTTON)
	{
		MauBinhService::autoBinh(_roomID, _boardID);
	}
	else if (name == "btn_finish_binh" && type_widget == UI_BUTTON)
	{
		std::vector<Card*> cards = getAllCardInGroups();
		if (cards.size() == MAUBINH_MAX_CARD)
		{
			ubyte list_card[MAUBINH_MAX_CARD];
			for (int i = MAUBINH_MAX_CARD - 1; i >= 0; i--)
			{
				list_card[MAUBINH_MAX_CARD - 1 - i] = cards[i]->getID();
			}
			MauBinhService::doFire(_roomID, _boardID, list_card);
		}
	}
	else if (name == "btn_sort_again" && type_widget == UI_BUTTON)
	{
		layoutStartGame(false);
	}
	else if (name == "panel_trans_0")
	{
		if (_lock_trans) return;
		hideLockScreen();

		for (auto player_ui : _pool_player_ui) {
			player_ui->layout_result->reset();
		}
	}
/*	else if (name == "btn_in_game_menu")
	{
		getPlayerGroup(0)->SetTextChat("thua sap ham aaaaaa toan bo la 15 chi");
	}*/
	else
	{
		GameScreen::OnProcessWidgetChild(name, type_widget, _widget);
	}
}

void MauBinhScreen::LoadXMLGameScreenCustom(TiXmlDocument * p_objectXML)
{
	auto p_list_sample_dec = LoadWidgetCustom(p_objectXML);
	PASSERT2(p_list_sample_dec.size() > 0, "no widget loaded!");
	//insert into custom menu

	ParseIntoCustom(p_list_sample_dec.at("custom_root"), p_panel_custom, "panel_custom_menu");
	for (auto it = p_list_sample_dec.begin(); it != p_list_sample_dec.end(); it++)
	{
		delete it->second;
	}
	p_list_sample_dec.clear();
}

void MauBinhScreen::layoutMyCard()
{
	doAlignCenterListCard();
	if(!_list_cards.empty())
		doLayoutGroup(_list_cards.at(0)->GetPosition().y + _list_cards.at(0)->GetSize().y, GROUP_SCALE_DRAG);
	else
		doLayoutGroup(p_panel_bottom->GetPosition().y, GROUP_SCALE_FINAL);

}

void MauBinhScreen::createCard()
{
	for (MauBinhPlayerUI* ui : _pool_player_ui)
	{
		while (ui->cards.size() < MAUBINH_MAX_CARD)
		{
			ui->cards.push_back(createCardMauBinh());
		}

	}
}

void MauBinhScreen::doLayoutGroup(int offset_y, float scale)
{
	if (scale == GROUP_SCALE_FINAL)
	{
		updateLung();
		showLung(_img_lung->isVisible());
		showGroupButton(true, true, true);
	}
	cocos2d::Size win_size = Director::getInstance()->getWinSize();
	Vec2 start_pos = Vec2(win_size.width / 2, offset_y);
	cocos2d::Size visibleSize = Director::getInstance()->getVisibleSize();
	cocos2d::Size e_size = _group1->getRealSize();
	_group1->doLayout(scale, start_pos);
	e_size = _group1->getRealSize();
	start_pos.y += e_size.height;
	_group2->doLayout(scale, start_pos);
	e_size = _group2->getRealSize();

	start_pos.y += e_size.height;
	_group3->doLayout(scale, start_pos);


	Vec2 lung_bot_left = _group1->getCardPosition();
	Vec2 lung_top_right = _group2->getTopRightCard();
	_spr_lung_down->setPosition(lung_bot_left);
	_spr_lung_down->setContentSize(cocos2d::Size(lung_top_right.x - lung_bot_left.x, lung_top_right.y - lung_bot_left.y));

	lung_bot_left = _group3->getCardPosition();
	lung_top_right = _group3->getTopRightCard();
	_spr_lung_up->setPosition(lung_bot_left);
	_spr_lung_up->setContentSize(cocos2d::Size(lung_top_right.x - lung_bot_left.x, lung_top_right.y - lung_bot_left.y));


}

void MauBinhScreen::doLayoutCardComplete()
{
	showLayoutGroup(false);

	showGroupButton(false, false, false);
	showLayoutComplete(getMe()->IDDB, false);
	if (getMe())
	{
		VecCard cards = getMe()->ui->cards;
		for (Card* c : cards)
		{
			c->SetVisible(false);
		}
	}
	_group1->empty();
	_group2->empty();
	_group3->empty();
	showLung(false);
	_img_lung->setVisible(false);
}

void MauBinhScreen::showLayoutComplete(int playerId, bool up)
{
	MauBinhPlayer* p = static_cast<MauBinhPlayer*>(getPlayer(playerId));

	if (!p) return;

	p->ui->layout_result->showCard();

	if (p == getMe())
	{
		std::vector<Card*> list_cards = getAllCardInGroups();
		if (list_cards.size() != MAUBINH_MAX_CARD)
		{
			list_cards = _list_cards;
		}
		MauBinhResult* result = getMe()->ui->layout_result;
		result->setCards(list_cards);
		result->removeFromParent();
		_my_deal_layer->GetResource()->addChild(result);
		cocos2d::Size result_size = result->getSize();
		cocos2d::Size game_size = GetGameSize();
		result->setPosition(
			Vec2((game_size.width - result_size.width) / 2, p_panel_bottom->GetPosition_TopLeft().y));
		result->setVisible(true);
		_panel_trans_1->SetVisible(false);
	}
	else
	{
		MauBinhResult* result = p->ui->layout_result;
		if (up)
		{
			p->ui->layout_result->setCardsState(up);
		}
		else
		{
			result->setCards(p->ui->cards);
		}

		for (Card* c : p->ui->cards)
		{
			c->SetVisible(false);
		}

		if (!result->getParent())
			_other_deal_layer->GetResource()->addChild(result);
		cocos2d::Size game_size = GetGameSize();
		cocos2d::Size result_size = result->getSize();
		PlayerGroup* pg = getPlayerGroup(playerId);
		Vec2 result_pos;
		if (pg->GetSeat() == 1)
		{
			result_pos.x = 0;
			result_pos.y = game_size.height / 2 - result_size.height / 2;
		}
		else if (pg->GetSeat() == 2)
		{
			result_pos.x = (game_size.width - result_size.width) / 2;
			result_pos.y = game_size.height - result_size.height;
		}
		else if (pg->GetSeat() == 3)
		{
			result_pos.x = game_size.width - result_size.width;
			result_pos.y = (game_size.height - result_size.height) / 2;
		}
		result->setPosition(result_pos);
		result->setVisible(true);
	}
}


void MauBinhScreen::initFake()
{
	int myCards[MAUBINH_MAX_CARD];
	int j = 0;
	CardUtils::initCardTest();
	/*for (j = 0; j < 6; j++)
	{
		myCards[j] = CardUtils::getCardTest(j, 0);
	}*/
	for (;j < 13; j++)
	{
		myCards[j] = CardUtils::getCardTestRandom();
	}
//	myCards[0] = 0;
//	myCards[1] = 2;
//	myCards[2] = 8;
//	myCards[3] = 12;
//	myCards[4] = 17;
//	myCards[5] = 18;
//	myCards[6] = 19;
//	myCards[7] = 23;
//	myCards[8] = 26;
//	myCards[9] = 29;
//	myCards[10] = 30;
//	myCards[11] = 49;
//	myCards[12] = 50;
    
	std::string str_cards;
	for (j = 0; j < 13; j++)
	{
		str_cards += StringUtils::format("%d ", myCards[j]);
	}
	CCLOG("List card = %s", str_cards.c_str());

	for (int i = 1; i < p_max_player; i++)
	{
		auto p = GameController::createPlayerByGameID();

		/*if (i != 0) {
			p_list_player_group[i]->addPlayer(p);
		}*/


		static_cast<MauBinhPlayer*>(p)->ui = getUnusedPlayerUI();
	}

	for (size_t i = 0; i < 4; i++)
	{
		auto p = p_list_player_group[i];
		if (!(p && _PLAYER(p)))
		{
			continue;
		}
		for (size_t j = 0; j < MAUBINH_MAX_CARD; j++)
		{
			if (i != 0)
			{
				myCards[j] = CardUtils::getCardTestRandom();
			}
			static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->cards[j]->ResetID(myCards[j], DECK_TYPE::DECK_MAUBINH);
			addChild(NODE(static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->cards[j]));
		}
		seats[i] = i;
	}

	GameController::myInfo->IDDB = 0;
	//Player* p;
	int idx = 0;
	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p))
		{
			_PLAYER(p)->IDDB = idx;
			_PLAYER(p)->money = 10000;
			_PLAYER(p)->name = StringUtils::format("testUser%d", idx);
			Result* test_result = new Result();
			test_result->winChi = 10;
			test_result->winChi01 = 10;
			test_result->winChi02 = -10;
			test_result->winChi03 = 10;
			test_result->winChiAce = 4;
			test_result->userID = idx;
			static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result->setResult(test_result);
		}
		idx++;
	}
	/*setPlayerSeat([this]()->std::vector<Player*> {
		std::vector<Player*> ret;
		for (auto p : this->p_list_player_group)
		{
			ret.push_back(_PLAYER(p));
		}
		return ret;
	}());*/
	_list_cards.clear();
	for (size_t i = 0; i < MAUBINH_MAX_CARD; i++) 
	{
		//static_cast<MauBinhPlayer*>(players[0])->cards[i]->ResetID(myCards[i], DECK_TYPE::DECK_MAUBINH);
		_list_cards.push_back(static_cast<MauBinhPlayer*>(_PLAYER(p_list_player_group[0]))->ui->cards[i]);
	}

	setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	for (Card *c : _list_cards)
	{
		c->SetVisible(true);
	}

	CardUtils::sortByOrder(_list_cards);
	updateSuggest();
	layoutMyCard(); 

	//onAutoBinh();
	//deal();
	//updateSuggest();
	//getPlayerGroup(0)->SetTextChat("thang sap ham aaa 4 chi\n thang sap ham bbb 4 chi.");
	//std::vector<Card*> combo = findThreeOfKind(_list_cards);
	//for (Card*c : combo)
	//{
	//	c->enableHint(true);
	//}
	//doAutoBinh();
	showLayoutGroup(true);
	//_group1->showHighlight(false);
	//_group2->showHighlight(false);
	//_group3->showHighlight(false);
	//onFinishBinh(GameController::myInfo->IDDB);
	
	//doLayoutCardComplete();
	//layoutDealCard();
	//showLayoutComplete( 0, false);
	//showLayoutComplete( 1, false);
	//showLayoutComplete( 2, false);
	//showLayoutComplete( 3, false);
	//showChi3();
	//showChiResult();
	//showChiAce();
	//showTransparent(true);
	//_binh_timebar->onStartime(100, 90);
	//showGroupButton(true, true, true);

	_handle_game_touch = true;
}

void MauBinhScreen::deal()
{
	layoutDealCard();
	Card* last_card;
	for (auto mp : p_list_player_group)
	{
		if (!_PLAYER(mp))
			continue;

		MauBinhPlayer* p = static_cast<MauBinhPlayer*>(_PLAYER(mp));

		if (p->IDDB == -1) continue;
		std::vector<Card*> deal_cards;
		if (getMe() == p)
		{
			deal_cards = _list_cards;
		}
		else
		{
			deal_cards = ((MauBinhPlayer*)p)->ui->cards;
		}

		float start_time = .0f;
		for (auto c : deal_cards)
		{
			last_card = c;
			_deal_card_layer->dealCard(start_time, c, [c, this]() {
				c->SetVisible(true);
				if (NODE(c)->getTag() == TAG_LAST_CARD)
				{
					doneDealCard();
				}
			});
			start_time += 0.1f;
		}
	}
	NODE(last_card)->setTag(TAG_LAST_CARD);
}

void MauBinhScreen::doneDealCard()
{
	_panel_trans_0->SetVisible(true);
	_panel_trans_1->SetVisible(true);

	this->showLayoutGroup(true);
	_group1->showHighlight(true);
	_group2->showHighlight(true);
	_group3->showHighlight(false);

	
	this->updateSuggest();
	_binh_timebar->setVisible(true);
	_binh_timebar->onStartime(getPlayingTime(), 0);
	if (myCardType >= 0) {
		doAutoBinh();
		doLayoutCardComplete();
		this->showGroupButton(false, false, false);
	}
	else {
		this->showGroupButton(true, false, true);
	}
	_handle_game_touch = true;
}

void MauBinhScreen::layoutDealCard()
{
	for (auto p : p_list_player_group)
	{
		if (!_PLAYER(p) || _PLAYER(p)->IDDB == -1)
		{
			continue;
		}
		PlayerGroup* pg = p;

		Vec2 pos_deal;
		Vec2 group_pos = _WIDGET(pg)->GetPosition();
		Vec2 group_size = _WIDGET(pg)->GetSize();
		bool horizontal = false;
		if (pg->GetSeat() == 0)
			continue;
		if(pg->GetSeat() == 1)
		{
			pos_deal = group_pos + Vec2(group_size.x, group_size.y);
		}
		else if (pg->GetSeat() == 2)
		{
			pos_deal = group_pos + Vec2(group_size.x, .0f);
			horizontal = true;
		}
		else if (pg->GetSeat() == 3)
		{
			pos_deal = group_pos - Vec2(group_size.x, - group_size.y / 2);
		}

		const float deal_scale = .5f;
		for (size_t j = 0; j < ((MauBinhPlayer*)_PLAYER(p))->ui->cards.size(); j++)
		{
			Card* c = ((MauBinhPlayer*)_PLAYER(p))->ui->cards[j];
			c->SetSize(Vec2(s_card_w/2, s_card_h/2));
			c->SetStatus(0);
			if (!horizontal)
			{
				if (pg->GetSeat() != 3)
					c->SetAngle(90);
				else
					c->SetAngle(-90);
				c->SetPosition(Vec2(pos_deal.x, pos_deal.y - s_card_w * 1 / 4 * deal_scale * j));
			}
			else
			{
				c->SetPosition(Vec2(pos_deal.x + s_card_w * 1 / 4 * deal_scale *j, pos_deal.y));
			}

			if (NODE(c)->getParent() != this)
			{
				NODE(c)->removeFromParent();
				_other_deal_layer->GetResource()->addChild(NODE(c));
			}
		}

	}
}

void MauBinhScreen::showDealCard(Node* card)
{
	card->setVisible(true);
}
void MauBinhScreen::doAlignCenterListCard()
{
	PASSERT2(p_panel_bottom != nullptr, "bottom panel is null");
	float card_y = p_panel_bottom->GetPosition().y;
	for (Card* c : _list_cards) 
	{
		c->SetSize(Vec2(s_card_w, s_card_h));
	}
	cocos2d::Size game_size = GetGameSize();
	CardUtils::setCardPosition(game_size.width / 2, card_y, game_size.width, _list_cards);
}

int MauBinhScreen::Update(float dt)
{
	if (GameScreen::Update(dt) == 0)
	{
		
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
	if (!_is_finish_update_ui)
	{
		
		MauBinhPlayerUI* ui = new MauBinhPlayerUI();
	/*	for (size_t j = 0; j < MAUBINH_MAX_CARD; j++)
		{
			ui->cards.push_back(createCardMauBinh());
		}*/

		ui->layout_result = MauBinhResult::create();
		ui->layout_result->retain();
		_pool_player_ui.push_back(ui);
		if (_pool_player_ui.size() >= p_max_player)
		{
			for (auto p : p_list_player_group)
			{
				if (p != nullptr
					&& _PLAYER(p))
				{
					if (!((MauBinhPlayer*)_PLAYER(p))->ui)
					{
						((MauBinhPlayer*)_PLAYER(p))->ui = getUnusedPlayerUI();
					}
				}
			}
			_is_finish_update_ui = true;
		}
	}
	else
	{
		for(MauBinhPlayerUI* ui : _pool_player_ui)
		{
			if (ui->cards.size() < MAUBINH_MAX_CARD)
			{
				ui->cards.push_back(createCardMauBinh());
			}
		}
		if (_list_mesage_delay.size() > 0)
		{
			OnHandleMsg(_list_mesage_delay[0]);
			delete _list_mesage_delay[0];
			_list_mesage_delay.erase(_list_mesage_delay.begin());
		}
	
	}
	
	if (_binh_timebar && _binh_timebar->getVisible()){
		_binh_timebar->update(dt);
	}

	for (auto p : p_list_player_group)
	{
		if (p &&_PLAYER(p) && _PLAYER(p)->IDDB != -1)
		{
			MauBinhPlayer* temp_p = static_cast<MauBinhPlayer*>(_PLAYER(p));
			if (temp_p && temp_p->ui)
			{
				for (auto c : temp_p->ui->cards)
				{
					c->Update(dt);
				}
			}
		}
	}

    if (_restTime > 0)
    {
        _restTime -= dt;
        if (_restTime <= 0)
        {
            _restTime = 0;
            static_cast<TextWidget*>(p_text_other1)->SetText(StringUtils::format("%s %d %s", LangMgr->GetString("play_tim").GetString(), playingTime, LangMgr->GetString("seconds").GetString()), true);
        }
        static_cast<TextWidget*>(p_text_other1)->SetText(StringUtils::format("%s %d %s",LangMgr->GetString("this_board's_playin").GetString(), (int)_restTime, LangMgr->GetString("seconds").GetString()), true);
    }

	return 1;
}


void MauBinhScreen::onStartGame(ubyte roomID, ubyte boardID, ubyte* myCards, int interval, char maubinhType)
{
    reset();
	doStartGame();
	if (roomID != this->_roomID || boardID != this->_boardID) {
		// Co the do server tra ve cham nen user da ra phong khac. cai nay
		// khong xu li
		return;
	}
	createCard();
	OnHideCountDown();
	setMyCardType(maubinhType);
	setPlayingTime(interval);
	static_cast<TextWidget*>(p_text_other1)->SetText(StringUtils::format("%s %d %s", LangMgr->GetString("play_tim").GetString(), interval, LangMgr->GetString("seconds").GetString()), true);
	p_text_other1->SetVisible(true);

	std::string log_card = "";
	for (int i = 0; i < MAUBINH_MAX_CARD; i++)
	{
		log_card += StringUtils::format("%d, ", (ubyte)myCards[i]);
	}
	CCLOG("%s", log_card.c_str());
	
	for (size_t i = 0; i < MAUBINH_MAX_CARD; i++) 
	{
		if (getMe()->ui)
		{
			getMe()->ui->cards[i]->ResetID(myCards[i], DECK_TYPE::DECK_MAUBINH);
		}
	}
	for (auto p : p_list_player_group)
	{
		if (p &&_PLAYER(p) && _PLAYER(p)->IDDB != -1)
		{
			getPlayerGroup(_PLAYER(p)->IDDB)->showFlare(false);
			if (static_cast<MauBinhPlayer*>(_PLAYER(p))->ui && static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result)
			{
				static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result->reset();
			}
		}
	}

	//if (maubinhType >= 0)
	//{
	//	_list_cards = getMe()->ui->cards;
	//	doAutoBinh();
	//	doLayoutCardComplete();
	//}
	//else
	{
		layoutStartGame();
	}
	

}

void MauBinhScreen::layoutStartGame(bool deal_or_not)
{
	_group1->empty();
	_group2->empty();
	_group3->empty();

    showLung(false);
    _img_lung->setVisible(false);

	_list_cards.clear();

	for (size_t i = 0; i < MAUBINH_MAX_CARD; i++)
	{
		if (getMe()->ui)
		{
			_list_cards.push_back(getMe()->ui->cards[i]);
			NODE(_list_cards[i])->removeFromParent();
		}
	}

	for (Card *c : _list_cards)
	{
		_my_deal_layer->GetResource()->addChild(NODE(c));
	}

	updateSuggest();

	doAlignCenterListCard();

	doLayoutGroup(_list_cards.at(0)->GetPosition().y + _list_cards.at(0)->GetSize().y, GROUP_SCALE_DRAG);


	if (deal_or_not)
	{
		deal();
	}
	else
	{
		_group1->showHighlight(true);
		_group2->showHighlight(true);
	}
}

std::vector<Card*> MauBinhScreen::findFlushStraight(std::vector<Card*> cards) {
	std::vector<Card*> cardStraight;
	std::vector<Card*> cardResult;
	CardUtils::sortBySuitOrder(cards);
	// thuc hien sap lai tim sanh lien tiep cung chat
	for (int i = (int)cards.size() - 1; i > 0; i--) {
		if (cards.at(i)->getID() / 4 - 1 == cards.at(i - 1)->getID() / 4 && cards.at(i)->getID() % 4 == cards.at(i - 1)->getID() % 4) {
			if (std::find(std::begin(cardStraight), std::end(cardStraight), cards.at(i)) == std::end(cardStraight)) {
				cardStraight.push_back(cards.at(i));
				if (cardStraight.size() >= 5) {
					CardUtils::sortByOrder(cardStraight);
					if (cardResult.size() > 0) {
						if (CardUtils::compareWithStraight(cardResult, cardStraight) == 1) {
							cardResult.clear();
							cardResult.insert(cardResult.begin(), cardStraight.begin(), cardStraight.end());
						}
					}
					else {
						cardResult.clear();
						cardResult.insert(cardResult.begin(), cardStraight.begin(), cardStraight.end());
					}
				}
			}

			if (std::find(std::begin(cardStraight), std::end(cardStraight), cards.at(i - 1)) == std::end(cardStraight)) {
				cardStraight.push_back(cards.at(i - 1));
				if (cardStraight.size() >= 5) {
					CardUtils::sortByOrder(cardStraight);
					if (cardResult.size() > 0) {
						if (CardUtils::compareWithStraight(cardResult, cardStraight) == 1) {
							cardResult.clear();
							cardResult.insert(cardResult.begin(), cardStraight.begin(), cardStraight.end());
						}
					}
					else {
						cardResult.clear();
						cardResult.insert(cardResult.begin(), cardStraight.begin(), cardStraight.end());
					}
				}
			}

		}
		else {
			cardStraight.clear();
		}
	}
	return cardResult;
}
//
///***
//* tim tu quy luu y: se tra ve ty quy lon nhat
//*/
std::vector<Card*> MauBinhScreen::findFourOfKind(std::vector<Card*> cards) {
	std::vector<Card*> resultCard;
	CardUtils::sortByOrder(cards);
	for (int i = (int)cards.size() - 1; i > 0; i--) {
		if (cards.at(i)->getID() / 4 == cards.at(i - 1)->getID() / 4) {
			if (std::find(resultCard.begin(), resultCard.end(), cards.at(i)) == resultCard.end())
			{
				resultCard.push_back(cards.at(i));
				if (resultCard.size() == 4) {
					CardUtils::sortByOrder(resultCard);
					return resultCard;
				}
			}
			if (std::find(resultCard.begin(), resultCard.end(), cards.at(i - 1)) == resultCard.end())
			{
				resultCard.push_back(cards.at(i - 1));
				if (resultCard.size() == 4) {
					CardUtils::sortByOrder(resultCard);
					return resultCard;
				}
			}
		}
		else {
			resultCard.clear();
		}
	}
	resultCard.clear();
	return resultCard;
}

///***
//* tim ba cay luu y: se tra ve ty quy lon nhat
//*/
std::vector<Card*> MauBinhScreen::findThreeOfKind(std::vector<Card*> cards) {
	std::vector<Card*> resultCard;
	CardUtils::sortByOrder(cards);
	for (int i = (int)cards.size() - 1; i > 0; i--) {
		if (cards.at(i)->getID() / 4 == cards.at(i - 1)->getID() / 4)
		{
			if (std::find(resultCard.begin(), resultCard.end(), cards.at(i)) == resultCard.end())
			{
				resultCard.push_back(cards.at(i));
				if (resultCard.size() == 3) {
					CardUtils::sortByOrder(resultCard);
					return resultCard;
				}
			}
			if (std::find(resultCard.begin(), resultCard.end(), cards.at(i - 1)) == resultCard.end())
			{
				resultCard.push_back(cards.at(i - 1));
				if (resultCard.size() == 3) {
					CardUtils::sortByOrder(resultCard);
					return resultCard;
				}
			}
		}
		else {
			resultCard.clear();
		}
	}
	resultCard.clear();
	return resultCard;
}

void MauBinhScreen::CreatePLayer(std::vector<Player*> playerInfos)
{
	for (auto p : playerInfos)
	{
		if (p->IDDB == -1)
		{
			continue;
		}
		MauBinhPlayerUI* ui = getUnusedPlayerUI();
		static_cast<MauBinhPlayer*>(p)->ui = ui;
	}
}
///***
//* tim thung, nam la cung mau khong phai la sanh
//*/
std::vector<Card*> MauBinhScreen::findFlush(std::vector<Card*> cards)
{
	std::vector<Card*> resultCard;
	std::vector<Card*> resultCardCheck;
	CardUtils::sortBySuitOrder(cards);
	for (int i = (int)cards.size() - 1; i > 0; i--) {
		if (cards.at(i)->getID() % 4 == cards.at(i - 1)->getID() % 4) {
			//if (!resultCard.contains(cards.get(i), true)) {
			if (std::find(resultCard.begin(), resultCard.end(), cards.at(i)) == resultCard.end())
			{
				resultCard.push_back(cards.at(i));
				if (resultCard.size() >= 5) {
					CardUtils::sortByOrder(resultCard);
					if (resultCardCheck.size() > 0) {
						if (compareCardByCard(resultCard, resultCardCheck) == -1) {
							resultCardCheck.clear();
							resultCardCheck.insert(resultCardCheck.end(), resultCard.begin(), resultCard.end());
						}
					}
					else {
						resultCardCheck.clear();
						resultCardCheck.insert(resultCardCheck.end(), resultCard.begin(), resultCard.end());
					}
				}
			}
			//if (!resultCard.contains(cards.get(i - 1), true)) {
			if (std::find(resultCard.begin(), resultCard.end(), cards.at(i - 1)) == resultCard.end())
			{
				resultCard.push_back(cards.at(i - 1));
				if (resultCard.size() >= 5) {
					CardUtils::sortByOrder(resultCard);
					// return resurlCard;
					if (resultCardCheck.size() > 0) {
						if (compareCardByCard(resultCard, resultCardCheck) == -1) {
							resultCardCheck.clear();
							resultCardCheck.insert(resultCardCheck.end(), resultCard.begin(), resultCard.end());
						}
					}
					else {
						resultCardCheck.clear();
						resultCardCheck.insert(resultCardCheck.end(), resultCard.begin(), resultCard.end());
					}
				}
			}
		}
		else {
			resultCard.clear();
		}
	}
	resultCard.clear();
	return resultCardCheck;
}

///***
//* tim sanh binh thuong luu y: se tra ve sanh lon nhat co the co
//*/
std::vector<Card*> MauBinhScreen::findStraight(std::vector<Card*> cards)
{
	std::vector<Card*> resultCard;
	CardUtils::sortByOrder(cards);
	resultCard.push_back(cards.at(cards.size() - 1));
	for (int i = (int)cards.size() - 2; i >= 0; i--) {
		int temp = cards.at(i + 1)->getID() / 4 - cards.at(i)->getID() / 4;
		switch (temp) {
		case 0:
			break;
		case 1:
			resultCard.push_back(cards.at(i));
			break;
		default:
			resultCard.clear();
			resultCard.push_back(cards.at(i));
			break;
		}
		if (resultCard.size() == 5) {
			return resultCard;
		}
	}
	resultCard.clear();
	return resultCard;
}
//
///**
//* tim doi type: 0 tim doi nho nhat 1 tim doi lon nhat
//*/
std::vector<Card*> MauBinhScreen::findTwoCard(std::vector<Card*> cards, int type)
{
	std::vector<Card*> result;
	CardUtils::sortByOrder(cards);

	if (type == 1) {
		for (int i = (int)cards.size() - 1; i > 0; i--) {
			if (cards.at(i)->getID() / 4 == cards.at(i - 1)->getID() / 4) {
				result.push_back(cards.at(i));
				result.push_back(cards.at(i - 1));
				return result;
			}
		}
	}
	else {
		for (size_t i = 0; i < cards.size() - 1; i++) {
			if (cards.at(i)->getID() / 4 == cards.at(i + 1)->getID() / 4) {
				result.push_back(cards.at(i));
				result.push_back(cards.at(i + 1));
				return result;
			}
		}
	}

	return result;
}
int MauBinhScreen::numberTwoCard(std::vector<Card*> cards) {
	std::vector<Card*> temp;
	temp.insert(temp.end(), cards.begin(), cards.end());
	int number = 0;
	bool haveTwoCard = true;
	while (haveTwoCard && temp.size() >= 2) {
		if (findTwoCard(temp, 0).size() == 2) {
			number++;
			std::vector<Card*> temp1 = findTwoCard(temp, 0);
			for (size_t i = 0; i < temp1.size(); i++)
			{
				auto it = std::find(temp.begin(), temp.end(), temp1.at(i));
				if (it != temp.end()) temp.erase(it);
			}
			haveTwoCard = true;
		}
		else {
			haveTwoCard = false;
		}

	}
	return number;
}

//
///**
//* tim hai doi
//*/
std::vector<Card*> MauBinhScreen::findTowPair(std::vector<Card*> cards)
{
	std::vector<Card*> result;
	std::vector<Card*> cardsTemp;
	if (findTwoCard(cardsTemp, 1).size() == 2) {
		// tim thay co doi lan 1
		std::vector<Card*> cardsTemp1 = findTwoCard(cardsTemp, 1);
		result.insert(result.end(), cardsTemp1.begin(), cardsTemp1.end());
		for (size_t i = 0; i < result.size(); i++) {
			auto it = std::find(cardsTemp.begin(), cardsTemp.end(), result.at(i));
			if (it != cardsTemp.end()) cardsTemp.erase(it);
		}
		// tim 1 doi tiep theo
		if (numberTwoCard(cards) != 5) {
			if (findTwoCard(cardsTemp, 0).size() == 2) {
				std::vector<Card*> cardsTemp2 = findTwoCard(cardsTemp, 0);
				result.insert(result.end(), cardsTemp2.begin(), cardsTemp2.end());
				return result;
			}
		}
		else {
			if (findTwoCard(cardsTemp, 1).size() == 2) {
				std::vector<Card*> cardsTemp2 = findTwoCard(cardsTemp, 0);
				result.insert(result.end(), cardsTemp2.begin(), cardsTemp2.end());
				return result;
			}
		}

	}
	result.clear();
	return result;
}
bool MauBinhScreen::isHaveValue(std::vector<Card*> cards, ubyte value) {
	for (size_t i = 0; i < cards.size(); i++) {
		if (cards.at(i)->getID() == value)
			return true;
	}
	return false;
}

std::vector<Card*> MauBinhScreen::findAndGetCard(std::vector<Card*> cards, ubyte chat, ubyte value) {
	std::vector<Card*> result;
	if (chat <= 0) {
		for (size_t i = 0; i < cards.size(); i++) {
			if (cards.at(i)->getID() / 4 == value) {
				result.push_back(cards.at(i));
				return result;
			}
		}
	}
	else {
		for (size_t i = 0; i < cards.size(); i++) {
			if (cards.at(i)->getID() == (ubyte)value * 4 + chat) {
				result.push_back(cards.at(i));
				return result;
			}
		}
	}

	return result;
}
///**
//* tim sanh lon thu A-2-3-4-5
//*
//*/
std::vector<Card*> MauBinhScreen::findStraightTwoSameSuit(std::vector<Card*> cards) {
	std::vector<Card*> result;
	// tim sanh nhi cung chat
	CardUtils::sortBySuitOrder(cards);
	result.push_back(cards.at(0));
	for (size_t i = 0; i < cards.size() - 1; i++) {
		int temp = cards.at(i + 1)->getID() / 4 - cards.at(i)->getID() / 4;
		if (cards.at(i + 1)->getID() % 4 == cards.at(i)->getID() % 4) {
			switch (temp) {
			case 0:
				break;
			case 1:
				result.push_back(cards.at(i + 1));
				break;
			default:
				result.clear();
				result.push_back(cards.at(i + 1));
				break;
			}
		}
		else {
			result.clear();
			result.push_back(cards.at(i + 1));
		}
		if (result.size() == 4) {
			// xem co phai bat dau bang con hai
			if (isHaveValue(result, (ubyte)0)) {
				// kiem tra xem co con at phu hop khong
				if (findAndGetCard(cards, (ubyte)(result.at(0)->getID() % 4), (ubyte)12).size() > 0) {
					result.insert(result.begin(), findAndGetCard(cards, (ubyte)(result.at(0)->getID() % 4), (ubyte)12).at(0));
					return result;
				}
			}

		}
	}
	result.clear();

	return result;
}
//
///**
//* tim sanh lon thu A-2-3-4-5
//*
//*/
std::vector<Card*> MauBinhScreen::findStraightTwoNotSameSuit(std::vector<Card*> cards)
{
	std::vector<Card*> result;
	// tim sanh nhi cung chat
	CardUtils::sortByOrder(cards);
	result.push_back(cards.at(0));
	for (size_t i = 0; i < cards.size() - 1; i++) {
		int temp = cards.at(i + 1)->getID() / 4 - cards.at(i)->getID() / 4;

		switch (temp) {
		case 0:
			break;
		case 1:
			result.push_back(cards.at(i + 1));
			break;
		default:
			result.clear();
			result.push_back(cards.at(i + 1));
			break;
		}

		if (result.size() == 4) {
			// xem co phai bat dau bang con hai
			if (isHaveValue(result, (ubyte)0)) {
				// kiem tra xem co con at phu hop khong
				std::vector<Card*> temp = findAndGetCard(cards, (ubyte)(-1), (ubyte)12);
				if (temp.size() > 0) {
					result.insert(result.begin(), temp.at(0));
					return result;
				}
			}

		}
	}

	result.clear();

	return result;
}

/**
* Compare card by card together. please check your cards before use this
* function.
*
* @param cards1
*            must have 5 or 3 elements.
* @param cards2
*            must have 5 or 3 elements.
* @return -1 if cards1 is lager than cards2; 0 if equal, 1 if cards1 is
*         smaller than cards2 and 1989 if error.
*/

int MauBinhScreen::compareCardByCard(std::vector<Card*> cards1, std::vector<Card*> cards2)
{
	int length01 = cards1.size();
	int length02 = cards2.size();
	int cardNo = MIN(length01, length02);

	if (cardNo > 0) {
		int check = 0;
		do {
			if (cards1[length01 - 1]->GetCardValue() > cards2[length02 - 1]->GetCardValue()) {
				return -1;
			}
			else if (cards1[length01 - 1]->GetCardValue() < cards2[length02 - 1]->GetCardValue()) {
				return 1;
			}
			else {
				length01--;
				length02--;
				if (length01 < 1 || length02 < 1)
					return 0;
			}
		} while (check == 0);
	}
	return -2;
}

void MauBinhScreen::enableHint(std::vector<Card*> cards, bool enable)
{
	for (Card*c : cards)
	{
		c->enableHint(enable);
	}
}
bool MauBinhScreen::isSameChat(std::vector<Card*> cards) {
	for (size_t i = 0; i < cards.size() - 1; i++) {
		if (cards.at(i)->getID() % 4 != cards.at(i + 1)->getID() % 4)
			return false;
	}
	return true;
}

int MauBinhScreen::numberCardSameValueInCard(std::vector<Card*> cards, ubyte idCard)
{
	int number = 0;
	for (size_t i = 0; i < cards.size(); i++) {
		if (cards.at(i)->getID() / 4 == idCard / 4)
			number++;
	}
	return number;
}

std::vector<Card*> MauBinhScreen::cardMinAndNotContandCardSmartOneCard(std::vector<Card*> cardNotSame, std::vector<Card*> cardFree) {
	std::vector<Card*> result;
	CardUtils::sortByOrder(cardFree);
	for (size_t i = 0; i < cardFree.size(); i++) {
		if (numberCardSameValueInCard(cardFree, cardFree.at(i)->getID()) == 1) {
			if (std::find(cardNotSame.begin(), cardNotSame.end(), cardFree.at(i)) == cardNotSame.end())
			{
				result.push_back(cardFree.at(i));
				return result;
			}
		}
	}

	if (std::find(cardNotSame.begin(), cardNotSame.end(), cardFree.at(0)) == cardNotSame.end())
	{
		result.push_back(cardFree.at(0));
		return result;
	}

	return result;
}


bool MauBinhScreen::haveThreeCardWithId(std::vector<Card*> cards, int idCard)
{
	int number = 0;
	for (size_t i = 0; i < cards.size(); i++) {
		if (cards.at(i)->getID() / 4 == idCard / 4)
			number++;
	}
	if (number >= 3)
		return true;
	else
		return false;
}

std::vector<Card*> MauBinhScreen::cardMinAndNotContandCard(std::vector<Card*> card, std::vector<Card*> cardFree, int sumNumber)
{
	std::vector<Card*> result;
	CardUtils::sortByOrder(cardFree);
	int number = 0;
	int i = 0;
	do {
		if (cardFree.at(i) != nullptr) {
			//if (!card.contains(cardFree.get(i), true)) {
			if (std::find(card.begin(), card.end(), cardFree.at(i)) == card.end())
			{
				result.push_back(cardFree.at(i));
				number++;
			}
			i++;
		}
		else
			break;
	} while (number != sumNumber);
	return result;
}

std::vector<Card*> MauBinhScreen::findMaxCard(std::vector<Card*> cards, int number)
{
	std::vector<Card*> result;
	CardUtils::sortByOrder(cards);
	if (cards.size() < (size_t)number)
		return result;
	for (int i = (int)cards.size() - 1; i > (int)cards.size() - 1 - number; i--) {
		result.push_back(cards.at(i));
	}
	return result;
}

void MauBinhScreen::updateSuggest()
{
	_group1->setCanTap(_group1->getAvailableSlot() > 0);
	_group2->setCanTap(_group2->getAvailableSlot() > 0);
	if (_group1->getAvailableSlot() != 0)
	{
		if (_group1->getAvailableSlot() != 5)
		{
			_group1->setFocus(false);
			enableHint(_list_cards, false);
			return;
		}
		else
		{
			_group1->setFocus(true);
			showArrow(_group1);
		}
	}
	else if (_group2->getAvailableSlot() != 0)
	{
		if (_group2->getAvailableSlot() != 5)
		{
			_group2->setFocus(false);
			enableHint(_list_cards, false);
			updateLung();
			showLung(_img_lung->isVisible());
			return;
		}
		else
		{
			_group2->setFocus(true);
			showArrow(_group2);
		}
	}
	else
	{
		addCardToGroup(_group3, _list_cards);
		hideArrow();
	}

	if (_list_cards.size() < 5)
		return;
	std::vector<Card*> result = findFlushStraight(_list_cards);
	if (result.size() == 5) {
		// kiem tra xem co at khong
		if (isHaveValue(result, (ubyte)12)) {
			// co at tuc no la bai lon nhat
			enableHint(result);
			return;
		}
		else {
			// khong co at check xem co sanh hai kh
			std::vector<Card*> temp = findStraightTwoSameSuit(_list_cards);
			if (temp.size() > 0) {
				// co sanh lon thu 2 check tiep xem co cung chat khong
				if (isSameChat(temp)) {
					enableHint(temp);
					return;
				}
				else {
					enableHint(result);
					return;
				}
			}
			else {
				enableHint(result);
				return;
			}
		}
		// setColorAllCardHind(findFlushStraight(cardInFree));
		// return true;
	}
	else {
		// co the kh co sanh cung chat nhung se ton tai sanh A-2-3-4-5
		// do thuat toan tim sanh chua tim sanh nay
		std::vector<Card*> temp = findStraightTwoSameSuit(_list_cards);
		if (temp.size() == 5) {
			enableHint(temp);
			return;
		}
	}

	result = findFourOfKind(_list_cards);
	if (result.size() == 4) {
		std::vector<Card*> tempCard;
		tempCard.insert(tempCard.end(), result.begin(), result.end());
		std::vector<Card*> temp1 = cardMinAndNotContandCardSmartOneCard(result, _list_cards);
		if (!temp1.empty()) {
			tempCard.insert(tempCard.end(), temp1.begin(), temp1.end());
		}
		if (tempCard.size() == 5) {
			enableHint(tempCard);
			return;
		}
	}
	// tim bo ba va doi
	// ???
	result = findThreeOfKind(_list_cards);
	if (result.size() == 3) {
		std::vector<Card*> cardTemp;
		for (size_t i = 0; i < _list_cards.size(); i++) {
			if (std::find(result.begin(), result.end(), _list_cards.at(i)) == result.end())
			{
				cardTemp.push_back(_list_cards.at(i));
			}
		}
		std::vector<Card*> temp2 = findTwoCard(cardTemp, 0);
		// kiem tra tiep doi ay co the nam trong 3 con nho hon
		if (temp2.size() == 2) {
			std::vector<Card*> cardTempForCheck;
			cardTempForCheck.insert(cardTempForCheck.end(), cardTemp.begin(), cardTemp.end());
			//cardTempForCheck.removeAll(findThreeOfKind(cardInFree), true);
			for (size_t i = 0; i < temp2.size(); i++)
			{
				auto it = std::find(cardTempForCheck.begin(), cardTempForCheck.end(), temp2[i]);
				if (it != cardTempForCheck.end()) cardTempForCheck.erase(it);
			}
			int numberTwoCardSize = numberTwoCard(cardTempForCheck);
			for (int i = 0; i < numberTwoCardSize; i++) {
				std::vector<Card*> temp3 = findTwoCard(cardTempForCheck, 0);
				if (!haveThreeCardWithId(cardTempForCheck, temp3.at(0)->getID())) {
					result.insert(result.end(), temp3.begin(), temp3.end());
					enableHint(result);
					return;
				}
				else {
					for (size_t j = 0; j < temp3.size(); j++)
					{
						auto it = std::find(cardTempForCheck.begin(), cardTempForCheck.end(), temp3.at(j));
						if (it != cardTempForCheck.end()) cardTempForCheck.erase(it);
					}
				}
			}
			// tat ca deu nam tran 3 con thi nhat dinh chon 1 3 cay xe
			// ra
			result.insert(result.end(), temp2.begin(), temp2.end());
			enableHint(result);
			return;
		}

	}
	// tim 5 la cung chat
	result = findFlush(_list_cards);
	if (result.size() == 5) {
		enableHint(result);
		return;
	}
	// tim sanh rong
	result = findStraight(_list_cards);
	if (result.size() == 5) {
		if (isHaveValue(result, (ubyte)12)) {
			enableHint(result);
			return;
		}
		else {
			std::vector<Card*> temp1 = findStraightTwoNotSameSuit(_list_cards);
			if (temp1.size() > 0) {
				enableHint(temp1);
				return;
			}
			else {
				enableHint(result);
				return;
			}
		}

	}
	else {
		// kh co sanh nhung co the se chua sanh A-2-3-4-5
		result = findStraightTwoNotSameSuit(_list_cards);
		if (result.size() == 5) {
			enableHint(result);
			return;
		}
	}

	result = findThreeOfKind(_list_cards);
	// tim ba cay va 2 la rac
	if (result.size() == 3) {

		std::vector<Card*> temp = cardMinAndNotContandCard(result, _list_cards, 2);
		if (!temp.empty()) {
			result.insert(result.end(), temp.begin(), temp.end());
		}
		if (result.size() == 5) {
			enableHint(result);
			return;
		}
	}
	result = findTowPair(_list_cards);
	// tim hai doi va 1 la rac
	if (result.size() == 4) {

		std::vector<Card*> temp1 = cardMinAndNotContandCardSmartOneCard(result, _list_cards);
		if (temp1.size() == 1) {
			result.push_back(temp1[0]);
		}
		if (result.size() == 5) {
			enableHint(result);
			return;
		}
	}

	result = findTwoCard(_list_cards, 1);
	// tim 1 doi va 3 la rac
	if (result.size() == 2) {
		std::vector<Card*> temp1 = cardMinAndNotContandCard(result, _list_cards, 3);
		if (!temp1.empty()) {
			result.insert(result.end(), temp1.begin(), temp1.end());
		}
		if (result.size() == 5) {
			enableHint(result);
			return;
		}
	}
	// tim 5 con rac
	// setColorAllCardHindWithFreeCard(findMaxCard(cardInFree, 5),
	// cardInFree);
	enableHint(findMaxCard(_list_cards, 5));
	return;
}


MauBinhPlayer* MauBinhScreen::getMe()
{
	return (MauBinhPlayer*)getMyPlayer();
}

void MauBinhScreen::showLayoutGroup(bool visible)
{
	_group1->showGroup(visible);
	_group2->showGroup(visible);
	_group3->showGroup(visible);
	

	showTransparent(visible);
}

std::vector<Card*> MauBinhScreen::getAllCardInGroups()
{
	std::vector<Card*> rs;
	std::vector<Card*> cards_in_group;
	cards_in_group = _group1->getCards();
	rs.insert(rs.end(), cards_in_group.begin(), cards_in_group.end());
	cards_in_group = _group2->getCards();
	rs.insert(rs.end(), cards_in_group.begin(), cards_in_group.end());
	cards_in_group = _group3->getCards();
	rs.insert(rs.end(), cards_in_group.begin(), cards_in_group.end());
	return rs;
}

void MauBinhScreen::showCard()
{
}

void MauBinhScreen::showChi1()
{
	for (auto p : p_list_player_group)
	{
		if (p &&_PLAYER(p) && _PLAYER(p)->IDDB != -1 && static_cast<MauBinhPlayer*>(_PLAYER(p))->ui && static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result->getResult())
		{
			static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result->showChi1();
		}
	}
}

void MauBinhScreen::showChi2()
{
	for (auto p : p_list_player_group)
	{
		if (p &&_PLAYER(p) && _PLAYER(p)->IDDB != -1 && static_cast<MauBinhPlayer*>(_PLAYER(p))->ui && static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result)
		{
			static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result->showChi2();
		}
	}
}

void MauBinhScreen::showChi3()
{
	for (auto p : p_list_player_group)
	{
		if (p &&_PLAYER(p) && _PLAYER(p)->IDDB != -1 && static_cast<MauBinhPlayer*>(_PLAYER(p))->ui && static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result)
		{
			static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result->showChi3();
		}
	}
}

void MauBinhScreen::showChiAce()
{
	for (auto p : p_list_player_group)
	{
		if (p &&_PLAYER(p) && _PLAYER(p)->IDDB != -1 && static_cast<MauBinhPlayer*>(_PLAYER(p))->ui)
		{
			static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result->showChiAce();
		}
	}
}

void MauBinhScreen::showChiResult()
{
	for (auto p : p_list_player_group)
	{
		if (p &&_PLAYER(p) && _PLAYER(p)->IDDB != -1 && static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result->getResult())
		{
			auto mp = static_cast<MauBinhPlayer*>(_PLAYER(p));
			mp->ui->layout_result->showTotal();
		}
	}
	endResult();
}

void MauBinhScreen::hideLayoutResult()
{
	for (auto p : p_list_player_group)
	{
		if (p && _PLAYER(p) && _PLAYER(p)->IDDB != -1 && static_cast<MauBinhPlayer*>(_PLAYER(p))->ui)
		{
			static_cast<MauBinhPlayer*>(_PLAYER(p))->ui->layout_result->removeFromParent();
		}
	}
}

void MauBinhScreen::playResult()
{
	if (!_list_action_result.empty())
	{
		MaubinhAction action = _list_action_result.front();
		Sequence* sequence = Sequence::create(action.func, DelayTime::create(action.delay_time), CallFunc::create(CC_CALLBACK_0(MauBinhScreen::playResult, this)), nullptr);
		runAction(sequence);
		_list_action_result.erase(_list_action_result.begin());
	}
}

void MauBinhScreen::addActionInStack(CallFunc* func, float delayTime)
{
	MaubinhAction action;
	action.func = func;
	action.delay_time = delayTime;
	_list_action_result.push_back(action);
}

void MauBinhScreen::doAutoBinh()
{
	int j = 0;
	for (;j < 3; j++)
	{
		_group3->addCard(_list_cards[j]);
	}
	for (;j < 8; j++)
	{
		_group2->addCard(_list_cards[j]);
	}
	for (;j < MAUBINH_MAX_CARD; j++)
	{
		_group1->addCard(_list_cards[j]);
	}
	_list_cards.clear();
	hideArrow();
	PASSERT2(p_panel_bottom != nullptr, "bottom panel is null");
	doLayoutGroup(p_panel_bottom->GetPosition().y, GROUP_SCALE_FINAL);
}

void MauBinhScreen::onAutoBinh(ubyte* cards)
{
	_group1->empty();
	_group2->empty();
	_group3->empty();
	_list_cards = getMe()->ui->cards;
	for (size_t i = 0; i < MAUBINH_MAX_CARD; i++) {
		_list_cards[i]->ResetID(cards[i], DECK_TYPE::DECK_MAUBINH);
	}

	enableHint(_list_cards, false);
	doAutoBinh();
	showGroupButton(true, true, true);
}

void MauBinhScreen::onFinishBinh(int userId)
{
	if (getMe()->IDDB == userId)
	{
		doLayoutCardComplete();
		showGroupButton(false, false, false);
		//GameController.playSound(iWinSound.READY);
		GetSound->PlaySoundEffect("SFX_READY");
		GetMauBinhScreen()->OnShowCountDown(_binh_timebar->getTimeRemain(), .0f);
        static_cast<TextWidget*>(p_panel_count_down->GetWidgetChildByName(".timer_text"))->SetText(LangMgr->GetString("wait_others"));
		_binh_timebar->onStop();
	}
	else
	{
		showLayoutComplete(userId, true);
	}

}

void MauBinhScreen::onResult(std::vector<Result*> results)
{
	_handle_game_touch = false;
	doStopGame();
	OnHideCountDown();
	_binh_timebar->onStop();
	Result* r;
	MauBinhPlayer* p;
	int count_maubinh = 0;
    bool hasAnyLung = false;
    for(size_t i = 0; i < results.size(); i++)
    {
        hasAnyLung = hasAnyLung || results.at(i)->mauBinhType == -2;
    }
    
	for (size_t i = 1; i < results.size(); i++)
	{
		r = results.at(i);
		p = static_cast<MauBinhPlayer*>(getPlayer(r->userID));
		if (p && p->ui)
		{

			r->cards = p->ui->cards;
			int idx = 0;
			for (size_t i = 8; i < 13; i++)
			{
				r->cards[idx++]->ResetID(r->cardIds[i], DECK_TYPE::DECK_MAUBINH);
			}
			for (size_t i = 3; i < 8; i++)
			{
				r->cards[idx++]->ResetID(r->cardIds[i], DECK_TYPE::DECK_MAUBINH);
			}
			for (size_t i = 0; i < 3; i++)
			{
				r->cards[idx++]->ResetID(r->cardIds[i], DECK_TYPE::DECK_MAUBINH);
			}
			for (int i = 0; i < MAUBINH_MAX_CARD; i++)
			{
				p->ui->cards[i]->SetVisible(false);
			}
			p->ui->layout_result->setResult(r);
			p->ui->layout_result->setCards(r->cards);
		}
		results[0]->winChi01 -= r->winChi01;
		results[0]->winChi02 -= r->winChi02;
		results[0]->winChi03 -= r->winChi03;
		showLayoutComplete(r->userID, false);
		if (r->mauBinhType >= 0) count_maubinh++;
	}

	showTransparent(true);
	_lock_trans = true;
	if (getMe() && getMe()->ui)
	{
		MauBinhResult* result = getMe()->ui->layout_result;
		if (!result->isVisible())
		{
			doLayoutCardComplete();

			for (MauBinhPlayerUI* ui : _pool_player_ui)
			{
				ui->layout_result->setVisible(false);
			}
		}
		getMe()->ui->layout_result->setResult(results.at(0));

	}
	if (results.at(0)->mauBinhType >= 0) count_maubinh++;
	CallFunc* func = CallFunc::create(CC_CALLBACK_0(MauBinhScreen::showCard, this));
	func->retain();
	addActionInStack(func, 0.0f);

	if (count_maubinh != results.size() - 1 && !hasAnyLung)
    {
		func = CallFunc::create(CC_CALLBACK_0(MauBinhScreen::showChi1, this));
		func->retain();
		addActionInStack(func, 2.0f);

		func = CallFunc::create(CC_CALLBACK_0(MauBinhScreen::showChi2, this));
		func->retain();
		addActionInStack(func, 2.0f);

		func = CallFunc::create(CC_CALLBACK_0(MauBinhScreen::showChi3, this));
		func->retain();
		addActionInStack(func, 2.0f);

		func = CallFunc::create(CC_CALLBACK_0(MauBinhScreen::showChiAce, this));
		func->retain();
		addActionInStack(func, 2.0f);
	}

	func = CallFunc::create(CC_CALLBACK_0(MauBinhScreen::showChiResult, this));
	func->retain();
	addActionInStack(func, 2.0f);

//    action to hide result after all
 /*   func = CallFunc::create(CC_CALLBACK_0(MauBinhScreen::reset, this));
    func->retain();
    addActionInStack(func, 4.0f);*/
    
	playResult();
}

void MauBinhScreen::onBinhTimer(int interval, int remain)
{
	_binh_timebar->setVisible(true);
	_binh_timebar->onStartime(interval, remain);
}

void MauBinhScreen::onPlayingTime(int time, int timeType)
{
	setPlayingTime(time);
	setTimeMode(timeType);
    
    static_cast<TextWidget*>(p_text_other1)->SetText(StringUtils::format("%s %d %s", LangMgr->GetString("play_tim").GetString(), time, LangMgr->GetString("seconds").GetString()), true);
    p_text_other1->SetVisible(true);
}

void MauBinhScreen::onBonusMoneyBinh(int userId, long money, std::string des)
{
	PlayerGroup* group = getUserGroup(userId);
	GetFlyMoney()->fly(money, _WIDGET(group)->GetPosition(), this);
}

void MauBinhScreen::onTextSapHam(std::string textSapHam)
{
	RKString text = RKString(textSapHam);
	RKList<RKString> list_str = text.Split(",");
	RKString final_str = "";
	for (size_t i = 0; i < list_str.Size(); i++)
	{
		if (i != list_str.Size() - 1) {
			final_str += list_str[i] + "\n";
		} else {
			final_str += list_str[i] + ".";
		}
		
	}
	setTextSapHam(final_str.GetString());
}

void MauBinhScreen::onSomeOneJoinBoard(int seat, Player* p)
{
	GameScreen::onSomeOneJoinBoard(seat, p);
	static_cast<MauBinhPlayer*>(p)->ui = getUnusedPlayerUI();
}

void MauBinhScreen::onSomeOneLeaveBoard(int leave, int newOwner)
{
	GameScreen::onSomeOneLeaveBoard(leave, newOwner);

}

void MauBinhScreen::updateGroupDragPos(Vec2 pos)
{
	cocos2d::Size group_size = _group_drag->getSize();
	_group_drag->setPosition(Vec2(pos.x - group_size.width / 2, pos.y - group_size.height));
}

void MauBinhScreen::updateGroupDrag()
{
	std::vector<Card*> selected;

	for (Card * c : _list_cards)
	{
		if (c->isHighlight() || c->isSelected())
		{
			selected.push_back(c);
			c->SetOpacity(128);
		}

	}

	if (!selected.empty())
	{
		_group_drag->setVisible(true);
		_group_drag->setCards(selected);
	}
	for (int i = 0; i < selected.size(); i++)
	{
		selected[i]->SelectCardActionMB(true);
	}

}

std::vector<Card*> MauBinhScreen::getCardsSelected()
{
	std::vector<Card*> selected;
	for (Card*c : _list_cards)
	{
		if (c->isSelected()) selected.push_back(c);
	}
	return selected;
}

void MauBinhScreen::addCardToGroup(GroupCard* group, std::vector<Card*> cards)
{
	if ((size_t)(group->getAvailableSlot()) < cards.size())
	{
		return;
	}
	group->addCards(cards);

	for (size_t i = 0; i < cards.size(); i++)
	{
		cards[i]->SelectCardActionMB(false, false);
		cards[i]->showHighlight(false);
		cards[i]->enableHint(false);
		auto it = std::find(_list_cards.begin(), _list_cards.end(), cards[i]);
		if (it != _list_cards.end())
		{
			_list_cards.erase(it);
		}

	}

	layoutMyCard();
}

void MauBinhScreen::addCardSelectToGroup(GroupCard* group)
{
	std::vector<Card*> selected = getCardsSelected();
	addCardToGroup(group, selected);
	updateSuggest();
}

void MauBinhScreen::addCardDragToGroup(GroupCard* group)
{
	std::vector<Card*> selected = _group_drag->getCards();
	for (Card* c : selected)
	{
		GroupCard* gr = getCardGroup(c);
		if (gr)
		{
			gr->removeCard(c);
		}
	}
	addCardToGroup(group, selected);
}

void MauBinhScreen::addCardHintToGroup(GroupCard* group)
{
	std::vector<Card*> cardHinted;
	for (size_t i = 0; i < _list_cards.size(); i++)
	{
		if (_list_cards[i]->isHint())
		{
			cardHinted.push_back(_list_cards[i]);
		}
	}
	addCardToGroup(group, cardHinted);
	updateSuggest();
	group->setFocus(false);
}

bool MauBinhScreen::checkGroupTouch(GroupCard* group, Vec2 pos)
{
	cocos2d::Rect touch_zone = group->getTouchZone();
	return touch_zone.containsPoint(pos);
}

bool MauBinhScreen::checkGroupSelect(GroupCard* group, Vec2 pos)
{
	if (group->getTouchZone().containsPoint(pos))
	{
		addCardDragToGroup(group);
		resetTouch(false);
		updateSuggest();
		return true;
	}
	
	return false;
}

MauBinhPlayerUI* MauBinhScreen::getUnusedPlayerUI()
{
	for (MauBinhPlayerUI* ui : _pool_player_ui)
	{
		if (!ui->isUsing)
		{
			ui->isUsing = true;
			return ui;
		}
	}
	return nullptr;
}

void MauBinhScreen::swapCardIn2Group(GroupCard* group1, Card* card1, GroupCard* group2, Card* card2)
{
	if (group1 == group2)
	{
		group1->swap(card1, card2);
	}
	else
	{
		group1->replace(card1, card2);
		group2->replace(card2, card1);
	}
    
    _deal_card_layer->moveCard(card1, NODE(card2)->convertToWorldSpace(Vec2::ZERO), card1->GetSize(), [card1]() {
        card1->SetVisible(true);
    });
    _deal_card_layer->moveCard(card2, NODE(card1)->convertToWorldSpace(Vec2::ZERO), card2->GetSize(), [card2]() {
        card2->SetVisible(true);
    });
    
	updateLung();
	showLung(_img_lung->isVisible());
}
int num_down_touch = 0;
void MauBinhScreen::OnTouchMenuBegin(const cocos2d::Point & p)
{
	HUDScreen::OnTouchMenuBegin(p);
	if (!_handle_game_touch) return;

	CCLOG("touch down");
	num_down_touch++;
	if (num_down_touch > 1) return;
	_stopCheckingMove = false;
	_check_touch = false;
	_no_move = true;
	_touch_card_group = false;
	_pos_touch_down = p;
	if (getMe() && getMe()->ui)
	{
		for (Card * card : getMe()->ui->cards)
		{
			if (card && card->IsVisible() && card->containsPoint(p))
			{
				_check_touch = true;

				if (getCardGroup(card) != nullptr)
				{
					_card_touch_in_group = card;
					_touch_card_group = true;
				}
				else
				{
					card->showHighlight(true);
					_card_touch_in_group = nullptr;
				}
				
				return;
			}
		}
	}
}

void MauBinhScreen::OnTouchMenuHold(const cocos2d::Point & p)
{
	if (!_handle_game_touch) return;
	if (p.getDistanceSq(_pos_touch_down) < 25) return;
	CCLOG("touch move");
	num_down_touch = 0;
	_no_move = false;
	if (!_check_touch) return;
	if (_touch_card_group)
	{
		VecCard cards;
		cards.push_back(_card_touch_in_group);
		_group_drag->setCards(cards);
		_group_drag->setVisible(true);
		_card_touch_in_group->SetOpacity(0);
		//updateGroupDrag();
		updateGroupDragPos(p);
		_touch_card_group = false;
		return;
	}
	else if (!_group_drag->isVisible() && !_stopCheckingMove)
	{
		bool moveOutsideCard = true;
		for (Card * card : getMe()->ui->cards)
		{
			if (card->containsPoint(p))
			{
				card->showHighlight(true);
				moveOutsideCard = false;
			}
		}
		if (moveOutsideCard)
		{
			_stopCheckingMove = true;
			updateGroupDrag();
			updateGroupDragPos(p);
		}
	}
	else if(_group_drag->isVisible())
	{
		updateGroupDragPos(p);
		if (_group_drag->getNumCardDrag() == 1)
		{
			updateCardHover(p);
		}
		
	}
}

void MauBinhScreen::OnTouchMenuEnd(const cocos2d::Point & p)
{
	if (!_handle_game_touch) return;
	CCLOG("touch end");

	num_down_touch = 0;
	bool keep_handling_touch = true;
	if (_no_move)
	{
		_group_drag->reset();
		for (Card * card : getMe()->ui->cards)
		{
			if (card->containsPoint(p))
			{
				if (getCardGroup(card) == nullptr)
				{
					card->showHighlight(false);
					card->SelectCardActionMB(!card->isSelected());
				}
				else
				{
					Card* card_hl = getCardHLInGroup();
					if (card_hl && _card_touch_in_group && card_hl != _card_touch_in_group)
					{
						GroupCard* group1 = getCardGroup(_card_touch_in_group);
						GroupCard* group2 = getCardGroup(card_hl);
                        swapCardIn2Group(group1, _card_touch_in_group, group2, card_hl);
                        card_hl->showHighlight(false);
                        _card_touch_in_group->showHighlight(false);
                        return;
					}
					else
					{
                        if(_card_touch_in_group)
                        {
                            _card_touch_in_group->showHighlight(true);
                        }
					}
					_card_touch_in_group = nullptr;
				}

				keep_handling_touch = false;
				break;
			}
		}
		if (keep_handling_touch)
		{
			if (!checkGroupTouchRelease(_group1, p))
			{
				if (!checkGroupTouchRelease(_group2, p))
				{
					if (!checkGroupTouchRelease(_group3, p))
					{

					}
				}
			}
		}


	}
	else
	{
		if (_group_drag->isVisible())
		{
			if (_group_drag->getNumCardDrag() == 1 && checkCardDrop(p))
			{
				_group_drag->reset();
				return;
			}
			else if (_group_drag->getNumCardDrag() == 1 && _card_touch_in_group)
			{
				_card_touch_in_group->showHighlight(true);
				_card_touch_in_group = nullptr;
				_group_drag->reset();
				return;
			}
		
			if (!checkGroupSelect(_group1, p))
			{
				if (!checkGroupSelect(_group2, p))
				{
					if (!checkGroupSelect(_group3, p))
					{
						for (Card * card : getMe()->ui->cards)
						{
							if (card->isSelected())
							{
								card->SelectCardActionMB(false);
							}
						}
					}
				}
			}

			for (Card * card : getMe()->ui->cards)
			{
				card->showHighlight(false);
			}

			_group_drag->reset();
		}
		else
		{
			if (!_check_touch) return;

			


			for (Card * card : _list_cards)
			{
				if (card->isHighlight())
				{
					if (getCardGroup(card) == nullptr)
					{
						card->SelectCardActionMB(!card->isSelected());
					}

					card->showHighlight(false);
				}
			}
		}

	}

}

bool MauBinhScreen::checkCardDrop(Vec2 pos)
{
	if (!checkGroupTouchDrop(_group1, pos))
		if (!checkGroupTouchDrop(_group2, pos))
			if (!checkGroupTouchDrop(_group3, pos))
			{
				return false;
			}

	return true;
}

bool MauBinhScreen::checkGroupTouchDrop(GroupCard* group, Vec2 pos)
{
	VecCard list_card = group->getCards();
	VecCard drag_card = _group_drag->getCards();
	for(Card* c:list_card)
	{
		if (c->containsPoint(pos))
		{
			GroupCard* group_from = getCardGroup(drag_card.at(0));
			c->SelectCardAction(false, false);
			c->enableHint(false);
			c->showHighlight(false);
			if (group_from) {
				if (group != group_from) {
					group->replace(c, drag_card.at(0));
					group_from->replace(drag_card.at(0), c);
				}
				else
				{
					group->swap(c, drag_card.at(0));
				}
			}
			else
			{
				drag_card.at(0)->SelectCardAction(false, false);
				drag_card.at(0)->enableHint(false);
				drag_card.at(0)->showHighlight(false);
				group->replace(c, drag_card.at(0));
				for (int i = 0; i < _list_cards.size(); i++)
				{
					if (_list_cards[i] == drag_card.at(0))
					{
						_list_cards[i] = c;
						break;
					}
				}
				updateSuggest();
			}
			_group_drag->reset();
			layoutMyCard();
			return true;
		}
	}
	return false;
}


void MauBinhScreen::checkGroupTouchHover(GroupCard* group, Vec2 pos)
{
	VecCard list_card = group->getCards();
	for (Card* c : list_card)
	{
		if (c->containsPoint(pos)) 
		{
			c->showHighlight(true);
		}
		else
		{
			c->showHighlight(false);
		}
	}
}

bool MauBinhScreen::checkGroupTouchRelease(GroupCard* group, Vec2 pos)
{
	if (group->getCanTap() && group->isVisible() && checkGroupTouch(group, pos))
	{
		std::vector<Card*> selected = getCardsSelected();
		if (selected.size() == 0)
		{
			addCardHintToGroup(group);
			group->setFocus(false);
			group->showHighlight(false);
		}
		else
		{
			int num_available = group->getAvailableSlot();
			if (num_available < selected.size())
			{
				for (Card* c : selected)
				{
					c->SelectCardActionMB(false);
				}
			}
			else
			{
				addCardSelectToGroup(group);
			}
		}

		return true;
	}
	return false;
}


bool MauBinhScreen::checkInternalGroup(GroupCard* group, Card* card, Vec2 pos)
{
	auto camera = Camera::getVisitingCamera();
	if (group->getTouchZone().containsPoint(pos))
	{
		if (NODE(card)->getParent()->getUserData() != group)
		{
			static_cast<GroupCard*>(NODE(card)->getParent()->getUserData())->removeCard(card);
			//static_cast<GroupCard*>(NODE(card)->getParent()->getUserData())->doLayout(GROUP_SCALE_DRAG);
			group->addCard(card);
			//group->doLayout(GROUP_SCALE_DRAG);

		}
		return true;
	}

	return false;
}

Card* MauBinhScreen::findHLCardIn3GroupExcept(Card* card)
{
	Card* card_hl = _group1->getCardHL(card);
	if (card_hl) return card_hl;
	card_hl = _group2->getCardHL(card);
	if (card_hl) return card_hl;
	card_hl = _group3->getCardHL(card);
	if (card_hl) return card_hl;
	return nullptr;
}

/**
* Compare two specified cards. Please check number of element before use
* this function.
*
* @param card1
*            must have 5 or 3 elements.
* @param card2
*            must have 5 or 3 elements.
* @param type
*            type of cards.
* @return -1 if cards1 is lager than cards2; 0 if equal, 1 if cards1 is
*         smaller than cards2 and 1989 if error.
*/
int MauBinhScreen::compareWith(std::vector<Card*> cards1, std::vector<Card*> cards2) {
	// Check input condition.
	if (cards1.size() == 0 || cards2.size() == 0) {
		return 1989;
	}
	// Type is smaller.
	s16 type1 = getHandType(cards1, false);
	s16 type2 = getHandType(cards2, false);
	if (type1 > type2) {
		return -1;
	}
	else if (type1 < type2) {
		return 1;
	}
	else {
        CardUtils::sortByOrder(cards1);
        CardUtils::sortByOrder(cards2);
		return compareWithSameType(cards1, cards2, type1);
	}
}

/**
* Compare two specified cards with the same type. Please check number of
* element before use this function.
*
* @param card1
*            must have 5 or 3 elements.
* @param card2
*            must have 5 or 3 elements.
* @param type
*            type of cards.
* @return -1 if cards1 is lager than cards2; 0 if equal, 1 if cards1 is
*         smaller than cards2 and 1989 if error.
*/
int MauBinhScreen::compareWithSameType(std::vector<Card*> card1, std::vector<Card*> card2, int type) {
	switch (type) {
	case TYPE_HIGH_CARDS:
		return compareCardByCard(card1, card2);
	case TYPE_ONE_PAIR:
		return compareWithOnePair(card1, card2);
	case TYPE_TWO_PAIRS:
		return compareWithTwoPair(card1, card2);
	case TYPE_THREE_OF_KIND:
		return compareWithThreeOfKind(card1, card2);
	case TYPE_STRAIGHT:
		return compareWithStraight(card1, card2);
	case TYPE_FLUSH:
		return compareCardByCard(card1, card2);
	case TYPE_FULL_HOUSE:
		return compareWithFullHouse(card1, card2);
	case TYPE_FOUR_OF_KIND:
		return compareWithFullHouse(card1, card2);
	case TYPE_FLUSH_STRAIGHT:
		return compareWithStraight(card1, card2);
	default:
		return 1989;
	}
}

/**
* Compare two (one pair) together. please check your cards before use this
* function.
*
* @param cards1
*            must have 5 or 3 elements.
* @param cards2
*            must have 5 or 3 elements.
* @return -1 if cards1 is lager than cards2; 0 if equal, 1 if cards1 is
*         smaller than cards2 and 1989 if error.
*/
int MauBinhScreen::compareWithOnePair(std::vector<Card*> cards1, std::vector<Card*> cards2) {
	char pair01 = -1;
	// Get card number of pair of self.
	for (size_t i = 1; i < cards1.size(); i++) {
		if (cards1[i - 1]->GetCardValue() == cards1[i]->GetCardValue()) {
			pair01 = cards1[i]->GetCardValue();
			break;
		}
	}
	char pair02 = -1;
	// Get card number of pair of set.
	for (size_t i = 1; i < cards2.size(); i++) {
		if (cards2[i - 1]->GetCardValue() == cards2[i]->GetCardValue()) {
			pair02 = cards2[i]->GetCardValue();
			break;
		}
	}
	if (pair01 == -1 || pair02 == -1) {
		return 1989;
	}
	if (pair01 > pair02) {
		return -1;
	}
	else if (pair01 < pair02) {
		return 1;
	}
	else {
		return compareCardByCard(cards1, cards2);
	}
}

/**
* Compare two (two pairs) together. please check your cards before use this
* function.
*
* @param cards1
*            must have 5 elements.
* @param cards2
*            must have 5 elements.
* @return -1 if cards1 is lager than cards2; 0 if equal, 1 if cards1 is
*         smaller than cards2.
*/
int MauBinhScreen::compareWithTwoPair(std::vector<Card*> cards1, std::vector<Card*> cards2) {
	// Get card number of big pairs.
	// The 4th card is card in the big pair because card lists are always
	// sorted.
	if (cards1[3]->GetCardValue() > cards2[3]->GetCardValue()) {
		return -1;
	}
	else if (cards1[3]->GetCardValue() < cards2[3]->GetCardValue()) {
		return 1;
	}
	// Get card number of small pairs.
	// The 2nd card is card in the small pair because card lists are always
	// sorted.
	if (cards1[1]->GetCardValue() > cards2[1]->GetCardValue()) {
		return -1;
	}
	else if (cards1[1]->GetCardValue() < cards2[1]->GetCardValue()) {
		return 1;
	}
	// If there are 2 equal pairs then compare all cards.
	return compareCardByCard(cards1, cards2);
}

/**
* Compare two three of kind together. please check your cards before use
* this function.
*
* @param cards1
*            must have 5 or 3 elements.
* @param cards2
*            must have 5 or 3 elements.
* @return -1 if cards1 is lager than cards2; 0 if equal, 1 if cards1 is
*         smaller than cards2.
*/
int MauBinhScreen::compareWithThreeOfKind(std::vector<Card*> cards1, std::vector<Card*> cards2) {
	// Get card number of three of a kind.
	// It is the middle card of big list, or the last of small list
	// because card lists are always sorted.
	// Xam win.
	if (cards1[2]->GetCardValue() > cards2[2]->GetCardValue()) {
		return -1;
	}
	else if (cards1[2]->GetCardValue() < cards2[2]->GetCardValue()) { // Xam lose.
		return 1;
	}
	else { // Cannot draw, because there is no 2 equal three of a kind in
		   // game.
		return 0;
	}
}

/**
* Compare two straight together. please check your cards before use this
* function.
*
* @param cards1
*            must have 5 elements.
* @param cards2
*            must have 5 elements.
* @return -1 if cards1 is lager than cards2; 0 if equal, 1 if cards1 is
*         smaller than cards2.
*/
int MauBinhScreen::compareWithStraight(std::vector<Card*> cards1, std::vector<Card*> cards2) {

	for (int i = 4; i >= 0; i--) {
		if (cards1[i]->GetCardValue() > cards2[i]->GetCardValue())
			return -1;
		else if (cards1[i]->GetCardValue() < cards2[i]->GetCardValue())
			return 1;

	}
	return 0;
}

/**
* Compare two full house together. please check your cards before use this
* function.
*
* @param cards1
*            must have 5 elements.
* @param cards2
*            must have 5 elements.
* @return -1 if cards1 is lager than cards2; 0 if equal, 1 if cards1 is
*         smaller than cards2.
*/
int MauBinhScreen::compareWithFullHouse(std::vector<Card*> cards1, std::vector<Card*> cards2) {
	// Get card number of three of a kind.
	// It is the middle card because card lists are always sorted.
	// Xam win.
	if (cards1[2]->GetCardValue() > cards2[2]->GetCardValue()) {
		return -1;
	}
	else if (cards1[2]->GetCardValue() < cards2[2]->GetCardValue()) { // Xam lose.
		return 1;
	}
	else {
		// Cannot draw, because there is no 2 equal three of a kind in game.
		return 0;
	}
}
/**
* Get hand type.
*
* @param cards
*            must have 5 or 3 elements.
* @param isSort
*            true - sort input cards, false - otherwise.
* @return type of hand.
*/
ubyte MauBinhScreen::getHandType(std::vector<Card*> cards, bool isSort) {
	// Check cards.
	if (cards.size() == 0 || (cards.size() != 5 && cards.size() != 3)) {
		return TYPE_NONE;
	}

	int size = cards.size();
	if (isSort) {
		CardUtils::sortByOrder(cards);
	}
	std::vector<unsigned char> cardIDs;
	for (int i = 0; i < size; i++) {
		cardIDs.push_back(cards.at(i)->getID());
	}
	CardUtils::sort(cardIDs);
	// isFlush: (Thung), cards have the same type.
	bool isFlush = true;
	int pairNo = 0;
	int threeNo = 0;
	int sameCardNo = 0;
	for (int i = 1; i < size; i++) {
		// if 2 continuous cards have different types.
		if (isFlush && cardIDs[i] % 4 != cardIDs[i - 1] % 4) {
			isFlush = false;
		}

		// Check pair, three or four of a kind.
		if (cardIDs[i] / 4 == cardIDs[i - 1] / 4) {
			sameCardNo++;
		}
		else {
			switch (sameCardNo) {
			case 0:
				break;
			case 1: // a pair.
				if (size == 3) {
					return TYPE_ONE_PAIR;
				}
				else {
					pairNo++;
				}
				break;
			case 2: // Three of a kind.
				threeNo++;
				break;
			case 3: // Four of a Kind.
					// this.type = SetType.FOUR_OF_KIND;
				return TYPE_FOUR_OF_KIND;
			default:
				break;
			}
			sameCardNo = 0;
		}
	}

	switch (sameCardNo) {
	case 0:
		if (size == 3) {
			return TYPE_HIGH_CARDS;
		}
		break;
	case 1: // a pair.
		if (size == 3) {
			return TYPE_ONE_PAIR;
		}
		else {
			pairNo++;
		}
		break;
	case 2: // Three of a kind.
		if (size == 3) {
			return TYPE_THREE_OF_KIND;
		}
		else {
			threeNo++;
		}
		break;
	case 3: // Four of a Kind.
		return TYPE_FOUR_OF_KIND;
	default:
		break;
	}

	// There is a three.
	if (threeNo > 0) {
		// Cu lu.
		if (pairNo > 0) {
			return TYPE_FULL_HOUSE;
		}
		else {
			// Xam chi.
			return TYPE_THREE_OF_KIND;
		}
	}

	// There is NOT any three.
	// 2 dao.
	if (pairNo == 2) {
		// this.type = SetType.TWO_PAIR;
		return TYPE_TWO_PAIRS;
	}
	else if (pairNo == 1) {
		// 1 dao.
		// this.type = SetType.ONE_PAIR;
		return TYPE_ONE_PAIR;
	}
	// There is NOT any pair.
	int firstCardNumber = cardIDs[0] / 4;
	int lastCardNumber = cardIDs[cardIDs.size() - 1] / 4;
	// Sanh.
	if (lastCardNumber - firstCardNumber == size - 1
		|| (cardIDs[cardIDs.size() - 1] / 4 == 12 && cardIDs[cardIDs.size() - 2] / 4 == 3)) {
		if (isFlush) {
			// Thung pha sanh.
			// this.type = SetType.STRAIGHT_FLUSH;
			return TYPE_FLUSH_STRAIGHT;
		}
		else {
			// Sanh.
			// this.type = SetType.STRAIGHT;
			return TYPE_STRAIGHT;
		}
	}

	// Thung. Plus ret to compare between flush.
	if (isFlush) {
		// this.type = SetType.FLUSH;
		return TYPE_FLUSH;
	}
	// Mau thau.
	return TYPE_HIGH_CARDS;
}

/**
* Check three hands of a card set.
*
* @param firstHand
* @param middleHand
* @param lastHand
* @return true if Lung, false if otherwise.
*/
bool MauBinhScreen::checkLung(std::vector<Card*> firstHand, std::vector<Card*> middleHand, std::vector<Card*> lastHand) {
	int r1 = compareWith(firstHand, middleHand);
	int r2 = compareWith(middleHand, lastHand);
	if (r1 == 1989 || r2 == 1989 || r1 < 0 || r2 < 0) {
		return true;
	}
	else {
		return false;
	}
}

bool MauBinhScreen::isDelayLoadUi()
{
	return !_is_finish_update_ui;
}

void MauBinhScreen::pushDelayMessageUI(iwincore::Message* msg)
{
	_list_mesage_delay.push_back(new iwincore::Message(msg));
}

void MauBinhScreen::resetTouch(bool resetListCard)
{
	if (resetListCard)
	{
		for (Card*c : _list_cards)
		{
			c->SelectCardActionMB(false);
		}
	}
	_stopCheckingMove = false;
	_group_drag->reset();
	_group_drag->setVisible(false);
}

void MauBinhScreen::showGroupButton(bool autoBinh, bool finishBinh, bool sort)
{
	_btnAutoBinh->SetVisible(autoBinh);
	_btnFinishBinh->SetVisible(finishBinh);
	_btnSort->SetVisible(sort);
	layoutButton();
}

void MauBinhScreen::layoutButton()
{
	int offsetX = GetGameSize().width;
	int pad = 10.0f * (GetGameSize().width / this->GetDesignSize().x);
	std::vector<WidgetEntity*> list_btn;
	list_btn.push_back(_btnAutoBinh);
	list_btn.push_back(_btnFinishBinh);
	list_btn.push_back(_btnSort);

	for (WidgetEntity* entity : list_btn)
	{
		if (entity->IsVisible())
		{
			offsetX -= (entity->getWidth() + pad);
			entity->SetPosition(Vec2(offsetX, entity->GetPosition().y));
		}

	}

}

void MauBinhScreen::stopGame()
{
	if (GameController::myInfo->IDDB == _ownerID)
	{
		GameService::getBetRange();
	}
}

void MauBinhScreen::showTransparent(bool visible, int opacity)
{
	_panel_trans_0->SetVisible(visible);
	_panel_trans_0->SetOpacity(opacity);
}

void MauBinhScreen::hideArrow()
{
	_img_arrow->setVisible(false);
}

void MauBinhScreen::showArrow(GroupCard* group)
{
	_img_arrow->stopAllActions();
	Vec2 pos = group->getPosition();
	//cocos2d::Size layout_size = group->getLayout()->getContentSize();
	_img_arrow->setVisible(true);
	_img_arrow->setAnchorPoint(Vec2(.5f, .0f));
	//_img_arrow->setPosition(Vec2(pos.x, pos.y + layout_size.height / 2));
	_img_arrow->setRotation(-90);
	_img_arrow->runAction(RepeatForever::create( Sequence::create(MoveBy::create(.2f, Vec2(-30.0f, 0)), MoveBy::create(.1f, Vec2(30.0f, .0f)), nullptr)));
}

void MauBinhScreen::updateLung()
{
	if (checkLung(_group3->getCards(), _group2->getCards(), _group1->getCards()))
	{
		Vec2 pos1 = _group1->getPosition();
		Vec2 pos3 = _group3->getPosition();
		pos1.x += _group1->getRealSize().width;
		pos3.y += _group3->getRealSize().height / 2;
		Sequence* bounce = Sequence::create(EaseQuadraticActionOut::create(ScaleTo::create(.2f, 1.2f)), ScaleTo::create(.4f, 0.8f), nullptr);
		_img_lung->stopAllActions();
		_img_lung->setScale(1.0f);
		_img_lung->setPosition(Vec2(pos1.x, pos3.y));
		_img_lung->setVisible(true);
		_img_lung->runAction(RepeatForever::create(bounce));
	}
	else
	{
		_img_lung->setVisible(false);
	}
}

void MauBinhScreen::endResult()
{
	_lock_trans = false;
	Result* myResult = nullptr;
	for (auto mp : p_list_player_group)
	{
		if (!_PLAYER(mp))
			continue;

		MauBinhPlayer* p = static_cast<MauBinhPlayer*>(_PLAYER(mp));

		if (p->IDDB == -1)
		{
			continue;
		}
		Result* result = p->ui->layout_result->getResult();
        
        if(result)
        {
            if (p->IDDB == GameController::myInfo->IDDB)
            {
                myResult = result;
                //add money to player and play effect
                GameScreen::OnMyMoneyChange(result->winMoney, getPlayerGroup(p->IDDB), p->getMoney() + result->winMoney);
            }
            if (result->winMoney == 0)
            {
                continue;
            }
            flyMoney(result->winMoney, p->IDDB);
            
            if (result->winMoney > 0) 
            {
                getPlayerGroup(p->IDDB)->showFlare(true);
            }
        }
	}
	showLockScreen();
	if (getTextSapHam() != "") {
		getPlayerGroup(GameController::myInfo->IDDB)->SetTextChat(getTextSapHam());
	} else {
		if (myResult) {
			std::string left_text = "";
			std::string right_text = "";
			if (myResult->winChi > 0) {
				left_text = LangMgr->GetString("won").GetString();
				right_text = StringUtils::format(" %d %s", myResult->winChi, LangMgr->GetString("hands").GetString());
			} else if(myResult->winChi < 0){
				left_text = LangMgr->GetString("lose").GetString();
				right_text = StringUtils::format(" %d %s", myResult->winChi, LangMgr->GetString("hands").GetString());		
			} else {
				left_text = LangMgr->GetString("tie").GetString();
			}

			getPlayerGroup(GameController::myInfo->IDDB)->SetTextChat(left_text + right_text);
		}
	}
		
	setTextSapHam("");
    //hide image lung after end show chi results
    _img_lung->setVisible(false);
}

void MauBinhScreen::showLockScreen()
{
	showTransparent(true, 0);
	SetInteractiveWidget(_panel_trans_0,true);
}

void MauBinhScreen::hideLockScreen()
{
	showTransparent(false);
	SetInteractiveWidget(_panel_trans_0,false);
}

void MauBinhScreen::reset()
{
	for (MauBinhPlayerUI* ui : _pool_player_ui)
	{
		ui->layout_result->reset();
        ui->layout_result->removeFromParent();
	}
	_lock_trans = false;
}

void MauBinhScreen::onTouchEvent(Ref* ref, Widget::TouchEventType eventType)
{
	//Widget* widget = static_cast<Widget*>(ref);
	//std::string name = widget->getName();
	//switch (eventType)
	//{
	//case Widget::TouchEventType::MOVED:
	//	CCLOG("Touch Move %s", name.c_str());
	//	break;
	//case Widget::TouchEventType::ENDED:
	//{
	//	if (name == "group1")
	//	{
	//		if (_group1->isFocus())
	//		{
	//			addCardHintToGroup(_group1);
	//		}
	//		else
	//		{
	//			addCardSelectToGroup(_group1);
	//		}

	//	}
	//	else if (name == "group2")
	//	{
	//		if (_group2->isFocus())
	//		{
	//			addCardHintToGroup(_group2);
	//			hideArrow();
	//		}
	//		else
	//		{
	//			if(!_group1->isFocus())
	//				addCardSelectToGroup(_group2);
	//		}
	//	}
	//	else if (name == "group3")
	//	{
	//		addCardSelectToGroup(_group3);
	//	}
	//	CCLOG("Touch End %s", name.c_str());
	//}
	//break;
	//}

}

void MauBinhScreen::HandleAfterInitPlayer()
{
	GameScreen::HandleAfterInitPlayer();
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

	//initFake();
}


void MauBinhScreen::setBoardInfo(int playingTime, int restTime)
{
//    Gdx.app.debug("Mau Binh", "VAo ban dang choi : " + playingTime + " - res : " + restTime);
//    this.playingTime = playingTime;
//    this.restTime = restTime;
//    if (restTime > 0) {
//        // van dang choi hien vong countdown
//        hideReadyButton();
//        showViewerLabel();
//        btnQuickPlayAll.setVisible(false);
//        currentTime = TimeUtils.millis();
//        onChat(ownerID, L.gL(131) + " " + restTime + " " + L.gL(455));
//        gameLayer.addActor(lbRestTime);
//        // startCountDownStart(restTime * 1000);
//        lbRestTime.setText(L.gL(131) + " " + restTime + " " + L.gL(455));
//    } else {
//        gameLayer.addActor(lbRestTime);
//        lbRestTime.setText(L.gL(1202) + playingTime + " " + L.gL(455));
//    }
    this->playingTime = playingTime;
    this->_restTime = restTime;
    p_text_other1->SetVisible(true);
    if(_restTime > 0)
    {
        static_cast<TextWidget*>(p_text_other1)->SetText(StringUtils::format("%s %d %s",LangMgr->GetString("this_board's_playin").GetString(), restTime, LangMgr->GetString("seconds").GetString()), true);
        HUDScreen::SendChatToScreen(StringUtils::format("%s %d %s",LangMgr->GetString("this_board's_playin").GetString(), (int)_restTime, LangMgr->GetString("seconds").GetString()));
    }
//    else
//    {
//        static_cast<TextWidget*>(p_text_other1)->SetText(StringUtils::format("%s %d %s", LangMgr->GetString("play_tim").GetString(), playingTime, LangMgr->GetString("seconds").GetString()));
//    }
    
}

void MauBinhScreen::OnReconnect(ubyte isStatus, VecByte myCards, ubyte limitTime, ubyte restTime, char maubinhType) {
	// Khoi tao lai bo bai reuse de choi.
	//GameController.vibrateMedium();
	//GameController.playSound(iWinSound.BUZZ);
	onReconnect();
	//initialBeforePlayGame();
	createCard();
	_isStartGame = ((char)isStatus != -1);
	if ((char)isStatus != -1) {

		hideReadyButton();
		hideStartButton();
		layoutStartGame(false);
		layoutDealCard();
		doneDealCard();
		
		if (_isStartGame && _isViewer && _myInfo->GetIDDB() != _ownerID) {
			showViewerLabel();
		}
		this->playingTime = limitTime;
		_totalTimes = limitTime;
		this->_restTime = restTime;
		myCardType = maubinhType;
		_handle_game_touch = true;
		//int count = 0;
		//for (int i = 0; i < p_list_player_group.size(); i++) {
		//	MauBinhPlayer* p = (MauBinhPlayer*)p_list_player_group.at(i)->getPlayer();
		//	// if(p!=null)
		//	// p.isReady=true;
		//	if (p->IDDB == GameController::myInfo->IDDB) {
		//		p->ui->cards[i]->ResetID(cards[j])
		//		if (p.cards == null) {
		//			p.cards = new Array<Card>();
		//		}
		//		int max = count + 13;
		//		for (int j = count; j < max; j++) {
		//			p.cards.add(cards[j]);
		//		}
		//		count += 13;
		//	}
		//	else if (p.isReady || p.IDDB == ownerID) {
		//		if (p.cards == null) {
		//			p.cards = new Array<Card>();
		//		}
		//		int max = count + 13;
		//		for (int j = count; j < max; j++) {
		//			p.cards.add(cards[j]);
		//		}
		//		count += 13;
		//		layoutVirtualCards(p, getSeatFromPlayerID(p.IDDB));
		//	}
		//}
		// An het nhung la bai con du.
		/*for (int j = count; j < cards.length; j++) {
			cards[j].setVisible(false);
		}*/

		MauBinhPlayer* me = getMe();
		int idx = 0;
		for (auto c : myCards)
		{
			me->ui->cards[idx]->ResetID(c, DECK_TYPE::DECK_MAUBINH);
			me->ui->cards[idx]->SetVisible(true);
			idx++;
		}
		_list_cards = me->ui->cards;
		//startTimeBar(limitTime * 1000, restTime * 1000);
		_binh_timebar->onStartime(limitTime, restTime);
		if (isStatus == 0) {
			if (maubinhType >= 0) {
				// Bai minh la mau binh.
				doAutoBinh();
				doLayoutCardComplete();
			}
			else {
				layoutMyCard();
			}
		}
		else {
			// Minh da binh xong ui.
			doAutoBinh();
			doLayoutCardComplete();
		}
	}
	else {
		setBoardInfo(limitTime, restTime);
	}
}

void MauBinhScreen::showTextWaiting(bool visible)
{
	//_text_waiting->SetVisible(visible);
}

GroupCard* MauBinhScreen::getCardGroup(Card* c)
{
	if (_group1->contain(c)) {
		return _group1;
	}
	else if (_group2->contain(c))
	{
		return _group2;
	}
	else if (_group3->contain(c))
	{
		return _group3;
	}
    
	return nullptr;
}

void MauBinhScreen::updateCardHover(Vec2 posTouch)
{
	checkGroupTouchHover(_group1, posTouch);
	checkGroupTouchHover(_group2, posTouch);
	checkGroupTouchHover(_group3, posTouch);
}

void MauBinhScreen::showLung(bool visible)
{
	_spr_lung_up->setVisible(visible);
	_spr_lung_down->setVisible(visible);
}

void MauBinhScreen::addToMyDealLayer(Node* node)
{
	_my_deal_layer->GetResource()->addChild(node);
}


Card* MauBinhScreen::getCardHLInGroup()
{
	Card* c = _group1->getCardHL();
	if (c) return c;
	else
	{
		c = _group2->getCardHL();
		if (c) return c;
		else
		{
			c = _group3->getCardHL();
			if (c) return c;
			return nullptr;
		}
	}
}

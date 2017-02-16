#include "Card.h"
#include "AnimUtils.h"

#include "Common/GameController.h"
#include "Screens/GameScreens/GameScreens.h"
//12 = 2 , 0 = 3
int MAP_MAUBINH[13] = { 12, 0, 1, 2, 3, 4, 5,
6, 7, 8, 9, 10, 11 };
 int MAP_TIENLEN[13] = { 0, 1, 2, 3, 4, 5, 6, 7,
8, 9, 10, 11, 12 };
 int MAP_BAICAO[13] = { 11, 12, 0, 1, 2, 3, 4,
5, 6, 7, 8, 9, 10 };

int MAP_RENDER[13] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0, 1 };


//int*	Card::_mapping_value= nullptr;
Card::Card()
{
	p_name_card = NAME_CARD::CARD_A;
	p_element = ELEMENT_CARD::E_SPADE;
	p_UP_status = true;
	p_ID = -1;
	_isSelected = false;
	p_IsActive = true;

	_mapping_value = nullptr;
}

Card::Card(RKString name_object, NAME_CARD name, ELEMENT_CARD e)
{
	p_name_card = name;
	p_element = e;
	p_UP_status = true;
	_isSelected = false;
	Init(name_object);
	p_IsActive = true;


}

Card::Card(RKString name_object, int id, DECK_TYPE deck_type)
{
	SetID(id, deck_type);

	Init(name_object);

	this->GetResource()->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	this->GetResource()->setPosition(Vec2::ZERO);
	//this->addChild(this->GetResource());
	p_IsActive = true;
}

Card::~Card()
{
	p_IsActive = true;
	p_IsOnControl = false;

	_mapping_value = nullptr;
}

void Card::Init(RKString name_obj)
{
	if (p_IsInit)
	{
		CCLOG("object already init !");
		return;
	}
	bool res = OnInitWideObject(name_obj);

	if (p_ID < -1)
	{
		//SetStatus(false);
	}
	else
	{
		InitElementChild();

		CCASSERT(res, "init object failed !");
		SetStatus(true);
	}
	p_IsInit = true;
	_isSelected = false;
	_is_lock_select = false;
}

void Card::GetBigSymbolCardByType(wBasicObject * icon, NAME_CARD name_card, ELEMENT_CARD element)
{
	if (!icon)
		return;
	float ratio = GetGameSize().width / this->GetDesignSize().x;
	if (ratio > 1.f)
	{
		ratio = 1.f;
	}
	RKString path_name = "gamescreen-Cards/b.png";
	int case_op = 0;
	if (name_card == NAME_CARD::CARD_A && element == ELEMENT_CARD::E_SPADE)
	{
		path_name = "gamescreen-Cards/xi_bich.png";
		case_op = 1;
	}
	else if (name_card >= NAME_CARD::CARD_JACK)
	{
		if (name_card == NAME_CARD::CARD_JACK)
		{
			path_name = "gamescreen-Cards/j";
		}
		else if (name_card == NAME_CARD::CARD_QUEEN)
		{
			path_name = "gamescreen-Cards/q";
		}
		else if (name_card == NAME_CARD::CARD_KING)
		{
			path_name = "gamescreen-Cards/k";
		}

		if (element < ELEMENT_CARD::E_DIAMOND)
		{
			path_name += "b.png";
		}
		else if (element >= ELEMENT_CARD::E_DIAMOND)
		{
			path_name += "c.png";
		}
		case_op = 2;
	}
	else
	{
		if (element == ELEMENT_CARD::E_SPADE)
		{
			path_name = "gamescreen-Cards/b.png";
		}
		else if (element == ELEMENT_CARD::E_CLUB)
		{
			path_name = "gamescreen-Cards/ch.png";
		}
		else if (element == ELEMENT_CARD::E_DIAMOND)
		{
			path_name = "gamescreen-Cards/r.png";
		}
		else if (element == ELEMENT_CARD::E_HEART)
		{
			path_name = "gamescreen-Cards/c.png";
		}
	}

	auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(path_name.GetString());
	
	if (sprite_frame)
	{
		static_cast<Sprite*>(icon->GetResource())->setSpriteFrame(sprite_frame);
	}

	if (case_op == 0)
	{
		icon->SetSize(Vec2(95, 93)*ratio);
		icon->SetPosition(Vec2(95, 53)*ratio);
	}
	else if (case_op == 1)
	{
		icon->SetSize(Vec2(101, 143)*ratio);
		icon->SetPosition(Vec2(75, 75)*ratio);
	}
	else if (case_op == 2)
	{
		icon->SetSize(Vec2(104, 140)*ratio);
		icon->SetPosition(Vec2(92, 75)*ratio);
	}


}

void Card::GeSmallSymbolCardByType(wBasicObject * icon, ELEMENT_CARD element)
{
	if (!icon)
		return;

	RKString path_name = "gamescreen-Cards/mb.png";
	switch (element)
	{
	case E_SPADE:
		path_name = "gamescreen-Cards/mb.png";
		break;
	case E_CLUB:
		path_name = "gamescreen-Cards/mch.png";
		break;
	case E_DIAMOND:
		path_name = "gamescreen-Cards/mr.png";
		break;
	case E_HEART:
		path_name = "gamescreen-Cards/mc.png";
		break;
	default:
		break;
	}

	auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(path_name.GetString());

	if (sprite_frame)
	{
		static_cast<Sprite*>(icon->GetResource())->setSpriteFrame(sprite_frame);
	}
}

void Card::GetNumberSymbolCardByType(wBasicObject * icon, NAME_CARD name_card, ELEMENT_CARD element)
{
	if (!icon)
		return;

	float ratio = GetGameSize().width / this->GetDesignSize().x;
	if (ratio > 1.f)
	{
		ratio = 1.f;
	}
	Vec2 number_sprite = Vec2(1, 13);
	Vec2 idx_sprite = Vec2(0, 0);
	RKString path_name = "gamescreen-Cards/sobaiden.png";
	switch (element)
	{
	case E_SPADE:
	case E_CLUB:
		path_name = "gamescreen-Cards/sobaiden.png";
		break;
	case E_DIAMOND:
	case E_HEART:
		path_name = "gamescreen-Cards/sobaido.png";
		break;
	default:
		break;
	}

	switch (name_card)
	{
	case CARD_A:
		idx_sprite = Vec2(0, 11);
		break;
	case CARD_2:
		idx_sprite = Vec2(0, 12);
		break;
	case CARD_3:
		idx_sprite = Vec2(0, 0);
		break;
	case CARD_4:
		idx_sprite = Vec2(0, 1);
		break;
	case CARD_5:
		idx_sprite = Vec2(0, 2);
		break;
	case CARD_6:
		idx_sprite = Vec2(0, 3);
		break;
	case CARD_7:
		idx_sprite = Vec2(0, 4);
		break;
	case CARD_8:
		idx_sprite = Vec2(0, 5);
		break;
	case CARD_9:
		idx_sprite = Vec2(0, 6);
		break;
	case CARD_10:
		idx_sprite = Vec2(0, 7);
		break;
	case CARD_JACK:
		idx_sprite = Vec2(0, 8);
		break;
	case CARD_QUEEN:
		idx_sprite = Vec2(0, 9);
		break;
	case CARD_KING:
		idx_sprite = Vec2(0, 10);
		break;
	default:
		break;
	}

	auto sprite_frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(path_name.GetString());

	if (sprite_frame)
	{
		auto p = static_cast<Sprite*>(icon->GetResource());

		Vec2 size_spite_frame = sprite_frame->getOriginalSize();
		Vec2 size_each_sprite = Vec2(size_spite_frame.x / number_sprite.x, size_spite_frame.y / number_sprite.y);

		Vec2 pos_sprite_with_texture = Vec2(0, 0);
		if (!sprite_frame->isRotated())
		{
			pos_sprite_with_texture = Vec2(
				sprite_frame->getRect().origin.x + idx_sprite.x*size_each_sprite.x,
				sprite_frame->getRect().origin.y + idx_sprite.y*size_each_sprite.y
			);
		}
		else
		{
			pos_sprite_with_texture = Vec2(
				sprite_frame->getRect().origin.x + ((number_sprite.y - idx_sprite.y - 1) * size_each_sprite.y),
				sprite_frame->getRect().origin.y + ((number_sprite.x - idx_sprite.x - 1) * size_each_sprite.y)
			);
		}

		p->initWithTexture(
			sprite_frame->getTexture(),
			cocos2d::Rect(pos_sprite_with_texture.x, pos_sprite_with_texture.y, size_each_sprite.x, size_each_sprite.y)
			,
			sprite_frame->isRotated()
		);

	//	icon->SetSize(Vec2(50,40)*ratio);
	//	icon->SetPosition(Vec2(31,175)*ratio);
	}
}

void Card::InitElementChild()
{
	auto card_blank = this->GetObjectByName(".card_blank");

	GetBigSymbolCardByType(card_blank->GetObjectByName(".icon_element"), p_name_card, p_element);
	GeSmallSymbolCardByType(card_blank->GetObjectByName(".icon_element_small"),p_element);
	GetNumberSymbolCardByType(card_blank->GetObjectByName(".name_number"), p_name_card, p_element);
}

void Card::Draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (!p_IsActive)
		return;

	wBasicObject::Draw(renderer, transform, flags);
}

void Card::Visit(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	if (!p_IsActive)
		return;

	wBasicObject::Visit(renderer, transform, flags);
}

void Card::Update(float dt)
{
	if (!p_IsActive)
		return;

	ObjectEntity::Update(dt);
}
bool Card::isSelected()
{
	return _isSelected;
}

void Card::ResetID(int id, DECK_TYPE deck_type)
{
	if (p_ID == id)
		return;
	if (id < 0)
	{
		this->GetObjectByName(".card_blank")->SetVisible(false);
		this->GetObjectByName(".card_up")->SetVisible(true);
		p_ID = id;
		return;
	}
	else
	{
		this->GetObjectByName(".card_blank")->SetVisible(true);
		this->GetObjectByName(".card_up")->SetVisible(false);
	}

	SetID(id, deck_type);

	InitElementChild();
	_isSelected = false;
}

void Card::ConvertToType(int id, DECK_TYPE dectype, NAME_CARD & name_card, ELEMENT_CARD & element_card, unsigned char & value_card)
{
	int number_card = id / 4; //3 bich = number_car 0 , 2 co = number_card 
	int card_type = id % 4; // bich - chuon - ro - co

	element_card = (ELEMENT_CARD)(card_type);
	if (dectype == DECK_TYPE::DECK_TIENLEN)
	{
		name_card = (NAME_CARD)MAP_RENDER[MAP_TIENLEN[number_card]];
	}
	else if (dectype == DECK_TYPE::DECK_BAICAO)
	{
		name_card = (NAME_CARD)MAP_RENDER[MAP_BAICAO[number_card]];
	}
	else if (dectype == DECK_TYPE::DECK_MAUBINH)
	{
		name_card = (NAME_CARD)MAP_RENDER[MAP_MAUBINH[number_card]];
	}

	value_card = number_card;//_mapping_value[number_card];
}

void Card::SetID(int id, DECK_TYPE deck_type)
{
	p_deck_type = deck_type;
	p_ID = id;
	if (id < -1)
	{
		return;
	}

	ConvertToType(p_ID, (DECK_TYPE)p_deck_type, p_name_card, p_element, p_value);

	//int number_card = id / 4; //3 bich = number_car 0 , 2 co = number_card 
	//int card_type = id % 4; // bich - chuon - ro - co

	//p_element = (ELEMENT_CARD)(card_type);
	//if (deck_type == DECK_TYPE::DECK_TIENLEN)
	//{
	//	_mapping_value = MAP_TIENLEN;
	//}
	//else if (deck_type == DECK_TYPE::DECK_BAICAO)
	//{
	//	_mapping_value = MAP_BAICAO;
	//}
	//else if (deck_type == DECK_TYPE::DECK_MAUBINH)
	//{
	//	_mapping_value = MAP_MAUBINH;
	//}

	//p_value = number_card;//_mapping_value[number_card];
	//p_name_card = (NAME_CARD)MAP_RENDER[_mapping_value[number_card]];
}

Card * Card::NewInstance(int id, DECK_TYPE deck_type)
{
	int tmpId = id;
	if (id == -1)
	{
		id = 0;
	}
	auto ca =  new Card(id == -2 ? "card_up" : "card", id, deck_type);
	if (tmpId == -1) {
		ca->ResetID(tmpId, deck_type);
	}
	return ca;
}

void Card::SetStatus(bool stat)
{
	p_UP_status = stat;

	if (p_UP_status)
	{
		this->GetObjectByName(".card_blank")->SetVisible(true);
		this->GetObjectByName(".card_up")->SetVisible(false);
	}
	else
	{
		this->GetObjectByName(".card_blank")->SetVisible(false);
		this->GetObjectByName(".card_up")->SetVisible(true);
	}
}


char Card::getID()
{
	return p_ID;
}

int Card::GetCardType()
{
	return p_element;
}

int Card::GetCardValue()
{
	return p_value;
}

int Card::GetDeckType()
{
	return p_deck_type;
}

int Card::GetCardName()
{
	return p_name_card;
}

void Card::SelectCardActionMB(bool isSelect, bool anim)
{
	if (isSelect == _isSelected)
	{
		return;
	}
	_isSelected = isSelect;

	if (!anim) return;

	if (_is_lock_select)
	{
		PASSERT2(false, "this action is lock, why call this, check again !!!");
		return;
	}

	//_is_lock_select = true;
	if (isSelect)
	{
		SetActionCommandWithName("SELECT_CARD_MB" , CallFunc::create([this]() {
			this->EventActionFinish();
		}));
	}
	else
	{
		SetActionCommandWithName("DESELECT_CARD_MB" , CallFunc::create([this]() {
			this->EventActionFinish();
		}));
	}
}

void Card::SelectCardAction(bool isSelect, bool anim, bool have_effect)
{
	if (isSelect == _isSelected)
	{
		return;
	}
	_isSelected = isSelect;
	if (!anim)
	{
		float selectY = (GetGameSize().height / 540) * 20;
		NODE(this)->setPosition(NODE(this)->getPosition().x, NODE(this)->getPosition().y + (_isSelected ? 1 : -1)*selectY);
		return;
	}

	if (_is_lock_select)
	{
		PASSERT2(false, "this action is lock, why call this, check again !!!");
		return;
	}

	//_is_lock_select = true;
	if (isSelect)
	{
		AnimUtils::EffectSelectCard(this, CallFunc::create([this , have_effect]() {
			this->EventActionFinish();
			if (have_effect)
			{
				auto node = this->GetObjectByName(".effect_danh_1");
				node->SetVisible(true);
				node->RunActionAnimate(CallFunc::create([this, node]() {
					node->EventActionFinish();
					node->SetVisible(false);
				}));
			}
		}));		
	}
	else
	{
		AnimUtils::EffectDeselectCard(this, nullptr);
	}
}

void Card::ChiaBaiAction(int idx)
{
	float time_delay = 0.1f + 0.05*idx;
	Vec2 position_move_to = Vec2(150 + 50 * idx, 130);
	xml::BasicDec dec;
	dec.InsertDatafloat("DELAY", time_delay);
	dec.InsertDataVector2("POSITION", position_move_to);
	SetHookCommand("CHIA_BAI", "act0", 0, 1, dec, "DELAY");
	SetHookCommand("CHIA_BAI", "act0", 0, 2, dec, "POSITION");

	SetActionCommandWithName("CHIA_BAI", CallFunc::create([this]() {
		this->EventActionFinish();
		GetSound->PlaySoundEffect("SFX_GAMEBAI_CHIABAI",0.f,0,true);
	}));

	ClearHookCommand("CHIA_BAI", "act0", 0, 1);
	ClearHookCommand("CHIA_BAI", "act0", 0, 2);

}

void Card::enableHint(bool enable)
{
	GetObjectByName(".card_blank")->SetColor(enable ? Color4B(251, 255, 171, 255) : Color4B::WHITE);
}

bool Card::isHint()
{
	return GetObjectByName(".card_blank")->GetColor() != Color4B::WHITE;
}

void Card::showHighlight(bool isShow)
{
	this->GetObjectByName(".card_hightlight")->SetVisible(isShow);
}

bool Card::isHighlight()
{
	return this->GetObjectByName(".card_hightlight")->IsVisible();
}

void Card::showTrans(int opacity)
{
	GetObjectByName(".trans")->SetVisible(true);
	GetObjectByName(".trans")->SetColor(Color4B::BLACK);
	GetObjectByName(".trans")->SetOpacity(opacity);
}

void Card::hideTrans()
{
	GetObjectByName(".trans")->SetVisible(false);
}

bool Card::containsPoint(Vec2 pos)
{
	if (!GetResource()->getParent()) return false;
	cocos2d::Size card_size = GetResource()->getContentSize();
	cocos2d::Rect bound = GetResource()->getBoundingBox();//cocos2d::Rect(.0f, .0f, card_size.width, card_size.height);
	Vec2 new_pos = GetResource()->getParent()->convertToNodeSpace(pos);
	return bound.containsPoint(new_pos);
}

void Card::OnTouchBeginObject()
{
	auto current_game_screen = GameController::currentGameScreen;
	if (current_game_screen)
	{
		current_game_screen->ActionTouchBeginObject(this);
	}
}

void Card::OnTouchMoveObject()
{
	auto current_game_screen = GameController::currentGameScreen;
	if (current_game_screen)
	{
		current_game_screen->ActionTouchMoveObject(this);
	}
}

void Card::OnTouchEndObject()
{
	auto current_game_screen = GameController::currentGameScreen;
	if (current_game_screen)
	{
		current_game_screen->ActionTouchEndObject(this);
	}
}

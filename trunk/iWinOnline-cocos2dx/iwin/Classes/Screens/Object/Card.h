#ifndef __CARD_OBJECT_H__
#define __CARD_OBJECT_H__


#include "ObjectEntity.h"
#include "cocos2d.h"
using namespace cocos2d;
USING_NS_CC;
class Card;
class GameScreen;
typedef std::function<void(Card*, Touch*)> CardTouchHandler;
class Card : public ObjectEntity
{
private:

	int			*	_mapping_value;
	NAME_CARD		p_name_card;
	ELEMENT_CARD	p_element;
	unsigned char	p_value;

	int				p_deck_type;
	char			p_ID;
	bool			p_UP_status; //true is UP , false is ngua
	bool			_isSelected;
	bool			_is_lock_select;

protected:
	
	void InitElementChild();

public:
	static Card * NewInstance(int id, DECK_TYPE deck_type);

	Card();
	Card(RKString name_object, NAME_CARD name, ELEMENT_CARD e);
	Card(RKString name_object, int id, DECK_TYPE deck_type);
	virtual ~Card();

	virtual void Init(RKString name_obj) override;
	virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) override;
	virtual void Visit(Renderer *renderer, const Mat4& transform, uint32_t flags) override;
	virtual void Update(float dt) override;
	void SetID(int id, DECK_TYPE deck_type);
	bool containsPoint(Vec2 pos);

	bool isSelected();

	bool GetStatus() { return p_UP_status; }
	void SetStatus(bool stat);

	void ResetID(int id, DECK_TYPE deck_type);
	char getID();
	int GetCardType();
	int GetCardValue();
	int GetCardName();
	int GetDeckType();

	void ChiaBaiAction(int idx);
	void SelectCardAction(bool isDeselect = false, bool anim = true, bool have_animate_eff = true);
	void SelectCardActionMB(bool isSelect, bool anim = true);

	void enableHint(bool enable);
	bool isHint();

	void showHighlight(bool isShow);
	bool isHighlight();
	void showTrans(int opacity = 100);
	void hideTrans();

	virtual	void		OnTouchBeginObject() override;
	virtual void		OnTouchMoveObject()override;
	virtual void		OnTouchEndObject()override;


	static void			ConvertToType(int id, DECK_TYPE dectype, NAME_CARD & name_card, ELEMENT_CARD & element_card , unsigned char & value_card);

	void GetBigSymbolCardByType(wBasicObject * icon, NAME_CARD name_card, ELEMENT_CARD element);
	void GeSmallSymbolCardByType(wBasicObject * icon, ELEMENT_CARD element);
	void GetNumberSymbolCardByType(wBasicObject * icon, NAME_CARD name_card, ELEMENT_CARD element);
};

#endif //__CARD_OBJECT_H__


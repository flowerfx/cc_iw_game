#ifndef __IWIN_ITEM_LIST_CIRCLE_H__
#define __IWIN_ITEM_LIST_CIRCLE_H__

#include "UI/AdvanceUI/WidgetEntity.h"
#include <functional>
/*
* chien todo:
* this list is the static list, 
* so it cannot change the number of items in list 
* (you can change the element each item in list, move item and support circle)
*/

enum class EventType_ListCircleMove
{
	ON_MOVE,
	ON_STOP
};

class IwinListItemCircle
{

private:
	Utility::UI_Widget::WidgetEntity*						_parent_item;
	Utility::UI_Widget::WidgetEntity*						_refer_item;
	std::vector<Utility::UI_Widget::WidgetEntity*>			_list_items;
	std::function<void(Utility::UI_Widget::WidgetEntity* , int )>	_func_update_item;
	//number item in view
	int _number_item_per_view;

	//
	int * _list_idx_item_in_view;
	int * _list_idx_item_in_center_view;
	//when have item have pos reach out of the max and min, so circle its 
	int max_cord_x;
	int min_cord_x;

	//action move handle in update
	//action move = 0 none , 1 is forward, 2 is backward, -1 is on move, -2 is on check the visible view
	int have_action_move;
protected:
	void scrollListItemInView(bool forward);
public:
	IwinListItemCircle();
	virtual ~IwinListItemCircle();

	void init(
		Utility::UI_Widget::WidgetEntity* item, 
		Utility::UI_Widget::WidgetEntity* parent_item,
		const std::function<void(Utility::UI_Widget::WidgetEntity* , int )> & func_update_item);
	void update(float dt);
	void setNumberItem(int number_item);
	void callUpdateItemAtIdx(int idx);

	void MoveToNextItem();
	void MoveToPreviousItem();

	typedef std::function<void(Ref*, EventType_ListCircleMove)> wListCircleCallBack;
	void addEventListener(const wListCircleCallBack& callback);

	//return -1 if not found
	int GetIndexOfVisibleView(int idx);
private:
	wListCircleCallBack  _eventCallback;
};

#endif // __IWIN_LIST_VIEW_H__
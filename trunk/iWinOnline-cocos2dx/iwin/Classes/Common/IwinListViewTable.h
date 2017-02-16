#ifndef __IWIN_LIST_VIEW_TABLE_H__
#define __IWIN_LIST_VIEW_TABLE_H__

#include "UI/AdvanceUI/WidgetEntity.h"
#include "UI/AdvanceUI/wListViewTable.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class IwinListViewTable : public Utility::UI_Widget::ListViewTableWidget
{

private:

	Utility::UI_Widget::WidgetEntity*	_item_drag_top;
	Utility::UI_Widget::WidgetEntity*	_item_drag_bottom;


	std::function<void(Ref* , EventType_SetCell , WidgetEntity* )> _func_when_call_top;
	std::function<void(Ref* , EventType_SetCell , WidgetEntity* )> _func_when_call_bottom;
	
	std::function<void(Ref* , EventType_SetCell , WidgetEntity*)> _func_when_touch_begin_cell;
	std::function<void(Ref* , EventType_SetCell , WidgetEntity*)> _func_when_touch_end_cell;
	std::function<void(Ref* , EventType_SetCell , WidgetEntity*)> _func_when_touch_hold_cell;

	std::function<void(Ref* , EventType_SetCell , WidgetEntity* )> _func_when_set_cell;

	std::function<void(Ref*, EventType_SetCell, WidgetEntity*)> _func_when_cell_begin_move;
	std::function<void(Ref*, EventType_SetCell, WidgetEntity*)> _func_when_cell_end_move;

	bool					  _need_to_reload;
	float					  _time_reload;

	int						  _obj_on_drag;


	std::function<void(void)> _func_when_reload;

	Vec2					  _distance_drag;

public:
	IwinListViewTable();
	virtual ~IwinListViewTable();

	void InitWithParam(
		Utility::UI_Widget::WidgetEntity*	item_drag_top,
		Utility::UI_Widget::WidgetEntity*	item_drag_bottom,
		const std::function<void(Ref* , EventType_SetCell , WidgetEntity* )>&	call_back_set,
		const std::function<void(Ref* , EventType_SetCell , WidgetEntity* )>&	call_back_touch,
		const std::function<void(Ref* , EventType_SetCell , WidgetEntity* )>&	call_back_top,
		const std::function<void(Ref* , EventType_SetCell , WidgetEntity* )>&	call_back_bottom
	);

	void InitOptionalParam(
		const std::function<void(Ref*, EventType_SetCell, WidgetEntity*)>&	call_back_touch_begin_cell,
		const std::function<void(Ref*, EventType_SetCell, WidgetEntity*)>&	call_back_touch_hold_cell = nullptr,
		const std::function<void(Ref*, EventType_SetCell, WidgetEntity*)>&	call_back_begin_move_cell = nullptr,
		const std::function<void(Ref*, EventType_SetCell, WidgetEntity*)>&	call_back_end_move_cell = nullptr
	);


	void UpdateReload(float dt );

	void SetHaveReload(const std::function<void(void)> & func_call_back);

};

#endif // __IWIN_LIST_VIEW_H__
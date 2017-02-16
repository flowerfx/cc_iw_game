#include "IwinListViewTable.h"
#include "SoundManager.h"
using namespace Utility;
using namespace Utility::UI_Widget;
#define TIME_DELTA_PARSE 0.1f

IwinListViewTable::IwinListViewTable()
{
	_item_drag_top = nullptr;
	_item_drag_bottom = nullptr;

	_func_when_call_top = nullptr;
	_func_when_call_bottom = nullptr;

	_func_when_touch_end_cell = nullptr;
	_func_when_touch_begin_cell = nullptr;
	_func_when_touch_hold_cell = nullptr;

	_func_when_cell_begin_move = nullptr;
	_func_when_cell_end_move = nullptr;

	_func_when_set_cell = nullptr;
	_func_when_reload = nullptr;
	_need_to_reload = false;
	_time_reload = TIME_DELTA_PARSE;

	_obj_on_drag = 0;
}

IwinListViewTable::~IwinListViewTable()
{
	_item_drag_top = nullptr;
	_item_drag_bottom = nullptr;
	_func_when_call_bottom = nullptr;
	_func_when_call_top = nullptr;

	_func_when_touch_end_cell = nullptr;
	_func_when_touch_begin_cell = nullptr;
	_func_when_touch_hold_cell = nullptr;

	_func_when_set_cell = nullptr;
	_func_when_cell_begin_move = nullptr;
	_func_when_cell_end_move = nullptr;

	_need_to_reload = false;
	_time_reload = TIME_DELTA_PARSE;

	_obj_on_drag = 0;
}


void IwinListViewTable::InitWithParam(
	Utility::UI_Widget::WidgetEntity*	item_drag_top,
	Utility::UI_Widget::WidgetEntity*	item_drag_bottom,
	const std::function<void(Ref* , EventType_SetCell , WidgetEntity* )>&	call_back_set,
	const std::function<void(Ref* , EventType_SetCell , WidgetEntity* )>&	call_back_touch,
	const std::function<void(Ref* , EventType_SetCell , WidgetEntity* )>&	call_back_top,
	const std::function<void(Ref* , EventType_SetCell , WidgetEntity* )>&	call_back_bottom
)
{
	_item_drag_top = item_drag_top;
	_item_drag_bottom = item_drag_bottom;

	_func_when_set_cell = call_back_set;
	_func_when_touch_end_cell = call_back_touch;
	_func_when_call_top = call_back_top;
	_func_when_call_bottom = call_back_bottom;

	this->addEventListener(
		[this ](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		auto current_drag = this->current_stage_drag();

		if (type == EventType_SetCell::ON_SET_CELL)
		{
			if (_func_when_set_cell)
			{
				this->_func_when_set_cell(ref, type, _widget);
			}
		}
		else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
		{
			if (_func_when_touch_end_cell)
			{
				this->_func_when_touch_end_cell(ref, type, _widget);
			}
		}
		else if (type == EventType_SetCell::ON_BEGIN_TOUCH_CELL)
		{
			if (_func_when_touch_begin_cell)
			{
				this->_func_when_touch_begin_cell(ref, type, _widget);
			}
		}
		else if (type == EventType_SetCell::ON_MOVE_TOUCH_CELL)
		{
			if (_func_when_touch_hold_cell)
			{
				this->_func_when_touch_hold_cell(ref, type, _widget);
			}
		}

		else if (type == EventType_SetCell::ON_BEGIN_DRAG_CELL)
		{
			_distance_drag = Vec2(0, 0);
			if (current_drag == EventType_DragCell::DRAG_DOWN)
			{
				if (!_item_drag_top)
				{
					return;
				}
				_obj_on_drag = 0;
				this->_item_drag_top->Visible(true);
				this->_item_drag_top->SetActionCommandWithName("ORIGIN_POS");
				this->_item_drag_top->GetWidgetChildAtIdx(0)->Visible(true);
				this->_item_drag_top->GetWidgetChildAtIdx(1)->Visible(false);
				this->_item_drag_top->GetWidgetChildAtIdx(2)->SetRotate(0.f);
			}
			else if (current_drag == EventType_DragCell::DRAG_UP)
			{
				if (!_item_drag_bottom)
				{
					return;
				}
				_obj_on_drag = 1;
				this->_item_drag_bottom->Visible(true);
				this->_item_drag_bottom->SetActionCommandWithName("ORIGIN_POS");
				this->_item_drag_bottom->GetWidgetChildAtIdx(0)->Visible(true);
				this->_item_drag_bottom->GetWidgetChildAtIdx(1)->Visible(false);
				this->_item_drag_bottom->GetWidgetChildAtIdx(2)->SetRotate(0.f);
			}

			if (_func_when_cell_begin_move)
			{
				_func_when_cell_begin_move(ref, type, _widget);
			}
		}
		else if (type == EventType_SetCell::ON_DRAG_CELL)
		{
			WidgetEntity * p_object = nullptr;
			if (_obj_on_drag == 0)
			{
				p_object = this->_item_drag_top;
			}
			else if (_obj_on_drag == 1)
			{
				p_object = this->_item_drag_bottom;
			}
			if (!p_object)
			{
				return;
			}
			Vec2 curpos = p_object->GetPosition();
			curpos -= this->GetDeltaPos();
			_distance_drag = Vec2(_distance_drag.x + this->GetDeltaPos().x, _distance_drag.y + this->GetDeltaPos().y);
			p_object->SetPosition(curpos);

			float percentmove = this->GetPercentMoveDistance();
			float ro = (100.f - percentmove) * 180.f / 100.f;
			p_object->GetWidgetChildAtIdx(2)->SetRotate(ro);
		}
		else if (type == EventType_SetCell::ON_END_DRAG_CELL)
		{
			WidgetEntity * p_object = nullptr;
			if (_obj_on_drag == 0)
			{
				p_object = this->_item_drag_top;
			}
			else if (_obj_on_drag == 1)
			{
				p_object = this->_item_drag_bottom;
			}
			if (!p_object)
			{
				return;
			}

			p_object->GetWidgetChildAtIdx(0)->Visible(false);
			p_object->GetWidgetChildAtIdx(1)->Visible(true);
			p_object->GetWidgetChildAtIdx(2)->SetRotate(180.f);

		}
		else if (type == EventType_SetCell::ON_BEGIN_DROP_CELL)
		{
			if (current_drag == EventType_DragCell::DRAG_DOWN)
			{
				if (this->HaveEndDrag())
				{
					if (abs(_distance_drag.y) >= 85.f * this->GetFirstDistanceMove().y / 100.f)
					{
						if (_func_when_call_top)
						{
							this->_func_when_call_top(ref, type, _widget);
							PLAY_SOUND("SFX_UPDATELIST");
						}
					}
				}
			}
			else if (current_drag == EventType_DragCell::DRAG_UP)
			{
				if (this->HaveEndDrag())
				{
					//if (abs(_distance_drag.y) >= 70.f * this->GetFirstDistanceMove().y / 100.f)
					{
						if (_func_when_call_bottom)
						{
							this->_func_when_call_bottom(ref, type, _widget);
							PLAY_SOUND("SFX_UPDATELIST");
						}
					}
				}
			}

			_distance_drag = Vec2(0, 0);
		}
		else if (type == EventType_SetCell::ON_END_DROP_CELL)
		{
			if (this->_item_drag_top)
			{
				this->_item_drag_top->Visible(false);
			}
			if (this->_item_drag_bottom)
			{
				this->_item_drag_bottom->Visible(false);
			}

			//_distance_drag = Vec2(0, 0);
			if (_func_when_cell_end_move)
			{
				_func_when_cell_end_move(ref, type, _widget);
			}
		}
	});
	this->InitTable();
}

void IwinListViewTable::UpdateReload(float dt)
{
	if (_need_to_reload)
	{
		_time_reload -= dt;
		if (_time_reload <= 0)
		{
			_need_to_reload = false;
			_time_reload = TIME_DELTA_PARSE;
			if (_func_when_reload)
			{
				_func_when_reload();
			}
		}
	}
}

void IwinListViewTable::SetHaveReload(const std::function<void(void)> & func_call_back)
{
	_need_to_reload = true;
	_func_when_reload = func_call_back;
}

void IwinListViewTable::InitOptionalParam(
	const std::function<void(Ref*, EventType_SetCell, WidgetEntity*)>&	call_back_touch_begin_cell,
	const std::function<void(Ref*, EventType_SetCell, WidgetEntity*)>&	call_back_touch_hold_cell,
	const std::function<void(Ref*, EventType_SetCell, WidgetEntity*)>&	call_back_begin_move_cell ,
	const std::function<void(Ref*, EventType_SetCell, WidgetEntity*)>&	call_back_end_move_cell 
)
{
	_func_when_touch_begin_cell = call_back_touch_begin_cell;
	_func_when_touch_hold_cell = call_back_touch_hold_cell;

	_func_when_cell_begin_move = call_back_begin_move_cell;
	_func_when_cell_end_move = call_back_end_move_cell;
}
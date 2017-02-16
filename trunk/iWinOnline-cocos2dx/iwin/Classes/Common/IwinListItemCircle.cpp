#include "IwinListItemCircle.h"
#include "UI/AdvanceUI/wLayout.h"
#include "UI/AdvanceUI/wListView.h"

using namespace Utility::UI_Widget;

IwinListItemCircle::IwinListItemCircle()
{
	_func_update_item = nullptr;
	_refer_item = nullptr;
	_list_items.clear();
	_list_idx_item_in_view = nullptr;
	_list_idx_item_in_center_view = nullptr;
	max_cord_x = 0;
	min_cord_x = 0;

	_eventCallback = nullptr;
}

IwinListItemCircle::~IwinListItemCircle()
{
	_func_update_item = nullptr;
	_refer_item = nullptr;
	_list_items.clear();
	delete _list_idx_item_in_view;
	delete _list_idx_item_in_center_view;
	_eventCallback = nullptr;
}

void IwinListItemCircle::init(
	Utility::UI_Widget::WidgetEntity* item,
	Utility::UI_Widget::WidgetEntity* parent_item,
	const std::function<void(Utility::UI_Widget::WidgetEntity* , int )> & func_update_item)
{
	_refer_item = item;
	_parent_item = parent_item;
	_func_update_item = func_update_item;

	auto size_parent = _parent_item->GetSize();
	auto size_item = _refer_item->GetSize();
	_number_item_per_view = ((size_parent.x / size_item.x) + 0.5f);
	_list_idx_item_in_center_view = new int[_number_item_per_view];
	have_action_move = 0;
}

void IwinListItemCircle::update(float dt)
{
	if (have_action_move > 0)
	{
		if (have_action_move == 1)
		{
			for (int i = 0 ; i <  _list_items.size() ; i ++)
			{
				auto it = _list_items[i];
				auto pos_to = it->GetPosition().x + _refer_item->GetSize().x;
				if (i == _list_items.size() - 1)
				{
					it->GetResource()->runAction(Sequence::createWithTwoActions(
						MoveTo::create(0.5f, Vec2(pos_to, it->GetPosition().y)), 
					CallFunc::create([this, it]() {
						have_action_move = -2;
						if (_eventCallback)
						{
							_eventCallback(it->GetResource(), EventType_ListCircleMove::ON_STOP);
						}
					})));
				}
				else
				{
					it->GetResource()->runAction(MoveTo::create(0.5f, Vec2(pos_to, it->GetPosition().y)));
				}
			}
		}
		else
		{
			for (int i = 0; i < _list_items.size(); i++)
			{
				auto it = _list_items[i];
				auto pos_to = it->GetPosition().x - _refer_item->GetSize().x;
				if (i == _list_items.size() - 1)
				{
					it->GetResource()->runAction(Sequence::createWithTwoActions(
						MoveTo::create(0.5f, Vec2(pos_to, it->GetPosition().y)),
						CallFunc::create([this, it]() {
						have_action_move = -2;
						if (_eventCallback)
						{
							_eventCallback(it->GetResource(), EventType_ListCircleMove::ON_STOP);
						}
					})));
				}
				else
				{
					it->GetResource()->runAction(MoveTo::create(0.5f, Vec2(pos_to, it->GetPosition().y)));
				}
			}
		}
		have_action_move = -1;
	}

	if (have_action_move == -2)
	{
		int index_center_view = 0;
		for (int i = 0 ; i < _list_items.size() ; i ++)
		{
			auto it = _list_items[i];
			if (it->GetPosition_Middle().x >= 0 && it->GetPosition_Middle().x <= _number_item_per_view*_refer_item->GetSize().x)
			{
				if (index_center_view < _number_item_per_view)
				{
					_list_idx_item_in_center_view[index_center_view] = i;
					index_center_view++;
				}
			}
		}
		have_action_move = 0;
	}
}

void IwinListItemCircle::callUpdateItemAtIdx(int idx)
{
	if (idx < 0 || idx >= _list_items.size())
		return;
	auto it = _list_items[idx];
	if (_func_update_item)
	{
		_func_update_item(it , idx);
	}
}

void IwinListItemCircle::setNumberItem(int number_item)
{
	if (_list_items.size() > 0)
		return;
	_list_idx_item_in_view = new int[number_item];
	int index_center_view = 0;
	for (int i = 0; i < number_item; i++)
	{
		auto it = _refer_item->Clone();
		if (_list_items.size() <= 0)
		{
			it->SetPosition(Vec2(0, 0));
		}
		else
		{
			auto last_it = _list_items.back();
			it->SetPosition(last_it->GetPosition_BottomRight());
		}
		it->Visible(true);
		_list_idx_item_in_view[i] = i;
		if (it->GetPosition_Middle().x >= 0 && it->GetPosition_Middle().x <= _number_item_per_view*_refer_item->GetSize().x)
		{
			if (index_center_view < _number_item_per_view)
			{
				_list_idx_item_in_center_view[index_center_view] = i;
				index_center_view++;
			}
		}

		_list_items.push_back(it);
		_parent_item->AddChildWidget(it);
	}

	max_cord_x = number_item * _refer_item->GetSize().x;
	min_cord_x = -(number_item - _number_item_per_view)* _refer_item->GetSize().x;
}

void IwinListItemCircle::scrollListItemInView(bool forward)
{
	if (forward)
	{
		auto it_last = _list_idx_item_in_view[_list_items.size() - 1];
		for (int i = _list_items.size() - 1; i > 0 ; i--)
		{
			_list_idx_item_in_view[i] = _list_idx_item_in_view[i - 1];
		}
		_list_idx_item_in_view[0] = it_last;
	}
	else
	{
		auto it_first = _list_idx_item_in_view[0];
		for (int i = 0 ; i < _list_items.size() - 1; i++)
		{
			_list_idx_item_in_view[i] = _list_idx_item_in_view[i + 1];
		}
		_list_idx_item_in_view[_list_items.size() - 1] = it_first;
	}
}

void IwinListItemCircle::MoveToNextItem()
{
	if (have_action_move != 0)
		return;
	//get the last item
	auto it_last = _list_items[_list_idx_item_in_view[_list_items.size() - 1]];
	//get the first item
	auto it_first = _list_items[_list_idx_item_in_view[0]];
	//
	if (it_last->GetPosition_BottomRight().x >= max_cord_x - 1.f)
	{
		it_last->SetPosition(Vec2(it_first->GetPosition_BottomLeft().x - it_first->GetSize().x, it_last->GetPosition_BottomLeft().y));
		scrollListItemInView(true);
	}
	have_action_move = 1;

	if (_eventCallback)
	{
		_eventCallback(it_last->GetResource(), EventType_ListCircleMove::ON_MOVE);
	}
}
void IwinListItemCircle::MoveToPreviousItem()
{
	if (have_action_move != 0)
		return;
	//get the last item
	auto it_last = _list_items[_list_idx_item_in_view[_list_items.size() - 1]];
	//get the first item
	auto it_first = _list_items[_list_idx_item_in_view[0]];
	//
	if (it_first->GetPosition_BottomLeft().x <= min_cord_x + 1.f)
	{
		it_first->SetPosition(it_last->GetPosition_BottomRight());
		scrollListItemInView(false);
	}
	have_action_move = 2;

	if (_eventCallback)
	{
		_eventCallback(it_first->GetResource(), EventType_ListCircleMove::ON_MOVE);
	}
}

void IwinListItemCircle::addEventListener(const wListCircleCallBack& callback)
{
	_eventCallback = callback;
}

int IwinListItemCircle::GetIndexOfVisibleView(int idx)
{
	if (idx < 0 || idx >= _number_item_per_view)
		return -1;
	return _list_idx_item_in_center_view[idx];
}
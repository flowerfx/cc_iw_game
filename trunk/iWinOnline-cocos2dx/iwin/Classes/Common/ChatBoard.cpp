#include "ChatBoard.h"
#include "cocos2d.h"
#include "UI/AdvanceUI/wLayout.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wText.h"

using namespace Utility;
using namespace Utility::UI_Widget;
using namespace cocos2d::ui;

ChatBoard::ChatBoard()
{
	_size_chat = Vec2(0, 0);
	_container = nullptr;
}

ChatBoard::~ChatBoard()
{
	/*for (auto it : _list_items)
	{
		delete it;
	}*/
	_scroll_view->ClearChild();
	_list_items.clear();
	//_scroll_view->GetResource()->removeAllChildrenWithCleanup(true);
	_scroll_view = nullptr;
	_item = nullptr;
	_container = nullptr;
}


void ChatBoard::init(Utility::UI_Widget::ListViewWidget* scroll_view, Utility::UI_Widget::WidgetEntity* item)
{
	_scroll_view = scroll_view;
	_item = item;
	//_container = Layout::create();
	//_container->setCascadeOpacityEnabled(true);
	//scroll_view->GetResource()->addChild(_container);
	_num_chat = 0;
}

void ChatBoard::addChat(std::string user_name, std::string msg)
{
	auto current_inner = static_cast<ListView*>(_scroll_view->GetResource())->getInnerContainerSize();
	auto size_scroll_view = _scroll_view->GetSize();

	std::string message = StringUtils::format("[#ff5722]%s :[#ffffff]%s", user_name.c_str(), msg.c_str());
	_num_chat++;
	//if (_list_items.size() < _num_chat)
	{
		WidgetEntity* entity = _item->Clone();
		entity->Visible(true);
		//entity->SetParentWidget(_scroll_view);
		//entity->GetResource()->removeFromParent();
		_list_items.push_back(entity);

		WidgetEntity * text_item = entity->GetWidgetChildAtIdx(0);
		static_cast<TextWidget*>(text_item)->SetText(message, true);

		Vec2 size_text = static_cast<TextWidget*>(text_item)->GetActualContainSize();
		int number_line = static_cast<TextWidget*>(text_item)->GetNumberOfLine();
		float size_y_each_line = size_text.y / number_line;
		float font_size_text = static_cast<TextWidget*>(text_item)->GetFontSize();
		float delta = abs(size_y_each_line - font_size_text);
		size_text.y = (font_size_text * number_line) + delta;

		entity->SetSize(Vec2(current_inner.width , size_text.y));
		text_item->SetPosition(Vec2(5, size_text.y));

		_scroll_view->PushBackItem(entity);


		_size_chat.y += size_text.y;
		//_container->addChild(entity->GetResource());
		//_scroll_view->AddChildWidget(entity);
		/*if (_size_chat.y > current_inner.height)
		{
			_scroll_view->SetInnerSize(cocos2d::Size(current_inner.width, _size_chat.y));
		}*/

	}


	/*auto current_inner = static_cast<ScrollView*>(_scroll_view->GetResource())->getInnerContainerSize();
	auto size_scroll_view = _scroll_view->GetSize();

	WidgetEntity * text_item = _list_items[_num_chat - 1];
	static_cast<TextWidget*>(text_item)->SetText(message, true);
	text_item->SetPosition(Vec2(5, size_scroll_view.y - _size_chat.y ));

	Vec2 size_text = static_cast<TextWidget*>(text_item)->GetActualContainSize();
	int number_line = static_cast<TextWidget*>(text_item)->GetNumberOfLine();
	float size_y_each_line = size_text.y / number_line;
	float font_size_text = static_cast<TextWidget*>(text_item)->GetFontSize();
	float delta = abs(size_y_each_line - font_size_text);
	size_text.y = (font_size_text * number_line) + delta;
	_size_chat.y += size_text.y;


	if (_size_chat.y > current_inner.height)
	{
		_scroll_view->SetInnerSize(cocos2d::Size(current_inner.width, _size_chat.y));
	}*/

	//static_cast<ScrollView*>(_scroll_view->GetResource())->setInnerContainerSize(container_size);
	static_cast<ListView*>(_scroll_view->GetResource())->scrollToBottom(0.5,false);

}

void ChatBoard::reset()
{
	/*for (auto it : _list_items)
	{
		delete it;
	}*/
	_scroll_view->ClearChild();
	_list_items.clear();
	//_container->removeAllChildrenWithCleanup(true);
	_num_chat = 0;
	_size_chat = Vec2(0, 0);
}

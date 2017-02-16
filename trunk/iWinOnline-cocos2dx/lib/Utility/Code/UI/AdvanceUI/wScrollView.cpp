#include "wScrollView.h"
#include <string.h>
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		ScrollViewWidget::ScrollViewWidget()
		{
			p_typeUI = UI_TYPE::UI_SCROLLVIEW;

		}
		ScrollViewWidget::~ScrollViewWidget()
		{

		}

		void ScrollViewWidget::CopyValue(WidgetEntity * value)
		{
			LayoutWidget::CopyValue(value);
		}

		WidgetEntity * ScrollViewWidget::Clone()
		{
			ScrollViewWidget * p_clone = new ScrollViewWidget();
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void ScrollViewWidget::InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)
		{
			CreateLayout();
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void ScrollViewWidget::CreateLayout()
		{
			ScrollView* panel = ScrollView::create();
			p_Resource = panel;
		}

		void ScrollViewWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			LayoutWidget::InitTexture(xml_value);

			ScrollView* panel = static_cast<ScrollView*>(p_Resource);

			if (xml_value->GeneralValue->GetDataInt("scrollbar_enable") == 1)
			{
				panel->setScrollBarEnabled(true);
				panel->setScrollBarWidth(4);
				panel->setScrollBarPositionFromCorner(Vec2(2, 2));
				panel->setScrollBarColor(Color3B::WHITE);
			}
			else
			{
				panel->setScrollBarEnabled(false);
			}
			Vec2 inner_size = xml_value->GeneralValue->GetDataVector2("inner_size");

			inner_size.x = inner_size.x * GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
			inner_size.y = inner_size.y * GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;
            panel->setInnerContainerSize(cocos2d::Size(inner_size.x, inner_size.y));
			panel->setBounceEnabled(xml_value->GeneralValue->GetDataInt("bounce_enable") == 1);
			if (xml_value->GeneralValue->HaveDataVector2("Percent_Scroll"))
			{
				panel->SetPercentPosScrollBounce(xml_value->GeneralValue->GetDataVector2("Percent_Scroll"));
			}
			else
			{
				panel->SetPercentPosScrollBounce(Vec2(0.15f, 0.15f));
			}

			ui::ScrollView::Direction direction = (ui::ScrollView::Direction)xml_value->GeneralValue->GetDataInt("direction");
			panel->setDirection(direction);

			int touch_item = xml_value->GeneralValue->GetDataInt("touch_item");
			if (touch_item > 0)
			{
				panel->setPropagateTouchEvents(true);
			}
		}
		
		void ScrollViewWidget::SetSizeValueXML(xml::UILayerWidgetDec * xml_value)
		{
			ActionProcess::SetSizeValueXML(xml_value);

			static_cast<ScrollView*>(p_Resource)->jumpToTop();
		}

		void ScrollViewWidget::SetInteractive(bool value)
		{
			if (p_disable)
				return;
			WidgetEntity::SetInteractive(value);
			static_cast<ScrollView*>(p_Resource)->setTouchEnabled(value);
		}

		void ScrollViewWidget::SetInnerSize(const Vec2 & size)
		{
			static_cast<ScrollView*>(p_Resource)->setInnerContainerSize(cocos2d::Size(size.x, size.y));
		}

		void ScrollViewWidget::SetCallBackScroll(const ui::ScrollView::ccScrollViewCallback& callback)
		{
			if (callback)
			{
				static_cast<ScrollView*>(this->GetResource())->addEventListener(callback);
			}
		}
	}
}
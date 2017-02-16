#include "wPageView.h"
#include "wLayout.h"
#include "LayerEntity.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		PageViewWidget::PageViewWidget()
		{
			p_typeUI = UI_TYPE::UI_PAGEVIEW;
		}
		PageViewWidget::~PageViewWidget()
		{
		}

		void PageViewWidget::CopyValue(WidgetEntity * value)
		{
			ListViewWidget::CopyValue(value);
		}

		WidgetEntity * PageViewWidget::Clone()
		{
			PageViewWidget * p_clone = new PageViewWidget();
			p_clone->CopyValue(this);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void PageViewWidget::CreateLayout()
		{
			PageView* panel = PageView::create();
			p_Resource = panel;
		}

		void PageViewWidget::InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)
		{
			CreateLayout();
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);

			
		}

		void PageViewWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			ListViewWidget::InitTexture(xml_value);

			int touch_item = xml_value->GeneralValue->GetDataInt("touch_item");
			if (touch_item > 0)
			{
				static_cast<PageView*>(GetResource())->setPropagateTouchEvents(true);
			}

			bool enable_idicator = xml_value->GeneralValue->GetDataInt("indicator") == 1;
			if (enable_idicator)
			{
				static_cast<PageView*>(p_Resource)->setIndicatorEnabled(enable_idicator);

				Vec2 pos_indi = xml_value->GeneralValue->GetDataVector2("indicator_pos");
				pos_indi.x = pos_indi.x * GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
				pos_indi.y = pos_indi.y * GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;
				static_cast<PageView*>(p_Resource)->setIndicatorPosition(pos_indi);
				static_cast<PageView*>(p_Resource)->setIndicatorIndexNodesScale(xml_value->GeneralValue->GetDatafloat("indicator_scale"));
			}
			//panel->setLayoutComponentEnabled(true);
		}

		void PageViewWidget::FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)
		{
			for (int i = 0; i < (int)(xml_value->p_list_child_widget.size()); i++)
			{
				auto _widget_child = this->GetParentLayer()->InitWidgetEntity(xml_value->p_list_child_widget.at(i), this->GetParentLayer(), this);
				_widget_child->SetMenuParent(this->GetMenuParent());
				_widget_child->SetParentWidget(this);

				if (_widget_child->getUIWidgetType() != UI_TYPE::UI_LAYOUT
					||
					xml_value->p_list_child_widget.at(i)->GeneralValue->GetDataInt("not_default_layout") == 1
					)
				{
					int idx = static_cast<PageView*>(p_Resource)->getChildrenCount();

					static_cast<PageView*>(p_Resource)->insertPage(static_cast<Widget*>(_widget_child->GetResource()), idx);

					_widget_child->SetName(_widget_child->GetName() /*+ std::to_string(idx)*/);
					p_listChildWidget.push_back(_widget_child);
					_widget_child->GetResource()->setName((_widget_child->GetName() /*+ std::to_string(idx)*/).GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
					_widget_child->GetResource()->setLocalZOrder(p_listChildWidget.size());
					_widget_child->SetParentLayer(this->GetParentLayer());
				}
				else
				{
					p_refer_child.push_back(static_cast<LayoutWidget *>(_widget_child));
					p_refer_child.at(p_refer_child.size() - 1)->GetResource()->retain();
				}
			}
		}

		void PageViewWidget::SetSizeValueXML(xml::UILayerWidgetDec * xml_value)
		{
			ActionProcess::SetSizeValueXML(xml_value);

			int percent_ = xml_value->GeneralValue->GetDataInt("Threshold_scroll");

			//static_cast<PageView*>(p_Resource)->setCustomScrollThreshold(percent_ * 0.01 * p_Resource->getContentSize().width);
		}


		WidgetEntity * PageViewWidget::CreateWidgetRefer(int idx, int idx_refer)
		{
			auto page_view = static_cast<PageView*>(p_Resource);
			if (idx_refer >= (int)p_refer_child.size())
			{
				PASSERT2(idx_refer < (int)p_refer_child.size(), "idx_refer is out of range!");
				idx_refer = 0;
				if (p_refer_child.size() <= 0)
				{
					PASSERT2(false, "size of p_refer_child is zero");
					return nullptr;
				}
			}
			LayoutWidget * _layout_widget = nullptr;

			_layout_widget = static_cast<LayoutWidget*>(p_refer_child[idx_refer]->Clone());

			page_view->insertPage(static_cast<Layout*>(_layout_widget->GetResource()), idx);

			_layout_widget->SetName(_layout_widget->GetName() + std::to_string(idx));
			p_listChildWidget.push_back(_layout_widget);
			_layout_widget->GetResource()->setName((_layout_widget->GetName() + std::to_string(idx)).GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
			if (_layout_widget->GetZOrder() == 0)
			{
				_layout_widget->SetZOrder(p_listChildWidget.size());
			}
			_layout_widget->GetResource()->setLocalZOrder(_layout_widget->GetZOrder());
			_layout_widget->SetParentLayer(this->GetParentLayer());
			_layout_widget->SetMenuParent(this->GetMenuParent());
			_layout_widget->SetParentWidget(this);

			return _layout_widget;

		}


		void PageViewWidget::PushBackItem(WidgetEntity * _widget_child)
		{

			_widget_child->SetMenuParent(this->GetMenuParent());
			_widget_child->SetParentWidget(this);

			int idx = static_cast<PageView*>(p_Resource)->getChildrenCount();

			static_cast<PageView*>(p_Resource)->insertPage(static_cast<Widget*>(_widget_child->GetResource()), idx);

			_widget_child->SetName(_widget_child->GetName() /*+ std::to_string(idx)*/);
			p_listChildWidget.push_back(_widget_child);
			_widget_child->GetResource()->setName((_widget_child->GetName() /*+ std::to_string(idx)*/).GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
			_widget_child->GetResource()->setLocalZOrder(p_listChildWidget.size());
			_widget_child->SetParentLayer(this->GetParentLayer());
			//GetResource()->addChild(_widget_child->GetResource());
		}


		int PageViewWidget::GetCurrentPageIdx()
		{
			return static_cast<PageView*>(p_Resource)->getCurrentPageIndex();
		}

		void PageViewWidget::JumpToPageIdx(int idx)
		{
			static_cast<PageView*>(p_Resource)->setCurrentPageIndex(idx);
		}

		void PageViewWidget::ScrollToPageIdx(int idx)
		{
			static_cast<PageView*>(p_Resource)->scrollToPage(idx);
		}

		void PageViewWidget::SetInteractive(bool value)
		{
			if (p_disable)
				return;
			WidgetEntity::SetInteractive(value);
			static_cast<PageView*>(p_Resource)->setTouchEnabled(value);
		}

	}
}
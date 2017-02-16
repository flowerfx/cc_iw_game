#include "wListView.h"
#include "LayerEntity.h"
#include "wLayout.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		ListViewWidget::ListViewWidget()
		{
			p_typeUI = UI_TYPE::UI_LISTVIEW;
			p_refer_child.clear();
			current_idx_selected = -1;

		}
		ListViewWidget::~ListViewWidget()
		{
			for (auto p : p_refer_child)
			{
				delete p;
			}
			p_refer_child.clear();
		}

		void ListViewWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<ListViewWidget*>(value);
			ScrollViewWidget::CopyValue(value);
			current_idx_selected = val->current_idx_selected;
			p_Resource->removeAllChildrenWithCleanup(true);
			for (auto c : val->p_refer_child)
			{
				p_refer_child.push_back(static_cast<LayoutWidget*>(c->Clone()));
			}
		}

		WidgetEntity * ListViewWidget::Clone()
		{
			ListViewWidget * p_clone = new ListViewWidget();
			p_clone->CopyValue(this);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void ListViewWidget::CreateLayout()
		{
			ListView* panel = ListView::create();
			p_Resource = panel;
		}

		void ListViewWidget::InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)
		{
			CreateLayout();
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);

		}

		void ListViewWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			ScrollViewWidget::InitTexture(xml_value);

			ListView* panel = static_cast<ListView *>(p_Resource);

			//// set all items layout gravity
			if (xml_value->GeneralValue->HaveDataInt("gravity"))
			{
				ListView::Gravity type = (ListView::Gravity)xml_value->GeneralValue->GetDataInt("gravity");
				panel->setGravity(type);
			}
			else
			{
				panel->setGravity(ListView::Gravity::CENTER_VERTICAL);
			}
			//// set all items margetic type
			if (xml_value->GeneralValue->HaveDataInt("magnetic_type"))
			{
				ListView::MagneticType type = (ListView::MagneticType)xml_value->GeneralValue->GetDataInt("magnetic_type");
				panel->setMagneticType(type);
			}
			//
			//// set items margin
			//panel->setItemsMargin(3.0f);

			bool as_page_view =( xml_value->GeneralValue->GetDataInt("as_page_view") == 1);
			if (as_page_view)
			{
				int number_move = xml_value->GeneralValue->GetDataInt("number_move");
				panel->SetAsPageView(as_page_view);
				panel->SetNumberItemMove(number_move);
			}

			
		}

		void ListViewWidget::SetSizeValueXML(xml::UILayerWidgetDec * xml_value)
		{
			ActionProcess::SetSizeValueXML(xml_value);

			static_cast<ListView*>(p_Resource)->jumpToTop();
		}

		void ListViewWidget::FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)
		{
			//PASSERT2(xml_value->p_list_child_widget.size() == 1, "the sample list must have 1 value to refer");
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
					int idx = static_cast<ListView*>(p_Resource)->getChildrenCount();
					static_cast<ListView*>(p_Resource)->pushBackCustomItem(static_cast<Widget*>(_widget_child->GetResource()));

					_widget_child->SetName(_widget_child->GetName() /*+ std::to_string(idx)*/);
					p_listChildWidget.push_back(_widget_child);
					_widget_child->GetResource()->setName((_widget_child->GetName() /*+ std::to_string(idx)*/).GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
					_widget_child->GetResource()->setLocalZOrder(p_listChildWidget.size());
					_widget_child->SetParentLayer(this->GetParentLayer());
				}
				else
				{
					p_refer_child.push_back(static_cast<LayoutWidget *>(_widget_child));
					p_refer_child.at(p_refer_child.size() -1 )->GetResource()->retain();
				}
			}
			//static_cast<ListView*>(p_Resource)->setItemModel(static_cast<Widget*>(_widget_child->GetResource()));
			//InsertChildWidget(_widget_child);
		}

		void ListViewWidget::InsertChildWidget(WidgetEntity* child_widget)
		{
			PushBackItem(child_widget);
		}


		WidgetEntity * ListViewWidget::CreateWidgetReferNoPushBack(int refer_idx)
		{
			auto list_view = static_cast<ListView*>(p_Resource);
			if (refer_idx >= (int)p_refer_child.size())
			{
				PASSERT2(refer_idx < (int)p_refer_child.size(), "idx_refer is out of range!");
				refer_idx = 0;
				if (p_refer_child.size() <= 0)
				{
					PASSERT2(false, "size of p_refer_child is zero");
					return nullptr;
				}
			}
			LayoutWidget * _layout_widget = nullptr;
			
			_layout_widget = static_cast<LayoutWidget*>(p_refer_child[refer_idx]->Clone());
			//_layout_widget->SetName(_layout_widget->GetName() + std::to_string(idx));
			//_layout_widget->GetResource()->setName((_layout_widget->GetName() + std::to_string(idx)).GetString()); //this function is get the name define on xml and parse to the cocos2dx logic

			return _layout_widget;
		}

		WidgetEntity * ListViewWidget::CreateWidgetRefer(int idx , int idx_refer)
		{
			auto list_view = static_cast<ListView*>(p_Resource);
			if (idx_refer >= (int)p_refer_child.size() )
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

			list_view->pushBackCustomItem(static_cast<Widget*>(_layout_widget->GetResource()));

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

		void ListViewWidget::ResetListView()
		{
			GetResource()->removeAllChildren();
				
			//p_listChildWidget.clear();
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				p_listChildWidget.at(i)->SetDeleteResource(false);
				delete p_listChildWidget.at(i);
			}
			p_listChildWidget.clear();
		}

		void ListViewWidget::PushBackItem(WidgetEntity * _layout_widget)
		{
			auto list_view = static_cast<ListView*>(p_Resource);

			list_view->pushBackCustomItem(static_cast<Widget*>(_layout_widget->GetResource()));

			p_listChildWidget.push_back(_layout_widget);
			_layout_widget->GetResource()->setName(_layout_widget->GetName().GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
			if (_layout_widget->GetZOrder() == 0)
			{
				_layout_widget->SetZOrder(p_listChildWidget.size());
			}
			_layout_widget->GetResource()->setLocalZOrder(_layout_widget->GetZOrder());
			_layout_widget->SetParentLayer(this->GetParentLayer());
			_layout_widget->SetParentWidget(this);
			_layout_widget->SetMenuParent(this->GetMenuParent());
		}

		void ListViewWidget::JumpToItemIdx(int idx)
		{
			static_cast<ListView*>(p_Resource)->jumpToItem(idx, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
		}

		void ListViewWidget::ForceVisibleAllChildLayout(const RKString & path, bool visible)
		{
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				auto widget_layout = p_listChildWidget.at(i);
				if (widget_layout->getUIWidgetType() == UI_TYPE::UI_LAYOUT)
				{
					auto child_as_path = widget_layout->GetWidgetChildByName(path);
					if (child_as_path)
					{
						child_as_path->Visible(visible);
					}
				}
			}
		}

		void ListViewWidget::ClearChild(int count)
		{
			if (count != 0)
			{
				//static_cast<Widget*>(this->GetResource())->removeFromParentAndCleanup(true);
				count++;
			}

			this->GetResource()->removeAllChildren();
			
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				p_listChildWidget.at(i)->SetDeleteResource(false);
				delete p_listChildWidget.at(i);
			}
			p_listChildWidget.clear();
		}

		int ListViewWidget::GetCurrentIdxSelected() { return current_idx_selected; }

		void ListViewWidget::SetCurrentIdxSelected(int idx) { current_idx_selected = idx; }

		void ListViewWidget::SetNumberEachSlide(int number)
		{
			auto list_view = static_cast<ListView*>(GetResource());
			if (list_view->BehaviousAsPage())
			{
				list_view->SetNumberItemMove(number);
			}
		}

		void ListViewWidget::SetInteractive(bool value)
		{
			if (p_disable)
				return;
			WidgetEntity::SetInteractive(value);
			static_cast<ListView*>(p_Resource)->setTouchEnabled(value);
		}

		void ListViewWidget::SetCallBackList(const ui::ListView::ccListViewCallback& callback)
		{
			if (callback)
			{
				static_cast<ListView*>(this->GetResource())->addEventListener(callback);
			}
		}
	}
}
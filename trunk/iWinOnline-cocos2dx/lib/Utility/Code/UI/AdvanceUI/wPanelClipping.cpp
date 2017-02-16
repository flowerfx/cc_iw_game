#include "wPanelClipping.h"
#include "LayerEntity.h"
#include "wLayout.h"
#include "Screen.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		PanelClippingWidget::PanelClippingWidget()
		{
			p_typeUI = UI_TYPE::UI_PANEL_CLIPPING;
			p_resource = nullptr;

		}

		PanelClippingWidget::~PanelClippingWidget()
		{
			if (p_resource)
			{
				//p_resource->removeAllChildren();
			}
			p_resource = nullptr;
		}
		
		cocos2d::Node * PanelClippingWidget::GetResource() {
			if (p_resource)
			{
				return p_resource;
			}
			return p_Resource;
		}
		bool PanelClippingWidget::SetNewResource(cocos2d::Node * resource)
		{
			if (p_resource)
			{
				p_resource = static_cast<ClippingNode*>(resource);
			}
			else
			{
				return false;
			}

			return true;
		}

		void PanelClippingWidget::CopyValue(WidgetEntity * value)
		{
			WidgetEntity::CopyValue(value);
			GetParentWidget()->ResizeAndPosChildWidget(this);
		}

		WidgetEntity * PanelClippingWidget::Clone()
		{
			//PanelClippingWidget * p_clone = new PanelClippingWidget();		
			WidgetEntity * p_clone = this->GetParentLayer()->InitWidgetEntity(
				p_xml_dec, 
				this->GetParentLayer(), 
				this->GetParentWidget());

			p_clone->CopyValue(this);
			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void PanelClippingWidget::InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void PanelClippingWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			p_resource = ClippingNode::create();
			p_resource->setAlphaThreshold(0.5f);
		}
		
		void PanelClippingWidget::CloneChild(WidgetEntity * p_clone)
		{
			for (int i = 0; i < (int)p_listChildWidget.size(); i++)
			{
				auto child_widget = p_listChildWidget.at(i)->Clone();

				p_clone->AddChildWidgetToList(child_widget);

				p_clone->GetResource()->setName(child_widget->GetName().GetString()); //this function is get the name define on xml and parse to the cocos2dx logic
				if (child_widget->GetZOrder() == 0)
				{
					child_widget->SetZOrder(p_listChildWidget.size());
				}
				p_clone->GetResource()->setLocalZOrder(child_widget->GetZOrder());
				p_clone->SetParentLayer(this->GetParentLayer());
				p_clone->GetResource()->addChild(child_widget->GetResource() , child_widget->GetZOrder());
			}
		}
		void PanelClippingWidget::FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)
		{
			for (int i = 0; i < (int)(xml_value->p_list_child_widget.size()); i++)
			{
                if(xml_value->p_list_child_widget.at(i)->GeneralValue->GetDataInt("stencil") == 1)
                {
					if (!p_resource)
					{
						continue;
					}

                    auto xml_detail = xml_value->p_list_child_widget.at(i);
                    RKString resource = xml_detail->GeneralValue->GetDataChar("source");
					p_texture_source = resource;
                    
					int stage_scale = xml_value->GeneralValue->GetDataInt("state_scale");
					int is_stick_screen = xml_value->GeneralValue->GetDataInt("stick_screen");
                    auto originPosRatio = xml_detail->OriginValue->GetDataVector2("pos_ratio");
                    auto posOrigin = Vec2(GetGameSize().width * originPosRatio.x, GetGameSize().height * originPosRatio.y);

                    auto origin_ScaleRatio = xml_value->OriginValue->GetDataVector2("scale_ratio");		
					auto size_origin = Vec2(1, 1);
					Vec2 scale_screen = Vec2( GetGameSize().width / this->GetMenuParent()->GetDesignSize().x,
						GetGameSize().height / this->GetMenuParent()->GetDesignSize().y);
				
					if (is_stick_screen == 1)
					{
						size_origin = Vec2(origin_ScaleRatio.x * scale_screen.x,
							origin_ScaleRatio.y * scale_screen.y);
					}
					else
					{
						if (stage_scale == 0)
						{
							size_origin = Vec2(origin_ScaleRatio.x * scale_screen.x,
								origin_ScaleRatio.y * scale_screen.x);
						}
						else if (stage_scale == 1)
						{
							size_origin = Vec2(origin_ScaleRatio.x * scale_screen.y,
								origin_ScaleRatio.y * scale_screen.y);
						}
						else
						{
							size_origin = origin_ScaleRatio;
						}
					}
					

                    Sprite * sp = Sprite::createWithSpriteFrameName(resource.GetString());
                    sp->setPosition(posOrigin);
                    Vec2 p_Size(1,1);
                    p_Size.x = size_origin.x / sp->getContentSize().width;
                    p_Size.y = size_origin.y / sp->getContentSize().height;
					
					sp->setScaleX(p_Size.x);
					sp->setScaleY(p_Size.y);
					
                    
					this->ResizeAndPosNode(sp);
                    this->p_resource->setStencil(sp);
                }
                else
                {
                    auto _widget_child = this->GetParentLayer()->InitWidgetEntity(xml_value->p_list_child_widget.at(i), this->GetParentLayer(), this);

                    ResizeAndPosChildWidget(_widget_child);

                    _widget_child->SetMenuParent(this->GetMenuParent());
                    _widget_child->SetParentWidget(this);
                    InsertChildWidget(_widget_child);
                }

			}
		}

		bool PanelClippingWidget::Update(float dt)
		{
			if (!WidgetEntity::Update(dt))
				return false;

			return true;
		}
	}
}
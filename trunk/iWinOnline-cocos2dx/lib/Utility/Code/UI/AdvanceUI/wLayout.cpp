#include "wLayout.h"
#include "LayerEntity.h"
namespace Utility
{
	namespace UI_Widget
	{

		/////////////////////////////////////////////////////////////
		LayoutWidget::LayoutWidget()
		{
			p_typeUI = UI_TYPE::UI_LAYOUT;
		}
		LayoutWidget::~LayoutWidget()
		{
			//GetResource()->getEventDispatcher()->removeEventListener(_eventListener);
		}
	
		void LayoutWidget::CopyValue(WidgetEntity * value)
		{
			WidgetEntity::CopyValue(value);
		}

		WidgetEntity * LayoutWidget::Clone()
		{
			LayoutWidget * p_clone = new LayoutWidget();
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}
		/////////////////////////////////////////////////////////////

		void LayoutWidget::CreateLayout()
		{
			auto layout_widget = Layout::create();
			p_Resource = layout_widget;
		}

		void LayoutWidget::SetInitPos(xml::UILayerWidgetDec * xml_value)
		{
			//
			ActionProcess::SetInitPos(xml_value);
			return;
			//
			auto pos_ratio = p_parent_layer ? xml_value->OriginValue->GetDataVector2("pos_ratio") : ConvertPos(xml_value->OriginValue->GetDataVector2("pos_ratio"));
			auto pos = Vec2(GetGameSize().width * pos_ratio.x, GetGameSize().height * pos_ratio.y);

			SetPosition(pos);
			p_origin_value.ReplaceDataVector2("pos", pos);
		}

		void LayoutWidget::SetPosValueXML(xml::UILayerWidgetDec * xml_value)
		{
			if (!xml_value->FirstValue->HaveDataVector2("pos_ratio"))
			{
				return;
			}

			auto originPosRatio = p_parent_layer ? xml_value->OriginValue->GetDataVector2("pos_ratio") : ConvertPos(xml_value->OriginValue->GetDataVector2("pos_ratio"));
			auto firstPosRatio = p_parent_layer ? xml_value->FirstValue->GetDataVector2("pos_ratio") : ConvertPos(xml_value->FirstValue->GetDataVector2("pos_ratio"));

			auto posOrigin = Vec2(GetGameSize().width * originPosRatio.x, GetGameSize().height * originPosRatio.y);
			auto posFirst = Vec2(GetGameSize().width * firstPosRatio.x, GetGameSize().height * firstPosRatio.y);

			PASSERT2(GetResource()->getContentSize().width != 0 || GetResource()->getContentSize().height != 0, "the content size of resource must bigger than 0");

			SetFirstPos(posFirst, xml_value->GeneralValue->GetDataInt("not_fade_pos") == 0);
		}

		void LayoutWidget::InitParam(const RKString &  name, xml::UILayerWidgetDec * xml_value)
		{
			CreateLayout();
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}
		
		void LayoutWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			InitParamLayoutValue(xml_value);
		}

		void LayoutWidget::FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)
		{
			WidgetEntity::FindAndInsertChildWidget(xml_value);
			return;
			for (int i = 0; i < (int)(xml_value->p_list_child_widget.size()); i++)
			{
				auto _widget_child = this->GetParentLayer()->InitWidgetEntity(xml_value->p_list_child_widget.at(i), this->GetParentLayer(),this);
				_widget_child->SetMenuParent(this->GetMenuParent());
				_widget_child->SetParentWidget(this);
				ResizeAndPosChildWidget(_widget_child);
				Vec2 scale_layout = this->GetScale();
				
				Vec2 scale_widget = _widget_child->GetScale();

				Vec2 additon_scale;

				additon_scale.x = scale_layout.x <= 1.f ? (1.f / scale_layout.x) : scale_layout.x;
				additon_scale.y = scale_layout.y <= 1.f ? (1.f / scale_layout.y) : scale_layout.y;

				scale_widget.x = scale_widget.x * additon_scale.x;
				scale_widget.y = scale_widget.y * additon_scale.y;

				//_widget_child->SetScale(scale_widget);
				InsertChildWidget(_widget_child);
			}
		}

		void LayoutWidget::InitParamLayoutValue(xml::UILayerWidgetDec * xml_value)
		{
			if (xml_value->GeneralValue->GetDataInt("clipping_enable") == 1)
			{
				static_cast<Layout*>(p_Resource)->setClippingEnabled(true);
			}
			else
			{
				static_cast<Layout*>(p_Resource)->setClippingEnabled(false);
			}

			int background_type = xml_value->GeneralValue->GetDataInt("background_type");

			if (background_type >= 0 && background_type <= 2)
			{
				static_cast<Layout*>(p_Resource)->setBackGroundColorType((ui::Layout::BackGroundColorType)background_type);

				if (background_type == 1)
				{
					Color4B color_layout = xml_value->GeneralValue->GetDataColor("tint");
					static_cast<Layout*>(p_Resource)->setBackGroundColor(Color3B(color_layout.r, color_layout.g, color_layout.b));
					RKUtils::BYTE opacity_layout = xml_value->GeneralValue->GetDataInt("opacity");
					static_cast<Layout*>(p_Resource)->setBackGroundColorOpacity(opacity_layout);
				}
				else if (background_type == 2)
				{
					Color4B color_layout_from = xml_value->GeneralValue->GetDataColor("tint");
					Color4B color_layout_to = xml_value->GeneralValue->GetDataColor("tint_to");
					static_cast<Layout*>(p_Resource)->setBackGroundColor(Color3B(color_layout_from.r, color_layout_from.g, color_layout_from.b),
						Color3B(color_layout_to.r, color_layout_to.g, color_layout_to.b));
					RKUtils::BYTE opacity_layout = xml_value->GeneralValue->GetDataInt("opacity");
					static_cast<Layout*>(p_Resource)->setBackGroundColorOpacity(opacity_layout);
				}
			}
			else if (background_type == 3)
			{
				RKString name_image = xml_value->GeneralValue->GetDataChar("source");
				p_texture_source = name_image;
				bool IsUseAtlast = xml_value->GeneralValue->GetDataInt("use_atlas") == 1;

				//	auto t_sprite = TextureMgr->GetSpriteByName(name_image.GetString());
				//	Vec2 size_srpite = t_sprite->getContentSize();
				//	//p_Resource->setContentSize(Size(size_srpite.x, size_srpite.y));

				static_cast<Layout*>(p_Resource)->setBackGroundImage(name_image.GetString(), IsUseAtlast ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL);
				static_cast<Layout*>(p_Resource)->setBackGroundImageScale9Enabled(p_isScale9Enable);
				if (xml_value->GeneralValue->GetDataInt("use_nine_path") == 1)
				{
					auto cap_insert = SpriteFrameCache::getInstance()->getSpriteFrameByName(name_image.GetString())->GetCapInsert();

					RKString cap = xml_value->GeneralValue->GetDataChar("cap_insert");
					if (cap != "")
					{
						cap_insert = XMLMgr->ConvertToRect(cap);
					}

					static_cast<Layout*>(p_Resource)->setBackGroundImageCapInsets(cap_insert);
				}

			}
			else
			{
				PWARN1("Wrong input !");
			}
		}

		void LayoutWidget::SetSize(const Vec2 & size)
		{
			static_cast<ui::Layout*>(GetResource())->setContentSize(cocos2d::Size(size.x, size.y));
		}

	}
}
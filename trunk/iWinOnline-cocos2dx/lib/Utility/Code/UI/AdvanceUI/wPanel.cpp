#include "wPanel.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		PanelWidget::PanelWidget()
		{
			p_typeUI = UI_TYPE::UI_PANEL;

		}
		PanelWidget::~PanelWidget()
		{

		}
		
		void PanelWidget::CopyValue(WidgetEntity * value)
		{		
			WidgetEntity::CopyValue(value);
		}

		WidgetEntity * PanelWidget::Clone()
		{
			PanelWidget * p_clone = new PanelWidget();
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void PanelWidget::InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void PanelWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			auto source_texture = xml_value->GeneralValue->GetDataChar("source");
			p_texture_source = source_texture;
			bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);

			ImageView* panel = nullptr;
			if (source_texture != "")
			{
				if (IsUseAtlas)
				{
					panel = ImageView::create(source_texture, Widget::TextureResType::PLIST);
				}
				else
				{
					panel = ImageView::create(source_texture);
				}
			}
			else
			{
				panel = ImageView::create();
				p_scale_content = true;
			}
            cocos2d::Rect cap_insert = cocos2d::Rect(0, 0, 0, 0);
			if (xml_value->GeneralValue->GetDataInt("use_nine_path") == 1)
			{
				cap_insert = SpriteFrameCache::getInstance()->getSpriteFrameByName(source_texture)->GetCapInsert();

				RKString cap = xml_value->GeneralValue->GetDataChar("cap_insert");
				if (cap != "")
				{
					cap_insert = XMLMgr->ConvertToRect(cap);
				}

			}
			if (p_isScale9Enable)
			{
				panel->setScale9Enabled(p_isScale9Enable);
				panel->setCapInsets(cap_insert);
			}

			p_Resource = panel;
		}

		void PanelWidget::SetImage(const RKString & name_path, bool is_atlas)
		{
			static_cast<ImageView*>(p_Resource)->loadTexture(name_path.GetString(), is_atlas ? TextureResType::PLIST : TextureResType::LOCAL);
		}
		

	}
}
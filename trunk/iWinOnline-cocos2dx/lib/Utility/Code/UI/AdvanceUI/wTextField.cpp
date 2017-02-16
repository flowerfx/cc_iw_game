#include "wTextField.h"
namespace Utility
{
	namespace UI_Widget
	{
		/////////////////////////////////////////////////////////////
		TextFieldWidget::TextFieldWidget()
		{
			p_current_text = "";
			p_default_place_holder = "";
			_event_callback = nullptr;
			p_use_bmp_text = false;

			p_color_text = p_color_placeHolder = Color4B::WHITE;
			p_touch_zone = p_text_area = Vec2(0, 0);
			p_current_type = EventType_TextField::ON_NONE_IME;

		}
		TextFieldWidget::~TextFieldWidget()
		{
			p_current_text = "";
			p_default_place_holder = "";
			_event_callback = nullptr;
		}

		void TextFieldWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<TextFieldWidget*>(value);
			WidgetEntity::CopyValue(value);
			//
			p_color_text = val->p_color_text;
			p_color_placeHolder = val->p_color_placeHolder;

			p_touch_zone = val->p_touch_zone;
			p_text_area = val->p_text_area;

			p_use_bmp_text = val->p_use_bmp_text;
			p_current_text = val->p_current_text;
			p_default_place_holder = val->p_default_place_holder;
			_event_callback = val->_event_callback;
		}

		void TextFieldWidget::InitTexture(xml::UILayerWidgetDec * xml_value)
		{
			InitTextureTextField(xml_value);
		}

		RKString  TextFieldWidget::GetDefaultPlaceHolder()
		{
			auto place_holder = LANG_STR(p_default_place_holder);
			place_holder.ReplaceAll("\r", "");
			return place_holder;
		}

		RKString  TextFieldWidget::GetText()
		{
			return "";
		}

		void TextFieldWidget::AddEventHandler(const wEditBoxEventHandler & event)
		{
			_event_callback = event;
		}

		/////////////////////////////////////////////////////////////
		TextFieldHandleWidget::TextFieldHandleWidget()
		{
			p_typeUI = UI_TYPE::UI_TEXT_FIELD;
			p_border_width = 0;
			p_border_color = Color4B::WHITE;
			p_size_use_scale = false;

		}

		TextFieldHandleWidget::~TextFieldHandleWidget()
		{
		}

		void TextFieldHandleWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<TextFieldHandleWidget*>(value);
			TextFieldWidget::CopyValue(value);
			//
			p_border_width = val->p_border_width;
			p_border_color = val->p_border_color;
			p_size_use_scale = val->p_size_use_scale;
			p_use_origin_size = val->p_use_origin_size;
			if (p_border_width > 0)
			{
				static_cast<Text*>(p_Resource)->enableOutline(p_border_color, p_border_width);
			}

			static_cast<TextField*>(this->GetResource())->setCursorEnabled(true);
			static_cast<TextField*>(this->GetResource())->addEventListener(CC_CALLBACK_2(TextFieldHandleWidget::OnHappenEvent, this));
			static_cast<TextField*>(this->GetResource())->setSwallowTouches(true);
			this->SetTouchZone(p_touch_zone);
			this->SetTextArea(p_text_area);


			this->SetColor(val->GetColor());
		}

		WidgetEntity * TextFieldHandleWidget::Clone()
		{
			TextFieldHandleWidget * p_clone = new TextFieldHandleWidget();
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);


			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void TextFieldHandleWidget::InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);

			if (xml_value->GeneralValue->HaveDataVector2("touch_zone"))
			{
				auto value_touch_zone = xml_value->GeneralValue->GetDataVector2("touch_zone");
				value_touch_zone.x = value_touch_zone.x * GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
				value_touch_zone.y = value_touch_zone.y * GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;
				static_cast<TextField*>(GetResource())->setTouchAreaEnabled(true);
				static_cast<TextField*>(GetResource())->setTouchSize(cocos2d::Size(value_touch_zone.x, value_touch_zone.y));
				p_touch_zone = value_touch_zone;
			}

			if (xml_value->GeneralValue->HaveDataVector2("text_area"))
			{
				auto value_text_area = xml_value->GeneralValue->GetDataVector2("text_area");
				value_text_area.x = value_text_area.x * GetGameSize().width / xml_value->p_menu_widget->DesignSize.x;
				value_text_area.y = value_text_area.y * GetGameSize().height / xml_value->p_menu_widget->DesignSize.y;
				static_cast<TextField*>(GetResource())->SetTextArea(Vec2(value_text_area.x, value_text_area.y));
				p_text_area = value_text_area;
			}

			if (xml_value->GeneralValue->HaveDataInt("type_text"))
			{
				static_cast<TextField*>(GetResource())->SetTypeTextField((ui::TYPE_TEXT_FIELD)xml_value->GeneralValue->GetDataInt("type_text"));
			}
		}

		void TextFieldHandleWidget::SetSize(const cocos2d::Vec2 & s)
		{
			cocos2d::Vec2 p_Size(1, 1);

			if (HaveAligneRes())
			{
				SetSize(Vec2(
					p_aligne_res[3] >= 0 ? p_aligne_res[3] : s.x,
					p_aligne_res[4] >= 0 ? p_aligne_res[4] : s.y));
				return;
			}

			float p_size_font = (static_cast<TextField*>(p_Resource))->getFontSize();

			float add_ = 1;
			/*if (GetGameSize().width <= p_min_size_scale && p_min_size_scale > 0)
			{
				add_ *= (p_min_size_scale / GetGameSize().width);
			}*/

			p_Size.x = (s.x * add_) / p_size_font;
			p_Size.y = (s.y * add_) / p_size_font;

			if (p_size_use_scale)
			{


				GetResource()->setScaleX(p_Size.x);
				GetResource()->setScaleY(p_Size.y);
			}
			else
			{
				int size_target = (int)(s.x * add_);

				/*if (GetGameSize().width <= p_min_size_scale &&
					p_min_size_scale > 0)
				{
					size_target = static_cast<TextField*>(GetResource())->getFontSize();
				}*/

				static_cast<TextField*>(GetResource())->setFontSize(size_target);
			}


		}

		void TextFieldHandleWidget::InitTextureTextField(xml::UILayerWidgetDec * xml_value)
		{
			p_current_text = xml_value->GeneralValue->GetDataChar("source");
			p_default_place_holder = p_current_text;

			Vec2 origin_ScaleRatio = xml_value->OriginValue->GetDataVector2("scale_ratio");

			Vec2 size_origin = Vec2(origin_ScaleRatio.x * (GetGameSize().width / xml_value->p_menu_widget->DesignSize.x),
				origin_ScaleRatio.y * (GetGameSize().width / xml_value->p_menu_widget->DesignSize.x));

			int font_idx = xml_value->GeneralValue->GetDataInt("font_index");

			Label * label = TextureMgr->GetFontByIdx(font_idx);
			float size_text = XMLMgr->GetFontDecByIdx(font_idx)->d_size;


			p_use_bmp_text = (xml_value->GeneralValue->GetDataInt("bitmap_font") == 1);

			TextField* text = nullptr;

			text = TextField::create(GetDefaultPlaceHolder().GetString(), label->getTTFConfig().fontFilePath.c_str(), size_text);

			/*if (!p_use_bmp_text)
			{
				text = TextField::create(LANG_STR(p_current_text).GetString(), label->getTTFConfig().fontFilePath.c_str(), size_text);
			}
			else
			{
				int font_idx = xml_value->GeneralValue->GetDataInt("font_index");
				auto text_bmp = TextureMgr->GetFontBmpByIdx(font_idx);

				text = TextField::createBMP(LANG_STR(p_current_text).GetString(),
					File::ResolveNamePath(text_bmp->path_fnt).GetString(),
					size_text
				);
			}*/

			text->setPlaceHolderColor(xml_value->GeneralValue->GetDataColor("tint_place_holder"));
			text->setPasswordEnabled(xml_value->GeneralValue->GetDataInt("pass_input") == 1);

			p_size_use_scale = (xml_value->GeneralValue->GetDataInt("size_use_scale") == 1);

			Vec2 anchor_point = xml_value->GeneralValue->GetDataVector2("anchor_point");
			text->setAnchorPoint(anchor_point);
			text->setTextColor(xml_value->GeneralValue->GetDataColor("tint"));

			if (xml_value->GeneralValue->HaveDataInt("max_lenght"))
			{
				text->setMaxLengthEnabled(true);
				text->setMaxLength(xml_value->GeneralValue->GetDataInt("max_lenght"));
			}


			text->setCursorEnabled(true);
			text->addEventListener(CC_CALLBACK_2(TextFieldHandleWidget::OnHappenEvent, this));
			text->setSwallowTouches(true);
			p_Resource = text;
		}

		RKString  TextFieldHandleWidget::GetText()
		{
			auto currentstring = static_cast<TextField*>(p_Resource)->getString();
			auto store_text = static_cast<TextField*>(p_Resource)->getStoreText();

			return (store_text + currentstring);
		}

		void TextFieldHandleWidget::SetText(const RKString & text, bool set_native)
		{
			p_current_text = LANG_STR(text);
			static_cast<TextField*>(p_Resource)->setString(p_current_text.GetString());
		}

		void TextFieldHandleWidget::SetPlaceHolderText(const RKString & text)
		{
			p_default_place_holder = text;

			static_cast<TextField*>(p_Resource)->setPlaceHolder(GetDefaultPlaceHolder().GetString());
		}

		void TextFieldHandleWidget::ClearPlaceHolder()
		{
			static_cast<TextField*>(p_Resource)->setPlaceHolder("");
		}

		void TextFieldHandleWidget::ResetPlaceHolder()
		{
			static_cast<TextField*>(p_Resource)->setPlaceHolder(GetDefaultPlaceHolder().GetString());
		}


		void TextFieldHandleWidget::SetColor(const Color3B & p)
		{
			p_color_text = Color4B(p.r, p.g, p.b, 255);
			static_cast<TextField*>(p_Resource)->setTextColor(Color4B(p.r, p.g, p.b, 255));
		}

		Color3B  TextFieldHandleWidget::GetColor()
		{
			auto color = p_color_text;//static_cast<TextField*>(p_Resource)->getTextColor();
			return Color3B(color.r, color.g, color.b);
		}


		void TextFieldHandleWidget::ReloadText()
		{
			WidgetEntity::ReloadText();
			static_cast<TextField*>(p_Resource)->setPlaceHolder(GetDefaultPlaceHolder().GetString());
		}

		void TextFieldHandleWidget::SetInteractive(bool value)
		{
			if (p_disable)
				return;
			WidgetEntity::SetInteractive(value);
			static_cast<TextField*>(p_Resource)->setTouchEnabled(value);
		}

		void TextFieldHandleWidget::ClearText()
		{
			SetText("");
			static_cast<TextField*>(p_Resource)->SetStoreText("");
			static_cast<TextField*>(p_Resource)->setPlaceHolder(GetDefaultPlaceHolder().GetString());
			static_cast<TextField*>(p_Resource)->didNotSelectSelf();
		}

		void TextFieldHandleWidget::SetTextArea(const Vec2 & size)
		{
			if (size != Vec2(0, 0))
			{
				static_cast<TextField*>(GetResource())->SetTextArea(size);
			}
			p_text_area = size;
		}

		void TextFieldHandleWidget::SetTouchZone(const Vec2 & size)
		{
			if (size != Vec2(0, 0))
			{
				static_cast<TextField*>(GetResource())->setTouchAreaEnabled(true);
				static_cast<TextField*>(GetResource())->setTouchSize(cocos2d::Size(size.x, size.y));
			}
			p_touch_zone = size;
		}

		void TextFieldHandleWidget::OnHappenEvent(Ref * ref, ui::TextField::EventType event_type)
		{
			if (_event_callback)
			{
				if (event_type == ui::TextField::EventType::ATTACH_WITH_IME)
				{
					_event_callback(this, EventType_TextField::ON_ATTACH_WITH_IME);
					p_current_type = EventType_TextField::ON_ATTACH_WITH_IME;
				}
				else if (event_type == ui::TextField::EventType::DETACH_WITH_IME)
				{
					p_current_type = EventType_TextField::ON_DETACH_WITH_IME;
					_event_callback(this, EventType_TextField::ON_DETACH_WITH_IME);
					p_current_type = EventType_TextField::ON_NONE_IME;
				}
				else if (event_type == ui::TextField::EventType::INSERT_TEXT)
				{
					_event_callback(this, EventType_TextField::ON_INSERT_TEXT);
					p_current_type = EventType_TextField::ON_INSERT_TEXT;
				}
				else if (event_type == ui::TextField::EventType::DELETE_BACKWARD)
				{
					_event_callback(this, EventType_TextField::ON_DELETE_TEXT);
					p_current_type = EventType_TextField::ON_DELETE_TEXT;
				}
			}
		}
		void TextFieldHandleWidget::SetDettachIME()
		{
			static_cast<TextField*>(p_Resource)->setDetachWithIME(true);
		}
		void TextFieldHandleWidget::SetAttachIME()
		{
			static_cast<TextField*>(p_Resource)->attachWithIME();
		}

		//////////////////////

		/////////////////////////////////////////////////////////////
		EditBoxWidget::EditBoxWidget()
		{
			p_typeUI = UI_TYPE::UI_TEXT_FIELD;

		}
		EditBoxWidget::~EditBoxWidget()
		{

		}

		void EditBoxWidget::CopyValue(WidgetEntity * value)
		{
			auto val = static_cast<EditBoxWidget*>(value);
			TextFieldWidget::CopyValue(value);
			_event_callback = val->_event_callback;
			this->SetColor(Color3B(p_color_text.r , p_color_text.g, p_color_text.b));
			this->SetTouchZone(p_touch_zone);
			this->SetTextArea(p_text_area);
		}

		WidgetEntity * EditBoxWidget::Clone()
		{
			EditBoxWidget * p_clone = new EditBoxWidget();
			p_clone->InitParam(this->GetName(), this->p_xml_dec);
			p_clone->CopyValue(this);
			//remove child remain
			p_clone->p_Resource->removeAllChildrenWithCleanup(true);
			//copy child
			CloneChild(p_clone);
			CloneThis(p_clone);

			return p_clone;
		}

		/////////////////////////////////////////////////////////////
		void EditBoxWidget::InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)
		{
			InitTexture(xml_value);
			WidgetEntity::InitParam(name, xml_value);
		}

		void EditBoxWidget::SetSize(const cocos2d::Vec2 & s)
		{

			if (HaveAligneRes())
			{
				SetSize(Vec2(
					p_aligne_res[3] >= 0 ? p_aligne_res[3] : s.x,
					p_aligne_res[4] >= 0 ? p_aligne_res[4] : s.y));
				return;
			}


			float add_ = 1;
			/*if (GetGameSize().width <= p_min_size_scale && p_min_size_scale > 0)
			{
				add_ *= (p_min_size_scale / GetGameSize().width);
			}*/

			static_cast<EditBox*>(GetResource())->setFontSize((int)(s.x * add_));
			static_cast<EditBox*>(GetResource())->setPlaceholderFontSize((int)(s.x * add_));


		}

		void EditBoxWidget::InitTextureTextField(xml::UILayerWidgetDec * xml_value)
		{
			p_current_text = xml_value->GeneralValue->GetDataChar("source");
			p_default_place_holder = p_current_text;

			auto panel = xml_value->GeneralValue->GetDataChar("panel");

			if (RKString(panel) == "")
			{
				auto xml_parent = xml_value->p_parent_layer;
				panel = xml_parent->GeneralValue->GetDataChar("source");
				if (RKString(panel) == "")
				{
					PASSERT2(false, "wrong design");
				}
			}

			bool IsUseAtlas = (xml_value->GeneralValue->GetDataInt("use_atlas") == 1);

			Vec2 origin_ScaleRatio = xml_value->OriginValue->GetDataVector2("scale_ratio");
			float delta_scale = (GetGameSize().width / xml_value->p_menu_widget->DesignSize.x);
			Vec2 size_origin = Vec2(origin_ScaleRatio.x * delta_scale, origin_ScaleRatio.y * delta_scale);

			Vec2 touch_zone = Vec2(0, 0);

			if (xml_value->GeneralValue->HaveDataVector2("text_area"))
			{
				touch_zone = xml_value->GeneralValue->GetDataVector2("text_area");
				touch_zone.x = touch_zone.x * delta_scale;
				touch_zone.y = touch_zone.y * delta_scale;
			}
			else
			{
				if (xml_value->GeneralValue->HaveDataVector2("touch_zone"))
				{
					touch_zone = xml_value->GeneralValue->GetDataVector2("touch_zone");
					touch_zone.x = touch_zone.x * delta_scale;
					touch_zone.y = touch_zone.y * delta_scale;
				}
				else
				{
					PASSERT2(false, "edit box must have this value");
				}
			}

			p_touch_zone = p_text_area = touch_zone;

            p_use_bmp_text = (xml_value->GeneralValue->GetDataInt("bitmap_font") == 1);
            
            bool pass_input = (xml_value->GeneralValue->GetDataInt("pass_input") == 1);
            if(pass_input)
            {
                p_use_bmp_text = false;
            }

			EditBox* text = nullptr;

			if (!p_use_bmp_text)
			{
				int font_idx = xml_value->GeneralValue->GetDataInt("font_index");
				Label * label = TextureMgr->GetFontByIdx(font_idx);
				text = EditBox::create(cocos2d::Size(touch_zone.x, touch_zone.y), panel,
					IsUseAtlas ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL ,
					label ? label->getTTFConfig().fontFilePath : "" , true
				);
			}
			else
			{
				int font_idx = xml_value->GeneralValue->GetDataInt("font_index");
				auto text_bmp = TextureMgr->GetFontBmpByIdx(font_idx);
				text = EditBox::create(cocos2d::Size(touch_zone.x, touch_zone.y), panel,
					IsUseAtlas ? Widget::TextureResType::PLIST : Widget::TextureResType::LOCAL,
					File::ResolveNamePath(text_bmp->path_fnt).GetString(), false
				);
			}

			text->setPlaceHolder(GetDefaultPlaceHolder().GetString());
			text->setPlaceholderFontColor(xml_value->GeneralValue->GetDataColor("tint_place_holder"));
			text->setPlaceholderFontSize(size_origin.x);
			text->setContentSize(cocos2d::Size(touch_zone.x, touch_zone.y));
			if (pass_input)
			{
				text->setInputFlag(ui::EditBox::InputFlag::PASSWORD);
			}
			else
			{
				text->setInputFlag(ui::EditBox::InputFlag::INITIAL_CAPS_WORD);
			}

			if (xml_value->GeneralValue->HaveDataInt("max_lenght"))
			{
				text->setMaxLength(xml_value->GeneralValue->GetDataInt("max_lenght"));
			}

			Vec2 anchor_point = xml_value->GeneralValue->GetDataVector2("anchor_point");
			text->setAnchorPoint(anchor_point);
			text->setFontColor(xml_value->GeneralValue->GetDataColor("tint"));

			if (xml_value->GeneralValue->HaveDataInt("max_lenght"))
			{
				text->setMaxLength(xml_value->GeneralValue->GetDataInt("max_lenght"));
			}

			if (xml_value->GeneralValue->HaveDataInt("type_text"))
			{
				int current_type_text = xml_value->GeneralValue->GetDataInt("type_text");
				if (current_type_text == ui::TYPE_TEXT_FIELD::NUMBER_ONLY)
				{
					text->setInputMode(ui::EditBox::InputMode::PHONE_NUMBER);
				}
				else if (current_type_text == ui::TYPE_TEXT_FIELD::TEXT_AND_NUMBER)
				{
					text->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
				}
				else if (current_type_text == ui::TYPE_TEXT_FIELD::TEXT_ONLY)
				{
					text->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
				}
			}
			auto bg_sprite = text->getBackgroundSprite();
			bg_sprite->setVisible(false);

			text->setDelegate(this);
			text->setReturnType(EditBox::KeyboardReturnType::DONE);

			p_Resource = text;
		}

		RKString  EditBoxWidget::GetText()
		{
			return static_cast<EditBox*>(p_Resource)->getText();
		}

		void EditBoxWidget::SetText(const RKString & text, bool set_native)
		{
			p_current_text = text;
			if (set_native)
			{
				static_cast<EditBox*>(p_Resource)->setNativeText(LANG_STR(text).GetString());
			}
			else
			{
				static_cast<EditBox*>(p_Resource)->setText(LANG_STR(text).GetString());
			}
		}

		void EditBoxWidget::SetPlaceHolderText(const RKString & text)
		{
			p_default_place_holder = text;
			static_cast<EditBox*>(p_Resource)->setPlaceHolder(GetDefaultPlaceHolder().GetString());
		}

		void EditBoxWidget::ClearPlaceHolder()
		{
			static_cast<EditBox*>(p_Resource)->setPlaceHolder("");
		}

		void EditBoxWidget::ResetPlaceHolder()
		{
			static_cast<EditBox*>(p_Resource)->setPlaceHolder(GetDefaultPlaceHolder().GetString());
		}

		void EditBoxWidget::SetColor(const Color3B & p)
		{
			p_color_text = Color4B(p.r, p.g, p.b, 255);
			static_cast<EditBox*>(p_Resource)->setFontColor(p_color_text);
		}


		void EditBoxWidget::ReloadText()
		{
			WidgetEntity::ReloadText();
			static_cast<EditBox*>(p_Resource)->setPlaceHolder(GetDefaultPlaceHolder().GetString());
		}
        
        void EditBoxWidget::ClearText()
        {
            SetText("");
            static_cast<EditBox*>(p_Resource)->setPlaceHolder(GetDefaultPlaceHolder().GetString());
        }


		void EditBoxWidget::editBoxReturn(EditBox* editBox)
		{
			if (_event_callback)
			{
				_event_callback(this, EventType_TextField::ON_ENTER_BTN);
				p_current_type = EventType_TextField::ON_ENTER_BTN;
			}
		}

		void EditBoxWidget::editBoxEditingDidBegin(EditBox* editBox)
		{
			if (_event_callback)
			{
				_event_callback(this, EventType_TextField::ON_ATTACH_WITH_IME);
				p_current_type = EventType_TextField::ON_ATTACH_WITH_IME;
			}
		}
		void EditBoxWidget::editBoxEditingDidEnd(EditBox* editBox)
		{
			if (_event_callback)
			{
				p_current_type = EventType_TextField::ON_DETACH_WITH_IME;
				_event_callback(this, EventType_TextField::ON_DETACH_WITH_IME);
				p_current_type = EventType_TextField::ON_NONE_IME;
			}
		}
		void EditBoxWidget::editBoxTextChanged(EditBox* editBox, const std::string& text)
		{
			if (p_current_text.Length() < text.length())
			{
				if (_event_callback)
				{
					_event_callback(this, EventType_TextField::ON_INSERT_TEXT);
					p_current_type = EventType_TextField::ON_INSERT_TEXT;
				}
			}
			else if (p_current_text.Length() > text.length())
			{
				if (_event_callback)
				{
					_event_callback(this, EventType_TextField::ON_DELETE_TEXT);
					p_current_type = EventType_TextField::ON_DELETE_TEXT;
				}
			}


			p_current_text = text;

		}

		void EditBoxWidget::SetInteractive(bool value)
		{
			if (p_disable)
				return;
			WidgetEntity::SetInteractive(value);
			static_cast<EditBox*>(p_Resource)->setTouchEnabled(value);
		}

		void EditBoxWidget::SetAttachIME()
		{
			static_cast<EditBox*>(p_Resource)->setAttachIME();
		}

		void EditBoxWidget::SetDettachIME()
		{
			static_cast<EditBox*>(p_Resource)->setDettachIME();
		}

		void EditBoxWidget::SetTouchZone(const Vec2 & size)
		{
			static_cast<EditBox*>(p_Resource)->setTouchSize(cocos2d::Size(size.x, size.y));
			p_touch_zone = size;
		}

		void EditBoxWidget::SetTextArea(const Vec2 & size)
		{
			static_cast<EditBox*>(p_Resource)->setContentSize(cocos2d::Size(size.x, size.y));
			p_text_area = size;
		}
	}
}

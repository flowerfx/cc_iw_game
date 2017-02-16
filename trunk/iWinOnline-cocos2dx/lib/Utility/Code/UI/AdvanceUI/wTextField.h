#ifndef __TEXT_FIELD_WIDGET_H__
#define __TEXT_FIELD_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
        
        enum class EventType_TextField
        {
			ON_NONE_IME = -1,
            ON_INSERT_TEXT = 0,
            ON_DELETE_TEXT,
            ON_ATTACH_WITH_IME,
            ON_DETACH_WITH_IME,
			ON_ENTER_BTN
        };
        
		class TextFieldWidget : public WidgetEntity
		{
		protected:
			bool		p_use_bmp_text;
			RKString	p_current_text;
			RKString	p_default_place_holder;

			EventType_TextField p_current_type;

			Color4B		p_color_text;
			Color4B		p_color_placeHolder;

			Vec2		p_touch_zone;
			Vec2		p_text_area;
		public:
			TextFieldWidget();
			virtual ~TextFieldWidget();
            
            virtual void CopyValue(WidgetEntity * value)								override;
            virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
            
            virtual void InitTextureTextField(xml::UILayerWidgetDec * xml_value) {};
            
            virtual RKString  		GetText();
            virtual void			ClearText() {};
            virtual void			SetText(const RKString &  text , bool set_native = false) {};
            virtual RKString  		GetDefaultPlaceHolder();
			virtual void			SetPlaceHolderText(const RKString &  text) {}
			virtual void			ClearPlaceHolder() {};
			virtual void			ResetPlaceHolder() {};

            virtual void SetTextArea(const Vec2 & size) {};
            virtual void SetTouchZone(const Vec2 &  size){};

			virtual void SetAttachIME() {};
			virtual void SetDettachIME() {};
            
            typedef std::function<void(Ref*, EventType_TextField)> wEditBoxEventHandler;
            void AddEventHandler(const wEditBoxEventHandler & event);

			EventType_TextField GetCurrentType() {return p_current_type;}
        protected:
            wEditBoxEventHandler _event_callback;
		};
        
        class TextFieldHandleWidget : public TextFieldWidget
        {
        private:
            int			p_border_width;
            Color4B		p_border_color;
            
            bool		p_size_use_scale;
            int			p_use_origin_size;
        public:
            TextFieldHandleWidget();
            virtual ~TextFieldHandleWidget();
            
            virtual void SetSize(const Vec2 &  s)												override;
            virtual void InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)	override;
            virtual void InitTextureTextField(xml::UILayerWidgetDec * xml_value)		override;
            virtual void SetColor(const Color3B & p)											override;
			virtual Color3B  GetColor()													override;
            
            virtual RKString  GetText()                                                  override;
            virtual void ClearText()                                                    override;
            virtual void SetText(const RKString & text, bool set_native = false)                override;
			virtual void SetPlaceHolderText(const RKString & text)								override;
			virtual void ClearPlaceHolder()												override;
			virtual void ResetPlaceHolder()												override;

            virtual WidgetEntity * Clone()												override;
            virtual void CopyValue(WidgetEntity * value)								override;
            virtual void ReloadText()													override;
            virtual void SetInteractive(bool value)										override;
            
            virtual void SetTextArea(const Vec2 &  size)                                         override;
            virtual void SetTouchZone(const Vec2 &  size)                                        override;

			virtual void SetDettachIME()												override;
			virtual void SetAttachIME()													override;

            void OnHappenEvent(Ref * ref , ui::TextField::EventType event_type);
            
        };
        
        class EditBoxWidget : public TextFieldWidget , public cocos2d::ui::EditBoxDelegate
        {
        public:
            EditBoxWidget();
            virtual ~EditBoxWidget();
            
            virtual void SetSize(const Vec2 & s)												override;
            virtual void InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)	override;
            virtual void InitTextureTextField(xml::UILayerWidgetDec * xml_value)		override;
			virtual void SetColor(const Color3B & p)											override;

            virtual WidgetEntity * Clone()												override;
            virtual void CopyValue(WidgetEntity * value)								override;
            
            virtual RKString  GetText()													override;
            virtual void SetText(const RKString & text, bool set_native = false)				override;
			virtual void SetPlaceHolderText(const RKString & text)								override;
			virtual void ClearPlaceHolder()												override;
			virtual void ResetPlaceHolder()												override;
            virtual void ClearText()                                                    override;
			virtual void ReloadText()                                                   override;
            
            //override from cocos to handle event
            virtual void editBoxReturn(EditBox* editBox)                                override;
            virtual void editBoxEditingDidBegin(EditBox* editBox)                       override;
            virtual void editBoxEditingDidEnd(EditBox* editBox)                         override;
            virtual void editBoxTextChanged(EditBox* editBox, const std::string& text)  override;
            
            virtual void SetInteractive(bool value)										override;

			virtual void SetAttachIME() override;
			virtual void SetDettachIME() override;

			virtual void SetTouchZone(const Vec2 &  size)                                        override;
			virtual void SetTextArea(const Vec2 &  size)                                         override;
        };

	}
}

#endif //__TEXT_FIELD_WIDGET_H__


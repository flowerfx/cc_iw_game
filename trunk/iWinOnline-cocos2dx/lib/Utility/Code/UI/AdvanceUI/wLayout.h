#ifndef __LAYOUT_WIDGET_H__
#define __LAYOUT_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{

		class LayoutWidget : public WidgetEntity
		{
		public:
			LayoutWidget();
			virtual ~LayoutWidget();

			virtual void InitParam(const RKString &  name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetInitPos(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetPosValueXML(xml::UILayerWidgetDec * xml_value)				override;
			virtual void FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)	override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void SetSize(const Vec2 &  size)												override;
			virtual void CreateLayout()													;
			void InitParamLayoutValue(xml::UILayerWidgetDec * xml_value);
		};

	}
}

#endif //__LAYOUT_WIDGET_H__


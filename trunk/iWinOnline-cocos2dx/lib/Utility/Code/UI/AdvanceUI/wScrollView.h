#ifndef __SCROLL_WIDGET_H__
#define __SCROLL_WIDGET_H__

#include "wLayout.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class ScrollViewWidget : public LayoutWidget
		{

		public:
			ScrollViewWidget();
			virtual ~ScrollViewWidget();

			virtual void InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetSizeValueXML(xml::UILayerWidgetDec * xml_value)				override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void SetInteractive(bool value)										override;

			virtual void CreateLayout()													override;

			void SetCallBackScroll(const ui::ScrollView::ccScrollViewCallback& callback);

			void SetInnerSize(const Vec2 & size);
		};

	}
}

#endif //__SCROLL_WIDGET_H__


#ifndef __PANEL_WIDGET_H__
#define __PANEL_WIDGET_H__

#include "WidgetEntity.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class PanelWidget : public WidgetEntity
		{
		private:
			

		public:
			PanelWidget();
			virtual ~PanelWidget();

			virtual void InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;

			void SetImage(const RKString & name_path, bool is_atlas);
		};

	}
}
#endif //__PANEL_WIDGET_H__


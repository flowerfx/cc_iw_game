#ifndef __PAGE_WIDGET_H__
#define __PAGE_WIDGET_H__

#include "wListView.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class LayoutWidget;
		class PageViewWidget : public ListViewWidget
		{

		public:
			PageViewWidget();
			virtual ~PageViewWidget();
			virtual void CreateLayout()													override;
			virtual void InitParam(const RKString & name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetSizeValueXML(xml::UILayerWidgetDec * xml_value)				override;
			virtual void FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)	override;
			virtual WidgetEntity * Clone()												override;
			virtual void CopyValue(WidgetEntity * value)								override;
			virtual void SetInteractive(bool value)										override;

			virtual WidgetEntity * CreateWidgetRefer(int idx, int refer_idx = 0)		override;
			virtual void PushBackItem(WidgetEntity * item)								override;

			int GetCurrentPageIdx();
			void JumpToPageIdx(int idx);
			void ScrollToPageIdx(int idx);
		};

	}
}

#endif //__SCROLL_WIDGET_H__


#ifndef __LIST_WIDGET_H__
#define __LIST_WIDGET_H__

#include "wScrollView.h"
#include "RKString_Code/RKString.h"
using namespace RKUtils;
namespace Utility
{
	namespace UI_Widget
	{
		class LayoutWidget;
		class ListViewWidget : public ScrollViewWidget
		{
		private:
			int current_idx_selected;
		protected:
			std::vector<LayoutWidget *> p_refer_child;
		public:
			ListViewWidget();
			virtual ~ListViewWidget();
			virtual void CreateLayout()													override;
			virtual void InitParam(const RKString &  name, xml::UILayerWidgetDec * xml_value)	override;
			virtual void InitTexture(xml::UILayerWidgetDec * xml_value)					override;
			virtual void SetSizeValueXML(xml::UILayerWidgetDec * xml_value)				override;
			virtual void FindAndInsertChildWidget(xml::UILayerWidgetDec * xml_value)	override;
			virtual void InsertChildWidget(WidgetEntity* child_widget)					override;

			virtual WidgetEntity * CreateWidgetRefer(int idx , int refer_idx = 0);
			WidgetEntity * CreateWidgetReferNoPushBack(int refer_idx = 0);
			virtual void PushBackItem(WidgetEntity * item);

			virtual void CopyValue(WidgetEntity * value)								override;
			virtual WidgetEntity * Clone()												override;
			virtual void ClearChild(int count = 0)										override;
			virtual void SetInteractive(bool value)										override;

			int GetCurrentIdxSelected();
			void SetCurrentIdxSelected(int idx);
			void JumpToItemIdx(int idx);
			void ResetListView();
			void ForceVisibleAllChildLayout(const RKString & path, bool visible);
			void SetNumberEachSlide(int number);
			void SetCallBackList(const ui::ListView::ccListViewCallback& callback);
		};

	}
}

#endif //__LIST_WIDGET_H__


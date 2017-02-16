#ifndef __TRANSACTION_HISTORY_LAYER_H__
#define __TRANSACTION_HISTORY_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;
namespace iwinmesage
{
	class TransactionHistory;
	class TransactionHistoryDetail;
}
class TransactionHistoryLayer : public CommonLayer
{
public:
	TransactionHistoryLayer();
	virtual ~TransactionHistoryLayer();
private:
	WidgetEntity * p_page_trans_history;

	// 0 : choi win , 1 : nap win, 2 : choi ruby , 3 : nap ruby
	int			  p_state_transaction_history;
	iwinmesage::TransactionHistory * p_transaction_history;

	int			   p_current_page_history;
	WidgetEntity * p_layout_tab_btn_trans_history;
	WidgetEntity * p_layout_view_history;
	WidgetEntity * p_list_view_history;
	WidgetEntity * p_panel_history_detail;
protected:
	void OnChangeStateTransactionHistory();
	void OnShowLoadingListTransHistory();
	void OnParseHistoryDetail(WidgetEntity* widget, iwinmesage::TransactionHistoryDetail * history_detail, int idx);
public:

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual int  Update(float dt) override;

	void OnRecieveTransactionHistory(void * trans_history);
	void ResetStateHistory();

	bool isDetailHisPanelVisible();
	void CloseDetailHisPanel();
};

#endif //__TRANSACTION_HISTORY_LAYER_H__


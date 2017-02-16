#ifndef __TRANSFER_MONEY_LAYOUT_H__
#define __TRANSFER_MONEY_LAYOUT_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;

class TransferMoneyLayout : public CommonLayer
{
public:
	TransferMoneyLayout();
	virtual ~TransferMoneyLayout();
private:
	//is at state ruby or win
	//0 is win 1 is ruby
	int			   p_state_tranfer_money;
	WidgetEntity * p_layout_send_gift;
	WidgetEntity * p_textfield_gift_account;
	WidgetEntity * p_textfield_gift_number_win;
	WidgetEntity * p_textfield_gift_number_ruby;

	WidgetEntity * p_layout_sum_win;
	WidgetEntity * p_layout_sum_ruby;
	/*transfer money :
	*/
	int *		   p_transfer_info_list;
protected:

public:

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual int  Update(float dt) override;

	void OnReceiveTransferInfo(int * arg_list);
};

#endif //__TRANSFER_MONEY_LAYOUT_H__


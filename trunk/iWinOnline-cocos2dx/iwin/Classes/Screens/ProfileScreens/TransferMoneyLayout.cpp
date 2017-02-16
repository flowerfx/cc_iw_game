#include "TransferMoneyLayout.h"
#include "Network/Global/Globalservice.h"

#include "UI/AdvanceUI/wText.h"
TransferMoneyLayout::TransferMoneyLayout()
{
	p_layout_send_gift = nullptr;
	p_textfield_gift_account = nullptr;
	p_textfield_gift_number_win = nullptr;
	p_textfield_gift_number_ruby = nullptr;
	p_layout_sum_win = nullptr;
	p_layout_sum_ruby = nullptr;
	p_state_tranfer_money = 0;

	p_transfer_info_list = nullptr;
}

TransferMoneyLayout::~TransferMoneyLayout()
{

	SAFE_DELETE_ARRAY(p_transfer_info_list);
}

void TransferMoneyLayout::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);

	p_layout_send_gift = _base_screen->GetWidgetChildByName("profile_screen.gift_win_layout.gift_win");
	p_textfield_gift_account = p_layout_send_gift->GetWidgetChildByName(".gift_win_panel.layout_account.panel_input.send_account_input");
	p_textfield_gift_number_win = p_layout_send_gift->GetWidgetChildByName(".gift_win_panel.layout_number_win.panel_input.number_of_win_input");
	p_textfield_gift_number_ruby = p_layout_send_gift->GetWidgetChildByName(".gift_win_panel.layout_number_ruby.panel_input.number_of_ruby_input");
	p_layout_sum_win = p_layout_send_gift->GetWidgetChildByName(".gift_win_panel.layout_sum_win");
	p_layout_sum_ruby = p_layout_send_gift->GetWidgetChildByName(".gift_win_panel.layout_sum_ruby");


	auto _widget = p_textfield_gift_account;
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildAtIdx(1));
	w_tx->InitCallBack(
		nullptr);

	_widget = p_textfield_gift_number_win;
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildAtIdx(1));
	w_tx->InitCallBack(
		nullptr,
		nullptr,
		[this, _widget]()
	{
		RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		s64 current_number_win = Utils::GetNumberFromFormat(string_input.GetString());
		if (current_number_win > p_transfer_info_list[4])
		{
			current_number_win = p_transfer_info_list[4];
		}
		/*else if (current_number_win < p_transfer_info_list[2])
		{
		current_number_win = p_transfer_info_list[2];
		}*/

		RKString string_result = Utils::formatNumber_dot(current_number_win);
		static_cast<TextFieldWidget*>(_widget)->SetText(string_result,true);

		int tax = current_number_win * p_transfer_info_list[0] / 100.f;
		int total_money = current_number_win + tax;
		static_cast<TextWidget*>(p_layout_sum_win->GetWidgetChildByName(".title_tranfer_tax"))->SetText(Utils::formatMoney_dot_win(tax), true);
		static_cast<TextWidget*>(p_layout_sum_win->GetWidgetChildByName(".title_money_div"))->SetText(Utils::formatMoney_dot_win(total_money), true);

	},
		[this, _widget]()
	{
		RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		s64 current_number_win = Utils::GetNumberFromFormat(string_input.GetString());
		if (current_number_win > p_transfer_info_list[4])
		{
			current_number_win = p_transfer_info_list[4];
		}

		RKString string_result = Utils::formatNumber_dot(current_number_win);
		static_cast<TextFieldWidget*>(_widget)->SetText(string_result, true);

		int tax = current_number_win * p_transfer_info_list[0] / 100.f;
		int total_money = current_number_win + tax;
		static_cast<TextWidget*>(p_layout_sum_win->GetWidgetChildByName(".title_tranfer_tax"))->SetText(Utils::formatMoney_dot_win(tax), true);
		static_cast<TextWidget*>(p_layout_sum_win->GetWidgetChildByName(".title_money_div"))->SetText(Utils::formatMoney_dot_win(total_money), true);

	}, nullptr);

	_widget = p_textfield_gift_number_ruby;
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildAtIdx(1));
	w_tx->InitCallBack(
		nullptr,
		nullptr,
		[this, _widget]()
	{
		RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		s64 current_number_ruby = Utils::GetNumberFromFormat(string_input.GetString());
		if (current_number_ruby > p_transfer_info_list[5])
		{
			current_number_ruby = p_transfer_info_list[5];
		}
		/*else if (current_number_ruby < p_transfer_info_list[3])
		{
		current_number_ruby = p_transfer_info_list[3];
		}*/

		RKString string_result = Utils::formatNumber_dot(current_number_ruby);
		static_cast<TextFieldWidget*>(_widget)->SetText(string_result , true);
		int tax = current_number_ruby * p_transfer_info_list[1] / 100.f;
		int total_money = current_number_ruby + tax;
		static_cast<TextWidget*>(p_layout_sum_ruby->GetWidgetChildByName(".title_tranfer_tax"))->SetText(Utils::formatMoney_dot_ruby(tax), true);
		static_cast<TextWidget*>(p_layout_sum_ruby->GetWidgetChildByName(".title_money_div"))->SetText(Utils::formatMoney_dot_ruby(total_money), true);

	},
		[this, _widget]()
	{
		RKString string_input = static_cast<TextFieldWidget*>(_widget)->GetText();
		s64 current_number_ruby = Utils::GetNumberFromFormat(string_input.GetString());
		if (current_number_ruby > p_transfer_info_list[5])
		{
			current_number_ruby = p_transfer_info_list[5];
		}

		RKString string_result = Utils::formatNumber_dot(current_number_ruby);
		static_cast<TextFieldWidget*>(_widget)->SetText(string_result, true);

		int tax = current_number_ruby * p_transfer_info_list[1] / 100.f;
		int total_money = current_number_ruby + tax;
		static_cast<TextWidget*>(p_layout_sum_ruby->GetWidgetChildByName(".title_tranfer_tax"))->SetText(Utils::formatMoney_dot_ruby(tax), true);
		static_cast<TextWidget*>(p_layout_sum_ruby->GetWidgetChildByName(".title_money_div"))->SetText(Utils::formatMoney_dot_ruby(total_money), true);

	}, nullptr);
}

bool TransferMoneyLayout::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_close_panel_gift")
		{
			OnHideLayer();
			return true;
		}
		else if (name == "btn_gift_win")
		{
			RKString text_name = static_cast<TextFieldWidget*>(p_textfield_gift_account)->GetText();
			//
			RKString text_number = "0";
			if (p_state_tranfer_money == 0)
			{
				text_number = static_cast<TextFieldWidget*>(p_textfield_gift_number_win)->GetText() + " Win";
			}
			else
			{
				text_number = static_cast<TextFieldWidget*>(p_textfield_gift_number_ruby)->GetText() + " Ruby";
			}
			if (text_name == "" || text_number == "")
				return false;

			u64 number_money = Utils::GetNumberFromFormat(text_number.GetString());

			int minmoney = 0;
			int maxmoney = 0;
			int tax = 0;
			s64 current_money = 0;
			if (p_state_tranfer_money == 0)
			{
				current_money = GameController::myInfo->money;
				tax = p_transfer_info_list[0];
				minmoney = p_transfer_info_list[2];
				maxmoney = p_transfer_info_list[4];
			}
			else
			{
				current_money = GameController::myInfo->ruby;
				tax = p_transfer_info_list[1];
				minmoney = p_transfer_info_list[3];
				maxmoney = p_transfer_info_list[5];
			}

			if (number_money < minmoney || number_money > maxmoney)
				return false;

			RKString text_arg = LangMgr->GetString("do_you_agree_transfe");

			text_arg.ReplaceFirst("%s", text_number);
			text_arg.ReplaceFirst("%s", text_name);

			ScrMgr->OnShowDialog("send_win", text_arg, DIALOG_TWO_BUTTON, "",
				[this, number_money, text_name, current_money, tax](const char * str, const char * name_btn)
			{
				if (number_money*(1.f + (tax / 100.f)) > current_money)
				{
					ScrMgr->OnShowErrorDialog("invalid_money");
				}
				else
				{
					GlobalService::transferMoneyWithType(text_name.GetString(), number_money, this->p_state_tranfer_money + 1);
				}
			});

			return false;
		}

	}
	else if (type_widget == UI_TYPE::UI_RADIO_BTN)
	{
		if (name.Contains("ratio_layer"))
		{
			RKString Str_idx_select = name.SplitLast("_")[1];
			p_state_tranfer_money = atoi(Str_idx_select.GetString());
			if (p_state_tranfer_money == 0)
			{
				p_textfield_gift_number_win->GetParentWidget()->GetParentWidget()->Visible(true);
				p_textfield_gift_number_ruby->GetParentWidget()->GetParentWidget()->Visible(false);
				p_layout_sum_win->Visible(true);
				p_layout_sum_ruby->Visible(false);

				RKString title = LANG_STR("transfer_win_txt");
				static_cast<TextWidget*>(p_layout_send_gift->GetWidgetChildByName(".gift_win_panel.title"))->SetText(title, true);
			}
			else
			{
				p_textfield_gift_number_win->GetParentWidget()->GetParentWidget()->Visible(false);
				p_textfield_gift_number_ruby->GetParentWidget()->GetParentWidget()->Visible(true);
				p_layout_sum_win->Visible(false);
				p_layout_sum_ruby->Visible(true);

				RKString title = LANG_STR("transfer_ruby_txt");
				static_cast<TextWidget*>(p_layout_send_gift->GetWidgetChildByName(".gift_win_panel.title"))->SetText(title, true);
			}

			return true;
		}
	}
	
	return false;
}

void TransferMoneyLayout::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_layout_send_gift->GetParentWidget()->Visible(true);
	p_layout_send_gift->SetActionCommandWithName("ON_SHOW");

	GlobalService::getInfoTransferMoney();
}

void TransferMoneyLayout::OnReceiveTransferInfo(int * arg_list)
{
	SAFE_DELETE_ARRAY(p_transfer_info_list);

	p_transfer_info_list = arg_list;
	int taxWin = p_transfer_info_list[0];
	int taxRuby = p_transfer_info_list[1];
	int minWin = p_transfer_info_list[2];
	int minRuby = p_transfer_info_list[3];
	int maxWin = p_transfer_info_list[4];
	int maxRuby = p_transfer_info_list[5];

	//set info win and ruby
	RKString number_win_txt = StringUtils::format(LANG_STR("from_t_to_t").GetString(), Utils::formatMoney_dot_win(minWin).c_str(), Utils::formatMoney_dot_win(maxWin).c_str());
	RKString number_ruby_txt = StringUtils::format(LANG_STR("from_t_to_t").GetString(), Utils::formatMoney_dot_ruby(minRuby).c_str(), Utils::formatMoney_dot_ruby(maxRuby).c_str());

	static_cast<TextFieldWidget*>(p_textfield_gift_number_win)->SetPlaceHolderText(number_win_txt);
	static_cast<TextFieldWidget*>(p_textfield_gift_number_ruby)->SetPlaceHolderText(number_ruby_txt);
}

void TransferMoneyLayout::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_layout_send_gift->SetActionCommandWithName("ON_HIDE", CallFunc::create(
		[this , call_back]()
	{
		this->p_layout_send_gift->ForceFinishAction();
		this->p_layout_send_gift->GetParentWidget()->Visible(false);
		static_cast<TextFieldWidget*>(p_textfield_gift_number_ruby)->ClearText();
		static_cast<TextFieldWidget*>(p_textfield_gift_number_win)->ClearText();
		static_cast<TextFieldWidget*>(p_textfield_gift_account)->ClearText();

		static_cast<TextWidget*>(p_layout_sum_ruby->GetWidgetChildByName(".title_tranfer_tax"))->SetText("0", true);
		static_cast<TextWidget*>(p_layout_sum_ruby->GetWidgetChildByName(".title_money_div"))->SetText("0", true);

		static_cast<TextWidget*>(p_layout_sum_win->GetWidgetChildByName(".title_tranfer_tax"))->SetText("0", true);
		static_cast<TextWidget*>(p_layout_sum_win->GetWidgetChildByName(".title_money_div"))->SetText("0", true);

		if (call_back)
		{
			call_back();
		}
	}));
}
bool TransferMoneyLayout::IsVisibleLayer()
{
	return this->p_layout_send_gift->GetParentWidget()->Visible();
}
int TransferMoneyLayout::Update(float dt)
{
	return 1;
}



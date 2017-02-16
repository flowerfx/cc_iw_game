#include "TransactionHistoryLayer.h"
#include "Network/Global/Globalservice.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wCheckBox.h"
#include "Network/JsonObject/TransactionHistory/TransactionHistoryDetail.h"
#include "Network/JsonObject/TransactionHistory/TransactionHistory.h"
TransactionHistoryLayer::TransactionHistoryLayer()
{
	p_transaction_history = nullptr;
	p_state_transaction_history = 0;
	p_current_page_history = 0;
	p_list_view_history = nullptr;
}

TransactionHistoryLayer::~TransactionHistoryLayer()
{
	SAFE_DELETE(p_transaction_history);
}

void TransactionHistoryLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	auto p_page_view_profile = screen->GetWidgetChildByName("profile_screen.main_profile_layout");
	p_page_trans_history = p_page_view_profile->GetWidgetChildByName(".page_profile_.page_trans_history");

	p_layout_tab_btn_trans_history = p_page_trans_history->GetWidgetChildByName(".bg_list_btn");
	p_list_view_history = p_page_trans_history->GetWidgetChildByName(".layout_history_transaction.list_detail_history");
	p_panel_history_detail = p_page_trans_history->GetWidgetChildByName(".layout_detail_history.layout_detail_history");

	auto _list_view_history = static_cast<IwinListViewTable*>(p_list_view_history);
	_list_view_history->InitWithParam(
		nullptr,
		p_list_view_history->GetParentWidget()->GetWidgetChildAtIdx(5),
		[this, _list_view_history](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) {
		int current_idx_to = _list_view_history->GetCurrentIdxTo();
		if ((size_t)current_idx_to >= this->p_transaction_history->getDetails().size())
		{
			PASSERT2(false, "problem here !");
			current_idx_to = this->p_transaction_history->getDetails().size() - 1;
		}
		this->OnParseHistoryDetail(_widget, this->p_transaction_history->getDetails()[current_idx_to], current_idx_to);
	},
		[this, _list_view_history](Ref* ref, EventType_SetCell type, WidgetEntity* _widget) {
		size_t current_idx = _list_view_history->GetCurrentIdxSelected();
		if (current_idx >= 0 && current_idx < this->p_transaction_history->getDetails().size())
		{
			auto detail_history = this->p_transaction_history->getDetails()[current_idx];
			static_cast<TextWidget*>(this->p_panel_history_detail->GetWidgetChildByName(".title_notice"))->SetText(StringUtils::format(LANG_STR("detail").GetString(), std::to_string(detail_history->getId()).c_str()), true);

			static_cast<TextWidget*>(this->p_panel_history_detail->GetWidgetChildByName(".txt_11"))->SetText(detail_history->getService(), true);
			static_cast<TextWidget*>(this->p_panel_history_detail->GetWidgetChildByName(".txt_21"))->SetText(Utils::formatNumber_dot(detail_history->getValue()), true);
			static_cast<TextWidget*>(this->p_panel_history_detail->GetWidgetChildByName(".txt_31"))->SetText(Utils::formatNumber_dot(detail_history->getMoney()), true);
			static_cast<TextWidget*>(this->p_panel_history_detail->GetWidgetChildByName(".scroll_content.notice_content"))->SetText(detail_history->getDesc(), true);
			this->p_panel_history_detail->GetParentWidget()->Visible(true);
			this->p_panel_history_detail->SetActionCommandWithName("ON_SHOW");
		}
		else
		{
			PASSERT2(false, "have problem here!");
		}
	},
		nullptr,
		[this, _list_view_history](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		GlobalService::sendGetTransactionHistory(p_state_transaction_history, p_current_page_history++);
		OnShowLoadingListTransHistory();

	});
}

bool TransactionHistoryLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_close_history_layout")
		{
			this->p_panel_history_detail->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
				this->p_panel_history_detail->ForceFinishAction();
				this->p_panel_history_detail->GetParentWidget()->Visible(false);
			}));
			return true;
		}

	}
	else if (type_widget == UI_TYPE::UI_CHECK_BOX)
	{
		auto state_list = name.SplitLast("_");
		RKString name_check = state_list[0];
		RKString state = state_list[1];
		if (name_check == "btn_check_play_win")
		{
			if (state == "SELECTED" && p_state_transaction_history != 0)
			{
				p_state_transaction_history = 0;
				OnChangeStateTransactionHistory();
			}
			else if (state == "UNSELECTED" && p_state_transaction_history == 0)
			{
				static_cast<CheckBoxWidget*>(_widget)->OnSetSelected(true);
			}
			return true;
		}
		else if (name_check == "btn_check_pur_win")
		{
			if (state == "SELECTED" && p_state_transaction_history != 1)
			{
				p_state_transaction_history = 1;
				OnChangeStateTransactionHistory();
			}
			else if (state == "UNSELECTED" && p_state_transaction_history == 1)
			{
				static_cast<CheckBoxWidget*>(_widget)->OnSetSelected(true);
			}
			return true;
		}
		else if (name_check == "btn_check_play_ruby")
		{
			if (state == "SELECTED" && p_state_transaction_history != 2)
			{
				p_state_transaction_history = 2;
				OnChangeStateTransactionHistory();
			}
			else if (state == "UNSELECTED" && p_state_transaction_history == 2)
			{
				static_cast<CheckBoxWidget*>(_widget)->OnSetSelected(true);
			}
			return true;
		}
		else if (name_check == "btn_check_pur_ruby")
		{
			if (state == "SELECTED" && p_state_transaction_history != 3)
			{
				p_state_transaction_history = 3;
				OnChangeStateTransactionHistory();
			}
			else if (state == "UNSELECTED" && p_state_transaction_history == 3)
			{
				static_cast<CheckBoxWidget*>(_widget)->OnSetSelected(true);
			}
			return true;
		}
	}
	return false;
}

void TransactionHistoryLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_page_trans_history->SetActionCommandWithName("SET_POS", CallFunc::create(
		[this, call_back]() {
		this->p_page_trans_history->ForceFinishAction();
		this->p_page_trans_history->Visible(true);
		OnChangeStateTransactionHistory();
		if (call_back)
		{
			call_back();
		}
	}));
}

void TransactionHistoryLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_page_trans_history->Visible(false);
}
bool TransactionHistoryLayer::IsVisibleLayer()
{
	return p_page_trans_history->Visible();
}
int TransactionHistoryLayer::Update(float dt)
{
	static_cast<IwinListViewTable*>(p_list_view_history)->UpdateReload(dt);

	return 1;
}

void TransactionHistoryLayer::OnChangeStateTransactionHistory()
{
	for (int i = 0; i < p_layout_tab_btn_trans_history->GetNumberChildWidget(); i++) {
		static_cast<CheckBoxWidget*>(p_layout_tab_btn_trans_history->GetWidgetChildAtIdx(i))->OnSetSelected(false);
	}
	p_current_page_history = 0;
	static_cast<CheckBoxWidget*>(p_layout_tab_btn_trans_history->GetWidgetChildAtIdx(p_state_transaction_history))->OnSetSelected(true);
	SAFE_DELETE(p_transaction_history);
	GlobalService::sendGetTransactionHistory(p_state_transaction_history, p_current_page_history++);
	OnShowLoadingListTransHistory();
}

void TransactionHistoryLayer::OnShowLoadingListTransHistory()
{
	p_list_view_history->Visible(false);
	p_list_view_history->GetParentWidget()->GetWidgetChildByName(".txt_have_data")->Visible(false);
	p_list_view_history->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(true);
	p_list_view_history->GetParentWidget()->GetWidgetChildByName(".loading_circle")->SetActionCommandWithName("ROTATE");
}

void TransactionHistoryLayer::OnRecieveTransactionHistory(void * trans_history)
{
	auto list_tran_history = (TransactionHistory*)trans_history;
	if (!p_transaction_history)
	{
		p_transaction_history = list_tran_history;
	}
	else
	{
		for (auto l : list_tran_history->getDetails())
		{
			p_transaction_history->getDetails().push_back(l);
		}
	}
	if (list_tran_history->getDetails().size() <= 0)
	{
		p_current_page_history = 0;
	}

	static_cast<IwinListViewTable*>(p_list_view_history)->SetHaveReload([this]() {
		p_list_view_history->Visible(true);
		p_list_view_history->GetParentWidget()->GetWidgetChildByName(".loading_circle")->Visible(false);
		if (!p_transaction_history)
		{
			p_list_view_history->GetParentWidget()->GetWidgetChildByName(".txt_have_data")->Visible(false);
			static_cast<ListViewTableWidget*>(p_list_view_history)->SetNumberSizeOfCell(0);
		}
		else
		{
			if (p_transaction_history->getDetails().size() > 0)
			{
				p_list_view_history->GetParentWidget()->GetWidgetChildByName(".txt_have_data")->Visible(false);
			}
			else
			{
				p_list_view_history->GetParentWidget()->GetWidgetChildByName(".txt_have_data")->Visible(true);
			}

			static_cast<ListViewTableWidget*>(p_list_view_history)->SetNumberSizeOfCell(p_transaction_history->getDetails().size(), true);
		}
	});
}

void TransactionHistoryLayer::OnParseHistoryDetail(WidgetEntity* widget, iwinmesage::TransactionHistoryDetail * history_detail, int idx)
{
	if (!history_detail || !widget)
		return;
	static_cast<TextWidget*>(widget->GetWidgetChildAtIdx(1))->SetText(std::to_string(history_detail->getId()), true);
	static_cast<TextWidget*>(widget->GetWidgetChildAtIdx(2))->SetText(history_detail->getTime(), true);
	static_cast<TextWidget*>(widget->GetWidgetChildAtIdx(3))->SetText(history_detail->getService(), true);
	static_cast<TextWidget*>(widget->GetWidgetChildAtIdx(4))->SetText(Utils::formatNumber_dot(history_detail->getValue()), true);
	static_cast<TextWidget*>(widget->GetWidgetChildAtIdx(5))->SetText(Utils::formatNumber_dot(history_detail->getMoney()), true);
	static_cast<TextWidget*>(widget->GetWidgetChildAtIdx(6))->SetText(history_detail->getDesc(), true);

	auto text_widget = static_cast<TextWidget*>(widget->GetWidgetChildAtIdx(6));
	if (text_widget->GetNumberOfLine() > 1)
	{
		auto text = text_widget->GetStringAtLine(1);
		text.replace(text.size() - 3, 3, "...");
		text_widget->SetText(text, true);
	}

	if (idx % 2 == 0)
	{
		widget->GetWidgetChildAtIdx(0)->Visible(false);
	}
	else
	{
		widget->GetWidgetChildAtIdx(0)->Visible(true);
	}
}

void TransactionHistoryLayer::ResetStateHistory()
{
	p_state_transaction_history = 0;
	OnChangeStateTransactionHistory();
}

bool TransactionHistoryLayer::isDetailHisPanelVisible()
{
	return p_panel_history_detail->GetParentWidget()->Visible();
}
void TransactionHistoryLayer::CloseDetailHisPanel()
{
	this->p_panel_history_detail->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() {
		this->p_panel_history_detail->ForceFinishAction();
		this->p_panel_history_detail->GetParentWidget()->Visible(false);
	}));
}

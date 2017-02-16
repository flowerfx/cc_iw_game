#include "MainUserProfileLayer.h"
#include "UI/AdvanceUI/wText.h"

#include "Network/Global/Globalservice.h"
#include "RKThread_Code/RKThreadWrapper.h"

char * list_sx[3] = { "male" , "female" ,"unknown" };
MainUserProfileLayer::MainUserProfileLayer()
{
	p_thread_check_time = nullptr;

	p_user_dirty = false;
	p_is_create_list_choose = false;

	p_cur_day = 1;
	p_cur_month = 1;
	p_cur_year = 1917;
}

MainUserProfileLayer::~MainUserProfileLayer()
{
	SAFE_DELETE(p_thread_check_time);

	p_user_dirty = false;
	p_is_create_list_choose = false;
}

void MainUserProfileLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	auto p_page_view_profile = screen->GetWidgetChildByName("profile_screen.main_profile_layout");

	p_page_profile_name = p_page_view_profile->GetWidgetChildByName(".page_profile_.page_profile_name");

	p_list_choose_sx = p_page_profile_name->GetWidgetChildByName(".title_input_sx.list_choose_sx");
	p_list_choose_day = p_page_profile_name->GetWidgetChildByName(".title_input_birth.list_choose_day");
	p_list_choose_month = p_page_profile_name->GetWidgetChildByName(".title_input_birth.list_choose_month");
	p_list_choose_year = p_page_profile_name->GetWidgetChildByName(".title_input_birth.list_choose_year");

	InitTextFieldValue();
}

void MainUserProfileLayer::InitTextFieldValue()
{
	auto _widget = p_page_profile_name->GetWidgetChildByName(".title_2.panel_input.text_input_1");
	auto w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_1"));
	w_tx->InitCallBack(
		nullptr,
		[this, _widget]()
	{
		UpdatePlayerName_ToServer(static_cast<TextFieldWidget*>(_widget)->GetText());
	});

	_widget = p_page_profile_name->GetWidgetChildByName(".title_5.panel_input.text_input_phone");
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_phone"));
	w_tx->InitCallBack(
		nullptr,
		[this, _widget]()
	{
		UpdatePhoneNumber_ToServer(static_cast<TextFieldWidget*>(_widget)->GetText());
	});

	_widget = p_page_profile_name->GetWidgetChildByName(".title_6.panel_input.text_input_address");
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_address"));
	w_tx->InitCallBack(
		nullptr,
		[this, _widget]()
	{
		UpdateAddress_ToServer(static_cast<TextFieldWidget*>(_widget)->GetText());
	});

	_widget = p_page_profile_name->GetWidgetChildByName(".title_7.panel_input.text_input_cmnd");
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_cmnd"));
	w_tx->InitCallBack(
		nullptr,
		[this, _widget]()
	{
		UpdateCMNDNumber_ToServer(atoi(static_cast<TextFieldWidget*>(_widget)->GetText().GetString()));
	});


	_widget = p_page_profile_name->GetWidgetChildByName(".title_8.panel_input.text_input_email");
	w_tx = static_cast<IWIN_TF*>(_widget);
	w_tx->SetBtnClear(_widget->GetParentWidget()->GetWidgetChildByName(".clear_text_input_email"));
	w_tx->InitCallBack(
		nullptr,
		[this, _widget]()
	{
		UpdateEmail_ToServer(static_cast<TextFieldWidget*>(_widget)->GetText());
	});


}

bool MainUserProfileLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_input_sx")
		{
			OnShowOrHideListChoose(p_list_choose_sx, true);
			return true;
		}
		else if (name == "btn_input_day")
		{
			OnShowOrHideListChoose(p_list_choose_day, true);
			return true;
		}
		else if (name == "btn_input_month")
		{
			OnShowOrHideListChoose(p_list_choose_month, true);
			return true;
		}
		else if (name == "btn_input_year")
		{
			OnShowOrHideListChoose(p_list_choose_year, true);
			return true;
		}

		else if (name == "confirm_email")
		{
			auto wi = _widget->GetParentWidget()->GetWidgetChildByName(".panel_input.text_input_email");
			RKString current_email = static_cast<TextFieldWidget*>(wi)->GetText();
			if (current_email != "")
			{
				GlobalService::requestVerifyEmail(current_email.GetString());
			}
			else
			{
				ScrMgr->OnShowErrorDialog("not_empty_mail");
			}
			return true;
		}
		else if (name == "confirm_phone")
		{
			auto wi = _widget->GetParentWidget()->GetWidgetChildByName(".panel_input.text_input_phone");
			RKString current_phone = static_cast<TextFieldWidget*>(wi)->GetText();
			if (current_phone != "")
			{
				GlobalService::requestVerifyPhone(current_phone.GetString());
			}
			else
			{
				ScrMgr->OnShowErrorDialog("not_empty_phone");
			}
			return true;
		}


	}
	return false;
}

void MainUserProfileLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_page_profile_name->SetActionCommandWithName("SET_POS", CallFunc::create(
		[this, call_back]() {
		this->p_page_profile_name->ForceFinishAction();
		if (call_back)
		{
			call_back();
		}
	}));
}

void MainUserProfileLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_page_profile_name->Visible(false);
}
bool MainUserProfileLayer::IsVisibleLayer()
{
	return p_page_profile_name->Visible();
}
int MainUserProfileLayer::Update(float dt)
{

	if (p_thread_check_time && !p_thread_check_time->IsThreadRunning())
	{
		auto day = p_list_choose_day->GetParentWidget()->GetWidgetChildByName(".btn_input_day.text_input");
		auto mon = p_list_choose_month->GetParentWidget()->GetWidgetChildByName(".btn_input_month.text_input");
		auto year = p_list_choose_year->GetParentWidget()->GetWidgetChildByName(".btn_input_year.text_input");
		//parse data after recorrect time
		UpdateBirthDay_ToServer(p_cur_day, p_cur_month, p_cur_year);
		//
		static_cast<TextWidget*>(day)->SetText(std::to_string(p_cur_day), true);
		static_cast<TextWidget*>(mon)->SetText(std::to_string(p_cur_month), true);
		static_cast<TextWidget*>(year)->SetText(std::to_string(p_cur_year), true);

		//after thread check finish delete it away;
		delete p_thread_check_time;
		p_thread_check_time = nullptr;
	}

	return 1;
}

void MainUserProfileLayer::SetAccountName(const RKString & name)
{
	auto wi_1 = p_page_profile_name->GetWidgetChildByName(".title_1.title_detail");
	static_cast<TextWidget*>(wi_1)->SetText(name);
}

void MainUserProfileLayer::SetPlayerName(const RKString & name)
{
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_2.panel_input.text_input_1");
	static_cast<TextFieldWidget*>(wi)->SetText(name);
}

void MainUserProfileLayer::SetSx(int id)
{
	if (id >= 3 || id < 0)
		id = 0;
	RKString sx_type = list_sx[id];
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_input_sx.btn_input_sx.text_input");
	static_cast<TextWidget*>(wi)->SetText(sx_type);
}

void MainUserProfileLayer::SetPhoneNumber(const RKString & number)
{
	if (number == "")
		return;
	RKString number_phone = (number);
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_5.panel_input.text_input_phone");
	static_cast<TextFieldWidget*>(wi)->SetText(number_phone);
}

void MainUserProfileLayer::SetBirthDay(int day, int month, int year)
{
	auto wday = p_list_choose_day->GetParentWidget()->GetWidgetChildByName(".btn_input_day.text_input");
	auto wmon = p_list_choose_month->GetParentWidget()->GetWidgetChildByName(".btn_input_month.text_input");
	auto wyear = p_list_choose_year->GetParentWidget()->GetWidgetChildByName(".btn_input_year.text_input");
	static_cast<TextWidget*>(wday)->SetText(std::to_string(day), true);
	static_cast<TextWidget*>(wmon)->SetText(std::to_string(month), true);
	static_cast<TextWidget*>(wyear)->SetText(std::to_string(year), true);

	p_cur_day = day;
	p_cur_month = month;
	p_cur_year = year;

	RunThreadCheckDateTime();
}

void MainUserProfileLayer::SetAddress(const RKString & address)
{
	if (address == "")
		return;
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_6.panel_input.text_input_address");
	static_cast<TextFieldWidget*>(wi)->SetText(address);
}

void MainUserProfileLayer::SetCMNDNumber(const RKString & number)
{
	RKString number_CMND = (number);
	if (number == "")
		return;
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_7.panel_input.text_input_cmnd");
	static_cast<TextFieldWidget*>(wi)->SetText(number_CMND);
}

void MainUserProfileLayer::SetEmail(const RKString & email)
{
	if (email == "")
		return;
	auto wi = p_page_profile_name->GetWidgetChildByName(".title_8.panel_input.text_input_email");
	static_cast<TextFieldWidget*>(wi)->SetText(email);
}

void MainUserProfileLayer::UpdatePlayerName_ToServer(const RKString & name)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->fullname = name.GetString();
	p_user_dirty = true;
}

void MainUserProfileLayer::UpdateSx_ToServer(int id)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->gender = id;
	p_user_dirty = true;
}

void MainUserProfileLayer::UpdatePhoneNumber_ToServer(const RKString & number)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->phoneNo = number.GetString();
	p_user_dirty = true;
}

void MainUserProfileLayer::UpdateBirthDay_ToServer(int day, int month, int year)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->monthOfBirth = month;
	GetUser->dayOfBirth = day;
	GetUser->yearOfBirth = year;
	p_user_dirty = true;
}

void MainUserProfileLayer::UpdateAddress_ToServer(const RKString & address)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->address = address.GetString();
	p_user_dirty = true;
}

void MainUserProfileLayer::UpdateCMNDNumber_ToServer(int number)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->idnumber = std::to_string(number);
	p_user_dirty = true;
}


void MainUserProfileLayer::UpdateEmail_ToServer(const RKString & email)
{
	if (!GetUser)
	{
		//PASSERT2(p_user != nullptr, "p_user is null");
		return;
	}
	GetUser->email = email.GetString();
	p_user_dirty = true;
}

void MainUserProfileLayer::FuncWhenDeactiveLayer()
{
	SAFE_DELETE(p_thread_check_time)

	if (p_user_dirty)
	{
		if (GetUser)
		{
			GlobalService::UpdateProfile(GetUser);
		}
		p_user_dirty = false;
	}
}

void MainUserProfileLayer::OnShowOrHideListChoose(WidgetEntity * list, bool Show)
{
	if (Show && !list->Visible())
	{
		list->SetActionCommandWithName("ON_SHOW", CallFunc::create(
			[this, list]()
		{
			list->ForceFinishAction();
			list->GetParentWidget()->SetZOrder(100);
		}));
	}
	else if (!Show && list->Visible())
	{
		list->SetActionCommandWithName("ON_HIDE", CallFunc::create(
			[this, list]() {
			list->ForceFinishAction();
			list->GetParentWidget()->SetZOrder(0);
		}));
	}
}

void MainUserProfileLayer::CreateListDate()
{
	if (p_is_create_list_choose)
		return;

	p_is_create_list_choose = true;
	//gener
	{
		auto _list_choose_sx = static_cast<ListViewTableWidget*>(p_list_choose_sx);
		_list_choose_sx->addEventListener(
			[this, _list_choose_sx](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = _list_choose_sx->GetCurrentIdxTo();
				static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title"))->SetText(RKString(list_sx[current_idx_to]));
			}
			else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
			{
				int current_idx_to = _list_choose_sx->GetCurrentIdxSelected();
				auto list_widget = _list_choose_sx->GetListWidgetSample();
				for (size_t i = 0; i < list_widget.size(); i++)
				{
					list_widget[i]->GetWidgetChildByName(".hight_light")->Visible(false);
				}
				_widget->GetWidgetChildByName(".hight_light")->Visible(true);

				RKString idx_sx = list_sx[current_idx_to];
				UpdateSx_ToServer(current_idx_to);

				auto wi = _list_choose_sx->GetParentWidget()->GetWidgetChildByName(".btn_input_sx.text_input");
				static_cast<TextWidget*>(wi)->SetText(idx_sx);

				_list_choose_sx->JumpToItemIdx(current_idx_to);
				OnShowOrHideListChoose(_list_choose_sx, false);

			}
		});
		_list_choose_sx->InitTable();
	}
	//day
	{
		int current_day = 1;
		auto _list_choose_day = static_cast<ListViewTableWidget*>(p_list_choose_day);
		_list_choose_day->addEventListener(
			[this, current_day](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_day)->GetCurrentIdxTo();
				static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title"))->SetText(std::to_string(current_idx_to + current_day), true);
				_widget->GetWidgetChildByName(".hight_light")->Visible(false);
				if (current_idx_to + current_day == this->p_cur_day)
				{
					_widget->GetWidgetChildByName(".hight_light")->Visible(true);
				}
			}
			else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_day)->GetCurrentIdxSelected();
				auto list_widget = static_cast<ListViewTableWidget*>(this->p_list_choose_day)->GetListWidgetSample();
				for (size_t i = 0; i < list_widget.size(); i++)
				{
					list_widget[i]->GetWidgetChildByName(".hight_light")->Visible(false);
				}
				_widget->GetWidgetChildByName(".hight_light")->Visible(true);

				auto wi = this->p_list_choose_day->GetParentWidget()->GetWidgetChildByName(".btn_input_month.text_input");
				static_cast<TextWidget*>(wi)->SetText(std::to_string(current_idx_to + current_day), true);
				this->OnShowOrHideListChoose(this->p_list_choose_day, false);
				this->p_cur_day = current_idx_to + current_day;
				this->RunThreadCheckDateTime();

			}
		});
		_list_choose_day->InitTable();

	}
	//month
	{
		int current_month = 1;
		auto _list_choose_month = static_cast<ListViewTableWidget*>(p_list_choose_month);
		_list_choose_month->addEventListener(
			[this, current_month](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_month)->GetCurrentIdxTo();
				static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title"))->SetText(std::to_string(current_idx_to + current_month), true);
				_widget->GetWidgetChildByName(".hight_light")->Visible(false);
				if (current_idx_to + current_month == this->p_cur_month)
				{
					_widget->GetWidgetChildByName(".hight_light")->Visible(true);
				}
			}
			else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_month)->GetCurrentIdxSelected();
				auto list_widget = static_cast<ListViewTableWidget*>(this->p_list_choose_month)->GetListWidgetSample();
				for (size_t i = 0; i < list_widget.size(); i++)
				{
					list_widget[i]->GetWidgetChildByName(".hight_light")->Visible(false);
				}
				_widget->GetWidgetChildByName(".hight_light")->Visible(true);

				auto wi = this->p_list_choose_month->GetParentWidget()->GetWidgetChildByName(".btn_input_month.text_input");
				static_cast<TextWidget*>(wi)->SetText(std::to_string(current_idx_to + current_month), true);
				this->OnShowOrHideListChoose(this->p_list_choose_month, false);
				this->p_cur_month = current_idx_to + current_month;
				this->RunThreadCheckDateTime();

			}
		});
		_list_choose_month->InitTable();
	}
	//year
	{
		int current_year = 1917;
		auto _list_choose_year = static_cast<ListViewTableWidget*>(p_list_choose_year);
		_list_choose_year->addEventListener(
			[this, current_year](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
		{
			if (type == EventType_SetCell::ON_SET_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_year)->GetCurrentIdxTo();
				static_cast<TextWidget*>(_widget->GetWidgetChildByName(".title"))->SetText(std::to_string(current_idx_to + current_year), true);
				_widget->GetWidgetChildByName(".hight_light")->Visible(false);
				if (current_idx_to + current_year == this->p_cur_year)
				{
					_widget->GetWidgetChildByName(".hight_light")->Visible(true);
				}
			}
			else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
			{
				int current_idx_to = static_cast<ListViewTableWidget*>(this->p_list_choose_year)->GetCurrentIdxSelected();
				auto list_widget = static_cast<ListViewTableWidget*>(this->p_list_choose_year)->GetListWidgetSample();
				for (size_t i = 0; i < list_widget.size(); i++)
				{
					list_widget[i]->GetWidgetChildByName(".hight_light")->Visible(false);
				}
				_widget->GetWidgetChildByName(".hight_light")->Visible(true);

				auto wi = this->p_list_choose_year->GetParentWidget()->GetWidgetChildByName(".btn_input_year.text_input");
				static_cast<TextWidget*>(wi)->SetText(std::to_string(current_idx_to + current_year), true);
				this->OnShowOrHideListChoose(this->p_list_choose_year, false);
				this->p_cur_year = current_idx_to + current_year;
				//this->RunThreadCheckDateTime();

			}
		});
		_list_choose_year->InitTable();
	}
}

void MainUserProfileLayer::SetDetail(User * p_user)
{
	if (!p_user)
		return;

	SetAddress(p_user->address);
	SetAccountName(p_user->nickName);
	SetPlayerName(p_user->fullname);
	auto wi_name = p_page_profile_name->GetWidgetChildByName(".title_2.panel_input.text_input_1");
	int loginType = -1;
	ScrMgr->GetDataSaveInt(STR_SAVE_TYPE_LOGIN, loginType);
	if (loginType == LOGIN_FACEBOOK || loginType == LOGIN_APPLE || loginType == LOGIN_GOOGLE_PLAY)
	{
		wi_name->SetColor(Color3B::YELLOW);
		wi_name->SetDisable(true);
	}
	else
	{
		wi_name->SetColor(Color3B::WHITE);
		wi_name->SetDisable(false);
	}

	SetSx(p_user->gender);
	SetPhoneNumber(p_user->phoneNo);
	SetBirthDay(p_user->dayOfBirth, p_user->monthOfBirth, p_user->yearOfBirth);
	SetCMNDNumber(p_user->idnumber);
	SetEmail(p_user->email);



	p_page_profile_name->GetWidgetChildByName(".title_8.confirm_email")->Visible(!p_user->isEmailValidate);
	p_page_profile_name->GetWidgetChildByName(".title_8.txt_confirm")->Visible(p_user->isEmailValidate);

	auto tx_input_email = p_page_profile_name->GetWidgetChildByName(".title_8.panel_input.text_input_email");
	if (p_user->isEmailValidate)
	{
		tx_input_email->SetDisable(true);
		tx_input_email->SetColor(Color3B::YELLOW);
	}
	else
	{
		tx_input_email->SetDisable(false);
		tx_input_email->SetColor(Color3B::WHITE);
	}


	p_page_profile_name->GetWidgetChildByName(".title_5.confirm_phone")->Visible(!p_user->isPhoneValidate);
	p_page_profile_name->GetWidgetChildByName(".title_5.txt_confirm")->Visible(p_user->isPhoneValidate);

	auto tx_input_phone = p_page_profile_name->GetWidgetChildByName(".title_5.panel_input.text_input_phone");
	if (p_user->isPhoneValidate)
	{
		tx_input_phone->SetDisable(true);
		tx_input_phone->SetColor(Color3B::YELLOW);
	}
	else
	{
		tx_input_phone->SetDisable(false);
		tx_input_phone->SetColor(Color3B::WHITE);
	}

}

void MainUserProfileLayer::RunThreadCheckDateTime()
{
	if (p_thread_check_time && p_thread_check_time->IsThreadRunning())
	{
		//avoid spam thread
		return;
	}

	//delete current theard if its not run anymore
	if (p_thread_check_time)
	{
		delete p_thread_check_time;
		p_thread_check_time = nullptr;
	}

	//create thread
	p_thread_check_time = new ThreadWrapper();
	p_thread_check_time->CreateThreadWrapper("thread_check_date_time",
		([](void * data) -> uint32
	{
		MainUserProfileLayer * scr = (MainUserProfileLayer*)data;
		scr->p_thread_check_time->OnCheckUpdateThreadWrapper
		([scr]()
		{
			scr->OnRecorrectDateTime();
		});
		return 1;
	}),(void*)this);
}

void MainUserProfileLayer::OnRecorrectDateTime()
{
	bool res = CheckValidateDay(p_cur_day, p_cur_month, p_cur_year, true);
}

bool MainUserProfileLayer::CheckValidateDay(int &day, int &month, int &year, bool recorrect)
{
	if (year < 1917)
	{
		if (recorrect) { year = 1917; }
		else { PASSERT2(year > 1917, "wrong year"); }
		return CheckValidateDay(day, month, year, recorrect);
	}

	bool is_nam_nhuan = (year % 4 == 0 && year % 100 == 0);
	if (month > 12 || month < 1)
	{
		if (recorrect) { month = 1; }
		else { PASSERT2(month <= 12 && month >= 1, "wrong month"); }
		return CheckValidateDay(day, month, year, recorrect);
	}

	if (month == 2)
	{
		if (is_nam_nhuan)
		{
			if (day > 29)
			{
				if (recorrect) { day = 29; }
				else { PASSERT2(day < 29, "wrong day"); }
				return false;
			}
		}
		else
		{
			if (day > 28)
			{
				if (recorrect) { day = 28; }
				else { PASSERT2(day < 28, "wrong day"); }
				return CheckValidateDay(day, month, year, recorrect);
			}
		}
	}
	else if (month == 4 || month == 6 || month == 9 || month == 11)
	{
		if (day > 30)
		{
			if (recorrect) { day = 30; }
			else { PASSERT2(day < 30, "wrong day"); }
			return CheckValidateDay(day, month, year, recorrect);
		}
	}

	if (day > 31 || day < 1)
	{
		if (recorrect) { day = 1; }
		else { PASSERT2(day <= 31 && day >= 1, "wrong day"); }
		return CheckValidateDay(day, month, year, recorrect);
	}

	return true;
}


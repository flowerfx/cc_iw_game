#include "LuckyCircleScreen.h"

#include "Network/Global/Globalservice.h"
#include "network/JsonObject/LuckyCircle/LuckyCircleQuestList.h"
#include "network/JsonObject/LuckyCircle/LuckyCircleResultList.h"

#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wPanel.h"
#include "UI/AdvanceUI/wListView.h"
#include "UI/AdvanceUI/wCheckBox.h"
#include "UI/AdvanceUI/wAnimationWidget.h"
#include "UI/AdvanceUI/wButton.h"
#include "Utils.h"

#include "Common/IwinTextfield.h"
USING_NS_CC;

#define NAME_IMG_RESULT "ImageResult"

LuckyCircleScreen::LuckyCircleScreen()
{
	p_lucky_quest_list = nullptr;
	p_lucky_result = nullptr;
	index_result = -1;
	isRotation = false;

	_url_captcha = _token_captcha = "";
	
}

LuckyCircleScreen::~LuckyCircleScreen()
{
	SAFE_DELETE(p_lucky_quest_list);
	SAFE_DELETE(p_lucky_result);
}

void LuckyCircleScreen::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	p_layout_main = _base_screen->GetWidgetChildByName("common_screen.layout_crilce");
	p_img_circle =  p_layout_main->GetWidgetChildByName(".cricle_main.cricle_img");
	p_arow_tick =   p_layout_main->GetWidgetChildByName(".cricle_main.img_arow");
	p_list_view_result = p_layout_main->GetWidgetChildByName(".cricle_main.bg_result.scroll_content");
	p_popup_result = p_layout_main->GetWidgetChildByName(".layout_result");
	
	p_lb_money_win = p_layout_main->GetWidgetChildByName(".cricle_main.btn_purchase_win.title_");
	p_lb_ticket = p_layout_main->GetWidgetChildByName(".cricle_main.btn_ticket.title_");

	p_layout_auto_spin = p_layout_main->GetWidgetChildByName(".cricle_main.layout_check_auto_spin");
	p_check_auto = p_layout_auto_spin->GetWidgetChildByName(".check_box_auto");

	p_layout_captcha = p_layout_main->GetWidgetChildByName(".cricle_main.layout_captcha");
	auto text_field = static_cast<IWIN_TF*>(p_layout_captcha->GetWidgetChildByName(".panel_input.captcha_input"));
	text_field->SetBtnClear(p_layout_captcha->GetWidgetChildByName(".panel_input.btn_clear"));
	text_field->InitCallBack(nullptr);

	p_btn_help = p_layout_main->GetWidgetChildByName(".cricle_main.btn_help");

	//p_anim_money =p_layout_main->GetWidgetChildByName(".animate_effect_bonus");

	p_hl_result = p_layout_main->GetWidgetChildByName(".cricle_main.luckycircle_result_layout");
	//return 1;
}

bool LuckyCircleScreen::IsVisibleLayer()
{
	if (p_layout_main)
	{
		return p_layout_main->IsVisible();
	}
	return false;
}

bool LuckyCircleScreen::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;

	if (!p_layout_main || !p_layout_main->IsVisible())
	{
		return false;
	}
	if (type_widget == UI_BUTTON)
	{
		if (name == "btn_close")
		{
			SetVisible(false);
			return true;

		}
		else if (name == "btn_spin")
		{
			if (isRotation || !p_lucky_quest_list)
			{
				return false;
			}
			if (p_lucky_quest_list->getCircleId() == 1)
			{
				RKString code = static_cast<IWIN_TF*>(p_layout_captcha->GetWidgetChildByName(".panel_input.captcha_input"))->GetText();
				if (code == "")
				{
					ScrMgr->OnShowOKDialog("input_captcha_pls");
				}
				else
				{
					GlobalService::RequestTurningLuckyCircleWithCaptcha(p_lucky_quest_list->getCircleId(), code.GetString(), _token_captcha.GetString());

					static_cast<IWIN_TF*>(p_layout_captcha->GetWidgetChildByName(".panel_input.captcha_input"))->ClearText();
				}
			}
			else
			{
				GlobalService::RequestTurningLuckyCircle(p_lucky_quest_list->getCircleId());
			}
			return true;
		}
		else if (name == "btn_help")
		{
			if (p_lucky_quest_list && p_lucky_quest_list->getUrlRules() != "")
			{
				OPEN_URL(p_lucky_quest_list->getUrlRules());
			}
			else
			{
				OPEN_URL("http://iwin.sdklib.vn");
			}
			return true;
		}
		else if (name == "btn_purchase_win")
		{
			OnHideLayer(nullptr);
			ScrMgr->SwitchToMenu(IAP_SCREEN, ScrMgr->GetCurrentMenuUI());
		}
		else if (name == "btn_ticket")
		{
			OnHideLayer(nullptr);
			ScrMgr->SwitchToMenu(IAP_SCREEN, ScrMgr->GetCurrentMenuUI());
		}
	}

	return false;
	
}

void LuckyCircleScreen::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_layout_main->SetVisible(true);
}
void LuckyCircleScreen::OnHideLayer(const std::function<void(void)> & call_back)
{
	SetVisible(false);
}

void LuckyCircleScreen::SetVisible(bool isShow)
{
	if (!p_layout_main || p_layout_main->IsVisible() == isShow)
	{
		return;
	}
	p_layout_main->SetVisible(isShow);
	if (!isShow)
	{
		p_layout_auto_spin->Visible(false);
		p_layout_captcha->Visible(false);
		p_btn_help->Visible(false);
	}
}

int LuckyCircleScreen::Update(float dt)
{
	if (!IsVisibleLayer())
		return 0;

	if (isRotation)
	{
		float angleTick =360 / p_lucky_quest_list->getList().size();
		float angle = p_img_circle->GetRotate() - angleTick/2;
		float detal = fmod(angle, angleTick);
		if (detal <= 15)
		{
			p_arow_tick->SetRotate(-detal*2);
		}else if (detal <=17)
		{
			//p_arow_tick->SetRotate(0);
		}
	}
	return 1;
}

void LuckyCircleScreen::OnRecieveLuckyCircleResultList(LuckyCircleQuestList* lucky_result_list)
{
	SAFE_DELETE(p_lucky_quest_list);
	p_lucky_quest_list = lucky_result_list;
	if (p_lucky_quest_list)
	{
		if (p_lb_money_win)
		{
			static_cast<TextWidget*>(p_lb_money_win)->SetText(Utils::formatNumber(GameController::myInfo->getMoney()));
		}
		if (p_lb_ticket)
		{
			static_cast<TextWidget*>(p_lb_ticket)->SetText(Utils::formatNumber(p_lucky_quest_list->getUserPoint()));
		}
		RemoveImageFinish();
		GameMgr->HandleURLImgDownload(
			p_lucky_quest_list->getUrlImg(), "", [this](void * data, void * str, int tag)
		{
			Image * img = (Image *)data;
			if (img && (img->getWidth() == 0 || img->getHeight() == 0))
			{
				CC_SAFE_DELETE(img);
				return;
			}
			Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str ,Texture2D::PixelFormat::RGB5A1,false);
			_base_screen->ParseSpriteToPanel(p_img_circle, sprite_img, 100);
		}, false);
		p_lucky_quest_list->getUrlImg();
		SetVisible(true);

		if (p_lucky_quest_list->getCircleId() == 1) //captcha
		{
			p_layout_auto_spin->Visible(false);
			p_layout_captcha->Visible(true);
			p_btn_help->Visible(true);
			p_btn_help->SetActionCommandWithName("SET_POS_CAPTCHA");
		}
		else if (p_lucky_quest_list->getCircleId() > 1)
		{
			p_layout_auto_spin->Visible(true);
			p_layout_captcha->Visible(false);
			p_btn_help->Visible(true);
			p_btn_help->SetActionCommandWithName("SET_POS_AUTO");
		}
	}

}

void LuckyCircleScreen::OnRecieveLuckyCircleResultItem(LuckyCircleResultList* lucky_result_item)
{
	
	int index = 0;
	SAFE_DELETE(p_lucky_result);
	p_lucky_result = lucky_result_item;
	static_cast<TextWidget*>(p_lb_ticket)->SetText(Utils::formatNumber(p_lucky_result->getUserPoint()));
	if (!p_lucky_result || p_lucky_result->getList().size() <= 0)
	{
		if (p_lucky_result->getTiketsObject().getMessage() != "" && p_lucky_result->getTiketsObject().getScreenId() >= 0)
		{
			ScrMgr->OnShowDialog("notice", p_lucky_result->getTiketsObject().getMessage(), DIALOG_TWO_BUTTON, "", [this](const char * str, const char *name) {
				SetVisible(false);
				int screen_id = p_lucky_result->getTiketsObject().getScreenId();
				ScrMgr->OpenScreenByScreenID((iwinmesage::SCREEN_ID)screen_id);

			});
		}
		else if (p_lucky_result->getTiketsObject().getMessage() != "")
		{
			ScrMgr->OnShowOKDialog(p_lucky_result->getTiketsObject().getMessage());
		}
		else if (p_lucky_result->getResultString() != "")
		{
			ScrMgr->OnShowOKDialog(p_lucky_result->getResultString());
		}
		return;
	}
	RemoveImageFinish();
	int _size = p_lucky_quest_list->getList().size();
	for (int i = 0; i <_size; i++)
	{
		if (p_lucky_quest_list->getList()[i].getId() == lucky_result_item->getList()[lucky_result_item->getList().size()-1].getItemId())
		{
			index = i;
			break;
		}
	}

	isRotation = true;
	float _angle = p_img_circle->GetRotate();
	float angleMain = fmod(_angle ,360);
	double angleItem = (index * (360 / _size)) % 360;
	int rand =fmod(std::rand(),5);
	float _sRotation =(15+ rand )* 360 + (360 - ((angleItem + angleMain)));
	
	if (_angle > std::numeric_limits<int>::max())
	{
		_angle = 0;
	}
	
	float angle = (360 / _size);
	double r = (((double)(std::rand())*(2)) / RAND_MAX) - 1;
	_sRotation += r * (angle/3);
	ActionRotation(_sRotation);
	index_result = index;
}

void LuckyCircleScreen::ActionRotation(float angle)
{
	isRotation = true;
	auto rotateTo = RotateBy::create(12.0f, angle);
	//auto rotateTo_ease_in = EaseExponentialOut::create(rotateTo);
	auto rotateTo_ease_in =EaseSineOut::create(rotateTo);
	
	auto sequence = Sequence::create(rotateTo_ease_in, CallFunc::create([this]() {
		isRotation = false;
		p_arow_tick->GetResource()->setRotation(0);
		p_img_circle->GetResource()->stopAllActions();
		OnRotationDone();
	}), nullptr);
	p_img_circle->GetResource()->runAction(sequence);

}

void LuckyCircleScreen::OnRotationDone()
{
	ShowResult();
	
	int size = p_lucky_quest_list->getList().size();
	string file_name = ".icon_win_" + std::to_string(size);
	p_hl_result->SetColor(Color3B::WHITE);
	p_hl_result->Visible(true);
	
	//hide all child layout
	for (int i = 0; i < p_hl_result->GetNumberChildWidget(); i++)
	{
		p_hl_result->GetWidgetChildAtIdx(i)->Visible(false);
	}

	auto widget = p_hl_result->GetWidgetChildByName(file_name);

	if (widget)
	{
		widget->Visible(true);
		auto rotate = p_img_circle->GetRotate();
		float angle = (rotate + index_result*(360 / size));
		widget->SetRotate(angle);
		//widget->SetRotate(0);
		p_hl_result->SetActionCommandWithName("HL");
	}
	
	
}

void LuckyCircleScreen::UpdateMoney()
{
	if (p_lb_money_win)
	{
		static_cast<TextWidget*>(p_lb_money_win)->SetText(Utils::formatNumber(GameController::myInfo->getMoney()));
	}
	if (p_lb_ticket)
	{
		//static_cast<TextWidget*>(p_lb_ticket)->SetText(Utils::formatNumber(p_lucky_result->getUserPoint()));
	}
}

void LuckyCircleScreen::RemoveImageFinish()
{
	if (!p_img_circle)
		return;
	p_hl_result->SetColor(Color3B::WHITE);
	p_hl_result->Visible(false);
	p_popup_result->SetVisible(false);

}

void LuckyCircleScreen::ShowResult()
{

	UpdateMoney();
	p_popup_result->SetVisible(true);
	static_cast<TextWidget*>(p_popup_result->GetWidgetChildByName(".title"))->SetText(p_lucky_result->getResultString(), true);
	p_popup_result->GetResource()->setScaleY(0);
	auto actionScaleShow = ScaleTo::create(0.5,1,1);
	auto actionScaleHide = ScaleTo::create(0.5,1,0);
	p_popup_result->GetResource()->runAction(Sequence::create(actionScaleShow, DelayTime::create(3), actionScaleHide, CallFunc::create([this]() {
		if (this->p_check_auto && static_cast<CheckBox*>(this->p_check_auto->GetResource())->isSelected())
		{
			GlobalService::RequestTurningLuckyCircle(p_lucky_quest_list->getCircleId());
		}
	}), nullptr));

	ListViewWidget* listView = static_cast<ListViewWidget*>(p_list_view_result);
	auto w = listView->CreateWidgetRefer(listView->GetNumberChildWidget());

	auto wtext = static_cast<TextWidget*>(w->GetWidgetChildByName(".notice_content"));
	wtext->SetText("- "+p_lucky_result->getResultString(), true);

	auto current_size = w->GetSize();
	auto content_size = wtext->GetActualContainSize();
	w->SetSize(Vec2(current_size.x, content_size.y + (10 * (GetGameSize().width / this->_base_screen->GetDesignSize().x))));
	wtext->SetPosition(Vec2((10 * (GetGameSize().width / this->_base_screen->GetDesignSize().x)), w->GetSize().y - 5));

	/*s64 delta_change_money = 0;
	if (p_lucky_result->getBonusType() == BASE_MONEY)
	{
		delta_change_money = newMoney - GameController::myInfo->money;
	}
	else if (p_lucky_result->getBonusType() == RUBY_MONEY)
	{
		delta_change_money = newMoney - GameController::myInfo->ruby;
	}*/
	//ScrMgr->OnReceiveTransferMoney(p_lucky_result->get, p_lucky_result->getResultString(), p_lucky_result->getBonusType());
}

void LuckyCircleScreen::SetLinkUrlCaptcha(RKString url)
{
	_url_captcha = url;
	auto panel_cap = p_layout_captcha->GetWidgetChildByName(".panel_captcha");
	GameMgr->HandleURLImgDownload(
		_url_captcha, "", [this, panel_cap](void * data, void * str, int tag)
	{
		this->_base_screen->ParseImgToPanel(panel_cap, data, str, 100, Texture2D::PixelFormat::RGB5A1, false);
	} , false);

	auto w_btn_refresh = static_cast<ButtonWidget*>(p_layout_captcha->GetWidgetChildByName(".btn_refesh_captcha"));
	w_btn_refresh->SetActionBtn([this, panel_cap]() {
			GameMgr->HandleURLImgDownload(
				_url_captcha, "", [this, panel_cap](void * data, void * str, int tag)
			{
				this->_base_screen->ParseImgToPanel(panel_cap, data, str, 100, Texture2D::PixelFormat::RGB5A1, false);
			} , false);
	});
}

void LuckyCircleScreen::SetTokenCaptcha(RKString token)
{
	_token_captcha = token;
}


#include "ShopAvatarLayer.h"
#include "Network/Global/Globalservice.h"

#include "UI/AdvanceUI/wButton.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wListView.h"

#include "Network/JsonObject/Avatar/AvatarCat.h"
#include "Network/JsonObject/Avatar/AvatarCatList.h"
#include "Network/JsonObject/Avatar/AvatarList.h"
#include "Network/JsonObject/Avatar/BuyingAvatar.h"



ShopAvatarLayer::ShopAvatarLayer()
{
	p_shop_avatar_layout = nullptr;
	p_shop_avatar_list_cat = nullptr;
	p_shop_avatar_list = nullptr;
	p_confirm_buy_avatar_layout = nullptr;


	p_avatar_list = nullptr;
	p_current_avatar_idx_buy = 0;
	p_stage_avatar_cat_list = 0;
	p_list_avatar_download.clear();
}

ShopAvatarLayer::~ShopAvatarLayer()
{
	p_list_avatar_download.clear();

	SAFE_DELETE(p_avatar_list);
	p_stage_avatar_cat_list = 0;
}

void ShopAvatarLayer::Reset()
{
	SAFE_DELETE(p_avatar_list);
	p_stage_avatar_cat_list = 0;
}

void ShopAvatarLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);

	p_shop_avatar_layout = _base_screen->GetWidgetChildByName("profile_screen.shop_avatar_layout.shop_layout");
	p_shop_avatar_list_cat = p_shop_avatar_layout->GetWidgetChildByName(".list_shop_view");
	p_shop_avatar_list = p_shop_avatar_layout->GetWidgetChildByName(".list_avatar_view");
	p_confirm_buy_avatar_layout = p_shop_avatar_layout->GetParentWidget()->GetWidgetChildByName(".confirm_buy_avatar_layout._layout");

	static_cast<ListViewTableWidget*>(p_shop_avatar_list)->InitTable();
}

bool ShopAvatarLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_back_shop_avatar")
		{
			if (p_shop_avatar_list_cat->Visible())
			{
				this->p_shop_avatar_layout->GetParentWidget()->SetActionCommandWithName("ON_HIDE");
			}
			else
			{
				p_shop_avatar_list_cat->SetActionCommandWithName("ON_SHOW");
				p_shop_avatar_list->SetActionCommandWithName("ON_HIDE");
			}
			return true;
		}
		else if (name == "btn_buy_avatar" || name == "btn_cancel_buy_avatar")
		{
			OnCloseConfirmBuyAvatar();

			if (name == "btn_buy_avatar" && p_avatar_list)
			{
				if (p_current_avatar_idx_buy >= 0 && p_current_avatar_idx_buy < p_avatar_list->getList().size())
				{
					auto current_avatar_id = p_avatar_list->getList()[p_current_avatar_idx_buy];
					GlobalService::buyAvatar(current_avatar_id->getId(), current_avatar_id->getMoney());
				}
				else
				{
					PASSERT2(false, "problem here!");
				}
			}

			return true;
		}


	}
	return false;
}

void ShopAvatarLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_shop_avatar_layout->GetParentWidget()->SetActionCommandWithName("ON_SHOW");
	//request avatar cate from server
	GlobalService::getAvatarCategory();
}

void ShopAvatarLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	if (p_shop_avatar_list_cat->Visible())
	{
		this->p_shop_avatar_layout->GetParentWidget()->SetActionCommandWithName("ON_HIDE");
	}
	else
	{
		p_shop_avatar_list_cat->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]()
		{
			this->p_shop_avatar_list_cat->ForceFinishAction();
			this->p_shop_avatar_layout->GetParentWidget()->SetActionCommandWithName("ON_HIDE");
		}));
		p_shop_avatar_list->SetActionCommandWithName("ON_HIDE");
	}
}
bool ShopAvatarLayer::IsVisibleLayer()
{
	return p_shop_avatar_layout->GetParentWidget()->Visible();
}
int ShopAvatarLayer::Update(float dt)
{

	return 1;
}

void ShopAvatarLayer::OnParseInfoAvaToLayout(WidgetEntity * widget, iwinmesage::Avatar * info, bool request_avatar)
{
	PASSERT2(widget != nullptr, "widget is null");
	PASSERT2(info != nullptr, "info is null");

	int ID = info->getId();
	int cateID = info->getCateId();
	bool isBestSeller = info->getBestSeller();
	int day_exp = info->getExpireDay();
	auto cash_cost = info->getMoney();
	RKString name = info->getName();
	int have_bought = info->getBought();

	auto btn = widget; // ->GetWidgetChildAtIdx(0);

	static_cast<TextWidget*>(btn->GetWidgetChildByName(".title_"))->SetText(info->getName(), true);
	static_cast<TextWidget*>(btn->GetWidgetChildByName(".title_money"))->SetText(Utils::formatNumber_dot(cash_cost), true);

	auto panel_icon = btn->GetWidgetChildByName(".icon_bg");
	panel_icon->GetWidgetChildByName(".ico_hot")->SetVisible(isBestSeller);
	RKString day = LangMgr->GetString("days");
	RKString title_day = Utils::formatNumber_dot(day_exp) + " " + day;

	if (have_bought == 0)
	{
		auto w_title = panel_icon->GetWidgetChildByName(".bg_yeallow.title_");
		static_cast<TextWidget*>(w_title)->SetText(title_day, true);
		w_title->Visible(true);
		panel_icon->GetWidgetChildByName(".bg_yeallow.bg_bought")->Visible(false);
	}
	else
	{
		panel_icon->GetWidgetChildByName(".bg_yeallow.title_")->Visible(false);
		panel_icon->GetWidgetChildByName(".bg_yeallow.bg_bought")->Visible(true);
		auto w_title = panel_icon->GetWidgetChildByName(".bg_yeallow.bg_bought.title_1");
		static_cast<TextWidget*>(w_title)->SetText(title_day, true);
	}

	name = name + std::to_string(ID);

	if (!request_avatar)
	{
		if (p_list_avatar_download.size() > 0 && p_list_avatar_download.find(name.GetString()) != p_list_avatar_download.end())
		{
			_base_screen->ParseSpriteToPanel(panel_icon, p_list_avatar_download.at(name.GetString()), 100);
		}
	}
	else
	{
		panel_icon->retain();
		GameMgr->HandleURLImgDownload(info->getImg(), name,
			[this, panel_icon](void * data, void * str, int tag)
		{
			_base_screen->ParseImgToPanel(panel_icon, data, str, 1);
			panel_icon->release();
		});
	}

}

void ShopAvatarLayer::OnParseInfoAvaCatToLayout(WidgetEntity * widget, iwinmesage::AvatarCat * info)
{
	PASSERT2(widget != nullptr, "widget is null");
	PASSERT2(info != nullptr, "info is null");

	int id = info->getId();
	auto btn = widget->GetWidgetChildAtIdx(0);
	static_cast<ButtonWidget*>(btn)->SetActionBtn([this, id]() 
	{
			GlobalService::getAvatarOfCategory(id);

			p_shop_avatar_list_cat->SetActionCommandWithName("ON_HIDE");
			p_shop_avatar_list->SetActionCommandWithName("ON_SHOW");
	});

	static_cast<TextWidget*>(btn->GetWidgetChildByName(".top_bg.title_"))->SetText(info->getName(), true);
	auto panel_icon = btn->GetWidgetChildByName(".icon_bg");

	GameMgr->HandleURLImgDownload(
		info->getImg(), info->getName(), [this, panel_icon](void * data, void * str, int tag)
	{
		_base_screen->ParseImgToPanel(panel_icon, data, str, 100);
	});
}

void ShopAvatarLayer::onReceiveAvatarCatList(void * data)
{
	AvatarCatList * value = (AvatarCatList *)data;
	if (value == nullptr)
		return;

	if (p_shop_avatar_list_cat->GetNumberChildWidget() > 0)
	{
		//remove old ava cat
		for (size_t i = 0; i < p_shop_avatar_list_cat->GetNumberChildWidget(); i++)
		{
			auto w = p_shop_avatar_list_cat->GetWidgetChildAtIdx(i)->GetWidgetChildByName(".btn_shop.icon_bg");
			auto child = w->GetResource()->getChildByTag(99);
			if (child)
			{
				w->GetResource()->removeChildByTag(99);
			}
		}
		p_shop_avatar_list_cat->ClearChild();
	}

	size_t number_avatar_cat = value->getList().size();

	for (size_t i = 0; i < number_avatar_cat; i++)
	{
		auto widget = static_cast<ListViewWidget*>(p_shop_avatar_list_cat)->CreateWidgetRefer(i);
		OnParseInfoAvaCatToLayout(widget, &value->getList()[i]);
	}

	delete value;
}

void ShopAvatarLayer::onReceiveAvatarList(void * data)
{
	if (p_avatar_list)
	{
		delete p_avatar_list;
		p_avatar_list = nullptr;
		//chien todo : it support 4billion times, and afterthat will be crash , hope it's not happend :)
		p_stage_avatar_cat_list++;
	}

	p_avatar_list = (AvatarList *)data;
	if (p_avatar_list == nullptr)
		return;

	AvatarList * value = p_avatar_list;
	//if (p_shop_avatar_list->GetNumberChildWidget() > 0)
	{
		//remove old ava
		static_cast<ListViewTableWidget*>(p_shop_avatar_list)->ResetListView();
		CleanUpListAvatar();
	}

	size_t number_avatar = value->getList().size();
	auto _p_shop_avatar_list = static_cast<ListViewTableWidget*>(p_shop_avatar_list);

	_p_shop_avatar_list->addEventListener(
		[this, _p_shop_avatar_list, value](Ref* ref, EventType_SetCell type, WidgetEntity* _widget)
	{
		if (type == EventType_SetCell::ON_SET_CELL)
		{
			int current_idx_to = _p_shop_avatar_list->GetCurrentIdxTo();
			if ((size_t)current_idx_to >= value->getList().size())
			{
				PASSERT2(false, "problem here !");
				current_idx_to = value->getList().size() - 1;
			}
			OnParseInfoAvaToLayout(_widget->GetWidgetChildAtIdx(0), value->getList()[current_idx_to]);
		}
		else if (type == EventType_SetCell::ON_END_TOUCH_CELL)
		{
			p_current_avatar_idx_buy = _p_shop_avatar_list->GetCurrentIdxSelected();
			this->p_confirm_buy_avatar_layout->GetParentWidget()->Visible(true);
			this->p_confirm_buy_avatar_layout->SetActionCommandWithName("ON_SHOW");

			OnParseInfoAvaToLayout(this->p_confirm_buy_avatar_layout->GetWidgetChildByName(".bg_avatar"), value->getList()[p_current_avatar_idx_buy], true);
		}
	});

	_p_shop_avatar_list->SetNumberSizeOfCell(number_avatar);
	int current_stage_list = p_stage_avatar_cat_list;
	//download the avatar into the list
	for (size_t i = 0; i < number_avatar; i++)
	{
		auto idx_player = value->getList()[i];
		{
			GameMgr->HandleURLImgDownload(idx_player->getImg(), idx_player->getName(),
				[this, idx_player, i, current_stage_list](void * data, void * str, int tag)
			{
				Image * img = (Image*)data;
				if (current_stage_list != this->p_stage_avatar_cat_list)
				{
					PINFO("PROFILE SCREEN : avatar download and idx_player is delete , safe to continue!");
					CC_SAFE_DELETE(img);
				}
				else
				{
					Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, str);
					this->p_list_avatar_download.insert(idx_player->getName() + std::to_string(idx_player->getId()), sprite_img);

					auto widget_lb = static_cast<ListViewTableWidget*>(this->p_shop_avatar_list);
					if ((size_t)i < widget_lb->GetListWidgetSample().size())
					{
						widget_lb->UpdateCellAtIdx(i);
					}
				}
			});
		}
	}
	//delete value;
}

void ShopAvatarLayer::CleanUpListAvatar()
{
	p_list_avatar_download.clear();
}

void ShopAvatarLayer::onBuyingAvatarResult(void * data)
{
	BuyingAvatar * value = (BuyingAvatar *)data;
	int ttt = 0;
	ttt++;
}

bool ShopAvatarLayer::IsConfirmBuyAvatarVisible()
{
	return p_confirm_buy_avatar_layout->GetParentWidget()->Visible();
}

void ShopAvatarLayer::OnCloseConfirmBuyAvatar(const std::function<void(void)> & call_back)
{
	p_confirm_buy_avatar_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this , call_back]()
	{
		this->p_confirm_buy_avatar_layout->ForceFinishAction();
		this->p_confirm_buy_avatar_layout->GetParentWidget()->Visible(false);
		if (call_back)
		{
			call_back();
		}
	}));
}
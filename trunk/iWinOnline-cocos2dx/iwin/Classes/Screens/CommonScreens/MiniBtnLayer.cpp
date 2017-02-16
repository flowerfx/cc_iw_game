#include "MiniBtnLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTimeDigitalWidget.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"
#include "Screens/CommonScreens/TaiXiuLayer.h"
#include "Screens/CommonScreens/MiniPokerLayer.h"
#include "InputManager.h"
MiniBtnLayer::MiniBtnLayer()
{
	p_delta_move_btn_tx = 0;
}

MiniBtnLayer::~MiniBtnLayer()
{

}

int MiniBtnLayer::Update(float dt)
{
	OnDragBtnMini();
	return 1;
}

bool MiniBtnLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;

	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "btn_taixiu")
		{
			p_list_btn_mini_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]() 
			{
			
				if (GetCommonScr->IsLayerVisible(TAIXIU_LAYER))
				{
					static_cast<TaiXiuLayer*>(GetCommonScr->GetLayer(TAIXIU_LAYER))->OnShowTaiXiuLayout(false);
				}
				else
				{
					static_cast<TaiXiuLayer*>(GetCommonScr->GetLayer(TAIXIU_LAYER))->OnShowTaiXiuLayout(true);
				}
			}));
			return true;
		}
		else if (name == "btn_mini_poker")
		{
			p_list_btn_mini_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this]()
			{

				if (GetCommonScr->IsLayerVisible(MINI_POKER_LAYER))
				{
					static_cast<MiniPokerLayer*>(GetCommonScr->GetLayer(MINI_POKER_LAYER))->OnShowMiniPokerLayout(false);
				}
				else
				{
					static_cast<MiniPokerLayer*>(GetCommonScr->GetLayer(MINI_POKER_LAYER))->OnShowMiniPokerLayout(true);
				}
			}));
			return true;
		}
	}
	return false;
}

void MiniBtnLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);

	p_layout_mini_btn = _base_screen->GetWidgetChildByName("common_screen.mini_button_layout");
	p_list_btn_mini_layout = p_layout_mini_btn->GetWidgetChildByName(".event_list_layout");
	p_time_panel = p_list_btn_mini_layout->GetWidgetChildByName(".taixiu_timer");
}

void MiniBtnLayer::OnDragBtnMini()
{
	if (InputMgr->GetNumberTouch() > 0)
	{
		auto current_state_touch = InputMgr->GetStateTouchOnScreen();

		if (current_state_touch == Control::STATE_PRESS)
		{
			//click outside so visible this
			cocos2d::Vec2 wPos = InputMgr->GetPosAtId(0);// NODE(p_layout_tai_xiu_mini_btn)->convertTouchToNodeSpace(InputMgr->GetTouch());
			auto rect = p_layout_mini_btn->GetBound();
			if (rect.containsPoint(wPos))
			{

				NODE(p_layout_mini_btn)->setTag(1);
				p_layout_mini_btn->GetWidgetChildAtIdx(0)->SetOpacity(255);
				_previous_pos_touch = wPos;
			}
		}
		else if (current_state_touch == Control::STATE_HOLD)
		{
			if (NODE(p_layout_mini_btn)->getTag() == 1)
			{
				cocos2d::Vec2 wPos = InputMgr->GetPosAtId(0);
				Vec2 cur_pos_t = wPos;
				Vec2 delta_pos = Vec2(cur_pos_t.x - _previous_pos_touch.x, cur_pos_t.y - _previous_pos_touch.y);
				p_delta_move_btn_tx += sqrt(delta_pos.x * delta_pos.x + delta_pos.y*delta_pos.y);
				Vec2 cur_pos_card = p_layout_mini_btn->GetPosition();
				Vec2 new_pos = Vec2(cur_pos_card.x + delta_pos.x, cur_pos_card.y + delta_pos.y);
				p_layout_mini_btn->SetPosition(new_pos);

				auto rect_btntx = p_layout_mini_btn->GetBound();
				auto rect_win = cocos2d::Rect(Vec2(0, 0), cocos2d::Size(GetGameSize().width, GetGameSize().height));
				if (rect_btntx.getMinX() < rect_win.getMinX())
				{
					p_layout_mini_btn->SetPosition(Vec2(rect_btntx.size.width / 2, new_pos.y));
				}
				else if (rect_btntx.getMinY() < rect_win.getMinY())
				{
					p_layout_mini_btn->SetPosition(Vec2(new_pos.x, rect_btntx.size.height / 2));
				}
				else if (rect_btntx.getMaxX() > rect_win.getMaxX())
				{
					p_layout_mini_btn->SetPosition(Vec2(rect_win.getMaxX() - (rect_btntx.size.width / 2), new_pos.y));
				}
				else if (rect_btntx.getMaxY() > rect_win.getMaxY())
				{
					p_layout_mini_btn->SetPosition(Vec2(new_pos.x, rect_win.getMaxY() - (rect_btntx.size.height / 2)));
				}

				_previous_pos_touch = cur_pos_t;

			}
		}

	}
	else if (InputMgr->GetNumberTouchRelease() > 0)
	{
		auto current_state_touch = InputMgr->GetStateTouchOnScreen();
		if (current_state_touch == Control::STATE_RELEASE)
		{
			if (NODE(p_layout_mini_btn)->getTag() == 1)
			{
				p_layout_mini_btn->GetWidgetChildAtIdx(0)->SetOpacity(128);
				if (p_delta_move_btn_tx <= 0)
				{
					if (p_list_btn_mini_layout->Visible())
					{
						p_list_btn_mini_layout->SetActionCommandWithName("ON_HIDE");
					}
					else
					{
						p_list_btn_mini_layout->SetActionCommandWithName("ON_SHOW");
					}
				}
				else
				{

				}
				p_delta_move_btn_tx = 0;
			}
			NODE(p_layout_mini_btn)->setTag(0);
		}
	}

}

void MiniBtnLayer::OnShowLayer(const std::function<void(void)> & call_back)
{

}

void MiniBtnLayer::OnHideLayer(const std::function<void(void)> & call_back)
{

}

bool MiniBtnLayer::IsVisibleLayer()
{
	return p_layout_mini_btn->Visible();
}

void MiniBtnLayer::ResetTimer(bool value, float time1, float timer2)
{
	if (value)
	{
		p_time_panel->GetWidgetChildAtIdx(1)->Visible(false);
		p_time_panel->GetWidgetChildAtIdx(0)->Visible(true);
		static_cast<TimeDigitalWidget*>(p_time_panel->GetWidgetChildByName(".title_time"))->ResetTimer(time1, timer2);
	}
	else
	{
		p_time_panel->GetWidgetChildAtIdx(1)->Visible(true);
		p_time_panel->GetWidgetChildAtIdx(0)->Visible(false);
	}

}

void MiniBtnLayer::SetTimer(s64 timebet_remain, s64 timeresult_remain, s64 timebet_total, s64 timeresult_total, s64 default_time_bet, s64 default_time_result)
{

	s64 time_remain = 0;
	s64 time_total = 0;


	if (timebet_remain <= 0)
	{
		p_time_panel->GetWidgetChildAtIdx(1)->Visible(true);
		p_time_panel->GetWidgetChildAtIdx(0)->Visible(false);
		time_remain = timeresult_remain;
		time_total = timeresult_total > 0 ? timeresult_total : default_time_result;
	}
	else
	{
		p_time_panel->GetWidgetChildAtIdx(1)->Visible(false);
		p_time_panel->GetWidgetChildAtIdx(0)->Visible(true);
		time_remain = timebet_remain;
		time_total = timebet_total > 0 ? timebet_total : default_time_bet;
	}
	static_cast<TimeDigitalWidget*>(p_time_panel->GetWidgetChildByName(".title_time"))->SetTimer(time_total, (double)time_remain / 1000,
		[this , default_time_result](cocos2d::Ref * ref, UI_Widget::EventType_Digital event_type)
	{
		if (event_type == UI_Widget::EventType_Digital::ON_TIMER_ZERO)
		{
		//	p_time_panel->GetWidgetChildAtIdx(1)->Visible(!p_time_panel->GetWidgetChildAtIdx(1)->Visible());
		//	p_time_panel->GetWidgetChildAtIdx(0)->Visible(!p_time_panel->GetWidgetChildAtIdx(0)->Visible());

			s64 time_remain = default_time_result;
			s64 time_total = default_time_result;


			static_cast<TimeDigitalWidget*>(p_time_panel->GetWidgetChildByName(".title_time"))->ResetTimer(time_total, (double)(time_remain));
		}

	});
}

cocos2d::Rect MiniBtnLayer::GetPosBtnMini(WidgetEntity * &obj)
{
	cocos2d::Rect rect(0, 0, 0, 0);

	Vec2 s = p_layout_mini_btn->GetSize();
	Vec2 pos_bot_left = p_layout_mini_btn->GetPosition_BottomLeft();
	rect.size = cocos2d::Size(s.x, s.y);
	rect.origin = pos_bot_left;
	obj = p_layout_mini_btn;
	return rect;
}

void MiniBtnLayer::OnShowMiniBtn(bool value)
{
	p_layout_mini_btn->Visible(value);
	if (!value)
	{
		static_cast<TaiXiuLayer*>(GetCommonScr->GetLayer(TAIXIU_LAYER))->OnShowTaiXiuLayout(false);
	}
}

void MiniBtnLayer::OnEnableBtnNoHu(bool value)
{
	p_list_btn_mini_layout->GetWidgetChildByName(".btn_mini_poker")->Visible(value);
}

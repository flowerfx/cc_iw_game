#include "AnnouceServerLayer.h"
#include "UI/AdvanceUI/wText.h"

#include "Network/Global/Globalservice.h"
#include "Screens/MainScreen.h"
AnnouceServerLayer::AnnouceServerLayer()
{
	p_annouce_server_layout = nullptr;
	p_annouce_is_running = false;
	p_have_annouce = false;

	p_speed_move = SPEED_MOVE_ANNOUCE; //in 1 sec move 100 pixel
}

AnnouceServerLayer::~AnnouceServerLayer()
{
	p_annouce_server_layout = nullptr;
	p_announce_text = "";
}

void AnnouceServerLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	p_annouce_server_layout = _base_screen->GetWidgetChildByName("common_screen.layout_anoun_server");
}

int AnnouceServerLayer::Update(float dt)
{
	if (p_annouce_is_running)
	{
		if (ScrMgr->GetCurrentMenuUI() == MAIN_SCREEN)
		{
			p_annouce_server_layout->Visible(false);
		}
		else
		{
			p_annouce_server_layout->Visible(true);
		}
	}

	if (p_have_annouce)
	{
		p_have_annouce = false;
		p_annouce_server_layout->SetActionCommandWithName("ON_SHOW", CallFunc::create([this]() {
			this->p_annouce_server_layout->ForceFinishAction();
			//
			auto w_text = this->p_annouce_server_layout->GetWidgetChildAtIdx(1);
			static_cast<TextWidget*>(w_text)->SetText(this->p_announce_text, true);
			//
			w_text->SetActionCommandWithName("ON_SHOW");
			//
			this->p_annouce_is_running = true;
			//
			auto current_pos = p_annouce_server_layout->GetPosition();
			current_pos.y = GetGameSize().height;
			p_annouce_server_layout->SetPosition(current_pos);
			
		}));
		return 1;
	}

	if (p_annouce_server_layout->Visible())
	{
		auto w_text = p_annouce_server_layout->GetWidgetChildAtIdx(1);

		auto current_pos = w_text->GetPosition();
		current_pos.x -= (p_speed_move * dt);
		w_text->SetPosition(current_pos);

		if (w_text->GetPosition_BottomRight().x < 0)
		{
			p_annouce_is_running = false;
			w_text->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, w_text]() {
				w_text->ForceFinishAction();
				this->p_annouce_server_layout->SetActionCommandWithName("ON_HIDE");
				static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->forceStopAnnouce(true);

			}));
		}

		return 1;
	}

	return 1;
}

bool AnnouceServerLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	if (!IsVisibleLayer())
		return false;

	if (type_widget == UI_TYPE::UI_BUTTON)
	{
		if (name == "close_annouce_panel")
		{
			if (p_annouce_is_running && p_annouce_server_layout->Visible())
			{
				p_annouce_is_running = false;
				auto w_text = p_annouce_server_layout->GetWidgetChildAtIdx(1);
				w_text->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, w_text]() {
					w_text->ForceFinishAction();
					this->p_annouce_server_layout->SetActionCommandWithName("ON_HIDE");
					p_announce_text = "";
				}));
				static_cast<MainScreen*>(ScrMgr->GetMenuUI(MAIN_SCREEN))->forceStopAnnouce(true);
			}
			return true;
		}
	}

	return false;
}

void AnnouceServerLayer::OnReceiveAnnouncementFromServer(const RKString & text)
{
	if (!p_annouce_server_layout->Visible())
	{
		p_announce_text = text;
		p_have_annouce = true;
	}
	else
	{

		p_announce_text.Append("    |    ");
		p_announce_text.Append(text);
		auto w_text = this->p_annouce_server_layout->GetWidgetChildAtIdx(1);
		static_cast<TextWidget*>(w_text)->SetText(this->p_announce_text, true);
	}
}

void AnnouceServerLayer::forceStopAnnouce(bool value)
{
	p_annouce_is_running = !value;
	p_annouce_server_layout->Visible(!value);
}

bool AnnouceServerLayer::IsVisibleLayer()
{
	return p_annouce_server_layout->Visible();
}
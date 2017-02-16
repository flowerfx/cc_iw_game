#include "BaseScreen.h"
#include "RKThread_Code/RKThreadWrapper.h"
#include "MainGame.h"

#include "ScreenManager.h"

BaseScreen::BaseScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
    p_menu_show_next= MENU_LAYER::MENU_NONE;

	p_have_info_change = false;
	p_thread_init_component = nullptr;
	p_init_component_complete = false;


	p_list_event.clear();
	//
	p_list_event_parallel.clear();
	p_list_temp_event_parallel.clear();

}

BaseScreen::~BaseScreen()
{
	if (p_thread_init_component)
	{
		delete p_thread_init_component;
		p_thread_init_component = nullptr;
	}

	p_list_event.clear();
	//
	p_list_event_parallel.clear();
	p_list_temp_event_parallel.clear();
}

int BaseScreen::Init()
{
	return 1;
}

int	BaseScreen::InitComponent()
{
	return 1;
}

void BaseScreen::CallInitComponent(bool use_thread)
{
	use_thread = false;
	if (!use_thread)
	{
		InitComponent();
		p_init_component_complete = true;
	}
	else
	{
		if (p_thread_init_component)
		{
			delete p_thread_init_component;
			p_thread_init_component = nullptr;
		}
		//create thread
		p_thread_init_component = new ThreadWrapper();
		p_thread_init_component->CreateThreadWrapper("thread_init_component",
			([](void * data) -> uint32
		{
			BaseScreen * scr = (BaseScreen*)data;
			scr->p_thread_init_component->OnCheckUpdateThreadWrapper
			([scr]()
			{
				scr->InitComponent();
				scr->p_init_component_complete = true;
			});
			return 1;
		}),
		(void*)this);
	}
}

int BaseScreen::Update(float dt)
{
	if (!p_init_component_complete)
		return 0;

	if (p_IsStopUpdate)
	{
		return 0;
	}

	Screen::onUpdateMenuWidget(dt);

	OnHandleInfoChange();

	HandleEvent(dt);

	return 1;
}

void BaseScreen::VisitMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onVisitMenuWidget(renderer, transform, flags);
}

void BaseScreen::DrawMenu(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	onDrawMenuWidget(renderer, transform, flags);
}

void BaseScreen::OnFinishFadeAtCurrentIdx(int idx, STATUS_MENU status)
{
	OnAllProcessFinishFadeIdx(idx, p_max_idx_process_first, status);
}


void BaseScreen::OnDeactiveCurrentMenu()
{
	
}


void BaseScreen::OnBeginFadeIn()
{

}

void BaseScreen::OnFadeInComplete()
{
	ScrMgr->SetScreenIDDirty();

	PlayMusic();
}

void BaseScreen::PlayMusic()
{
	PLAY_MUSIC("MUSIC_MUSIC01");
}

void BaseScreen::OnCallBackProcess(const RKString & str)
{
	OnProcess(str);
}
void BaseScreen::OnProcess(const RKString & str)
{
	//abstract class not handle this function
}

void BaseScreen::SetInteractiveInsteadOf(bool value, WidgetEntity * widget)
{
	return;
	widget->GetParentLayer()->SetInteractive(value);
	widget->SetInteractive(true);
}

void BaseScreen::SetInteractiveWidget(WidgetEntity * widget, bool value)
{
	return;
	widget->SetInteractive(value);
}

void BaseScreen::RestoreAllWidgetHaveNone()
{
	return;
}

void BaseScreen::CheckTouchOutSideWidget(const cocos2d::Point & p, WidgetEntity * panel, const std::function<void(void)> & func)
{
	if (panel->Visible())
	{
        ActionProcess * ac = panel->GetParentWidget();
        if(ac == nullptr)
        {
            ac = panel->GetParentLayer();
            PASSERT2(ac != nullptr , "the layer parent is null, check the code again !");
        }
        
		Vec2 world_pos = ac->GetResource()->convertToWorldSpace(panel->GetPosition_Middle());
		cocos2d::Rect rect_panel_ =
			cocos2d::Rect(
				world_pos.x - panel->GetSize().x / 2,
				world_pos.y - panel->GetSize().y / 2,
				panel->GetSize().x, panel->GetSize().y);
		if (!rect_panel_.containsPoint(p))
		{
			func();
		}
	}
}

void BaseScreen::onMyInfoChanged()
{
	p_have_info_change = true;
}

void BaseScreen::ParseUserUI()
{
	//abstrast class not handle this function!
	return;
}

void BaseScreen::ParseImgToNode(cocos2d::Node * panel, void * data, void * url, int z_order, cocos2d::Texture2D::PixelFormat fmat, bool save_cache)
{
	Image * img = (Image *)data;
	if (img && (img->getWidth() == 0 || img->getHeight() == 0))
	{
		CC_SAFE_DELETE(img);
		return;
	}
	Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, url, fmat, save_cache);
	sprite_img->setTag(99);
	if (z_order == 0)
	{
		z_order = 100;
	}
	ParseSpriteToNode(panel, sprite_img, z_order);
}

void BaseScreen::ParseImgToPanel(WidgetEntity * panel, void * data, void * url, int z_order, cocos2d::Texture2D::PixelFormat fmat , bool save_cache)
{
	Image * img = (Image *)data;
	if (img && (img->getWidth() == 0 || img->getHeight() == 0))
	{
		CC_SAFE_DELETE(img);
		return;
	}
	Sprite * sprite_img = GameMgr->CreateAvatarFromImg(data, url, fmat, save_cache);
	sprite_img->setTag(99);
	if (z_order == 0)
	{
		z_order = 100;
	}
	ParseSpriteToPanel(panel, sprite_img, z_order);
}

void BaseScreen::ParseSpriteToNode(cocos2d::Node * panel, Sprite * sprite_img, int z_order, Vec2 anchor_sp)
{
	auto parent_panel_resource = panel;

	Vec2 size_panel_clipping = panel->getContentSize();
	float scale_panel_clipping = panel->getScale();
	sprite_img->setAnchorPoint(anchor_sp);
	sprite_img->setPosition(Vec2(size_panel_clipping.x / (2 * scale_panel_clipping), size_panel_clipping.y / (2 * scale_panel_clipping)));
	Vec2 contain_size = sprite_img->getContentSize();
	sprite_img->setScale(size_panel_clipping.x / (contain_size.x * scale_panel_clipping), size_panel_clipping.y / (contain_size.y * scale_panel_clipping));

	sprite_img->removeFromParent();
	sprite_img->setTag(99);
	auto child = parent_panel_resource->getChildByTag(99);
	if (child)
	{
		parent_panel_resource->removeChildByTag(99);
	}
	parent_panel_resource->addChild(sprite_img, z_order);
}

void BaseScreen::ParseSpriteToPanel(WidgetEntity * panel, Sprite * sprite_img, int z_order, bool keep_origin_size, Vec2 anchor_sp)
{
	auto parent_panel_resource = panel->GetResource();
	auto avatar_default = panel->GetWidgetChildByName(".avatar_default");
	//if (avatar_default)
	//{
		//avatar_default->Visible(false);
	//}

	Vec2 size_panel_clipping = panel->GetSize();
	Vec2 scale_panel_clipping = panel->GetScale();
	sprite_img->setAnchorPoint(anchor_sp);
	sprite_img->setPosition(Vec2(size_panel_clipping.x / (2 * scale_panel_clipping.x), size_panel_clipping.y / (2 * scale_panel_clipping.y)));
	Vec2 contain_size = sprite_img->getContentSize();
	if (!keep_origin_size)
	{
		sprite_img->setScale(size_panel_clipping.x / (contain_size.x * scale_panel_clipping.x), size_panel_clipping.y / (contain_size.y * scale_panel_clipping.y));
	}
	sprite_img->removeFromParent();
	sprite_img->setTag(99);
	auto child = parent_panel_resource->getChildByTag(99);
	if (child)
	{
		parent_panel_resource->removeChildByTag(99);
	}
	parent_panel_resource->addChild(sprite_img, z_order);
}

void BaseScreen::OnHandleInfoChange()
{
	if (p_have_info_change)
	{
		ParseUserUI();
		p_have_info_change = false;
	}
}

void BaseScreen::OnReceiveFriendList()
{
	//abstrast class not handle this function
}

void BaseScreen::ClearComponent()
{
	//abstrast class not handle this function
}

void BaseScreen::OnKeyBack()
{
	
}

void BaseScreen::HandleEvent(float dt)
{
	if (p_list_event.size() > 0)
	{
        float time = p_list_event[0].time_action;
        time-= dt;
        p_list_event[0].time_action = time;
        if(time <= 0)
        {
            p_list_event[0].event_action(this);
            p_list_event.erase(p_list_event.begin());
        }
	}

	for (auto ac : p_list_temp_event_parallel)
	{
		p_list_event_parallel.push_back(ac);
	}
	p_list_temp_event_parallel.clear();
	if (p_list_event_parallel.size() > 0)
	{
		for (auto &ac : p_list_event_parallel)
		{
			float time = ac.time_action;
			time -= dt;
			ac.time_action = time;
			if (time <= 0 && ac.isDone == false)
			{
				ac.event_action(this);
				ac.isDone = true;
			}
		}
		p_list_event_parallel.erase(
			std::remove_if(p_list_event_parallel.begin(), p_list_event_parallel.end(), [](const EventQueue & o) 
		{
				return o.isDone;		
		}), p_list_event_parallel.end());
	}
}

void BaseScreen::PushEvent(const std::function<void(BaseScreen* scr)> & event_action, float time , bool isParallel)
{
	if (!isParallel)
	{
		p_list_event.push_back(EventQueue(event_action, time));
	}
	else
	{
		p_list_temp_event_parallel.push_back(EventQueue(event_action, time));
	}
}
void BaseScreen::SetActionAfterShow(int action)
{
	//
}
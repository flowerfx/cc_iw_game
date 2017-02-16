#include "LoadingCircleLayer.h"
#include "Screens/CommonScreen.h"
#define MAX_TIME_CLOSE 3
LoadingCircleLayer::LoadingCircleLayer()
{
	p_time_show = -1;
}

LoadingCircleLayer::~LoadingCircleLayer()
{

}

void LoadingCircleLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);


	p_loading_circle_layer = _base_screen->GetWidgetChildByName("common_screen.loading_circle_layer");
	p_loading_circle = p_loading_circle_layer->GetWidgetChildByName(".loading_circle");
}

void LoadingCircleLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	p_loading_circle_layer->Visible(true);
	p_loading_circle->SetActionCommandWithName("ROTATE");
	p_time_show = MAX_TIME_CLOSE;
}

void LoadingCircleLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	p_loading_circle_layer->Visible(false);
	p_loading_circle->ForceFinishAction();
}

bool LoadingCircleLayer::IsVisibleLayer()
{
	return p_loading_circle_layer->Visible();
}

int LoadingCircleLayer::Update(float dt)
{
	if (p_time_show >= 0)
	{
		p_time_show -= dt;
		if (p_time_show < 0)
		{
			OnHideLayer();
			p_time_show = -1;
		}
		return 0;
	}
	return 1;
}

#include "CommonLayer.h"

CommonLayer::CommonLayer()
{
	_base_screen = nullptr;
}

CommonLayer::~CommonLayer()
{
	_base_screen = nullptr;
}

bool CommonLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	return false;
}

void CommonLayer::InitLayer(BaseScreen * screen)
{
	_base_screen = screen;
}

int CommonLayer::Update(float dt)
{
	return 1;
}

void CommonLayer::OnShowLayer(const std::function<void(void)> & call_back )
{
	return;
}

void CommonLayer::OnHideLayer(const std::function<void(void)> & call_back )
{
	return;
}

void CommonLayer::Reset()
{
	return;
}
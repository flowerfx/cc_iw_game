#include "DialogScreen.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wButton.h"
DialogScreen::DialogScreen()
{
	p_menu_come_from = MENU_LAYER::MENU_NONE;
}


DialogScreen::~DialogScreen()
{
	
}

int DialogScreen::Init()
{
	return 1;
}

int DialogScreen::Update(float dt)
{
	if (BaseScreen::Update(dt) == 0)
	{
		return 0;
	}


	return 1;
}


void DialogScreen::OnProcessWidgetChild(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
}

void DialogScreen::OnDeactiveCurrentMenu()
{
	ScrMgr->CloseCurrentMenu(CUSTOM_SCREEN);
	p_menu_show_next = MENU_NONE;
}

#include "EmoBoardLayer.h"
#include "UI/AdvanceUI/wText.h"
#include "UI/AdvanceUI/wTextField.h"

#include "Network/Global/Globalservice.h"
#include "Screens/CommonScreen.h"

#include "Common/EmotionBoard.h"
#define TIME_MOVE 0.5f
EmoBoardLayer::EmoBoardLayer()
{
	_emotion_board_layout = nullptr;
	_emotion_board = nullptr;

	_isOnMove = false;
}

EmoBoardLayer::~EmoBoardLayer()
{
	_emotion_board_layout->GetResource()->removeAllChildren();
}

void EmoBoardLayer::InitLayer(BaseScreen * screen)
{
	CommonLayer::InitLayer(screen);
	cocos2d::Size emoboard_size = GetGameSize();
	_emotion_board = EmotionBoard::create(cocos2d::Size(emoboard_size.width, emoboard_size.height / 2));
	_emotion_board->setAnchorPoint(Vec2(0, 0));
	_emotion_board->setPosition(Vec2(0, 0));


	_emotion_board_layout = _base_screen->GetWidgetChildByName("common_screen.emotion_board");
	_emotion_board_layout->GetResource()->addChild(_emotion_board);
}

bool EmoBoardLayer::ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget)
{
	return false;
}

void EmoBoardLayer::CheckTouchMenuBegin(const cocos2d::Point & p)
{
	if (_emotion_board_layout->Visible() && !_isOnMove)
	{
		_base_screen->CheckTouchOutSideWidget(p, _emotion_board_layout,
			[&](void) {
			OnHideLayer();
		});
	}

}

void EmoBoardLayer::OnHideLayer(const std::function<void(void)> & call_back)
{
	if (!_emotion_board_layout->Visible() || _isOnMove)
	{
		return;
	}
	_isOnMove = true;
	_emotion_board_layout->SetActionCommandWithName("ON_HIDE", CallFunc::create([this, call_back]() {
		_emotion_board_layout->ForceFinishAction();
		_isOnMove = false;
		if (call_back)
		{
			call_back();
		}
	}));


	auto current_screen = ScrMgr->GetMenuUI(ScrMgr->GetCurrentMenuUI());
	auto current_size = _emotion_board_layout->GetSize();
	auto current_pos = current_screen->getPosition();
	Vec2 pos_to = Vec2(current_pos.x, current_pos.y - current_size.y);

    
    
    auto action = Sequence::createWithTwoActions(MoveTo::create(TIME_MOVE, pos_to) , CallFunc::create([current_screen]() {
        current_screen->setPosition(Vec2(0,0));
    }));
    
	current_screen->stopAllActions();
	current_screen->runAction(action);
}

void EmoBoardLayer::OnShowLayer(const std::function<void(void)> & call_back)
{
	if (_emotion_board_layout->Visible() || _isOnMove)
	{
		return;
	}
	_isOnMove = true;
	_emotion_board_layout->SetActionCommandWithName("ON_SHOW", CallFunc::create([this, call_back]() {
		_emotion_board_layout->ForceFinishAction();
		_isOnMove = false;
		if (call_back)
		{
			call_back();
		}
	}));


	auto current_screen = ScrMgr->GetMenuUI(ScrMgr->GetCurrentMenuUI());

	auto current_size = _emotion_board_layout->GetSize();
	auto current_pos = current_screen->getPosition();
	Vec2 pos_to = Vec2(current_pos.x, current_pos.y + current_size.y);

	auto action = MoveTo::create(TIME_MOVE, pos_to);
	current_screen->stopAllActions();
	current_screen->runAction(action);
}

void EmoBoardLayer::SetCallBack(const std::function<void(int)> & func)
{
	_emotion_board->setHandleChat(func);
}

bool EmoBoardLayer::IsVisibleLayer()
{
	return _emotion_board_layout->Visible();
}

void EmoBoardLayer::ResetPosScreenWhenHaveKeyboard()
{
#if ! defined _WIN32_
    auto current_screen = ScrMgr->GetMenuUI(ScrMgr->GetCurrentMenuUI());
    current_screen->stopAllActions();
    auto current_size = _emotion_board_layout->GetSize();
    auto current_pos = current_screen->getPosition();
    current_screen->setPosition(Vec2(current_pos.x , current_size.y));
#endif
}
#ifndef __EMO_BOARD_LAYER_H__
#define __EMO_BOARD_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;
class EmotionBoard;
class EmoBoardLayer : public CommonLayer
{
public:
	EmoBoardLayer();
	virtual ~EmoBoardLayer();
private:
	EmotionBoard*		_emotion_board;
	WidgetEntity *		_emotion_board_layout;

	bool				_isOnMove;
public:
	/*
	handle show confirm box
	*/
	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;

	void SetCallBack(const std::function<void(int)> & func);
	void CheckTouchMenuBegin(const cocos2d::Point & p);
    void ResetPosScreenWhenHaveKeyboard();
};

#endif //__EMO_BOARD_LAYER_H__


#ifndef __SHOP_AVATAR_LAYER_H__
#define __SHOP_AVATAR_LAYER_H__

#include "Screens/CommonScreens/CommonLayer.h"

using namespace Utility;
using namespace Utility::UI_Widget;
namespace iwinmesage
{
	class AvatarCat;
	class Avatar;
	class AvatarList;
}
class ShopAvatarLayer : public CommonLayer
{
public:
	ShopAvatarLayer();
	virtual ~ShopAvatarLayer();
private:
	cocos2d::Map<std::string, Sprite*>	p_list_avatar_download;

	WidgetEntity * p_shop_avatar_layout;
	WidgetEntity * p_shop_avatar_list_cat;
	WidgetEntity * p_shop_avatar_list;
	WidgetEntity * p_confirm_buy_avatar_layout;
	iwinmesage::AvatarList   * p_avatar_list;
	int			   p_current_avatar_idx_buy;
	//determine when change state avatar cat to know download file or not;
	int			   p_stage_avatar_cat_list;

protected:

public:

	virtual void InitLayer(BaseScreen * screen) override;
	virtual bool ProcessWidget(const RKString & name, UI_TYPE type_widget, WidgetEntity * _widget) override;
	virtual void OnShowLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual void OnHideLayer(const std::function<void(void)> & call_back = nullptr) override;
	virtual bool IsVisibleLayer() override;
	virtual int  Update(float dt) override;
	virtual void Reset() override;

	void onReceiveAvatarCatList(void * data);
	void onReceiveAvatarList(void * data);
	void onBuyingAvatarResult(void * data);

	void OnParseInfoAvaCatToLayout(WidgetEntity * widget, iwinmesage::AvatarCat * info);
	void OnParseInfoAvaToLayout(WidgetEntity * widget, iwinmesage::Avatar * info, bool request_avatar = false);
	void CleanUpListAvatar();

	bool IsConfirmBuyAvatarVisible();
	void OnCloseConfirmBuyAvatar(const std::function<void(void)> & call_back = nullptr);
};

#endif //__SHOP_AVATAR_LAYER_H__


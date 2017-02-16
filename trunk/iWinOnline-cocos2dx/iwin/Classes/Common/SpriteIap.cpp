#include "Social/SocialManager.h"
#include "SpriteIap.h"
#include "SpriteIapMgr.h"

using namespace Utility;
SpriteIap* SpriteIap::create(const char* default_name, const char* url, const char* name_save)
{
	SpriteIap* rs = new SpriteIap();
	rs->initWithSpriteFrameName(default_name);
	rs->setURL(url);
	rs->setNameSave(name_save);
	rs->autorelease();
	return rs;
}


void SpriteIap::startDownload()
{
	if (_name_save == "")
	{
		SocialMgr->DownloadFile(_url, _name_save, CC_CALLBACK_3(SpriteIap::doneDownload, this));
	}
	else
	{
		GetSpriteIapMgr()->registerDownload(this, _url, _name_save);
	}
}

void SpriteIap::startDownload(std::string url)
{
	setURL(url);
	startDownload();
}

void SpriteIap::setFixedSize(cocos2d::Size fixed_size)
{
	_fixed_size = fixed_size;
}

void SpriteIap::doneDownload( void* data, void* str, int tag)
{
	CCLOG("Done download %s", _name_save.c_str());
	
	Image * img = (Image *)data;
	Texture2D* tex = new Texture2D();
	tex->initWithImage(img, cocos2d::Texture2D::PixelFormat::RGB5A1);
	initWithTextureDone(tex);
	CC_SAFE_DELETE(img);
}

void SpriteIap::initWithTextureDone(Texture2D* tex)
{
	Vec2 old_anchor = getAnchorPoint();
	cocos2d::Size old_size = getContentSize();
	old_size = cocos2d::Size(old_size.width * getScaleX(), old_size.height * getScaleY());
	initWithTexture(tex);
	setAnchorPoint(old_anchor);
	cocos2d::Size new_size = getContentSize();
	if (_fixed_size.width != 0 && _fixed_size.height != 0)
	{
		setScaleX(_fixed_size.width / new_size.width);
	}
	else
	{

		float ratio = 1.0f;
		float delta_w, delta_h;
		delta_w = old_size.width / new_size.width;
		delta_h = old_size.height / new_size.height;
		if (delta_w < delta_h)
		{
			setScale(delta_w);
		}
		else
		{
			setScale(delta_h);
		}
	}

}
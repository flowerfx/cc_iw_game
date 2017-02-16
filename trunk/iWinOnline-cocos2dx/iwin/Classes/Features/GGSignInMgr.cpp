//
//  GGSignInMgr.cpp
//  iwin_ios
//
//  Created by LA NGOC MINH TRUNG on 10/4/16.
//
//

#include "GGSignInMgr.h"
#include "MainGame.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "../Network/Core/JsonMessage.h"
#include "hellocpp/Java_org_cocos2dx_cpp_AndroidPlatform.h"
#include "Platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "Platform/ios/Platform_ios.h"
#endif

#if defined WP8 || defined OS_W8 || defined OS_W10
#include "Platform/WindowsPhone/WPPlatform.h"
#include "../Network/JsonObject/ServiceSocial/GoogleUser.h"
#endif

GGSignInMgr* GGSignInMgr::s_instance = nullptr;

GGSignInMgr* GGSignInMgr::getInstance()
{
    if (!s_instance)
    {
        s_instance = new GGSignInMgr();
    }
    return s_instance;
}

GGSignInMgr::GGSignInMgr()
{
    
}

GGSignInMgr::~GGSignInMgr()
{
    
}


void GGSignInMgr::init()
{
    
}

void GGSignInMgr::signIn()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    CCLOG("======= on google sign in android");
    nativeGGSignIn();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    Platform::ios::ggSignIn();
#elif defined WP8 || defined OS_W8 || defined OS_W10
    WPPlatform::WPHelper::loginGoogle([](void* sender, std::string access_token) {
        GameMgr->Network_loginWithToken(MainGame::TYPE_GOOGLE, access_token);
    });
#endif
}

void GGSignInMgr::signOut()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    CCLOG("======= on google sign out android");
    nativeGGSignOut();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    Platform::ios::ggSignOut();
#endif
}

bool GGSignInMgr::isConnected()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    return nativeIsGGConnected();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    return Platform::ios::getIsConnected();
#else 
	return false;
#endif
	return false;
}

//void GGSignInMgr::getGGToken(GGTokenCallBack callback)
//{
//#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
////    Platform::onGGSignInCompleted(nativeGetGGToken());
//    if(callback)
//    {
//        callback(nativeGetGGToken());
//    }
//#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
//    if(callback)
//    {
//        callback(Platform::ios::getAccessToken());
//    }
//#endif
//}

std::string GGSignInMgr::getGGToken()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    return nativeGetGGToken();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    return Platform::ios::getAccessToken();
#else
	return "";
#endif
}

void GGSignInMgr::getProfile(ProfileCallBack callback)
{
#if defined WP8 || defined OS_W8 || defined OS_W10
WPPlatform::WPHelper::getProfileGoogle([callback](std::string access_token) {
	GoogleUser * googleData = new GoogleUser();
	googleData->toData(access_token);
	if (callback)
	{
		callback(googleData->getName());
	}
	CC_SAFE_DELETE(googleData);
});
#endif
}


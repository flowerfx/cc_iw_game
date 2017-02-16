//
//  GCSignInMgr.cpp
//  iwin_ios
//
//  Created by LA NGOC MINH TRUNG on 11/18/16.
//
//

#include "GCSignInMgr.h"
#include "cocos2d.h"
#include "../Platform/Platform.h"
#include "MainGame.h"
#ifdef WINDOWS
#include "Platform/WindowsPhone/WPPlatform.h"
#endif // WINDOWS

USING_NS_CC;

GCSignInMgr* GCSignInMgr::s_instance = nullptr;

GCSignInMgr* GCSignInMgr::getInstance()
{
    if (!s_instance)
    {
        s_instance = new GCSignInMgr();
    }
    return s_instance;
}

GCSignInMgr::GCSignInMgr()
{
    
}

GCSignInMgr::~GCSignInMgr()
{
    
}


void GCSignInMgr::init()
{
#ifdef SDKBOX_ENABLED
    sdkbox::PluginSdkboxPlay::init();
    PluginSdkboxPlay::setListener(this);
#endif
}

void GCSignInMgr::signIn()
{
#ifdef SDKBOX_ENABLED
    sdkbox::PluginSdkboxPlay::signin();
#endif
}

void GCSignInMgr::signOut()
{
#ifdef SDKBOX_ENABLED
    sdkbox::PluginSdkboxPlay::signout();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
	WPPlatform::WPHelper::logoutGoogle();
#endif
}

bool GCSignInMgr::isSignedIn()
{
#ifdef SDKBOX_ENABLED
    return sdkbox::PluginSdkboxPlay::isSignedIn();
#endif
	return false;
}

bool GCSignInMgr::isSupportSignInGC()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    return true;
#else
    return false;
#endif
}

void GCSignInMgr::getAccessToken(GCCallBack callback)
{
    Platform::generateAppleVerify(callback);
}

#ifdef SDKBOX_ENABLED

void GCSignInMgr::onConnectionStatusChanged( int status )
{
    CCLOG("connection status change: %d", status);

    char str[256];
    sprintf(str, "Connection status: %d. Is Connected: %d", status, sdkbox::PluginSdkboxPlay::isConnected() ? 1 : 0 );

    if ( status == 1000 ) {
        //here continue get info send to server for authen third party
        getAccessToken([=](std::string accessToken){
            GameMgr->Network_loginWithToken(MainGame::TYPE_APPLEID, accessToken);
        });
    }
}

void GCSignInMgr::onScoreSubmitted( const std::string& leaderboard_name, int score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday )
{
    
    
}

void GCSignInMgr::onIncrementalAchievementUnlocked( const std::string& achievement_name )
{
    
}

void GCSignInMgr::onIncrementalAchievementStep( const std::string& achievement_name, int step )
{
    
}

void GCSignInMgr::onAchievementUnlocked( const std::string& achievement_name, bool newlyUnlocked )
{
    
}


#endif



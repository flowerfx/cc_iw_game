//
//  GCSignInMgr.h
//  iwin_ios
//
//  Created by LA NGOC MINH TRUNG on 11/18/16.
//
//

#ifndef GCSignInMgr_h
#define GCSignInMgr_h

#include <stdio.h>
#include <string>
#include <functional>

#ifdef SDKBOX_ENABLED
#include "PluginSdkboxPlay/PluginSdkboxPlay.h"
using namespace sdkbox;

class GCSignInMgr : public sdkbox::SdkboxPlayListener
#else
class GCSignInMgr
#endif
{
private:
    static GCSignInMgr*	s_instance;
    
    GCSignInMgr();
    
public:
    
    virtual ~GCSignInMgr();
    
    static GCSignInMgr* getInstance();
    void init();
    void signIn();
    void signOut();
    bool isSignedIn();
    
    typedef std::function<void(std::string)> GCCallBack;
    void getAccessToken(GCCallBack callback);
    
    bool isSupportSignInGC();
    
#ifdef SDKBOX_ENABLED
protected:
    /**
     * Call method invoked when the Plugin connection changes its status.
     * Values are as follows:
     *   + GPS_CONNECTED:       successfully connected.
     *   + GPS_DISCONNECTED:    successfully disconnected.
     *   + GPS_CONNECTION_ERROR:error with google play services connection.
     */
    virtual void onConnectionStatusChanged( int status ) override;
    
    /**
     * Callback method invoked when an score has been successfully submitted to a leaderboard.
     * It notifies back with the leaderboard_name (not id, see the sdkbox_config.json file) and the
     * subbmited score, as well as whether the score is the daily, weekly, or all time best score.
     * Since Game center can't determine if submitted score is maximum, it will send the max score flags as false.
     */

    virtual void onScoreSubmitted( const std::string& leaderboard_name, int score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday ) override;
    
    /**
     * Callback method invoked when the request call to increment an achievement is succeessful and
     * that achievement gets unlocked. This happens when the incremental step count reaches its maximum value.
     * Maximum step count for an incremental achievement is defined in the google play developer console.
     */
    virtual void onIncrementalAchievementUnlocked( const std::string& achievement_name ) override;
    
    /**
     * Callback method invoked when the request call to increment an achievement is successful.
     * If possible (Google play only) it notifies back with the current achievement step count.
     */
    virtual void onIncrementalAchievementStep( const std::string& achievement_name, int step ) override;
    
    /**
     * Call method invoked when the request call to unlock a non-incremental achievement is successful.
     * If this is the first time the achievement is unlocked, newUnlocked will be true.
     */
    virtual void onAchievementUnlocked( const std::string& achievement_name, bool newlyUnlocked ) override;
#endif
};

#define GetGCSignInMgr() GCSignInMgr::getInstance()

#endif /* GCSignInMgr_h */

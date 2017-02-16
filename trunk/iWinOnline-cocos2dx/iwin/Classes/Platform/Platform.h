//
//  Platform.hpp
//  iwin_ios
//
//  Created by truongps on 5/5/16.
//
//

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <sstream>
#include "cocos2d.h"
#include "network/HttpClient.h"
#include "DisplayView.h"

#include "Network/JsonObject/Facebook/FacebookMyInfo.h"

namespace Platform
{

	//This class for Android platform
	class GooglePlayServices
	{
	public:
		static bool isSignedIn();
		static void signIn();
		static void signOut();
		static std::string getPlayerID();

		static void submitScore(const char *leaderboardID, long score);
		static void unlockAchievement(const char *achievementID);
		static void incrementAchievement(const char *achievementID, int numSteps);
		static void showAchievements();
		static void showLeaderboards();
		static void showLeaderboard(const char *leaderboardID);
	};
	//This class for IOS platform
	class GameCenter
	{
	public:

		static void signIn();
		static void showLeaderboard();
		static void showAchievements();
		static void submitScore(int scoreNumber, cocos2d::__String leaderboardID);
		static void unlockAchievement(cocos2d::__String achievementID);
		static void unlockAchievement(cocos2d::__String achievementID, float percent);
		static void resetPlayerAchievements();

	};

	class Notifications
	{
	public:

		static void scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, int notificationTag);
		static void scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, cocos2d::__String notificationAction, int notificationTag);
		static void scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, int repeatInterval, int notificationTag);
		static void scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, cocos2d::__String notificationAction, int repeatInterval, int notificationTag);
		static void unscheduleAllLocalNotifications();
		static void unscheduleLocalNotification(int notificationTag);

	};

	class GPSLocation
	{
	public:
		GPSLocation(double longitude, double latitude)
		{
			this->longitude = longitude;
			this->latitude = latitude;
		}

		double latitude;
		double longitude;

		std::string toString()
		{
			std::ostringstream oss;
			oss << "longitude: " << longitude << ", latitude: " << latitude;
			return  oss.str();
		}
	};

	class AppInfo
	{
	public:
//        AppInfo();
		virtual ~AppInfo() {};
		virtual std::string getName();
		virtual std::string getVersion();
		virtual std::string getChanel();
		virtual std::string getLanguage();
		virtual std::string getBundleID();
		virtual int getAppWidth();
		virtual int getAppHeight();
		virtual std::string getNotifyKey();
	};

	class DeviceInfo
	{
	public:
//        DeviceInfo() ;
		virtual ~DeviceInfo() {};
		virtual std::string getCarrier();
		virtual std::string getDeviceName();
		virtual int getIsJailBreak();
		virtual std::string getNet();
		virtual std::string getOS();
		virtual std::string getOSVersion();
		virtual std::string getDeviceUDID();
        virtual std::string getMacAddress();
        virtual std::string getAdvertisingID();
        //for android
        virtual std::string getAndroidID();
        virtual std::string getSimSeri();
        virtual std::string getFingerPrint();
        virtual std::string getSubscriberID();
        virtual std::string getBluetoothAddress();
	};

	void getCurrentLocation(std::function<void(const double, const double)> _resultCallback);
	void makeCall(std::string phoneNumber, std::string showName);
	void showWeb(const std::string url);
	bool isWebviewShow();
	void closeWebview();
	void showSMSComposer(std::string content, std::string toPhoneNumber);
	void makeCall(std::string phone_number);
	void sendSMS(std::string content, std::string toPhoneNumber);
	void vibrate(int milis);
	void dismissSplash();
	std::string getVersion();
	std::string getPackageName();
	std::string getAppName();
    std::string makeSHA1Hash(std::string input);
	std::string getClipboardText(std::function<void(std::string)> return_handler);
	void        setClipboardText(std::string text);

	void getListAppEmail(std::vector<std::string>& listAppEmail);
	void openAppEmailWithName(std::string appName);
	void openEmail(std::string send_to, RKUtils::RKString title);

	//for ios game center
	typedef std::function<void(std::string)> TokenCallBack;
	void generateAppleVerify(TokenCallBack callback);

	void onGGSignInCompleted(std::string token);

	void handleSMSSucceed(std::string phone_number, std::string sms_body);
	void handleSMSFail();

	AppInfo* getAppInfo();
	DeviceInfo* getDeviceInfo();
	void HttpRequest(std::string url, std::function<void(std::string)> callback);
	void downloadAppAndSetup(std::string link, std::function<void()> actionCallback);

	void RateApp(std::string link);
	void LoadImageFromFile(const std::function<void(const char * , unsigned int)> & call_back);
	void CaptureImageFromCamera(const std::function<void(const char *, unsigned int)> & call_back);

	//show or hide btn clear native (wp/android/ios) and set position of btn
	void OnShowNativeClearTextBtn(bool visible, float X, float Y);

	namespace FBUtils
	{
		bool isFacebookLoggedIn();
		std::string getAccessToken();
        void getFacebookMyInfo(const std::function<void(bool, void*)>& call_back);
		void loginFacebook(const std::function<void(bool, void*)>& call_back);
		void likePage(std::string url, const std::function<void(bool, void*)>& call_back);
		void postWall(std::string json_data, const std::function<void(bool, void*)>& call_back);
		void shareLink(std::string link, const std::function<void(bool, void*)> & call_back);
//		void showInviteFriendDialog();
		void getInvitableFriends(const std::function<void(bool, void*)> & call_back);
		void InviteFriend(std::vector<std::string> ls_id, const std::function<void(bool, void*)> & call_back);


		void requestSendGift(std::string fbID, const std::function<void(bool)> & call_back);
		void requestSendGift(std::vector<std::string> facebookIDs, const std::function<void(bool, void*)> & call_back);
		void requestAskGift(std::vector<std::string> facebookIDs, const std::function<void(bool, void*)> & call_back);
		void acceptSendGift(std::string fbID, const std::function<void(bool)> & call_back);
		void acceptSendGift(std::vector<std::string> facebookIDs, const std::function<void(bool, void*)> & call_back);
		void sendRequestWith(std::vector<std::string> facebookIDs, std::string jsonData, std::string title, std::string msg, const std::function<void(bool, void*)> & call_back);
	}

}

#endif /* __PLATFORM_H__ */

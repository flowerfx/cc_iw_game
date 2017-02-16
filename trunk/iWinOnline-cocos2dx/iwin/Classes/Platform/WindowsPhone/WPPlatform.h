#ifndef __WPPLATFORM_H__
#define __WPPLATFORM_H__

#include "cocos2d.h"
#include "..\Platform.h"
#include <ppltasks.h>
//using namespace Windows;
USING_NS_CC;
//
#define URL_FACEBOOK_LOGIN "https://www.facebook.com/dialog/oauth"
#define URL_FACEBOOK_DERECT "https://www.facebook.com/connect/login_success.html"
#define GOOGLE_CLIENT_ID  "638139229792-h4pua8mu7mkenvk3tvjrgukj9bff9hol.apps.googleusercontent.com"
#define GOOGLE_CLIENT_SECRET  "-1g2IuLyrhyWZ4vAEFhnipbE"
#ifndef CC_WINDOWS_PHONE_8_1
#define GOOGLE_CLIENT_URL_CALBACK  "urn:ietf:wg:oauth:2.0:oob"
#else
#define GOOGLE_CLIENT_URL_CALBACK  "http://localhost"
#endif
#define GOOGLE_CLIENT_URL_PROFILE  "https://www.googleapis.com/oauth2/v1/userinfo?access_token="
#define  FB_GRAPH_FRIEND_LIST  "me/friends"
#define FB_GRAPH_PATH  "https://graph.facebook.com/"
#define MAX_INVITE_FRIEND  25
#define  GRAPH_URL_REQUEST  "https://m.facebook.com/dialog/apprequests?"


using namespace Platform;
namespace iwinmesage
{
	class FacebookFriendList;
}

namespace WPPlatform
{
#if defined WP8 || defined OS_W8 || defined OS_W10
	class WPHelper
	{
	public:
		//static System::Action^ actionShowWebView;
		static std::function<void(std::string, std::function<void(void*,std::string)>)> actionShowWebView;
		static std::function<void()> actionCloseWebView;
		static std::function<bool()> isWebViewShow;
		static std::function<void()> hideImageLoadGame;
		static std::function<void(bool, float X, float Y)> onShowBtnClear;
		static void sendSMS(std::string content, std::string toNumber);
		static void makeCall(std::string _displayName, std::string _phoneNumber);
		static void vibrate(int milisecond);

		static std::wstring stows(std::string s);
		static void showWebView(std::string url,  std::function<void(void*,std::string)> callback);
		static void closeWebView();
		static void loginFacebook(std::function<void(void*, std::string)> callback);
		static void loginGoogle(std::function<void(void*, std::string)> callback);
		static void Finalize(Windows::ApplicationModel::Activation::WebAuthenticationBrokerContinuationEventArgs^ obj);
		static void getProfileGoogle(std::function<void(std::string)> onCompleted);

		static void getClipboardText(std::function<void(std::string)> return_handler);
		static void setClipboardText(std::string text);
		static void getInvitableFriends(const std::function<void(bool,void*)> & call_back);
		static void inviteFriendList(std::string accessToken,std::vector<string> sendIDs, std::string title, std::string link, std::function<void(std::string)> onCompleted);
		static std::string getFBAcessToken();
		static std::string getGoogleAcessToken();
		static void logoutFacebook();
		static void logoutGoogle();
		static void clearWebviewCookie(std::string link);

		static void sendEmail(std::string email_to, std::string title);
	private:
		static std::function<void(void*, std::string)> callbackGoogle;
		static void parseAssesTokenFB(std::string webAuthResultResponseData, std::string& accessToken, std::string& expiresIn);
		static void parseAcessTokenGG(Platform::String^ code, std::function<void(void*, std::string)> callBack);
		static void getSessionGoogle(Windows::ApplicationModel::Activation::WebAuthenticationBrokerContinuationEventArgs^ result);
		static std::string getCodeGoogle(Platform::String^ result);
		static void GetGraphObject(std::string path, std::string prm, std::string token, std::function<void(std::string)> onComplete);
		static void ParseFriendList(std::string json, iwinmesage::FacebookFriendList* lsFriend, const std::function<void(bool, void*)> & call_back);
		static std::map<std::string, std::string> ParseParamsFBInvite(string url);
		
		
	};

	class WPAppInfo :public AppInfo
	{
	public:
		WPAppInfo();
		virtual ~WPAppInfo();
		virtual std::string getName() ;
		virtual std::string getLanguage() ;

		virtual std::string getBundleID() ;


		virtual float getAppHieght() ;

		virtual std::string getNotifyKey() ;

	};

	class WPDeviceInfo :public DeviceInfo
	{
	public:
		 WPDeviceInfo();
		 virtual ~WPDeviceInfo();
		virtual std::string getCarrier() ;
		virtual  std::string getDeviceName() ;
		virtual int getIsJailBreak() ;
		virtual std::string getNet() ;
		virtual std::string getOS() ;
		virtual std::string getOsVersion() ;
		virtual std::string getDeviceUDID() ;
		virtual std::string getMacAddress();
	private:
		std::string deviceName;
		std::string os;
		std::string osVersion;
		std::string udid;
	};
#endif
}
#endif
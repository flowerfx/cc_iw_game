//
//  Platform.cpp
//  iwin_ios
//
//  Hiện thực các tính năng call qua các platform.
//
//
//This class for Android platform
#include "Platform.h"
#include "Screens/ScreenManager.h"
#include "../MainGame.h"
#include "Features/FacebookMgr.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
#include "WindowsPhone\WPPlatform.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID 
#include "hellocpp/Java_org_cocos2dx_cpp_AndroidPlatform.h"
#include "Platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#include "Platform/android/AndroidPlatform.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "ios/Platform_ios.h"
#include "ios/IosPlatform.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#include "win32/DesktopPlatform.h"
#endif

#include "NetWork/Global/GlobalService.h"
#include "platform/android/CCApplication-android.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
using namespace Platform;
using namespace WPPlatform;
using namespace network;
#endif

namespace Platform
{

	bool GooglePlayServices::isSignedIn()
	{
        return true;
	}
    
	void GooglePlayServices::signIn()
    {
    }
    
	void GooglePlayServices::signOut()
    {
    }
    
    std::string GooglePlayServices::getPlayerID()
    {
        return "";
    }
    
	void GooglePlayServices::submitScore(const char *leaderboardID, long score) {}
	void GooglePlayServices::unlockAchievement(const char *achievementID) {}
	void GooglePlayServices::incrementAchievement(const char *achievementID, int numSteps) {}
	void GooglePlayServices::showAchievements() {}
	void GooglePlayServices::showLeaderboards() {}
	void GooglePlayServices::showLeaderboard(const char *leaderboardID) {}


	void GameCenter::signIn() {}
	void GameCenter::showLeaderboard() {}
	void GameCenter::showAchievements() {}
	void GameCenter::submitScore(int scoreNumber, cocos2d::__String leaderboardID) {}
	void GameCenter::unlockAchievement(cocos2d::__String achievementID) {}
	void GameCenter::unlockAchievement(cocos2d::__String achievementID, float percent) {}
	void GameCenter::resetPlayerAchievements() {}


	void Notifications::scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, int notificationTag) {}
	void Notifications::scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, cocos2d::__String notificationAction, int notificationTag) {}
	void Notifications::scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, int repeatInterval, int notificationTag) {}
	void Notifications::scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, cocos2d::__String notificationAction, int repeatInterval, int notificationTag) {}
	void Notifications::unscheduleAllLocalNotifications() {}
	void Notifications::unscheduleLocalNotification(int notificationTag) {}


	void getCurrentLocation(std::function<void(const double, const double)> _resultCallback)
	{
	}

	void makeCall(std::string phoneNumber, std::string showName)
	{
#if defined CC_WINDOWS_PHONE_8_1
		WPPlatform::WPHelper::makeCall(phoneNumber, showName);
#elif defined CC_PLATFORM_IOS

#endif
	}
	void showWeb(const std::string url) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID	
		nativeShowWeb(url, "iWinOnline");
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeShowWebView(url);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		WPPlatform::WPHelper::showWebView(url,nullptr);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		Application::sharedApplication()->openURL(url);
#endif
	}

	bool isWebviewShow()
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID	

#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		return WPPlatform::WPHelper::isWebViewShow? WPPlatform::WPHelper::isWebViewShow():false;
#endif
		return false;
	}

	void closeWebview()
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID	
		
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		WPPlatform::WPHelper::closeWebView();
#endif
	}

	void showSMSComposer(std::string content, std::string toPhoneNumber)
	{

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		nativeShowSmsComposer(toPhoneNumber, content);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeShowSMSComposer(toPhoneNumber, content);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		WPPlatform::WPHelper::sendSMS(content, toPhoneNumber);
#endif
	}

    void sendSMS(std::string toPhoneNumber, std::string content)
	{

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		nativeSendSMS(toPhoneNumber, content);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeShowSMSComposer(toPhoneNumber, content);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		WPPlatform::WPHelper::sendSMS(content, toPhoneNumber);
#endif
	}

	void makeCall(std::string phone_number)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		nativeMakeCall(phone_number);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeMakeCall(phone_number);
#endif
	}

	void vibrate(int milis)
	{
		Device::vibrate(milis * 1.0f / 1000);
	}

	std::string getVersion()
	{
		return Application::getInstance()->getVersion();
	}

	std::string getPackageName()
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		return getPackageNameJNI();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        return Platform::ios::nativeGetBundleId();
#else
		return "";
#endif
	}
    
    void generateAppleVerify(TokenCallBack callback)
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeGenerateAppleVerify(callback);
#endif
    }

    void onGGSignInCompleted(std::string token)
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        GameMgr->Network_loginWithToken(MainGame::TYPE_GOOGLE, token.c_str());
#endif
    }
    
	std::string getAppName()
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		return nativeGetAppName();
#else
		return "iWinonline";
#endif
	}
    
    std::string makeSHA1Hash(std::string input)
    {
        std::string result = "";
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        result = Platform::ios::nativeMakeSHA1Hash(input);
#endif
        return result;
    }
    
	void handleSMSSucceed(std::string phone_number, std::string sms_body)
	{
		ScrMgr->OnShowOKDialog(StringUtils::format(LANG_STR("sms_send_succ").GetString(), phone_number.c_str(), sms_body.c_str()));
	}

	void handleSMSFail()
	{
		ScrMgr->OnShowErrorDialog("sms_send_fail");
	}

    //here implement appinfo for target platform
    std::string AppInfo::getName()
    {
        return "IwinOnline";
    }
    
    std::string AppInfo::getVersion()
    {
        return "5.1.9";
    }
    
    std::string AppInfo::getChanel()
    {
        return "0|me|" + getVersion();
    }
    
    std::string AppInfo::getLanguage()
    {
		return XMLMgr->GetLangDecByType(LangMgr->GetCurLanguage())->name_id.GetString();
    }
    
    std::string AppInfo::getBundleID()
    {
        return "iwin.vn.full";
    }
    
    int AppInfo::getAppWidth()
    {
		return cocos2d::Director::getInstance()->getVisibleSize().width;
    }
    
    int AppInfo::getAppHeight()
    {
		return cocos2d::Director::getInstance()->getVisibleSize().height;
    }
    
    std::string AppInfo::getNotifyKey()
    {
        return "";
    }
	AppInfo* s_appinfo = nullptr;
	AppInfo* getAppInfo()
	{
		if (!s_appinfo)
		{
#if     CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
			s_appinfo = (AppInfo*)(new WPPlatform::WPAppInfo());
#elif   CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
			s_appinfo = (AppInfo*)(new DesktopPlatform::Win32AppInfo());
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID 
			s_appinfo = (AppInfo*)(new AndroidPlatform::AndroidAppInfo());
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
            s_appinfo = (AppInfo*)(new IosPlatform::IosAppInfo());
#else
			s_appinfo = new AppInfo();
#endif
		}
		return s_appinfo;

	}

    //here implement appinfo for target platform    
    std::string DeviceInfo::getCarrier()
    {
        return "";
    }
    
    std::string DeviceInfo::getDeviceName()
    {
        return "";
    }
    
    int DeviceInfo::getIsJailBreak()
    {
        return 0;
    }
    
    std::string DeviceInfo::getNet()
    {
        return "";
    }
    
    std::string DeviceInfo::getOS()
    {
        return "";
    }
    
    std::string DeviceInfo::getOSVersion()
    {
        return "";
    }
    
    std::string DeviceInfo::getDeviceUDID()
    {
        return "";
    }
    
    std::string DeviceInfo::getMacAddress()
    {
        return "";
    }
    
    std::string DeviceInfo::getAdvertisingID()
    {
        return "";
    }
    
    std::string DeviceInfo::getAndroidID()
    {
        return "";
    }
    std::string DeviceInfo::getSimSeri()
    {
        return "";
    }
    std::string DeviceInfo::getFingerPrint()
    {
        return "";
    }
    std::string DeviceInfo::getSubscriberID()
    {
        return "";
    }
    std::string DeviceInfo::getBluetoothAddress()
    {
        return "";
    }
    
	DeviceInfo* s_deviceinfo = nullptr;
	DeviceInfo* getDeviceInfo()
	{
		if (!s_deviceinfo)
		{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
			s_deviceinfo = (DeviceInfo*)(new WPDeviceInfo());
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
			s_deviceinfo = (DeviceInfo*)(new AndroidPlatform::AndroidDeviceInfo());
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
            s_deviceinfo = (DeviceInfo*)(new IosPlatform::IosDeviceInfo());
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
			s_deviceinfo = (DeviceInfo*)(new DesktopPlatform::Win32DeviceInfo());
#else
			s_deviceinfo = new DeviceInfo();
#endif
		}
		return s_deviceinfo;
		
	}

	void HttpRequest(std::string url, std::function<void(std::string)> callback)
	{
		cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
		request->setUrl(url);
		request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
		request->setResponseCallback([callback](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
		
			if (response && response->getResponseCode() == 200 && response->getResponseData()) {
				vector<char> *data = response->getResponseData();
				string ret(&(data->front()), data->size());
				CCLOG("%s", ("Response message: " + ret).c_str());
				if (callback)
				{
					callback(ret);
				}
			}
			else {
				CCLOG("%s", ("Error " + to_string(response->getResponseCode()) + " in request").c_str()); //error
			}
		});
		request->setTag("Http GET");
		cocos2d::network::HttpClient::getInstance()->send(request);
		request->release();
	}

	void downloadAppAndSetup(std::string link, std::function<void()> actionCallback)
	{

	}

	std::string getClipboardText(std::function<void(std::string)> return_handler)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		nativeGetClipboardText(return_handler);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        return_handler(Platform::ios::nativeGetClipboard());
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT 
		WPPlatform::WPHelper::getClipboardText(return_handler);
#endif
		return "";
	}

    void setClipboardText(std::string text)
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		nativeSetClipboardText(text);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeSetClipboard(text);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		WPPlatform::WPHelper::setClipboardText(text);
#endif
    }
    
    void dismissSplash()
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        nativeDismissSplash();
#endif
    }
    
    void getListAppEmail(std::vector<std::string>& listAppEmail)
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        std::vector<std::string> result = nativeAppEmail();
        for(int i = 0; i < result.size(); i++) {
            listAppEmail.push_back(result.at(i));
        }
#endif
    }
    
    void openAppEmailWithName(std::string appName)
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        nativeOpenAppEmailWithName(appName);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        Platform::ios::nativeOpenAppEmailWithName(appName);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT

#endif
    }

	void openEmail(std::string send_to, RKUtils::RKString title)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		//do some thing
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		//do some thing
        Platform::ios::nativeOpenEmail(send_to, title.GetString());
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		WPPlatform::WPHelper::sendEmail(send_to, title.GetString());
		//do some thing
#else
		RKString text = StringUtils::format(LANG_STR("any_comments_please").GetString(), send_to.c_str(), title.GetString());
		ScrMgr->OnShowOKDialog(text);
#endif
	}

	void RateApp(std::string link)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		//do some thing
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		//do some thing
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		//do some thing
#endif
	}

	void LoadImageFromFile(const std::function<void(const char *, unsigned int)> & call_back)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		//do some thing
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		//do some thing
        Platform::ios::nativeLoadImageFromLibrary(call_back);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		//do some thing
#endif
	}

	void CaptureImageFromCamera(const std::function<void(const char *, unsigned int)> & call_back)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		//do some thing
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		//do some thing
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		//do some thing
#endif
	}

	void OnShowNativeClearTextBtn(bool visible, float X, float Y)
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		//do some thing
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
		//do some thing
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
		//do some thing
		if (WPPlatform::WPHelper::onShowBtnClear)
		{
			WPPlatform::WPHelper::onShowBtnClear(visible, X, Y);
		}
#endif
	}

	namespace FBUtils
	{
		std::string TAG = "FBUtils";

		std::string graphUrl = "https://graph.facebook.com/";

		std::string jSON_DATA_REQUEST_ASK_GIFT = "{\"type\":\"1\"}";
		std::string jSON_DATA_REQUEST_SEND_GIFT = "{\"type\":\"2\"}";
		std::string JSON_DATA_ACCEPT_SEND = "{\"type\":\"3\"}";
		std::string JSON_DATA_ACCEPT_RECEIVE = "{\"type\":\"4\"}";

		std::map<std::string, std::string> cacheFBName;
		std::string loadingName = "...";



		bool isFacebookLoggedIn()
		{
			return GetFacebookMgr()->isLoggedIn();
		}

		std::string getAccessToken()
		{
			return GetFacebookMgr()->getAccessToken();
		}

		void loginFacebook(const std::function<void(bool, void*)> & call_back)
		{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
			WPPlatform::WPHelper::loginFacebook([call_back](void* data, std::string access_token) {
				call_back(true,static_cast<void*>(&access_token));
			});
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS
            GetFacebookMgr()->login(call_back);
#endif
		}


		void likePage(std::string url, const std::function<void(bool, void*)> & call_back)
		{
            loginFacebook([url, call_back](bool res, void* data) {
                if (res)
                {
                    GetFacebookMgr()->likePage(url, call_back);
                }
                else
                {
                    CCLOG("can not do action because not login before");
                }
            });
		}

		void postWall(std::string json_data, const std::function<void(bool, void*)> & call_back)
		{
            loginFacebook([json_data, call_back](bool res, void* data) {
                if (res)
                {
                    GetFacebookMgr()->postWall(json_data, call_back);
                }
                else
                {
                    CCLOG("can not do action because not login before");
                }
            });
		}

		void shareLink(std::string link, const std::function<void(bool, void*)> & call_back)
		{
            loginFacebook([link, call_back](bool res, void* data) {
                if (res)
                {
                    GetFacebookMgr()->shareLink(link, call_back);
                }
                else
                {
                    CCLOG("can not do action because not login before");
                }
            });
		}

//		void showInviteFriendDialog()
//		{
//			if (isFacebookLoggedIn())
//			{
//                GetFacebookMgr()->getInvitableFriend([](bool res, void* data) {
//                    if(res) {
////                        GetFacebookMgr()->openInviteFriendDialog();
//                        //1. open invite dialog
//                        //2. call invite with list friend ids
//                        //3. success invite
//                    }
//                });
//			}
//			else
//			{
//				loginFacebook([](bool res, void* data) {
//					if (res)
//					{
//						showInviteFriendDialog();
//					}
//					else
//					{
//						ScrMgr->OnHideDialog();	
//						ScrMgr->OnShowErrorDialog(L"Lấy danh sách bạn facebook có thể mời chơi thất bại !");
//					}
//
//				});
//			}
//		}

		void getInvitableFriends(const std::function<void(bool, void*)> & call_back)
		{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS
            loginFacebook([call_back](bool res, void* data) {
                if (res)
                {
                    if(GetFacebookMgr()->checkPermissionExist(sdkbox::FB_PERM_READ_USER_FRIENDS))
                    {
                        GetFacebookMgr()->getInvitableFriend(call_back);
                    }
                    else
                    {
                        GetFacebookMgr()->requestReadPermission([call_back](bool res, void* data)
                                                                {
                                                                    if(res)
                                                                    {
                                                                        GetFacebookMgr()->getInvitableFriend(call_back);
                                                                    }
                                                                });
                    }
                }
                else
                {
                    CCLOG("can not do action because not login before");
                }
            });
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
            WPPlatform::WPHelper::getInvitableFriends(call_back);
#endif
		}

		void InviteFriend(std::vector<std::string> ls_id, const std::function<void(bool, void*)> & call_back)
		{
            loginFacebook([ls_id, call_back](bool res, void* data) {
                if (res)
                {
                    GetFacebookMgr()->inviteFriends(ls_id, call_back);
                }
                else
                {
                    CCLOG("can not do action because not login before");
                }
            });
		}

		void requestSendGift(std::string fbID, const std::function<void(bool, void*)> & call_back)
		{
            
			std::vector<std::string> list_id;
			list_id.push_back(fbID);
			requestSendGift(list_id, call_back);
		}

		void requestSendGift(std::vector<std::string> facebookIDs, const std::function<void(bool, void*)> & call_back)
		{
			sendRequestWith(facebookIDs, jSON_DATA_REQUEST_SEND_GIFT, "Gửi quà cho bạn", "Mình đã gởi cho bạn 1 món quà! Mở ngay bạn nhé", call_back);
		}

		void requestAskGift(std::vector<std::string> facebookIDs, const std::function<void(bool, void*)> & call_back)
		{
			sendRequestWith(facebookIDs, jSON_DATA_REQUEST_ASK_GIFT, "Gửi yêu cầu trợ giúp", "Tôi hết tiền rồi! Tặng quà trợ giúp đi bạn hiền !", call_back);
		}

		void acceptSendGift(std::string fbID, const std::function<void(bool, void*)> & call_back)
		{
			std::vector<std::string> list_id;
			list_id.push_back(fbID);
			acceptSendGift(list_id, call_back);
		}

		void acceptSendGift(std::vector<std::string> facebookIDs, const std::function<void(bool, void*)> & call_back)
		{
			sendRequestWith(facebookIDs, JSON_DATA_ACCEPT_SEND, "Chấp nhận gửi quà", "Mình mới gởi quà cho bạn đó, vào iWin Online xem nhé.", call_back);
		}

		void sendRequestWith(std::vector<std::string> facebookIDs, std::string jsonData, std::string title, std::string msg, const std::function<void(bool, void*)> & call_back)
		{
//			if (isFacebookLoggedIn())
//			{
//				GetFacebookMgr()->sendRequest(facebookIDs, title, msg, jsonData, [facebookIDs](std::string ibj) {
//				
//					//GlobalService::requestSendGift(, getAccessToken(), facebookIDs);
//				});
//			}
//			else
//			{
//				loginFacebook([&](bool res, void* data) {
//					if (res)
//					{
//						sendRequestWith(facebookIDs, jsonData, title, msg, call_back);
//					}
//					else
//					{
//                        
//					}
//
//				});
//			}
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS
            loginFacebook([facebookIDs, jsonData, title, msg, call_back](bool res, void* data) {
                if (res)
                {
                    GetFacebookMgr()->sendRequest(facebookIDs, title, msg, jsonData, [](bool res, void* data){
                        std::string resData = static_cast<const char*>(data);
                        rapidjson::Document document;
                        document.Parse<0>(resData.c_str());
                        
                        std::string requestID = document["requestID"].GetString();
                        std::string accessToken = document["token"].GetString();
                        std::string listID = document["ids"].GetString();
                        std::vector<std::string> ids;
                        size_t i = 0;
                        size_t j = listID.find(",");
                        while (j != string::npos) {
                            ids.push_back(listID.substr(i, j-i));
                            i = ++j;
                            j = listID.find(",", j);
                            
                            if (j == string::npos)
                                ids.push_back(listID.substr(i, listID.length()));
                        }
                        
                        GlobalService::requestSendGift(requestID, accessToken, ids);
                    });
                }
                else
                {
                    CCLOG("can not do action because not login before");
                }
		});
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
                
#endif
          
		}

	}
}

void Platform::FBUtils::getFacebookMyInfo(const std::function<void(bool, void*)>& call_back)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    
    loginFacebook([call_back](bool res, void* data) {
        if (res)
        {
            GetFacebookMgr()->getMyInfo(call_back);
        }
        else
        {
            CCLOG("can not do action because not login before");
        }
    });
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
    
#endif
}

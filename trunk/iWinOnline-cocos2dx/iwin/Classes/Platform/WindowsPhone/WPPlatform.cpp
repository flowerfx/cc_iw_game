#include "WPPlatform.h"
#include "RKString_Code\RKString.h"
#include "UI\CtrWebOverlay.xaml.h"
#include "..\Platform.h"
#include <condition_variable>
#include <mutex>
#include "../../WinRTAPIsEmul/WinRTStringUtils.h"
#include <memory>
#include "Network\JsonObject\ServiceSocial\ResponseGoogle.h"
#include "screens/MainScreen.h"
#include "FileManager.h"
#include "json/writer.h"
#include "Network\JsonObject\facebook\FacebookFriend.h"
#include "Network\JsonObject\facebook\FacebookFriendList.h"
#include "Utils.h"
using namespace RKUtils;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Security::Authentication::Web;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace std;
using namespace Windows::ApplicationModel::Email;
namespace Utility
{
	extern Platform::String ^stops(std::string s);
}

namespace WPPlatform
{
#pragma region  info client



	WPAppInfo::WPAppInfo()
	{

	}
	WPAppInfo::~WPAppInfo()
	{

	}
	std::string WPAppInfo::getName()
	{
		return "iWinnOnline";
	}

	std::string WPAppInfo::getLanguage()
	{
		return "vi";
	}

	std::string WPAppInfo::getBundleID()
	{
		return "me.iwin.wp";
	}


	float WPAppInfo::getAppHieght()
	{
		return 640;
	}

	std::string WPAppInfo::getNotifyKey()
	{
		return "";
	}
	WPDeviceInfo::WPDeviceInfo()
	{
		Windows::Security::ExchangeActiveSyncProvisioning::EasClientDeviceInformation^ deviceInfo = ref new  Windows::Security::ExchangeActiveSyncProvisioning::EasClientDeviceInformation();
		deviceName = pstos(deviceInfo->SystemSku);
		os = pstos(deviceInfo->SystemFirmwareVersion);
		osVersion = pstos(deviceInfo->SystemHardwareVersion);
		Windows::System::Profile::HardwareToken^ token = Windows::System::Profile::HardwareIdentification::GetPackageSpecificToken(nullptr);
		Windows::Storage::Streams::IBuffer^ hardwareId = token->Id;
		Windows::Security::Cryptography::Core::HashAlgorithmProvider^ hasher = Windows::Security::Cryptography::Core::HashAlgorithmProvider::OpenAlgorithm("MD5");
		Windows::Storage::Streams::IBuffer^ hashed = hasher->HashData(hardwareId);
		udid = pstos(Windows::Security::Cryptography::CryptographicBuffer::EncodeToHexString(hashed));
		delete deviceInfo;
		delete token;
		delete hasher;
	}
	WPDeviceInfo::~WPDeviceInfo()
	{}
	std::string WPDeviceInfo::getCarrier()
	{
		return "wifi";
	}

	std::string WPDeviceInfo::getDeviceName()
	{
		return deviceName;
	}

	int WPDeviceInfo::getIsJailBreak()
	{
		return 0;
	}

	std::string WPDeviceInfo::getNet()
	{
		return "";
	}

	std::string WPDeviceInfo::getOS()
	{
		return "WindowsStore";
	}

	std::string WPDeviceInfo::getOsVersion()
	{
		return osVersion;
	}

	std::string WPDeviceInfo::getDeviceUDID()
	{
		return udid;
	}
	std::string WPDeviceInfo::getMacAddress()
	{
		return udid;
	}
#pragma endregion
	std::function<void(std::string, std::function<void(void*, std::string)>)> WPHelper::actionShowWebView = nullptr;
	std::function<void(void*, std::string)> WPHelper::callbackGoogle = nullptr;
	std::function<void()> WPHelper::actionCloseWebView = nullptr;
	std::function<bool()> WPHelper::isWebViewShow = nullptr;
	std::function<void()> WPHelper::hideImageLoadGame = nullptr;
	std::function<void(bool, float X, float Y)> WPHelper::onShowBtnClear = nullptr;
	void WPHelper::sendSMS(std::string content, std::string toNumber)
	{
#if defined WP8
		Windows::ApplicationModel::Chat::ChatMessage^ sms = ref  new Windows::ApplicationModel::Chat::ChatMessage();
		sms->Body = ref new Platform::String(stows(content).c_str());
		sms->Recipients->Append(ref new Platform::String(stows(toNumber).c_str()));
		Windows::ApplicationModel::Chat::ChatMessageManager::ShowComposeSmsMessageAsync(sms);
#endif
	}
	void WPHelper::makeCall(std::string _displayName, std::string _phoneNumber)
	{
#if defined WP8
		Platform::String^ displayName = ref new  Platform::String(stows(_displayName).c_str());
		Platform::String^ phoneNumber = ref new Platform::String(stows(_phoneNumber).c_str());
		Windows::ApplicationModel::Calls::PhoneCallManager::ShowPhoneCallUI(phoneNumber, displayName);
#endif
	}
	void WPHelper::vibrate(int milisecond)
	{

	}

	void WPHelper::sendEmail(std::string email_to, std::string title)
	{
		EmailMessage^ _email = ref new EmailMessage();
		_email->To->Append(ref new EmailRecipient(Utility::stops(email_to)));
		_email->Subject = Utility::stops(title);

		create_task(EmailManager::ShowComposeNewEmailAsync(_email)).then([]() {
		
		});
	}

	std::wstring WPHelper::stows(std::string s)
	{
		std::wstring ws;
		ws.assign(s.begin(), s.end());
		return ws;
	}
	void WPHelper::showWebView(std::string url, std::function<void(void*, std::string)> callback)
	{
		if (actionShowWebView)
		{
			actionShowWebView(url, callback);
		}
	}



	void WPHelper::closeWebView()
	{
		if (actionCloseWebView)
		{
			actionCloseWebView();
		}
	}

	void WPHelper::loginFacebook(std::function<void(void*, std::string)> callback)
	{
		RKString acessToken;
		if (getFBAcessToken().length() > 0)
		{
			if (callback)
			{
				callback(nullptr, acessToken.GetString());
			}
			return;
		}
		std::string urlLogin = StringUtils::format("%s?client_id=%s&redirect_uri=%s&response_type=token&display=popup", URL_FACEBOOK_LOGIN, FACEBOOK_APP_ID, URL_FACEBOOK_DERECT);
		showWebView(urlLogin, [callback](void* sender, std::string url) {
			auto str = RKString(url);
			bool isClose = str.StartsWith(RKString(URL_FACEBOOK_DERECT));
			if (isClose)
			{
				std::string accessToken;
				std::string expiresIn;
				parseAssesTokenFB(url, accessToken, expiresIn);
				time_t _time = time(0); // now
				float t_exp = ::atof(expiresIn.c_str());
				_time += t_exp;
				ScrMgr->SaveDataString(STR_SAVE_FB_STATE, accessToken+"|"+std::to_string(_time));

				if (callback != nullptr)
				{
					CocosAppWinRT::CtrWebOverlay^ webview = reinterpret_cast<CocosAppWinRT::CtrWebOverlay^>(sender);
					webview->close();
					callback(sender, accessToken);
				}
			}

		});
	}
	void WPHelper::parseAssesTokenFB(std::string webAuthResultResponseData, std::string& accessToken, std::string& expiresIn)
	{
		RKString strData = RKString(webAuthResultResponseData);
		int index = webAuthResultResponseData.find("access_token", 0);
		RKString responseData = strData.Substring(index, strData.Length() - index);
		RKList<RKString> keyValPairs = responseData.Split("&");
		accessToken = "";
		expiresIn = "";
		for (int i = 0; i < keyValPairs.Size(); i++)
		{
			RKList<RKString> splits = keyValPairs[i].Split("=");
			if (splits.GetAt(0) == "access_token")
			{
				accessToken = splits.GetAt(1).GetString();
			}
			else if (splits.GetAt(0) == "expires_in")
			{
				expiresIn = splits.GetAt(1).GetString();
			}

		}
	}
	void WPHelper::loginGoogle(std::function<void(void*, std::string)> callback)
	{
		std::string token = getGoogleAcessToken();;
		if (token.length() > 0)
		{
			if (callback)
			{
				callback(nullptr, token);
			}
			return;
		}

		Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new Windows::UI::Core::DispatchedHandler([callback]()
		{

			Platform::String^ googleURL = "https://accounts.google.com/o/oauth2/auth?";
			googleURL += "client_id=" + Uri::EscapeComponent(GOOGLE_CLIENT_ID);
			googleURL += "&scope=https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fplus.login+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.email+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.profile+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fplus.login+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.email+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.profile+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fplus.login";
			googleURL += " &redirect_uri=" + Uri::EscapeComponent(GOOGLE_CLIENT_URL_CALBACK);
			googleURL += "&state=foobar";
			googleURL += "&response_type=code";
			try
			{
				auto startURI = ref new Uri(googleURL);
				auto endURI = ref new Uri("https://accounts.google.com/o/oauth2/approval?");
#ifndef CC_WINDOWS_PHONE_8_1
				create_task(WebAuthenticationBroker::AuthenticateAsync(WebAuthenticationOptions::UseTitle, startURI, endURI)).then([callback](WebAuthenticationResult^ result)
				{

					switch (result->ResponseStatus)
					{
					case WebAuthenticationStatus::ErrorHttp:
						break;
					case WebAuthenticationStatus::Success:
					{
						std::string code = getCodeGoogle(result->ResponseData);
						parseAcessTokenGG(ref new Platform::String(stows(code).c_str()), callback);

					}
					break;
					case WebAuthenticationStatus::UserCancel:

						break;
					}


				});
#else
				callbackGoogle = callback;
				WebAuthenticationBroker::AuthenticateAndContinue(startURI, ref new Uri(GOOGLE_CLIENT_URL_CALBACK), nullptr, WebAuthenticationOptions::None);
#endif
			}
			catch (Exception^ ex)
			{
				return;
			}
		}));

	}


	void WPHelper::Finalize(Windows::ApplicationModel::Activation::WebAuthenticationBrokerContinuationEventArgs^ obj)
	{
		getSessionGoogle(obj);
	}

	void WPHelper::getProfileGoogle(std::function<void(std::string)> onCompleted)
	{
		RKString token;
		ScrMgr->GetDataSaveString(STR_SAVE_GG_STATE, token);
		std::string urlP = GOOGLE_CLIENT_URL_PROFILE;
		urlP += token.GetString();
		Platform::HttpRequest(urlP, onCompleted);

	}

	void  WPHelper::parseAcessTokenGG(Platform::String^ code, std::function<void(void*, std::string)> callBack)
	{
		Platform::String^ TokenUrl = "https://accounts.google.com/o/oauth2/token";
		Platform::String^ body = code;
		body += "&client_id=";
		body += Uri::EscapeComponent(GOOGLE_CLIENT_ID);
		body += "&client_secret=";
		body += Uri::EscapeComponent(GOOGLE_CLIENT_SECRET);
		body += "&redirect_uri=";
		body += Uri::EscapeComponent(GOOGLE_CLIENT_URL_CALBACK);
		body += "&grant_type=authorization_code";

		auto client = ref new HttpClient();
		auto request = ref new HttpRequestMessage(HttpMethod::Post, ref new Uri(TokenUrl));
		request->Content = ref new HttpStringContent(body, Windows::Storage::Streams::UnicodeEncoding::Utf8, "application/x-www-form-urlencoded");
		create_task(client->SendRequestAsync(request)).then([callBack](HttpResponseMessage^ reponse)
		{
			create_task(reponse->Content->ReadAsStringAsync()).then([callBack](Platform::String^ content)
			{
				if (callBack)
				{
					std::string data = pstos(content);
					iwinmesage::ResponseGoogle * googleData = new iwinmesage::ResponseGoogle();
					googleData->toData(data);
					time_t _time = time(0); // now
					_time += googleData->getExpires_in();
					ScrMgr->SaveDataString(STR_SAVE_GG_STATE, googleData->getAccess_token()+ "|" + std::to_string(_time));
					callBack(nullptr, googleData->getAccess_token());
				}

			});
		});

	}



	void WPHelper::getSessionGoogle(Windows::ApplicationModel::Activation::WebAuthenticationBrokerContinuationEventArgs^ result)
	{

		if (result->WebAuthenticationResult->ResponseStatus == WebAuthenticationStatus::Success)
		{
			std::string code = getCodeGoogle(result->WebAuthenticationResult->ResponseData);
			parseAcessTokenGG(ref new Platform::String(stows(code).c_str()), callbackGoogle);

		}
		if (result->WebAuthenticationResult->ResponseStatus == WebAuthenticationStatus::ErrorHttp)
		{

		}
		if (result->WebAuthenticationResult->ResponseStatus == WebAuthenticationStatus::UserCancel)
		{

		}
	}



	std::string WPHelper::getCodeGoogle(Platform::String^ result)
	{
		std::string strcode = pstos(result);
		RKString* str = new RKString(strcode);
		RKList<RKString> listStr = str->Split("&");
		int indexz = -1;
		for (int i = 0; i < listStr.Size(); i++)
		{
			indexz = listStr[i].FindFirst("code");
			if (indexz >= 0)
			{
				strcode = listStr[i].GetString();
				break;
			}
		}
		if (indexz < 0)
		{
			return "";
		}
		CC_SAFE_DELETE(str);
		return strcode.substr(indexz, strcode.length() - indexz);
	}

	void WPHelper::getClipboardText(std::function<void(std::string)> return_handler)
	{
#if !CC_WINDOWS_PHONE_8_1
		Windows::ApplicationModel::DataTransfer::DataPackageView^ dataView = Windows::ApplicationModel::DataTransfer::Clipboard::GetContent();
		if (dataView->Contains(Windows::ApplicationModel::DataTransfer::StandardDataFormats::Text))
		{
			create_task(dataView->GetTextAsync()).then([return_handler](Platform::String^ content)
			{
				if (return_handler)
				{
					std::string data = pstos(content);
					return_handler(data);
				}

			});
		}
#endif
	}
	void WPHelper::setClipboardText(std::string text)
	{
#if !CC_WINDOWS_PHONE_8_1
		Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new Windows::UI::Core::DispatchedHandler([text]()
		{
			Windows::ApplicationModel::DataTransfer::DataPackage^ dataPackage = ref new Windows::ApplicationModel::DataTransfer::DataPackage();
			dataPackage->SetText(ref new Platform::String(stows(text).c_str()));
			Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(dataPackage);
		}));
#endif
	}

	void WPHelper::getInvitableFriends(const std::function<void(bool,void*)> & call_back)
	{
		loginFacebook([call_back](void* sender,std::string accessToken) {
			std::string paras = "limit="+std::to_string(MAX_INVITE_FRIEND) +"&pretty=1";
			GetGraphObject(FB_GRAPH_FRIEND_LIST, paras, accessToken, [call_back](std::string data) {
					FacebookFriendList* lsFriend = new FacebookFriendList();
					ParseFriendList(data, lsFriend,call_back);
					
			});
		
		});

	
	}



	void WPHelper::inviteFriendList(std::string accessToken, std::vector<std::string> sendIDs, std::string title, std::string link, std::function<void(std::string)> onCompleted)
	{
		std::string message = LangMgr->GetString("killer_13_chinese").GetString();
		std::vector<std::string> lsIDs;
		std::vector<std::string> lsIDsRemain ;
		std::string jsoIDs = "";
		if (sendIDs.size()>0)
		{
			jsoIDs += "[";
			for (int i = 0; i < sendIDs.size(); i++)
			{
				if (i <MAX_INVITE_FRIEND)
				{
					lsIDs.push_back(sendIDs[i]);
					string s = "";
					if (i <= MAX_INVITE_FRIEND - 1 && i < sendIDs.size() - 1)
					{
						s = ",";
					}
					jsoIDs += sendIDs[i]+s;
				}
				else
				{
					lsIDsRemain.push_back(sendIDs[i]);
				}
			}
			jsoIDs += "]";
		}

		//Platform::String^ messageEndCode =Uri::EscapeComponent(ref new  Platform::String(stows(message).c_str()) );
		//Platform::String^ titleEncode = Uri::EscapeComponent(ref new  Platform::String(stows(title).c_str()));
		std::string url = ("https://m.facebook.com/dialog/apprequests?to=" + jsoIDs + "&message=" + message + "&title=" + title
			+ "&redirect_uri=" + link + "&display=touch&app_id=" + FACEBOOK_APP_ID
			+ "&type=user_agent" + "&access_token=" + accessToken);
		showWebView(url, [accessToken, title, message, link,lsIDsRemain,onCompleted](void* sender, std::string url)
		{
			if (url.find("http://www.iwin.me/?request=") != std::string::npos)
			{
				if (lsIDsRemain.size() <= 0)
				{
					closeWebView();
				}
				if (onCompleted)
				{
					Utils::replaceAllString(url, "http://www.iwin.me/?", "");
					std::map<string,string> prms = ParseParamsFBInvite(url);
					std::string request = prms["request"];
					std::string jsonstring = "\"requestID\": \"%s\", \"token\" : \"%s\", \"ids\": \"%s\"";
					std::string ids = "";
					int size = prms.size() - 1;
					for (int i = 0; i < size; i++)
					{
						auto s = prms.find("to[" + std::to_string(i) + "]");
						if (s != prms.end()) {
							ids += s->second;
						}
						if (i < size - 1)
						{
							ids += ",";
						}
					}
					std::string returnData = StringUtils::format(jsonstring.c_str(),request.c_str(), accessToken.c_str(), ids.c_str());
					returnData = "{" + returnData + "}";
					if (onCompleted)
					{
						onCompleted(returnData);
					}
					if (lsIDsRemain.size() > 0)
					{
						inviteFriendList(accessToken,lsIDsRemain, title, link, onCompleted);
					}

				}
			}
			else if (url.find("http://www.iwin.me/?error_code") != std::string::npos)
			{
				if (lsIDsRemain.size() <= 0)
				{
					closeWebView();
				}
			}

		});
		
	}

	std::string WPHelper::getFBAcessToken()
	{
		
		RKString acessToken;
		if (ScrMgr->GetDataSaveString(STR_SAVE_FB_STATE, acessToken) && acessToken.Length() > 0)
		{
			 RKList<RKString> arrs=  acessToken.Split("|");
			 if (arrs.Size() >= 2)
			 {
				 float t_exp = ::atof(arrs.GetAt(1).GetString());
				 time_t _time = time(0); // now
				 if (t_exp > _time)
				 {
					 return arrs[0].GetString();
				 }
				 else
				 {
					 ScrMgr->SaveDataString(STR_SAVE_FB_STATE, "");
				 }
			 }
		}
		return "";
	}

	std::string WPHelper::getGoogleAcessToken()
	{
		RKString acessToken;
		if (ScrMgr->GetDataSaveString(STR_SAVE_GG_STATE, acessToken) && acessToken.Length() > 0)
		{
			RKList<RKString> arrs = acessToken.Split("|");
			if (arrs.Size() >= 2)
			{
				float t_exp = ::atof(arrs.GetAt(1).GetString());
				time_t _time = time(0); // now
				if (t_exp > _time)
				{
					return arrs[0].GetString();
				}
				else
				{
					ScrMgr->SaveDataString(STR_SAVE_GG_STATE, "");
				}
			}
		}
		return "";
	}

	void WPHelper::logoutFacebook()
	{
		ScrMgr->SaveDataString(STR_SAVE_FB_STATE, "");
		clearWebviewCookie(FACEBOOK_LINK);
	}

	void WPHelper::logoutGoogle()
	{
		ScrMgr->SaveDataString(STR_SAVE_GG_STATE, "");
		clearWebviewCookie(GOOGLE_LINK);
	}

	void WPHelper::clearWebviewCookie(std::string url)
	{
		Windows::Web::Http::Filters::HttpBaseProtocolFilter^ myFilter =ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter();
		Windows::Web::Http::HttpCookieManager^ cookieManager = myFilter->CookieManager;
		HttpCookieCollection^ myCookieJar = cookieManager->GetCookies(ref new Uri(ref new Platform::String(stows(url).c_str())));
		for each (Windows::Web::Http::HttpCookie^ cookie in myCookieJar)
		{
			cookieManager->DeleteCookie(cookie);

		}
	}

	/************************* GRAPH API **********************/
	void WPHelper::GetGraphObject(std::string path, std::string prm, std::string token,std::function<void(std::string)> onComplete)
	{
		std::string url = FB_GRAPH_PATH+path;
		url += "?access_token=" + token;
		HttpRequest(url,[onComplete](std::string str)
		{
			onComplete(str);
		});
	}


	void WPHelper::ParseFriendList(std::string data, FacebookFriendList* lsFriend, const std::function<void(bool, void*)> & call_back)
	{
		rapidjson::Document document;
		document.Parse(data.c_str());
		/*std::string data = document["data"].GetString();
		std::string pages = document["paging"].GetString();
		FacebookFriendList lsF;
		lsF.toData(data);
*/
		if (document.HasMember("data") && !document["data"].IsNull() && document["data"].IsArray())
		{
			const rapidjson::Value& arrdata = document["data"];
			for (rapidjson::SizeType i = 0; i < arrdata.Size(); i++)
			{
				const rapidjson::Value& c = arrdata[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				const char* json = buffer.GetString();
				FacebookFriend* d = new FacebookFriend();
				d->toData(json);
				lsFriend->addFriend(d);

			}
		}

		if (document.HasMember("paging"))
		{
			const rapidjson::Value&  document1 = document["paging"];
			if (!document1.HasMember("next"))
			{
				if (call_back)
				{
					call_back(true, lsFriend);
				}
				return;
			}
			std::string pageNext = document1["next"].GetString();
			if (pageNext.length() > 0)
			{
				HttpRequest(pageNext, [pageNext,lsFriend, call_back](std::string str)
				{
					ParseFriendList(str, lsFriend, call_back);
				});
			}

		}
	}

	std::map<std::string, std::string> WPHelper::ParseParamsFBInvite(string url)
	{
		Utils::replaceAllString(url,"#_=_", "");
		Platform::String^ raw = Uri::UnescapeComponent(ref new  Platform::String(stows(url).c_str()));

		std::vector<std::string> pairs =Utils::split(pstos(raw),"&");

		std::map<std::string, std::string> prms;

		for(std::string pair : pairs)
		{
			std::vector<std::string> kv = Utils::split(pair,"=");
			prms.insert(std::make_pair(kv[0], kv[1]));
		}

		return prms;
	}

}
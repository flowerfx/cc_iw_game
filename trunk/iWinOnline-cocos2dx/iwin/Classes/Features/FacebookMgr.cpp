#include "FacebookMgr.h"
#include "MainGame.h"
#include "json/document.h"


#include "Network/Core/NetworkMacros.h"

#include "Network/JsonObject/Facebook/FacebookMyInfo.h"
#include "Network/JsonObject/Facebook/FacebookFriend.h"
#include "Network/JsonObject/Facebook/FacebookFriendList.h"

#include "Platform/Platform.h"

#if defined WP8 || defined OS_W8 || defined OS_W10
#include "Platform/WindowsPhone/WPPlatform.h"
#endif
#include "LanguageMgr.h"
//#include "Screens/ScreenManager.h"
using namespace rapidjson;

FacebookMgr* FacebookMgr::s_instance = nullptr;

FacebookMgr* FacebookMgr::getInstance()
{
	if (!s_instance)
	{
		s_instance = new FacebookMgr();
	}
	return s_instance;
}


FacebookMgr::FacebookMgr()
{
	_call_back_command = nullptr;
    
#if SDKBOX_ENABLED
    SAFE_DELETE_VECTOR(_invitableUsers);
    _inviteUserIdxs.clear();
#endif
}

FacebookMgr::~FacebookMgr()
{
	_call_back_command = nullptr;
}

void FacebookMgr::SetCallBackCommand(const std::function<void(bool, void*)> & call_back)
{
	_call_back_command = call_back;
}

void FacebookMgr::init()
{
#ifdef SDKBOX_ENABLED
	sdkbox::PluginFacebook::init();
	PluginFacebook::setListener(this);
#endif
}

void FacebookMgr::login(const std::function<void(bool, void*)> &call_back)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#ifdef SDKBOX_ENABLED
    //login with request permissions
//    std::vector<std::string> permissions;
//    permissions.push_back(sdkbox::FB_PERM_READ_EMAIL);
//    permissions.push_back(sdkbox::FB_PERM_READ_PUBLIC_PROFILE);
//    permissions.push_back(sdkbox::FB_PERM_READ_USER_FRIENDS);
//    PluginFacebook::login(permissions);
    
    
    _call_back_command = call_back;
    if(!isLoggedIn())
    {
        //login normal --> FB_PERM_READ_PUBLIC_PROFILE only
            std::vector<std::string> permissions;
            permissions.push_back(sdkbox::FB_PERM_READ_EMAIL);
            permissions.push_back(sdkbox::FB_PERM_READ_PUBLIC_PROFILE);
        PluginFacebook::login(permissions);
    }
    else
    {
        onLogin(true, "");
    }
#endif
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
	WPPlatform::WPHelper::loginFacebook([](void* data, std::string access_token) {
        GameMgr->Network_loginWithToken(MainGame::TYPE_FACEBOOK, access_token);
		});
#endif
}

void FacebookMgr::logout()
{
#ifdef SDKBOX_ENABLED
    PluginFacebook::logout();
#elif defined WP8 || defined OS_W8 || defined OS_W10
	WPPlatform::WPHelper::logoutFacebook();
#endif
}

bool FacebookMgr::isLoggedIn()
{
#ifdef SDKBOX_ENABLED
	return PluginFacebook::isLoggedIn();
#else
	return false;
#endif
}

std::string FacebookMgr::getAccessToken()
{
#ifdef SDKBOX_ENABLED
	return PluginFacebook::getAccessToken();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
	return WPPlatform::WPHelper::getFBAcessToken();
#else
	return "";
#endif
}

#ifdef SDKBOX_ENABLED
void FacebookMgr::onLogin(bool isLogin, const std::string& msg)
{
	CCLOG(isLogin ? "Login success:%s" : "Login failed:%s", msg.c_str());

	if(_call_back_command)
	{
		_call_back_command(isLogin, nullptr);
	}
}

void FacebookMgr::onSharedSuccess(const std::string& msg)
{
    CCLOG("##FacebookMgr onSharedSuccess:%s", msg.c_str());
    
    if(_call_back_command)
    {
        _call_back_command(true, nullptr);
    }
}

void FacebookMgr::onSharedFailed(const std::string& msg)
{
    CCLOG("##FacebookMgr onSharedFailed:%s", msg.c_str());
    
    if(_call_back_command)
    {
        _call_back_command(false, nullptr);
    }
}

void FacebookMgr::onSharedCancel()
{
    CCLOG("##FacebookMgr onSharedCancel");
    
    if(_call_back_command)
    {
        _call_back_command(false, nullptr);
    }
}

void FacebookMgr::onAPI(const std::string& tag, const std::string& jsonData)
{
    CCLOG("##FacebookMgr onAPI: tag -> %s, json -> %s", tag.c_str(), jsonData.c_str());
    if (tag == "__fetch_picture_tag__")
    {
        rapidjson::Document document;
        document.Parse<0>(jsonData.c_str());
        
        std::string url = document["data"]["url"].GetString();
        CCLOG("api picture on receive: %s", url.c_str());
    }
    else if(tag == "__fb_me_friends_tag__")
    {
        rapidjson::Document document;
        document.Parse<0>(jsonData.c_str());
        CCLOG("api friend on receive: %s", jsonData.c_str());
        
        std::string nextURL = document["paging"]["cursors"]["after"].GetString();
        if(nextURL != "")
        {
            sdkbox::FBAPIParam params;
            params["limit"] = "25";
            params["pretty"] = "1";
            params["after"] = nextURL;
            params["access_token"] = PluginFacebook::getAccessToken();
//            params["fields"] = "id%name%installed%first_name%last_name%picture.type%small%";
            PluginFacebook::api("me/friends", "GET", params, "__fb_me_friends_tag__");
            
            return;
        }
    }
    else if(tag == "__fb_me_invitable_friends_tag__")
    {
        rapidjson::Document document;
        document.Parse<0>(jsonData.c_str());
        CCLOG("api invitable friends on receive: %s", jsonData.c_str());
        
        if(document.HasMember("data"))
        {
            const rapidjson::Value& data = document["data"];
            std::string nextURL = data.Size() != 0 ? document["paging"]["cursors"]["after"].GetString() : "";
            
            for (int idx = 0; idx < data.Size(); idx++)
            {
                const rapidjson::Value& user = data[idx];
                //test value
                std::string id = user["id"].GetString();
                std::string name = user["name"].GetString();
                bool isSil = user["picture"]["data"]["is_silhouette"].GetBool();
                std::string url = user["picture"]["data"]["url"].GetString();
                
                
                FBGraphUser* fbUser = new FBGraphUser();
                fbUser->setField(sdkbox::FBGraphUser::kGU_USERID, id);
                fbUser->setField(sdkbox::FBGraphUser::kGU_NAME, name);
                fbUser->setField(sdkbox::FBGraphUser::kGU_PICTURE_IS_SILHOUETTE, isSil);
                fbUser->setField(sdkbox::FBGraphUser::kGU_PICTURE_URL, url);
                
                _invitableUsers.push_back(fbUser);
            }
            
            if(nextURL != "")
            {
                sdkbox::FBAPIParam params;
                params["limit"] = "25";
                params["pretty"] = "1";
                params["after"] = nextURL;
                params["access_token"] = PluginFacebook::getAccessToken();
                PluginFacebook::api("me/invitable_friends", "GET", params, "__fb_me_invitable_friends_tag__");
                
                return;
            }
        }
        
        //convert to FacebookFriend to return
        FacebookFriendList* listFriend = new FacebookFriendList();
        for(std::vector<FBGraphUser*>::iterator it = _invitableUsers.begin(); it != _invitableUsers.end(); it++)
        {
            FacebookFriend* fbFriend = new FacebookFriend();
            fbFriend->setId((*it)->getUserId());
            fbFriend->setName((*it)->getName());
            
            FacebookPictureData fbPictureData;
            fbPictureData.setUrl((*it)->getPictureURL());
            fbPictureData.setIs_silhouette((*it)->isAppInstalled());
            
            FacebookPicture fbPicture;
            fbPicture.setData(fbPictureData);
            fbFriend->setPicture(fbPicture);
            
            fbFriend->setSelected(true);
            
            listFriend->addFriend(fbFriend);
        }
        
        if(_call_back_command)
        {
            _call_back_command(true, (void*)(listFriend));
            SAFE_DELETE_VECTOR(_invitableUsers);
        }
    }
    else if(tag == "me")
    {
        rapidjson::Document document;
        document.Parse<0>(jsonData.c_str());
        CCLOG("api me on receive: %s", jsonData.c_str());
        
        //here parse user my info
        FacebookMyInfo* myInfo = new FacebookMyInfo();
        myInfo->setId(document["id"].GetString());
        myInfo->setEmail(document["email"].GetString());
        myInfo->setFirst_name(document["first_name"].GetString());
        myInfo->setLast_name(document["last_name"].GetString());
        myInfo->setGender(document["gender"].GetString());
        myInfo->setLink(document["link"].GetString());
        myInfo->setLocale(document["locale"].GetString());
        myInfo->setName(document["name"].GetString());
        myInfo->setTimezone(StringUtils::format("%d",document["timezone"].GetInt()));
        myInfo->setUpdated_time(document["updated_time"].GetString());
        myInfo->setVerified(document["verified"].GetBool());
        
        if(_call_back_command)
        {
            _call_back_command(true, (void*)(myInfo));
        }
    }
    else if(tag == "__fb_me_send_request_tag__")
    {
        rapidjson::Document document;
        document.Parse<0>(jsonData.c_str());
        CCLOG("api apprequests on receive: %s", jsonData.c_str());
        
        std::string request = "";
        std::string listIDs = "";
        std::string result = "";
        if(document.HasMember("request"))
        {
            request = document["request"].GetString();
            if(document.HasMember("to"))
            {
                const rapidjson::Value& data = document["to"];
                for(int i = 0; i < data.Size(); i++)
                {
                    std::string id = data[i].GetString();
                    listIDs += id.c_str();
                    listIDs += (i + 1) < data.Size() ? "," : "";
                }
            }
            
            result = StringUtils::format("{ \"requestID\": \"%s\", \"token\" : \"%s\", \"ids\": %s}", request.c_str(), GetFacebookMgr()->getAccessToken().c_str(), listIDs.c_str());
        }
        
        if(_call_back_command)
        {
            _call_back_command(true, (void*)(result.c_str()));
        }
    }
}

void FacebookMgr::onPermission(bool isLogin, const std::string& error)
{
    CCLOG("##FacebookMgr onPermission: %d, error: %s", isLogin, error.c_str());
    
    std::string title = "permission ";
    title.append((isLogin ? "success" : "failed"));
    
    if(_call_back_command)
    {
        _call_back_command(isLogin, nullptr);
    }
}

void FacebookMgr::onFetchFriends(bool isOK, const std::string& msg)
{
    CCLOG("##FacebookMgr %s: %d = %s", __FUNCTION__, isOK, msg.data());
    
    rapidjson::Document document;
    document.Parse<0>(msg.c_str());
    std::string nextURL = document["paging"]["next"].GetString();
    
    std::vector<sdkbox::FBGraphUser> friends = PluginFacebook::getFriends();
    //convert to FacebookFriend to return
    FacebookFriendList listFriend;
    for(std::vector<FBGraphUser>::iterator it = friends.begin(); it != friends.end(); it++)
    {
        FacebookFriend* fbFriend = new FacebookFriend();
        fbFriend->setId(it->getUserId());
        fbFriend->setName(it->getName());
        
        FacebookPictureData fbPictureData;
        fbPictureData.setUrl(it->getPictureURL());
        fbPictureData.setIs_silhouette(it->isAppInstalled());
        
        FacebookPicture fbPicture;
        fbPicture.setData(fbPictureData);
        fbFriend->setPicture(fbPicture);
        
        fbFriend->setSelected(true);
        
        listFriend.addFriend(fbFriend);
    }
        
    if(_call_back_command) {
        _call_back_command(isOK, (void*) &listFriend);
    }
}

void FacebookMgr::onRequestInvitableFriends(const sdkbox::FBInvitableFriendsInfo& friends)
{
#if SDKBOX_ENABLED
    CCLOG("Request Invitable Friends Begin");
    for (auto it = friends.begin(); it != friends.end(); ++it) {
//        _invitableUsers.push_back(*it);
        CCLOG("Invitable friend: %s", it->getName().c_str());
    }
    
    CCLOG("Request Invitable Friends End");
    if (0 == _invitableUsers.size()) {
        CCLOG("WARNING! Your Invitable Friends number is 0");
    }
    
#endif
}

void FacebookMgr::onInviteFriendsWithInviteIdsResult(bool result, const std::string& msg)
{
    CCLOG("on invite friends with invite ids %s= '%s'", result ? "ok" : "error", msg.c_str());
    
    if(result)
    {
        if(existListFriendID.size() > 0)
        {
            inviteFriends(existListFriendID, _call_back_command);
            return;
        }
    }
    
    if(_call_back_command)
    {
        _call_back_command(result, (void*)(msg.c_str()));
    }
}

void FacebookMgr::onInviteFriendsResult(bool result, const std::string& msg)
{
    CCLOG("on invite friends %s= '%s'", result ? "ok" : "error", msg.c_str());
}

void FacebookMgr::onGetUserInfo(const sdkbox::FBGraphUser& userInfo)
{
    CCLOG("Facebook id:'%s' name:'%s' last_name:'%s' first_name:'%s' email:'%s' installed:'%d'",
          userInfo.getUserId().c_str(),
          userInfo.getName().c_str(),
          userInfo.getFirstName().c_str(),
          userInfo.getLastName().c_str(),
          userInfo.getEmail().c_str(),
          userInfo.isInstalled ? 1 : 0
          );
    
    FacebookMyInfo myInfo;
    myInfo.setId(userInfo.getUserId());
    myInfo.setEmail(userInfo.getEmail());
    myInfo.setFirst_name(userInfo.getFirstName());
    myInfo.setLast_name(userInfo.getLastName());
    myInfo.setName(userInfo.getName());
    myInfo.setVerified(userInfo.isInstalled);
    
    if(_call_back_command) {
        _call_back_command(true, (void*) &myInfo);
    }
}

#endif

void FacebookMgr::postWall(std::string json_string, const std::function<void(bool, void*)> & call_back)
{
#if SDKBOX_ENABLED
    CCLOG("##FacebookMgr %s", __FUNCTION__);
    _call_back_command = call_back;
    
    FBShareInfo info;
    info.type  = FB_LINK;
    info.link  = "https://iwin.me";
    info.title = "Iwin Online";
    info.text  = "Game bai doi thuong";
    info.image = "http://cocos2d-x.org/images/logo.png";
    //this's only work by customize UI dialog ???
    PluginFacebook::share(info);
    
//    PluginFacebook::dialog(info);
    
#endif
}

void FacebookMgr::likePage(std::string url, const std::function<void(bool, void*)> & call_back)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    _call_back_command = call_back;
    OPEN_URL(url);
    
    if(_call_back_command)
    {
        _call_back_command(true, nullptr);
    }
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
    
#endif
}

void FacebookMgr::shareLink(std::string link, const std::function<void(bool, void*)> & call_back)
{
#if SDKBOX_ENABLED
    CCLOG("##FacebookMgr %s", __FUNCTION__);
    _call_back_command = call_back;
    
    FBShareInfo info;
    info.type  = FB_LINK;
    info.link  = "https://iwin.me";
    info.title = "Iwin Online";
    info.text  = "Game bai doi thuong";
    info.image = "http://cocos2d-x.org/images/logo.png";
    PluginFacebook::dialog(info);
    
    //this's only work by customize UI dialog ???
    //PluginFacebook::share(info);
    
#endif
}

void FacebookMgr::getInvitableFriend(const std::function<void(bool, void*)> & call_back)
{
#if SDKBOX_ENABLED
    CCLOG("##FacebookMgr %s", __FUNCTION__);    
//    PluginFacebook::fetchFriends();
    
    _call_back_command = call_back;

    sdkbox::FBAPIParam params;
    params["limit"] = "25";
    params["pretty"] = "1";
    //params["after"] = "";
    params["access_token"] = PluginFacebook::getAccessToken();
    //params["req_perms"] = "[user_friends]";
//    params["fields"] = "id,name,installed,first_name,last_name,pictures";
    
    PluginFacebook::api("me/invitable_friends", "GET", params, "__fb_me_invitable_friends_tag__");
    
    //sdkbox::FBAPIParam params;
    //PluginFacebook::requestInvitableFriends(params);
    
//    sdkbox::FBAPIParam paramsMe;
//    paramsMe["fields"] = "id,name,email,first_name,last_name,picture,gender,link,locale,verified,timezone,updated_time";
//    PluginFacebook::api("me", "GET", paramsMe, "me");
    
#endif
}

void FacebookMgr::inviteFriends(std::vector<std::string> listID, const std::function<void(bool, void*)> & call_back)
{
#if SDKBOX_ENABLED
    CCLOG("##FacebookMgr %s", __FUNCTION__);
    
//        PluginFacebook::inviteFriends(
//                                      //"https://fb.me/322164761287181",
//                                      "https://fb.me/402104549959868",
//                                      "http://www.cocos2d-x.org/attachments/802/cocos2dx_landscape.png");
    
    _call_back_command = call_back;
    if(listID.size() <= MAX_INVITE_FRIEND_PER_REQUEST)
    {
        existListFriendID.clear();
        PluginFacebook::inviteFriendsWithInviteIds(listID, "Iwin Online Invitation", "Choi game doi thuong ne");
    }
    else
    {
        std::vector<std::string> subListID(listID.begin(), listID.begin() + MAX_INVITE_FRIEND_PER_REQUEST - 1);
        existListFriendID = std::vector<std::string>(listID.begin() + MAX_INVITE_FRIEND_PER_REQUEST, listID.end());
        
//        std::string toStr = "";
//        int idx = 0;
//        for(auto fbID : listID)
//        {
//            idx++;
//            toStr += (fbID.c_str());
//            toStr += idx < listID.size() ? "," : "";
//        }
        
//        sdkbox::FBAPIParam params;
//        params["action_type"] = "SEND";
//        params["to"] = toStr;
//        params["message"] = "send request facebook";
//        params["data"] = "{\"type\":\"1\"}";
//        params["access_token"] = GetFacebookMgr()->getAccessToken();
//        PluginFacebook::api("me/apprequests", "POST", params, "__fb_me_send_request_tag__");
    
        PluginFacebook::inviteFriendsWithInviteIds(subListID, "Iwin Online Invitation", "Choi game doi thuong ne");
    }
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WINRT


	WPPlatform::WPHelper::loginFacebook([listID](void* data, std::string accessToken) {
		WPPlatform::WPHelper::inviteFriendList(accessToken, listID, "Iwin Online", "http://www.iwin.me/", [](std::string ids) {


		});
	});
#else
			
#endif
}

void FacebookMgr::sendRequest(std::vector<std::string> facebookIDs, std::string title, std::string msg, std::string jsonData, const std::function<void(bool,void*)> & call_back)
{
#if SDKBOX_ENABLED
    _call_back_command = call_back;
    
//    FBShareInfo info;
//    info.type  = FB_LINK;
//    info.link  = "https://iwin.me";
//    info.title = title;
//    info.text  = msg;
//    info.image = "http://cocos2d-x.org/images/logo.png";
//    PluginFacebook::dialog(info);
    
    std::string toStr = "";
    int idx = 0;
    for(auto fbID : facebookIDs)
    {
        idx++;
        toStr += (fbID.c_str());
        toStr += idx < facebookIDs.size() ? "," : "";
    }
    
    sdkbox::FBAPIParam params;
    params["action_type"] = "SEND";
    params["to"] = toStr;
    params["message"] = "send request facebook";
    params["data"] = "{\"type\":\"1\"}";
    params["access_token"] = GetFacebookMgr()->getAccessToken();
    PluginFacebook::api("me/apprequests", "POST", params, "__fb_me_send_request_tag__");
#endif
}

void FacebookMgr::checkStatus()
{
#if SDKBOX_ENABLED
    CCLOG("##FacebookMgr %s", __FUNCTION__);
    
    CCLOG("##FacebookMgr> permission list: ");
    for (auto& permission : PluginFacebook::getPermissionList())
    {
        CCLOG("##FacebookMgr>> permission %s", permission.data());
    }
    CCLOG("##FacebookMgr> access token: %s", PluginFacebook::getAccessToken().c_str());
    CCLOG("##FacebookMgr> user id: %s", PluginFacebook::getUserID().c_str());
    CCLOG("##FacebookMgr> FBSDK version: %s", PluginFacebook::getSDKVersion().c_str());
#endif
}

void FacebookMgr::getMyInfo(const std::function<void(bool, void*)> &call_back)
{
#if SDKBOX_ENABLED
    CCLOG("##FacebookMgr %s", __FUNCTION__);
    _call_back_command = call_back;
    
    sdkbox::FBAPIParam params;
    params["fields"] = "id,name,email,first_name,last_name,picture,gender,link,locale,verified,timezone,updated_time";
    PluginFacebook::api("me", "GET", params, "me");
#endif
}

void FacebookMgr::requestReadPermission(const std::function<void(bool, void*)> &call_back)
{
#if SDKBOX_ENABLED
    CCLOG("##FacebookMgr %s", __FUNCTION__);
    _call_back_command = call_back;
    
    PluginFacebook::requestPublishPermissions({FB_PERM_READ_USER_FRIENDS});
#endif
}

void FacebookMgr::requestPublishPermission(const std::function<void(bool, void*)> &call_back)
{
#if SDKBOX_ENABLED
    CCLOG("##FacebookMgr %s", __FUNCTION__);
    _call_back_command = call_back;
    
    PluginFacebook::requestPublishPermissions({FB_PERM_PUBLISH_POST});
#endif
}

bool FacebookMgr::checkPermissionExist(std::string permission)
{
    bool result = false;
#if SDKBOX_ENABLED
    for (auto& permission : PluginFacebook::getPermissionList())
    {
        result = permission.data() == permission ? "true" : "false";
        CCLOG("##FacebookMgr>> permission %d", result);
        if(result)
            break;
    }
    
#endif
    
    return result;
}




#ifndef __FACEBOOK_MGR_H__
#define __FACEBOOK_MGR_H__

#include <string>
#include <functional>
#include <vector>

#ifdef SDKBOX_ENABLED
#include "PluginFacebook/PluginFacebook.h"
using namespace sdkbox;
class FacebookMgr :public FacebookListener
#else
class FacebookMgr
#endif
{
private:
	static FacebookMgr*	s_instance;
    
	std::function<void(bool, void*)> _call_back_command;
    
#if SDKBOX_ENABLED
    std::vector<sdkbox::FBGraphUser*> _invitableUsers;
    std::vector<int> _inviteUserIdxs;
#endif
    
public:
    const static int MAX_INVITE_FRIEND_PER_REQUEST = 50;
//    const static int MAX_REQUEST_INVITABLE_FRIEND = 5;
//    int maxCount = 0;
    
    std::vector<std::string> existListFriendID;
    
	FacebookMgr();
	virtual ~FacebookMgr();
    
	static FacebookMgr* getInstance();
    
	void init();
    void login(const std::function<void(bool, void*)> &call_back);
    void logout();
	bool isLoggedIn();
	std::string getAccessToken();

	void SetCallBackCommand(const std::function<void(bool, void*)> & call_back);
    
    void checkStatus();
    void getMyInfo(const std::function<void(bool, void*)> &call_back);
    bool checkPermissionExist(std::string permission);
    void requestReadPermission(const std::function<void(bool, void*)> &call_back);
    void requestPublishPermission(const std::function<void(bool, void*)> &call_back);
    
#ifdef SDKBOX_ENABLED
    
	virtual void onLogin(bool isLogin, const std::string& msg) override;
	virtual void onSharedSuccess(const std::string& msg)override;
	virtual void onSharedFailed(const std::string& msg)override;
	virtual void onSharedCancel()override;
	virtual void onAPI(const std::string& tag, const std::string& jsonData)override;
	virtual void onPermission(bool isLogin, const std::string& error)override;
	virtual void onFetchFriends(bool ok, const std::string& msg)override;
	virtual void onRequestInvitableFriends(const FBInvitableFriendsInfo& friends)override;
	virtual void onInviteFriendsWithInviteIdsResult(bool result, const std::string& msg)override;
	virtual void onInviteFriendsResult(bool result, const std::string& msg)override;
	virtual void onGetUserInfo(const FBGraphUser& userInfo)override;
#else

#endif

	/*
	* post lên tường nhà mình, với chuổi string và call back trả về có post thành công hay kok !
	*/
	void postWall(std::string json_string, const std::function<void(bool, void*)> & call_back);
	/*
	* like page fb theo URL truyền vào, trả về call_back có like thành công hay kok 
	*/
	void likePage(std::string url, const std::function<void(bool, void*)> & call_back);
	/*
	* chia sể 1 đường link lên fb, trả về fb khi thành công hay kok
	*/
	void shareLink(std::string link, const std::function<void(bool, void*)> & call_back);
	// có call back
	void getInvitableFriend(const std::function<void(bool, void*)> & call_back);
	//
	/*
	* mời danh sách người chơi để chơi game, trả về call back có thành công hay kok
	*/
	void inviteFriends(std::vector<std::string> ls_ids, const std::function<void(bool, void*)> & call_back);

	//gửi 1 đoạn request lên , xem thêm trong Platform::FBUtils để biết thêm chi tiết
	void sendRequest(std::vector<std::string> facebookIDs, std::string title, std::string msg, std::string jsonData, const std::function<void(bool, void*)> & call_back);
};

#define GetFacebookMgr() FacebookMgr::getInstance()

#endif //__FACEBOOK_MGR_H__

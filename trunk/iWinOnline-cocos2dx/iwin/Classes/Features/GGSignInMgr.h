//
//  GooglePlayMgr.hpp
//  iwin_ios
//
//  Created by LA NGOC MINH TRUNG on 10/4/16.
//
//

#ifndef __GOOGLE_SIGNIN_MGR_H__
#define __GOOGLE_SIGNIN_MGR_H__

#include <stdio.h>
#include <string>
#include <functional>
#include "Platform/Platform.h"

class GGSignInMgr
{
private:
    static GGSignInMgr* s_instance;
    std::string _ggToken = "";
    bool _isConnected = false;
    
    GGSignInMgr();
public:
    virtual ~GGSignInMgr();
    
    static GGSignInMgr* getInstance();
    void init();
    
    void signIn();
    void signOut();
    bool isConnected();
    std::string getGGToken();

    typedef std::function<void(std::string)> ProfileCallBack;
	void getProfile(ProfileCallBack callback);
    
};

#define GetGGSignInMgr() GGSignInMgr::getInstance()

#endif /* __GOOGLE_SIGNIN_MGR_H__ */

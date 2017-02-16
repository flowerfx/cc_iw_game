#ifndef _GUIDE_SERVICE_H__
#define _GUIDE_SERVICE_H__

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "network/JsonObject/Guides.h"

using namespace iwinmesage;

class GuideService
{
private:
	const char* URL_REQUEST_ALL_HELP = "http://av.iwin.me/wf/help/help_link";
	const char* URL_REQUEST_IOS_HELP = "http://av.iwin.me/wf/help/help_link_ios";
	const char* IWIN_PAGE = "http://iwin.me/news/huong-dan";
	static	GuideService*		s_instance;
	Guides*						p_guides;
	CC_SYNTHESIZE(bool, _isLoadDataFromServerOK, IsLoadDataFromServerOK)
public:
	static GuideService* getInstance();
	GuideService();
	~GuideService();
	void onHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
	void requestAllHelp();
	void ShowCustomerService();
	void ShowHelpGameType();
	void ShowIwinPage();
	Guides * GetGuide();
};

#define GetGuideService() GuideService::getInstance()
#define GET_GUIDE GuideService::getInstance()->GetGuide()
#endif //_GUIDE_SERVICE_H__

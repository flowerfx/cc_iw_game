#ifndef __NOHU_USER_HISTORY_SERVER_H__
#define __NOHU_USER_HISTORY_SERVER_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "NoHuUserHistoryDetail.h"
namespace iwinmesage
{
	class NoHuUserHistoryServer
	{
	
CC_SYNTHESIZE(int, page, Page);
CC_SYNTHESIZE(int, moneyType, MoneyType);
CC_SYNTHESIZE(int, totalPage, TotalPage);

CC_SYNTHESIZE(std::vector<NoHuUserHistoryDetail>, history, History);

	public:
		
		NoHuUserHistoryServer();
		virtual ~NoHuUserHistoryServer();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__NOHU_USER_HISTORY_SERVER_H__

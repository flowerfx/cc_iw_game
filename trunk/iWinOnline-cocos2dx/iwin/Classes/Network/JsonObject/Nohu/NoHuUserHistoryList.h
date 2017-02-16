#ifndef __NOHU_USER_HISTORY_LIST_H__
#define __NOHU_USER_HISTORY_LIST_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "NoHuUserHistory.h"
namespace iwinmesage
{
	class NoHuUserHistoryList
	{
		
CC_SYNTHESIZE(std::vector<NoHuUserHistory>, userHistoryList, UserHistoryList);

	public:
		
		NoHuUserHistoryList();
		virtual ~NoHuUserHistoryList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__NOHU_USER_HISTORY_LIST_H__

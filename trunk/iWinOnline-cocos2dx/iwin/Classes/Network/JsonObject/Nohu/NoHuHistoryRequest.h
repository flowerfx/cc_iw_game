#ifndef __NOHU_HISTORY_REQUEST_H__
#define __NOHU_HISTORY_REQUEST_H__

#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class NoHuHistoryRequest
	{
		
CC_SYNTHESIZE(int, page, Page);
CC_SYNTHESIZE(int, moneyType, MoneyType);

	public:
		
		NoHuHistoryRequest();
		virtual ~NoHuHistoryRequest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif

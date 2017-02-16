#ifndef __NOHU_USER_HISTORY_DETAIL_H__
#define __NOHU_USER_HISTORY_DETAIL_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class NoHuUserHistoryDetail
	{
		CC_SYNTHESIZE(long long,betMoney,BetMoney);
		CC_SYNTHESIZE(long long,receivedMoney, ReceivedMoney);
		CC_SYNTHESIZE(long long,time,Time);
		CC_SYNTHESIZE(std::vector<int>, cards, Cards);

	public:
		
		NoHuUserHistoryDetail();
		virtual ~NoHuUserHistoryDetail();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__NOHU_USER_HISTORY_DETAIL_H__

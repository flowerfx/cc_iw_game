#ifndef __NOHU_BET_REQUEST_H__
#define __NOHU_BET_REQUEST_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class NoHuBetRequest
	{
		
		//id hu dat cuoc
	CC_SYNTHESIZE(int, jarId, JarId); 
		// tien cuoc
	CC_SYNTHESIZE(long long, betMoney, BetMoney);
	CC_SYNTHESIZE(int, moneyType, MoneyType);

	public:
		
		NoHuBetRequest();
		virtual ~NoHuBetRequest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__NOHU_BET_REQUEST_H__

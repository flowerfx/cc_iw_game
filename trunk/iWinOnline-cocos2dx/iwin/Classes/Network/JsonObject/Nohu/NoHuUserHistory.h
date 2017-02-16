#ifndef __NOHU_USER_HISTORY_H__
#define __NOHU_USER_HISTORY_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "CardResultData.h"
namespace iwinmesage
{
	class NoHuUserHistory
	{
	
//
// card tra ve
//
CC_SYNTHESIZE(std::string, result, Result);
//
// user cua thang dc hu danh cho tab history
//
CC_SYNTHESIZE(std::string, userName, UserName);
//
// gia tri cua hu
//
CC_SYNTHESIZE(long long, time, Time);
//
// gia tri cua hu
//

CC_SYNTHESIZE(long long, resultMoney, ResultMoney);

CC_SYNTHESIZE(long long, HuChoice, HuChoice);


CC_SYNTHESIZE(std::vector<CardResultData>, cardResult, CardResult);

	public:
		
		NoHuUserHistory();
		virtual ~NoHuUserHistory();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__NOHU_USER_HISTORY_H__

#ifndef __NOHU_GAME_INFO_H__
#define __NOHU_GAME_INFO_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "NoHuJar.h"
namespace iwinmesage
{
	class NoHuGameInfo
	{
		//
		// loai tien (1 - win , 2 ruby)
		//	
CC_SYNTHESIZE(int, moneyType, MoneyType);
		//
		// tong tien hu lon
		//
CC_SYNTHESIZE(long long, totalMoney, TotalMoney);
		//
		//id cua hu dc goi y dat cuoc
		//
CC_SYNTHESIZE(int, suggestId, SuggestId);
		//
		//danh sach cac hu
		//
CC_SYNTHESIZE(std::vector<NoHuJar>, jars, Jars);
	public:
		
		NoHuGameInfo();
		virtual ~NoHuGameInfo();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__NOHU_GAME_INFO_H__

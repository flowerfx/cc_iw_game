#ifndef __NOHU_JAR_H__
#define __NOHU_JAR_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class NoHuJar
	{
		
		// id hu
CC_SYNTHESIZE(int, id, ID);
		// muc cuoc cua hu
CC_SYNTHESIZE(long long, betMoney, BetMoney);
		// tong tien trong hu
CC_SYNTHESIZE(long long, totalMoney, TotalMoney);
		// money type
CC_SYNTHESIZE(int, moneyType, MoneyType);
		// link lay hinh anh hu
CC_SYNTHESIZE(std::string, imgUrl, ImgUrl);
		// cho biet user co the dat cuoc hay khong
CC_SYNTHESIZE(bool, isCanBet, IsCanBet);

	public:
		
		NoHuJar();
		virtual ~NoHuJar();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__NOHU_JAR_H__

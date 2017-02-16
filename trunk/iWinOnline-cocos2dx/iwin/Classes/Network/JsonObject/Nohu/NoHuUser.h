#ifndef __NOHU_USER_H__
#define __NOHU_USER_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class NoHuUser
	{
		
CC_SYNTHESIZE(std::string, username, Username);
CC_SYNTHESIZE(long long, betMoney, BetMoney); //tien cuoc
CC_SYNTHESIZE(long long, receivedMoney, ReceivedMoney); //. tien thang
CC_SYNTHESIZE(std::vector<int>, cards, Cards); //bai
CC_SYNTHESIZE(long long, time, Time);

	public:
		
		NoHuUser();
		virtual ~NoHuUser();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__NOHU_USER_H__

#ifndef __TAIXIU_USER_BET_H__
#define __TAIXIU_USER_BET_H__
#include <string>
#include <vector>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
namespace iwinmesage
{
	class TaiXiuUserBet
	{
	public:

		CC_SYNTHESIZE(std::string, username, Username);
		//0 tai 1 xiu
		CC_SYNTHESIZE(int, betChoice, BetChoice);

		CC_SYNTHESIZE(long long, time, Time);

		CC_SYNTHESIZE(long long, betMoney, BetMoney);
	public:
		TaiXiuUserBet();
		virtual ~TaiXiuUserBet();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__TAIXIU_USER_BET_H__

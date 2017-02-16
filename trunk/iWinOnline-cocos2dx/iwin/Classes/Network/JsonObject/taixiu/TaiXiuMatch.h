#ifndef __TAIXIU_MATCH_H__
#define __TAIXIU_MATCH_H__
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "TaiXiuUserBet.h"
namespace iwinmesage
{
	class TaiXiuMatch
	{
	public:
		CC_SYNTHESIZE(int, id, Id);
		//thoi gian start van
		CC_SYNTHESIZE(long long, time, Time);
		//so nut cua 3 xuc xac
		CC_SYNTHESIZE(int, dice1, Dice1);
		CC_SYNTHESIZE(int, dice2, Dice2);
		CC_SYNTHESIZE(int, dice3, Dice3);
		//tai 0 xiu 1
		CC_SYNTHESIZE(int, result, Result);
		//
		CC_SYNTHESIZE(std::vector<TaiXiuUserBet>, userBets, UserBets);

		TaiXiuMatch();
		virtual ~TaiXiuMatch();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);

		void ReserveBetList();
	};
}

#endif //__TAIXIU_MATCH_H__

#ifndef __NOHU_BET_RESULT_H__
#define __NOHU_BET_RESULT_H__

#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "NoHuJar.h"
namespace iwinmesage
{
	class NoHuBetResult
	{
	//
	// 1 thanh cong, nguoc lai that bai
	//
CC_SYNTHESIZE(int, code, Code);
	//
	// thong bao loi (hien thi khi code != 1)
	//
CC_SYNTHESIZE(std::string , message, Message);
	//
	// dung de hien thi hieu ung dat cuoc
	//
CC_SYNTHESIZE(std::string, username, UserName);
CC_SYNTHESIZE(int, jarId, JarId); //id hu dat cuoc
CC_SYNTHESIZE(int, moneyType, MoneyType); //loai tien
	 //
	 // so tien da dat
	 //
CC_SYNTHESIZE(long long, betMoney, BetMoney); 
CC_SYNTHESIZE(long long, receivedMoney, ReceivedMoney); //so tien duoc nhan
	//
	// tong tien tat ca hu
	//
CC_SYNTHESIZE(long long, totalMoney, TotalMoney);
CC_SYNTHESIZE(long long, jarTotalMoney, JarTotalMoney); //tong tien trong hu vua dat cuoc
	//
	// thung , sanh , xam , ...
	//
CC_SYNTHESIZE(int, rewardId, RewardId);
	//
	// true neu no hu
	//
CC_SYNTHESIZE(bool, isNohu, IsNohu);
	//
	// mang chua gia tri 5 ka bai, sap xep tang dan
	//
CC_SYNTHESIZE(std::vector<int>, cards, Cards);
	
CC_SYNTHESIZE(std::vector<NoHuJar>, jars, Jars);


	public:
		
		NoHuBetResult();
		virtual ~NoHuBetResult();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
		//-1 is not find
		int GetIndexOfJarId(int jarId);
	};
}
#endif //__NOHU_BET_RESULT_H__

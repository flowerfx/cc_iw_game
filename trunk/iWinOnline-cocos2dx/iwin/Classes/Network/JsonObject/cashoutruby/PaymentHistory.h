#ifndef _PaymentHistory_H_
#define _PaymentHistory_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class PaymentHistory
	{
		
CC_SYNTHESIZE(int, logId, LogId);
CC_SYNTHESIZE(int, status, Status);
CC_SYNTHESIZE(std::string, time, Time);
CC_SYNTHESIZE(std::string, item, Item);
CC_SYNTHESIZE(std::string, statusDes, StatusDes);
CC_SYNTHESIZE(std::string, detail, Detail);
CC_SYNTHESIZE(std::string, pin, Pin);
CC_SYNTHESIZE(std::string, seri, Seri);

CC_SYNTHESIZE(int, chargeId, ChargeID);
CC_SYNTHESIZE(int, subChargeId, SubChargeID);

	public:
		
		PaymentHistory();
		virtual ~PaymentHistory();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif

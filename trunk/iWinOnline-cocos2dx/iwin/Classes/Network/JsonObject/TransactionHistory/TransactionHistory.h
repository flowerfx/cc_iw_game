#ifndef _TRANSACTION_HISTORY_H__
#define _TRANSACTION_HISTORY_H__
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "Common/Common.h"
namespace iwinmesage
{
	class TransactionHistoryDetail;
	class TransactionHistory
	{
		CC_SYNTHESIZE(std::vector<TransactionHistoryDetail*>, details, Details);
		CC_SYNTHESIZE(int, type, Type);
	public:
		TransactionHistory();
		virtual ~TransactionHistory();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //_TRANSACTION_HISTORY_H__
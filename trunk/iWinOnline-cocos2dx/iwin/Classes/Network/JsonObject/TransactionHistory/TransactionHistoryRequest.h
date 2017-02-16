#ifndef __TRANSACTION_HISTORY_REQUEST_H__
#define __TRANSACTION_HISTORY_REQUEST_H__
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "Common/Common.h"
namespace iwinmesage
{
	class TransactionHistoryRequest
	{
		// 0 : choi win , 1 : nap win, 2 : choi ruby , 3 : nap ruby
		CC_SYNTHESIZE(int, type, Type);
		CC_SYNTHESIZE(int, page, Page);
	public:
		TransactionHistoryRequest();
		virtual ~TransactionHistoryRequest();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__TRANSACTION_HISTORY_REQUEST_H__
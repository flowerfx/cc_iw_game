#ifndef __TRANSACTION_HISTORY_DETAIL_H__
#define __TRANSACTION_HISTORY_DETAIL_H__
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "Common/Common.h"
namespace iwinmesage
{
	class TransactionHistoryDetail
	{
		/*
			ma giao dich
		*/
		CC_SYNTHESIZE(s64, id, Id);
		/*
			thoi gian giao dich
		*/
		CC_SYNTHESIZE(std::string, time, Time);
		/*
			dich vu su dung
		*/
		CC_SYNTHESIZE(std::string, service, Service);
		/*
			phi phat sinh
		*/
		CC_SYNTHESIZE(int, value, Value);
		/*
			so du
		*/
		CC_SYNTHESIZE(u64, money, Money);
		/*
			mieu ta
		*/
		CC_SYNTHESIZE(std::string, desc, Desc);
	public:
		TransactionHistoryDetail();
		virtual ~TransactionHistoryDetail();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__TRANSACTION_HISTORY_DETAIL_H__
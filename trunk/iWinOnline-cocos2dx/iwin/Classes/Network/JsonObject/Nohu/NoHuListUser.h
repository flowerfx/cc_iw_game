#ifndef __NOHU_LIST_USER_H__
#define __NOHU_LIST_USER_H__

#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "NoHuUser.h"
namespace iwinmesage
{
	class NoHuListUser
	{
		
CC_SYNTHESIZE(int, page, Page);
CC_SYNTHESIZE(int, totalPage, TotalPage);
CC_SYNTHESIZE(int, moneyType, MoneyType);
CC_SYNTHESIZE(std::vector<NoHuUser>, users, Users);

	public:
		
		NoHuListUser();
		virtual ~NoHuListUser();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__NOHU_LIST_USER_H__

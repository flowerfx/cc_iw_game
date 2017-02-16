#ifndef __TAIXIU_MATCH_LIST_H__
#define __TAIXIU_MATCH_LIST_H__
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
#include "TaiXiuMatch.h"
namespace iwinmesage
{
	class TaiXiuMatchList
	{
	public:
		CC_SYNTHESIZE(std::vector<TaiXiuMatch>, matchList, MatchList);

		TaiXiuMatchList();
		virtual ~TaiXiuMatchList();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}

#endif //__TAIXIU_MATCH_LIST_H__

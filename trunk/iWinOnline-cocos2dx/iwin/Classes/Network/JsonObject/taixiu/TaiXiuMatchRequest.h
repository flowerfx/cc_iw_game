#ifndef __TAIXIU_MATCH_REQUEST_H__
#define __TAIXIU_MATCH_REQUEST_H__
#include <string>
#include "platform/CCPlatformMacros.h"
#include "json/document.h"
namespace iwinmesage
{
	class TaiXiuMatchRequest
	{
	public:
		CC_SYNTHESIZE(int, matchId, MatchId);

		TaiXiuMatchRequest();
		virtual ~TaiXiuMatchRequest();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}

#endif //__TAIXIU_MATCH_REQUEST_H__

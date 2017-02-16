#ifndef __NOHU_UNSUPPORT_LIST_H__
#define __NOHU_UNSUPPORT_LIST_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class NoHuUnsupportedList
	{
		
CC_SYNTHESIZE(bool, isWinEnabled, IsWinEnabled);
CC_SYNTHESIZE(bool, isRubyEnabled, IsRubyEnabled);
CC_SYNTHESIZE(std::vector<int>, gameList, GameList);
CC_SYNTHESIZE(std::vector<int>, screenList, ScreenList);

	public:
		
		NoHuUnsupportedList();
		virtual ~NoHuUnsupportedList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__NOHU_UNSUPPORT_LIST_H__

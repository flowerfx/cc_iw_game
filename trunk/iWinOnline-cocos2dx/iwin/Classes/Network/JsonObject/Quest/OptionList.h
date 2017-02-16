#ifndef _OptionList_H_
#define _OptionList_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "OptionItem.h"

namespace iwinmesage
{
	class OptionList
	{
		
CC_SYNTHESIZE(std::vector<OptionItem>, optionList, optionList);
CC_SYNTHESIZE(int, listType, ListType);

	public:
		static  int LIST_TYPE_SINGLE_CHOICE;
static  int LIST_TYPE_MULTI_CHOICE;

		OptionList();
		virtual ~OptionList();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);

		void pushItem(OptionItem op);
		void changeValueItemAtIdx(int idx, bool value);
	};
}
#endif

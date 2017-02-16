#ifndef __CARD_RESULT_DATA_H__
#define __CARD_RESULT_DATA_H__

#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class CardResultData
	{
	public:
		enum CARD_TYPE
		{
			BICH = 0,
			CHUON,
			RO,
			CO
		};

CC_SYNTHESIZE(std::string, dataCard, DataCard);
CC_SYNTHESIZE(int, type, Type);

	public:
		
		CardResultData();
		virtual ~CardResultData();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__CARD_RESULT_DATA_H__

#include "CardResultData.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	CardResultData::CardResultData()
	{
		dataCard = "";
		type = CARD_TYPE::BICH;
	}

	CardResultData::~CardResultData()
	{

	}

	rapidjson::Document CardResultData::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		rapidjson::Value vname(dataCard.c_str(), dataCard.size());
		document.AddMember("dataCard", vname,locator);
		document.AddMember("type", type,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void CardResultData::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void CardResultData::toData(rapidjson::Document & document)
	{
		if(document.HasMember("dataCard"))
		{
			setDataCard(document["dataCard"].GetString());
		}
		if(document.HasMember("type"))
		{
			setType(document["type"].GetInt());
		}
		
	}
}

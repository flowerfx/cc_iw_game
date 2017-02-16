#include "NoHuBetRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuBetRequest::NoHuBetRequest()
	{
		jarId = 0;
		betMoney = 0;
		moneyType = 0;

	}

	NoHuBetRequest::~NoHuBetRequest()
	{

	}

	rapidjson::Document NoHuBetRequest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		document.AddMember("jarId", jarId, locator);
		document.AddMember("betMoney", betMoney, locator);
		document.AddMember("moneyType", moneyType,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuBetRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuBetRequest::toData(rapidjson::Document & document)
	{

		if(document.HasMember("betMoney"))
		{
			setBetMoney(document["betMoney"].GetInt64());
		}
		if (document.HasMember("jarId"))
		{
			setJarId(document["jarId"].GetInt());
		}
		if (document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}
	}
}

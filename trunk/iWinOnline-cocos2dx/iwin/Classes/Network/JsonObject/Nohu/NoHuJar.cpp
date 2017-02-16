#include "NoHuJar.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuJar::NoHuJar()
	{
		id = 0;
		betMoney = 0;
		totalMoney = 0;
		moneyType = 0;
		imgUrl = "";
		isCanBet = false;
	}

	NoHuJar::~NoHuJar()
	{

	}

	rapidjson::Document NoHuJar::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		
		rapidjson::Value vname(imgUrl.c_str(), imgUrl.size());
		document.AddMember("imgUrl", vname,locator);
		document.AddMember("id", id,locator);
		document.AddMember("betMoney", betMoney, locator);
		document.AddMember("totalMoney", totalMoney, locator);
		document.AddMember("moneyType", moneyType, locator);
		document.AddMember("isCanBet", isCanBet, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuJar::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuJar::toData(rapidjson::Document & document)
	{
		if(document.HasMember("id"))
		{
			setID(document["id"].GetInt());
		}
		if(document.HasMember("betMoney"))
		{
			setBetMoney(document["betMoney"].GetInt64());
		}
		if (document.HasMember("totalMoney"))
		{
			setTotalMoney(document["totalMoney"].GetInt64());
		}
		if (document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}
		if (document.HasMember("imgUrl"))
		{
			setImgUrl(document["imgUrl"].GetString());
		}
		if (document.HasMember("isCanBet"))
		{
			setIsCanBet(document["isCanBet"].GetBool());
		}


	}
}

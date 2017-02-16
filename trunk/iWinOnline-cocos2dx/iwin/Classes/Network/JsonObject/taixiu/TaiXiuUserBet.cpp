#include "TaiXiuUserBet.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuUserBet::TaiXiuUserBet()
	{
		username = "";
		betChoice = 0;
		time = 0;
		betMoney = 0;
	}
	TaiXiuUserBet::~TaiXiuUserBet()
	{
	}

	rapidjson::Document TaiXiuUserBet::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		rapidjson::Value vname(username.c_str(), username.size());
		document.AddMember("username", vname, locator);
		document.AddMember("betChoice", betChoice, locator);
		document.AddMember("time", time, locator);
		document.AddMember("betMoney", betMoney, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuUserBet::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuUserBet::toData(rapidjson::Document & document)
	{
		if (document.HasMember("username"))
		{
			setUsername(document["username"].GetString());
		}
		if (document.HasMember("betChoice"))
		{
			setBetChoice(document["betChoice"].GetInt());
		}
		if (document.HasMember("time"))
		{
			setTime(document["time"].GetInt64());
		}
		if (document.HasMember("betMoney"))
		{
			setBetMoney(document["betMoney"].GetInt64());
		}
	}
}
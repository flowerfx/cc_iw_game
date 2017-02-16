#include "NoHuBetResult.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuBetResult::NoHuBetResult()
	{
		code = 1;
		message = "";
		username = "";
		jarId = moneyType = 0;
		betMoney = receivedMoney = 0;
		totalMoney = jarTotalMoney = 0;
		rewardId = 0;
		isNohu = false;
		jars.clear();
		cards.clear();
	}

	NoHuBetResult::~NoHuBetResult()
	{
		jars.clear();
		cards.clear();
	}

	rapidjson::Document NoHuBetResult::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		rapidjson::Value arrCard(rapidjson::kArrayType);
		for (size_t i = 0; i < cards.size(); i++)
		{
			arrCard.PushBack(rapidjson::Value().SetInt(cards[i]), locator);
		}
		document.AddMember("cards", arrCard, locator);

		rapidjson::Value arrList(rapidjson::kArrayType);
		for (size_t i = 0; i < jars.size(); i++)
		{
			rapidjson::Value opt(jars.at(i).toJson(), locator);
			arrList.PushBack(opt, locator);
		}
		document.AddMember("jars", arrList, locator);

		rapidjson::Value vname(message.c_str(), message.size());
		document.AddMember("message", vname,locator);
		document.AddMember("code", code,locator);
		rapidjson::Value vusername(username.c_str(), username.size());
		document.AddMember("username", vusername, locator);
		document.AddMember("jarId", jarId, locator);
		document.AddMember("moneyType", moneyType, locator);
		document.AddMember("betMoney", betMoney, locator);
		document.AddMember("receivedMoney", receivedMoney, locator);
		document.AddMember("totalMoney", totalMoney, locator);
		document.AddMember("jarTotalMoney", jarTotalMoney, locator);

		document.AddMember("rewardId", rewardId, locator);
		document.AddMember("isNohu", isNohu, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuBetResult::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuBetResult::toData(rapidjson::Document & document)
	{
		if(document.HasMember("code"))
		{
			setCode(document["code"].GetInt());
		}
		if(document.HasMember("message"))
		{
			setMessage(document["message"].GetString());
		}
		if (document.HasMember("username"))
		{
			setUserName(document["username"].GetString());
		}
		if (document.HasMember("jarId"))
		{
			setJarId(document["jarId"].GetInt());
		}
		if (document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}
		if (document.HasMember("betMoney"))
		{
			setBetMoney(document["betMoney"].GetInt64());
		}
		if (document.HasMember("receivedMoney"))
		{
			setReceivedMoney(document["receivedMoney"].GetInt64());
		}
		if (document.HasMember("totalMoney"))
		{
			setTotalMoney(document["totalMoney"].GetInt64());
		}
		if (document.HasMember("jarTotalMoney"))
		{
			setJarTotalMoney(document["jarTotalMoney"].GetInt64());
		}
		if (document.HasMember("rewardId"))
		{
			setRewardId(document["rewardId"].GetInt());
		}
		if (document.HasMember("isNohu"))
		{
			setIsNohu(document["isNohu"].GetBool());
		}

		if(document.HasMember("cards") && !document["cards"].IsNull() && document["cards"].IsArray() )
		{
			const rapidjson::Value& arrList = document["cards"];
			for (rapidjson::SizeType i = 0; i < arrList.Size(); i++)
			{
				cards.push_back(arrList[i].GetInt());
			}
		}
		if(document.HasMember("jars") && !document["jars"].IsNull() && document["jars"].IsArray() )
		{
			const rapidjson::Value& arrList = document["jars"];
			for (rapidjson::SizeType i = 0; i < arrList.Size(); i++)
			{
				const rapidjson::Value& c = arrList[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				NoHuJar d;
				d.toData(buffer.GetString());
				jars.push_back(d);
			}
		}


	}

	int  NoHuBetResult::GetIndexOfJarId(int jarId)
	{
		for (int i = 0; i < jars.size(); i++)
		{
			if (jarId == jars[i].getID())
			{
				return i;
			}
		}
		return -1;
	}


}

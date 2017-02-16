#include "NoHuGameInfo.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuGameInfo::NoHuGameInfo()
	{
		moneyType = 1;
		totalMoney = 0;
		suggestId = -1;
		jars.clear();
	}

	NoHuGameInfo::~NoHuGameInfo()
	{
		jars.clear();
	}

	rapidjson::Document NoHuGameInfo::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		

		rapidjson::Value arrList(rapidjson::kArrayType);
		for (size_t i = 0; i < jars.size(); i++)
		{
			rapidjson::Value opt(jars.at(i).toJson(), locator);
			arrList.PushBack(opt, locator);
		}
		document.AddMember("jars", arrList, locator);

		document.AddMember("moneyType", moneyType,locator);
		document.AddMember("totalMoney", totalMoney, locator);
		document.AddMember("suggestId", suggestId, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuGameInfo::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuGameInfo::toData(rapidjson::Document & document)
	{

		if(document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}
		if (document.HasMember("totalMoney"))
		{
			setTotalMoney(document["totalMoney"].GetInt64());
		}
		if (document.HasMember("suggestId"))
		{
			setSuggestId(document["suggestId"].GetInt());
		}

		if (document.HasMember("jars") && !document["jars"].IsNull() && document["jars"].IsArray())
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
}

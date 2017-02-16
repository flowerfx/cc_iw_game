#include "NoHuUserHistory.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuUserHistory::NoHuUserHistory()
	{
		result = userName = "";
		time = HuChoice = resultMoney = 0;
		cardResult.clear();
	}

	NoHuUserHistory::~NoHuUserHistory()
	{
		cardResult.clear();
	}

	rapidjson::Document NoHuUserHistory::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		rapidjson::Value arrList(rapidjson::kArrayType);
		for (size_t i = 0; i < cardResult.size(); i++)
		{
			rapidjson::Value opt(cardResult.at(i).toJson(), locator);
			arrList.PushBack(opt, locator);
		}
		document.AddMember("cardResult", arrList, locator);

		rapidjson::Value vname(userName.c_str(), userName.size());
		document.AddMember("userName", vname,locator);
		rapidjson::Value vresult(result.c_str(), result.size());
		document.AddMember("result", vresult, locator);

		document.AddMember("time", time,locator);
		document.AddMember("HuChoice", HuChoice, locator);
		document.AddMember("resultMoney", resultMoney, locator);


		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuUserHistory::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuUserHistory::toData(rapidjson::Document & document)
	{
		if(document.HasMember("result"))
		{
			setResult(document["result"].GetString());
		}
		if (document.HasMember("userName"))
		{
			setUserName(document["userName"].GetString());
		}
		if(document.HasMember("time"))
		{
			setTime(document["time"].GetInt64());
		}
		if (document.HasMember("HuChoice"))
		{
			setHuChoice(document["HuChoice"].GetInt64());
		}
		if (document.HasMember("resultMoney"))
		{
			setResultMoney(document["resultMoney"].GetInt64());
		}
		if (document.HasMember("cardResult") && !document["cardResult"].IsNull() && document["cardResult"].IsArray())
		{
			const rapidjson::Value& arrList = document["cardResult"];
			for (rapidjson::SizeType i = 0; i < arrList.Size(); i++)
			{
				const rapidjson::Value& c = arrList[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				CardResultData d;
				d.toData(buffer.GetString());
				cardResult.push_back(d);
			}
		}


	}
}

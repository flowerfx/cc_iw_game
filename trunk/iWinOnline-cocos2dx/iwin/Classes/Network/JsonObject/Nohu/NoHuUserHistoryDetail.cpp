#include "NoHuUserHistoryDetail.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuUserHistoryDetail::NoHuUserHistoryDetail()
	{
		betMoney = receivedMoney = time = 0;
		cards.clear();
	}

	NoHuUserHistoryDetail::~NoHuUserHistoryDetail()
	{
		cards.clear();
	}

	rapidjson::Document NoHuUserHistoryDetail::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		rapidjson::Value arrlist(rapidjson::kArrayType);
		for (size_t i = 0; i < cards.size(); i++)
		{
			arrlist.PushBack(rapidjson::Value().SetInt(cards[i]), locator);
		}
		document.AddMember("cards", arrlist, locator);

		document.AddMember("betMoney", betMoney,locator);
		document.AddMember("receivedMoney", receivedMoney, locator);
		document.AddMember("time", time, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuUserHistoryDetail::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuUserHistoryDetail::toData(rapidjson::Document & document)
	{

		if(document.HasMember("betMoney"))
		{
			setBetMoney(document["betMoney"].GetInt64());
		}
		if (document.HasMember("receivedMoney"))
		{
			setReceivedMoney(document["receivedMoney"].GetInt64());
		}
		if (document.HasMember("time"))
		{
			setTime(document["time"].GetInt64());
		}
		if(document.HasMember("cards") && !document["cards"].IsNull() && document["cards"].IsArray() )
		{
			const rapidjson::Value& arrlist = document["cards"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
				cards.push_back(arrlist[i].GetInt());
			}
		}


	}
}

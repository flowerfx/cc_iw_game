#include "NoHuUserHistoryServer.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuUserHistoryServer::NoHuUserHistoryServer()
	{
		page = totalPage = 0;
		moneyType = 1;
		history.clear();
	}

	NoHuUserHistoryServer::~NoHuUserHistoryServer()
	{
		history.clear();
	}

	rapidjson::Document NoHuUserHistoryServer::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		rapidjson::Value arrList(rapidjson::kArrayType);
		for (size_t i = 0; i < history.size(); i++)
		{
			rapidjson::Value opt(history.at(i).toJson(), locator);
			arrList.PushBack(opt, locator);
		}
		document.AddMember("history", arrList, locator);

		document.AddMember("page", page, locator);
		document.AddMember("totalPage", totalPage, locator);
		document.AddMember("moneyType", moneyType,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuUserHistoryServer::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuUserHistoryServer::toData(rapidjson::Document & document)
	{

		if(document.HasMember("page"))
		{
			setPage(document["page"].GetInt());
		}
		if (document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}
		if (document.HasMember("totalPage"))
		{
			setTotalPage(document["totalPage"].GetInt());
		}

		if (document.HasMember("history") && !document["history"].IsNull() && document["history"].IsArray())
		{
			const rapidjson::Value& arrList = document["history"];
			for (rapidjson::SizeType i = 0; i < arrList.Size(); i++)
			{
				const rapidjson::Value& c = arrList[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				NoHuUserHistoryDetail d;
				d.toData(buffer.GetString());
				history.push_back(d);
			}
		}

	}
}

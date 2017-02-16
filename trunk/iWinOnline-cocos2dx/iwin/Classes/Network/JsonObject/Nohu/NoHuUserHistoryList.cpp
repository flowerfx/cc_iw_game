#include "NoHuUserHistoryList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuUserHistoryList::NoHuUserHistoryList()
	{
		userHistoryList.clear();
	}

	NoHuUserHistoryList::~NoHuUserHistoryList()
	{
		userHistoryList.clear();
	}

	rapidjson::Document NoHuUserHistoryList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		rapidjson::Value arrList(rapidjson::kArrayType);
		for (size_t i = 0; i < userHistoryList.size(); i++)
		{
			rapidjson::Value opt(userHistoryList.at(i).toJson(), locator);
			arrList.PushBack(opt, locator);
		}
		document.AddMember("userHistoryList", arrList, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuUserHistoryList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuUserHistoryList::toData(rapidjson::Document & document)
	{
		if (document.HasMember("userHistoryList") && !document["userHistoryList"].IsNull() && document["userHistoryList"].IsArray())
		{
			const rapidjson::Value& arrList = document["userHistoryList"];
			for (rapidjson::SizeType i = 0; i < arrList.Size(); i++)
			{
				const rapidjson::Value& c = arrList[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				NoHuUserHistory d;
				d.toData(buffer.GetString());
				userHistoryList.push_back(d);
			}
		}

	}
}

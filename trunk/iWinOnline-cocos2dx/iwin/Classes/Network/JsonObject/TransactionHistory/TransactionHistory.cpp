#include "TransactionHistory.h"
#include "TransactionHistoryDetail.h"
#include "json/writer.h"
namespace iwinmesage
{
	TransactionHistory::TransactionHistory() {
		details.clear();
		type = 0;
	}


	TransactionHistory::~TransactionHistory() {
		for (auto d : details)
		{
			delete d;
		}
		details.clear();
	}


	rapidjson::Document TransactionHistory::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		document.AddMember("type", type, locator);
		rapidjson::Value arrList(rapidjson::kArrayType);
		for (size_t i = 0; i < details.size(); i++)
		{
			arrList.PushBack(details[i]->toJson(), locator);
		}
		document.AddMember("details", arrList, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TransactionHistory::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TransactionHistory::toData(rapidjson::Document & document)
	{
		if (document.HasMember("type"))
		{
			setType(document["type"].GetInt());
		}
		if (document.HasMember("details") && !document["details"].IsNull() && document["details"].IsArray())
		{
			const rapidjson::Value& arrlist = document["details"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
				const rapidjson::Value& c = arrlist[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				const char* json = buffer.GetString();

				TransactionHistoryDetail * d = new TransactionHistoryDetail();
				d->toData(json);

				details.push_back(d);
			}
		}
	}
}

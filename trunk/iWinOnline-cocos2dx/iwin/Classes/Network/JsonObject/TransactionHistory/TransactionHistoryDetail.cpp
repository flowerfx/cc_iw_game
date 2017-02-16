#include "TransactionHistoryDetail.h"
#include "json/writer.h"
namespace iwinmesage
{
	TransactionHistoryDetail::TransactionHistoryDetail() {
		id = 0;
		time = "";
		service = "";
		value = 0;
		money = 0;
		desc = "";
	}


	TransactionHistoryDetail::~TransactionHistoryDetail() {

	}


	rapidjson::Document TransactionHistoryDetail::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		document.AddMember("id", id, locator);

		rapidjson::Value vtime(time.c_str(), time.size());
		document.AddMember("time", vtime, locator);

		rapidjson::Value vservice(service.c_str(), service.size());
		document.AddMember("service", vservice, locator);

		document.AddMember("value", value, locator);
		document.AddMember("money", money, locator);

		rapidjson::Value vdesc(desc.c_str(), desc.size());
		document.AddMember("desc", vdesc, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TransactionHistoryDetail::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TransactionHistoryDetail::toData(rapidjson::Document & document)
	{
		if (document.HasMember("id"))
		{
			setId(document["id"].GetInt64());
		}
		if (document.HasMember("time"))
		{
			setTime(document["time"].GetString());
		}
		if (document.HasMember("service"))
		{
			setService(document["service"].GetString());
		}
		if (document.HasMember("value"))
		{
			setValue(document["value"].GetInt());
		}
		if (document.HasMember("money"))
		{
			setMoney(document["money"].GetUint64());
		}
		if (document.HasMember("desc"))
		{
			setDesc(document["desc"].GetString());
		}
	}
}

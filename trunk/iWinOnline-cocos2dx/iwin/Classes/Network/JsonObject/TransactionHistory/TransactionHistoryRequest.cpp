#include "TransactionHistoryRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	TransactionHistoryRequest::TransactionHistoryRequest() {
		type = 0;
		page = 0;
	}


	TransactionHistoryRequest::~TransactionHistoryRequest() {

	}


	rapidjson::Document TransactionHistoryRequest::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		document.AddMember("type", type, locator);
		document.AddMember("page", page, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TransactionHistoryRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TransactionHistoryRequest::toData(rapidjson::Document & document)
	{
		if (document.HasMember("type"))
		{
			setType(document["type"].GetInt());
		}
		if (document.HasMember("page"))
		{
			setPage(document["page"].GetInt());
		}
	}
}

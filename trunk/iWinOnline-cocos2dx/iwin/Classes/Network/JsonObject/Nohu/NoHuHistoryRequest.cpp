#include "NoHuHistoryRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuHistoryRequest::NoHuHistoryRequest()
	{
		moneyType = 1;
		page = 0;
	}


	NoHuHistoryRequest::~NoHuHistoryRequest()
	{

	}

	rapidjson::Document NoHuHistoryRequest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		document.AddMember("page", page,locator);
		document.AddMember("moneyType", moneyType, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuHistoryRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuHistoryRequest::toData(rapidjson::Document & document)
	{
		if(document.HasMember("page"))
		{
			setPage(document["page"].GetInt());
		}
		if (document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}
	}
}

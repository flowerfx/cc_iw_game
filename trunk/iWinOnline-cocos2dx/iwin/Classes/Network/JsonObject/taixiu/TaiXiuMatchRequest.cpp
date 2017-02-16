#include "TaiXiuMatchRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuMatchRequest::TaiXiuMatchRequest()
	{
		matchId = 0;
	}


	TaiXiuMatchRequest::~TaiXiuMatchRequest()
	{

	}

	rapidjson::Document TaiXiuMatchRequest::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		document.AddMember("matchId", matchId, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuMatchRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuMatchRequest::toData(rapidjson::Document & document)
	{
		if (document.HasMember("matchId"))
		{
			setMatchId(document["matchId"].GetInt());
		}
	}
}
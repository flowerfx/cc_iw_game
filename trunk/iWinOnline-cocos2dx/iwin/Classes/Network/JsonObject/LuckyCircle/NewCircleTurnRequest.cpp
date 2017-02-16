#include "NewCircleTurnRequest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NewCircleTurnRequest:: NewCircleTurnRequest ()
	{
		token = "";
		captchaChar = "";
		circleId = 0;
	}

	NewCircleTurnRequest::~NewCircleTurnRequest()
	{

	}

	rapidjson::Document NewCircleTurnRequest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("circleId", circleId,locator);
				rapidjson::Value vcaptchaChar(captchaChar.c_str(), captchaChar.size());
				document.AddMember("captchaChar", vcaptchaChar, locator);
				rapidjson::Value vtoken(token.c_str(), token.size());
				document.AddMember("token", vtoken, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NewCircleTurnRequest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NewCircleTurnRequest::toData(rapidjson::Document & document)
	{
		if(document.HasMember("circleId"))
		{
			setCircleId(document["circleId"].GetInt());
		}
		if (document.HasMember("captchaChar"))
		{
			setCaptchaChar(document["captchaChar"].GetString());
		}
		if (document.HasMember("token"))
		{
			setToken(document["token"].GetString());
		}

	}
}

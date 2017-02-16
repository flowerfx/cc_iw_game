#include "AuthorizeOTP.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	AuthorizeOTP::AuthorizeOTP()
	{
		code = "";
		type = 0;
		purpose = 0;
		trustDevice = 0;
	}

	AuthorizeOTP::~AuthorizeOTP()
	{
		code = "";
		type = 0;
		purpose = 0;
		trustDevice = 0;
	}

	rapidjson::Document AuthorizeOTP::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vcode(code.c_str(), code.size());
		document.AddMember("code", vcode,locator);

		document.AddMember("type", type,locator);
		document.AddMember("purpose", purpose,locator);
		document.AddMember("trustDevice", trustDevice,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void AuthorizeOTP::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void AuthorizeOTP::toData(rapidjson::Document & document)
	{
		if(document.HasMember("code"))
		{
			setCode(document["code"].GetString());
		}
		if(document.HasMember("type"))
		{
			setType(document["type"].GetInt());
		}
		if(document.HasMember("purpose"))
		{
			setPurpose(document["purpose"].GetInt());
		}
		if(document.HasMember("trustDevice"))
		{
			setTrustDevice(document["trustDevice"].GetInt());
		}


	}
}

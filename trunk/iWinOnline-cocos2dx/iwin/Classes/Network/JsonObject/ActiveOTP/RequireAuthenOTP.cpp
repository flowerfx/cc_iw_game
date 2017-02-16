#include "RequireAuthenOTP.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RequireAuthenOTP::RequireAuthenOTP()
	{
		typeOTP = 0;
		data = secretKey = desc = "";
		purpose = 0;
		expireTime = 0;
	}

	RequireAuthenOTP::~RequireAuthenOTP()
	{
		typeOTP = 0;
		data = secretKey = desc = "";
		purpose = 0;
		expireTime = 0;
	}

	rapidjson::Document RequireAuthenOTP::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		document.AddMember("typeOTP", typeOTP,locator);

		rapidjson::Value vdata(data.c_str(), data.size());
		document.AddMember("data", vdata,locator);

		rapidjson::Value vsecretKey(secretKey.c_str(), secretKey.size());
		document.AddMember("secretKey", vsecretKey,locator);

		rapidjson::Value vdesc(desc.c_str(), desc.size());
		document.AddMember("desc", vdesc, locator);

		document.AddMember("purpose", purpose, locator);
		document.AddMember("expireTime", expireTime, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RequireAuthenOTP::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RequireAuthenOTP::toData(rapidjson::Document & document)
	{
		if(document.HasMember("typeOTP"))
		{
			setTypeOTP(document["typeOTP"].GetInt());
		}
		if(document.HasMember("data"))
		{
			setData(document["data"].GetString());
		}
		if(document.HasMember("secretKey"))
		{
			setSecretKey(document["secretKey"].GetString());
		}
		if (document.HasMember("desc"))
		{
			setDesc(document["desc"].GetString());
		}
		if (document.HasMember("purpose"))
		{
			setPurpose(document["purpose"].GetInt());
		}
		if (document.HasMember("expireTime"))
		{
			setExpireTime(document["expireTime"].GetInt64());
		}
	}
}

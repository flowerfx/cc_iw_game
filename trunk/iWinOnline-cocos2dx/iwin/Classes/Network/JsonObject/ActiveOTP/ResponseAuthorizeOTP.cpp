#include "ResponseAuthorizeOTP.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ResponseAuthorizeOTP::ResponseAuthorizeOTP()
	{
		result = false;
		desc = "";
		typeOTP = 1;
	}

	ResponseAuthorizeOTP::~ResponseAuthorizeOTP()
	{

	}

	rapidjson::Document ResponseAuthorizeOTP::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		document.AddMember("result", result,locator);

		rapidjson::Value vdesc(desc.c_str(), desc.size());
		document.AddMember("desc", vdesc,locator);

		document.AddMember("typeOTP", typeOTP,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ResponseAuthorizeOTP::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ResponseAuthorizeOTP::toData(rapidjson::Document & document)
	{
		if(document.HasMember("result"))
		{
			setResult(document["result"].GetBool());
		}
		if(document.HasMember("desc"))
		{
			setDesc(document["desc"].GetString());
		}
		if(document.HasMember("typeOTP"))
		{
			setTypeOTP(document["typeOTP"].GetInt());
		}

	}
}

#include "ResponseGoogle.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	ResponseGoogle::ResponseGoogle()
	{

	}

	ResponseGoogle::~ResponseGoogle()
	{

	}

	rapidjson::Document ResponseGoogle::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vaccess_token(access_token.c_str(), access_token.size());
		document.AddMember("access_token", vaccess_token,locator);

		rapidjson::Value vtoken_type(token_type.c_str(), token_type.size());
		document.AddMember("token_type", vtoken_type, locator);

		document.AddMember("expires_in", expires_in, locator);

		rapidjson::Value vid_token(id_token.c_str(), id_token.size());
		document.AddMember("id_token", vid_token, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void ResponseGoogle::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ResponseGoogle::toData(rapidjson::Document & document)
	{
		if(document.HasMember("access_token"))
		{
			setAccess_token(document["access_token"].GetString());
		}
		if(document.HasMember("token_type"))
		{
			setToken_type(document["token_type"].GetString());
		}
		if(document.HasMember("expires_in"))
		{
			setExpires_in(document["expires_in"].GetInt());
		}
		if(document.HasMember("id_token"))
		{
			setId_token(document["id_token"].GetString());
		}


	}
}

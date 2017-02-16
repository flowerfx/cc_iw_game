#include "CardTelco.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	CardTelco::CardTelco()
	{

	}

	CardTelco::~CardTelco()
	{

	}

	rapidjson::Document CardTelco::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				document.AddMember("cardId", cardId,locator);
		document.AddMember("vnd", vnd,locator);
		document.AddMember("ruby", ruby,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void CardTelco::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void CardTelco::toData(rapidjson::Document & document)
	{
				if(document.HasMember("cardId"))
		{
			setCardId(document["cardId"].GetInt());
		}
		if(document.HasMember("vnd"))
		{
			setVnd(document["vnd"].GetInt());
		}
		if(document.HasMember("ruby"))
		{
			setRuby(document["ruby"].GetInt());
		}


	}
}

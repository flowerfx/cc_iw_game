#include "|classname|.h"
#include "json/writer.h"
namespace iwinmesage
{
	|static_field|
	|classname|:: |classname| ()
	{

	}

	|classname|::~|classname|()
	{

	}

	rapidjson::Document |classname|::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		|to_json_content|
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void |classname|::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void |classname|::toData(rapidjson::Document & document)
	{
		|to_data_content|

	}
}
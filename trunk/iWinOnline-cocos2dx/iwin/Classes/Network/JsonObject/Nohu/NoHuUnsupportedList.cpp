#include "NoHuUnsupportedList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuUnsupportedList::NoHuUnsupportedList()
	{
		isRubyEnabled = isWinEnabled = false;
		gameList.clear();
		screenList.clear();
	}

	NoHuUnsupportedList::~NoHuUnsupportedList()
	{
		gameList.clear();
		screenList.clear();
	}

	rapidjson::Document NoHuUnsupportedList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		rapidjson::Value arrgame(rapidjson::kArrayType);
		for (size_t i = 0; i < gameList.size(); i++)
		{
			arrgame.PushBack(rapidjson::Value().SetInt(gameList[i]), locator);
		}
		document.AddMember("gameList", arrgame, locator);

		rapidjson::Value arrscreen(rapidjson::kArrayType);
		for (size_t i = 0; i < screenList.size(); i++)
		{
			arrscreen.PushBack(rapidjson::Value().SetInt(screenList[i]), locator);
		}
		document.AddMember("screenList", arrscreen, locator);

		document.AddMember("isRubyEnabled", isRubyEnabled,locator);
		document.AddMember("isWinEnabled", isWinEnabled, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuUnsupportedList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuUnsupportedList::toData(rapidjson::Document & document)
	{
		if(document.HasMember("isRubyEnabled"))
		{
			setIsRubyEnabled(document["isRubyEnabled"].GetBool());
		}
		if(document.HasMember("isWinEnabled"))
		{
			setIsWinEnabled(document["isWinEnabled"].GetBool());
		}
		if(document.HasMember("gameList") && !document["gameList"].IsNull() && document["gameList"].IsArray() )
		{
			const rapidjson::Value& arrlist = document["gameList"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
				gameList.push_back(arrlist[i].GetInt());
			}
		}
		if(document.HasMember("screenList") && !document["screenList"].IsNull() && document["screenList"].IsArray() )
		{
			const rapidjson::Value& arrlist = document["screenList"];
			for (rapidjson::SizeType i = 0; i < arrlist.Size(); i++)
			{
				screenList.push_back(arrlist[i].GetInt());
			}
		}


	}
}

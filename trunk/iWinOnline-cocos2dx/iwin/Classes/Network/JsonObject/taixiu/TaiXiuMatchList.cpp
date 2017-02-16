#include "TaiXiuMatchList.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuMatchList::TaiXiuMatchList()
	{
		matchList.clear();
	}


	TaiXiuMatchList::~TaiXiuMatchList()
	{
		matchList.clear();
	}

	rapidjson::Document TaiXiuMatchList::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		rapidjson::Value arrlist(rapidjson::kArrayType);
		for (size_t i = 0; i < matchList.size(); i++)
		{
			rapidjson::Value opt(matchList.at(i).toJson(), locator);
			arrlist.PushBack(opt, locator);
		}
		document.AddMember("matchList", arrlist, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuMatchList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuMatchList::toData(rapidjson::Document & document)
	{
		if (document.HasMember("matchList") && !document["matchList"].IsNull() && document["matchList"].IsArray())
		{
			const rapidjson::Value& arrList = document["matchList"];
			for (rapidjson::SizeType i = 0; i < arrList.Size(); i++)
			{
				const rapidjson::Value& c = arrList[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				const char* json = buffer.GetString();
				TaiXiuMatch d;
				d.toData(json);
				matchList.push_back(d);

			}
		}
	}
}
#include "RequestQuest.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	RequestQuest:: RequestQuest ()
	{
		quests.clear();
	}

	RequestQuest::~RequestQuest()
	{
		for (auto q : quests)
		{
			delete q;
		}
		quests.clear();
	}
	void RequestQuest::pushQuest(Quest* q)
	{
		quests.push_back(q);
	}
	rapidjson::Document RequestQuest::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrquests(rapidjson::kArrayType);
		for (size_t i = 0; i < quests.size(); i++)
		{
			rapidjson::Value vquest(quests.at(i)->toJson(), locator);
			arrquests.PushBack(vquest, locator);
		}
		document.AddMember("quests",arrquests, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void RequestQuest::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void RequestQuest::toData(rapidjson::Document & document)
	{
				if(document.HasMember("quests") && !document["quests"].IsNull() && document["quests"].IsArray() )
		{
			const rapidjson::Value& arrquests = document["quests"];
			for (rapidjson::SizeType i = 0; i < arrquests.Size(); i++)
			{
							const rapidjson::Value& c = arrquests[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			Quest * d = new Quest();
			d->toData(json);
			quests.push_back(d);

			}
		}


	}
}

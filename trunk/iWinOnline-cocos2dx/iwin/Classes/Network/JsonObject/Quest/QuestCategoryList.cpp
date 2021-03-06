#include "QuestCategoryList.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	QuestCategoryList:: QuestCategoryList ()
	{
		currentCategory = -1;
		categoryList.clear();
	}

	QuestCategoryList::~QuestCategoryList()
	{
		currentCategory = -1;
		for (auto q : categoryList)
		{
			delete q;
		}
		categoryList.clear();
	}

	void QuestCategoryList::pushQuestCategory(QuestCategory* q)
	{
		categoryList.push_back(q);
	}

	rapidjson::Document QuestCategoryList::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
				rapidjson::Value arrcategoryList(rapidjson::kArrayType);
		for (size_t i = 0; i < categoryList.size(); i++)
		{
			rapidjson::Value opt(categoryList.at(i)->toJson(), locator);
			arrcategoryList.PushBack(opt, locator);
		}
		document.AddMember("categoryList",arrcategoryList, locator);
		document.AddMember("currentCategory", currentCategory,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void QuestCategoryList::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		if (document.GetParseError() != 0)
		{
//			CC_ASSERT(false);
			return;
		}
		toData(document);
	}
	void QuestCategoryList::toData(rapidjson::Document & document)
	{
		if(document.HasMember("currentCategory"))
		{
			setCurrentCategory(document["currentCategory"].GetInt());
		}
		if(document.HasMember("categoryList") && !document["categoryList"].IsNull() && document["categoryList"].IsArray() )
		{
			const rapidjson::Value& arrcategoryList = document["categoryList"];
			for (rapidjson::SizeType i = 0; i < arrcategoryList.Size(); i++)
			{
							const rapidjson::Value& c = arrcategoryList[i];	
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			QuestCategory* d = new QuestCategory();
			d->toData(json);
			categoryList.push_back(d);

			}
		}


	}
}

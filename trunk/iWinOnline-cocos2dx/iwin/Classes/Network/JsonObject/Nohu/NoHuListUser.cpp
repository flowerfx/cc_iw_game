#include "NoHuListUser.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	NoHuListUser::NoHuListUser()
	{
		page = totalPage = 0;
		moneyType = 1;
		users.clear();
	}

	NoHuListUser::~NoHuListUser()
	{
		users.clear();
	}

	rapidjson::Document NoHuListUser::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		
		document.AddMember("page", page, locator);
		document.AddMember("totalPage", totalPage, locator);
		document.AddMember("moneyType", moneyType,locator);

		rapidjson::Value arrList(rapidjson::kArrayType);
		for (size_t i = 0; i < users.size(); i++)
		{
			rapidjson::Value opt(users.at(i).toJson(), locator);
			arrList.PushBack(opt, locator);
		}
		document.AddMember("users", arrList, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void NoHuListUser::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void NoHuListUser::toData(rapidjson::Document & document)
	{

		if(document.HasMember("page"))
		{
			setPage(document["page"].GetInt());
		}
		if (document.HasMember("totalPage"))
		{
			setTotalPage(document["totalPage"].GetInt());
		}
		if (document.HasMember("moneyType"))
		{
			setMoneyType(document["moneyType"].GetInt());
		}

		if (document.HasMember("users") && !document["users"].IsNull() && document["users"].IsArray())
		{
			const rapidjson::Value& arrList = document["users"];
			for (rapidjson::SizeType i = 0; i < arrList.Size(); i++)
			{
				const rapidjson::Value& c = arrList[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				NoHuUser d;
				d.toData(buffer.GetString());
				users.push_back(d);
			}
		}


	}
}

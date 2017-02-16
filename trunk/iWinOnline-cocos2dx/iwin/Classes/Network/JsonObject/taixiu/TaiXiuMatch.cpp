#include "TaiXiuMatch.h"
#include "json/writer.h"
namespace iwinmesage
{
	TaiXiuMatch::TaiXiuMatch()
	{
		id = -1;
		time = 0;
		dice1 = dice2 = dice3 = 0;
		result = 0;
		userBets.clear();
	}


	TaiXiuMatch::~TaiXiuMatch()
	{
		userBets.clear();
	}

	void TaiXiuMatch::ReserveBetList()
	{
		if (userBets.size() > 1)
		{
			std::reverse(userBets.begin(), userBets.end());
		}
	}

	rapidjson::Document TaiXiuMatch::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

		document.AddMember("id", id, locator);
		document.AddMember("time", time, locator);
		document.AddMember("dice1", dice1, locator);
		document.AddMember("dice2", dice2, locator);
		document.AddMember("dice3", dice3, locator);
		document.AddMember("result", result, locator);

		rapidjson::Value arrlist(rapidjson::kArrayType);
		for (size_t i = 0; i < userBets.size(); i++)
		{
			rapidjson::Value opt(userBets.at(i).toJson(), locator);
			arrlist.PushBack(opt, locator);
		}
		document.AddMember("userBets", arrlist, locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void TaiXiuMatch::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void TaiXiuMatch::toData(rapidjson::Document & document)
	{
		if (document.HasMember("id"))
		{
			setId(document["id"].GetInt());
		}
		if (document.HasMember("time"))
		{
			setTime(document["time"].GetInt64());
		}
		if (document.HasMember("dice1"))
		{
			setDice1(document["dice1"].GetInt());
		}
		if (document.HasMember("dice2"))
		{
			setDice2(document["dice2"].GetInt());
		}
		if (document.HasMember("dice3"))
		{
			setDice3(document["dice3"].GetInt());
		}
		if (document.HasMember("result"))
		{
			setResult(document["result"].GetInt());
		}
		if (document.HasMember("userBets") && !document["userBets"].IsNull() && document["userBets"].IsArray())
		{
			const rapidjson::Value& arrList = document["userBets"];
			for (rapidjson::SizeType i = 0; i < arrList.Size(); i++)
			{
				const rapidjson::Value& c = arrList[i];
				rapidjson::StringBuffer buffer;
				buffer.Clear();
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				c.Accept(writer);
				const char* json = buffer.GetString();
				TaiXiuUserBet d;
				d.toData(json);
				userBets.push_back(d);

			}
		}
	}
}
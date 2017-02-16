
#include "CoPhoMgr.h"
#include "json/writer.h"

CoPhoMgr* CoPhoMgr::s_instance = nullptr;

CoPhoMgr* CoPhoMgr::getInstance()
{
	if (s_instance) return s_instance;
	s_instance = new CoPhoMgr();
	return s_instance;
 }

CoPhoMgr::CoPhoMgr()
{
}

CoPhoMgr::~CoPhoMgr()
{
}

void CoPhoMgr::add(CoPhoBoard copho)
{
	_list_copho.push_back(copho);
}
	
void CoPhoMgr::load(s64 userId)
{
	if (_userId == userId && _game_type == GameController::getCurrentGameType() 
		&& _money_type == GameController::getMoneyType()) return;
	std::string game_type = (GameController::getCurrentGameType() == GAMEID_CHESS) ? "co" : "up";
	std::string money_type = GameController::getMoneyType();
	std::string save_name = StringUtils::format("%lld_%s_%s", userId, game_type.c_str(), money_type.c_str());
	_userId = userId;
	_list_copho.clear();
	if (FileUtils::getInstance()->isFileExist(save_name))
	{
		cocos2d::Data data = FileUtils::getInstance()->getDataFromFile(save_name);
		rapidjson::Document doc;
		char* buffer = new char[data.getSize() + 1];
		memcpy(buffer, data.getBytes(), data.getSize());
		buffer[data.getSize()] = '\0';

		doc.Parse(buffer);
		const rapidjson::Value& list_copho = doc["list_copho"];
		_list_copho.clear();
		for (rapidjson::SizeType i = 0; i < list_copho.Size(); i++)
		{
			const rapidjson::Value& c = list_copho[i];
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			c.Accept(writer);
			const char* json = buffer.GetString();
			rapidjson::Document doc_temp;
			doc_temp.Parse(json);
			CoPhoBoard board;
			board.toData(doc_temp);
			_list_copho.push_back(board);
		}
		if (_list_copho.size() > 1)
		{
			std::reverse(_list_copho.begin(), _list_copho.end());
		}
		delete[] buffer;
	}
	else
	{
		
	}
}

void CoPhoMgr::save()
{
	std::string game_type = (GameController::getCurrentGameType() == GAMEID_CHESS) ? "co" : "up";
	std::string money_type = GameController::getMoneyType();
	std::string save_name = StringUtils::format("%lld_%s_%s", _userId, game_type.c_str(), money_type.c_str());

	//cocos2d::Data data = FileUtils::getInstance()->getDataFromFile(save_name);
	//FileUtils::getInstance()->writeStringToFile()
	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& locator = doc.GetAllocator();
	rapidjson::Value arr(rapidjson::kArrayType);
	for(int i = 0; i < _list_copho.size(); i++)
	{
		arr.PushBack(rapidjson::Value(_list_copho.at(i).toJson(), locator), locator);
	}
	
	doc.AddMember("list_copho", arr, locator);
	rapidjson::StringBuffer s;
	s.Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	doc.Accept(writer);
	FileUtils::getInstance()->writeStringToFile(s.GetString(), save_name);
	//CCLOG("%s", s.GetString());

}

int CoPhoMgr::getNumCoPho()
{
	return _list_copho.size();
}

CoPhoBoard CoPhoMgr::getCoPhoIdx(int idx)
{
	return _list_copho.at(idx);
}
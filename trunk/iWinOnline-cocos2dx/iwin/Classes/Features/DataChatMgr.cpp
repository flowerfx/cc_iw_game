#include "DataChatMgr.h"
#include "time.h"

DataChatMgr* DataChatMgr::m_instance = nullptr;

DataChatMgr* DataChatMgr::getInstance()
{
	if (!m_instance)
	{
		m_instance = new DataChatMgr();
	}
	return m_instance;
}

DataChatMgr::DataChatMgr()
{
	_IDDB_handler = 0;
	_IDDB_friend = 0;
	_is_file_open = false;
	_file_save = "";
}

DataChatMgr::~DataChatMgr()
{
	OnCloseDBChat();
}

FILE * DataChatMgr::OpenChatLogWithID(int iddb, bool read_from_first)
{
	if (_IDDB_handler == 0)
		return nullptr;

	_file_save = std::to_string(_IDDB_handler) + "_"+ std::to_string(iddb) + ".dat";

	bool file_exist = false;
	//first check if file not exit
	FILE * fp = Utility::File::GetLocalFile(_file_save, "r");

	if (fp)
	{
		file_exist = true;
		fclose(fp);
	}

	//if file exist
	if (file_exist)
	{
		if (read_from_first)
		{
			//, read and write from the begin
			fp = Utility::File::GetLocalFile(_file_save, "r+b");
		}
		else
		{
			//, read and write to the end
			fp = Utility::File::GetLocalFile(_file_save, "a+b");
		}
	}
	else
	{
		//other wise : create the file and open it
		fp = Utility::File::GetLocalFile(_file_save, "w+b");
	}

	PASSERT2(fp != nullptr, "the file handler is null");

	return fp;
}

void DataChatMgr::OnOpenDBChat(int IDDB)
{
	if (_IDDB_handler == IDDB)
	{
		PASSERT2(false, "already have file handler");
		return;
	}
	_IDDB_handler = IDDB;
}

void DataChatMgr::OnCloseDBChat()
{
	_IDDB_handler = 0;
	_file_save = "";
}

void DataChatMgr::SaveToDBChat(int fromID, RKString chat_msg, long long time)
{
	auto fp = OpenChatLogWithID(_IDDB_friend);

	if (!fp)
	{
		return;
	}

	if (time < 0)
	{
		time = GetCurTime();
	}

	std::string time_str = std::to_string(time);
	std::string id_str = std::to_string(fromID);
	std::string buffer_to = "][" + id_str + "][" + time_str + "][" + std::string(chat_msg.GetString()) + "][\n";
	int res = fputs(buffer_to.c_str(), fp);

	fclose(fp);
}

void DataChatMgr::LoadDBChatWithID(int fromID, const std::function<void(std::map<long long, ChatDetail> list_chat)> & call_back)
{
	auto fp = OpenChatLogWithID(fromID);

	auto str = Utility::File::GetDataFromHandleFile(fp);
	if (str)
	{
		std::map<long long, ChatDetail> t_list_chat;
		auto number_chat_list = RKString(str).Split("\n");
		for (int i = 0; i < number_chat_list.Size(); i++)
		{
			auto detail = number_chat_list[i].Split("][");
			int id_chat = atoi(detail[0].GetString());
			long long time_chat = atoll(detail[1].GetString());
			RKString msg_chat = detail[2];
			t_list_chat.insert({ time_chat, ChatDetail(id_chat, time_chat, msg_chat.GetString()) });
		}
		if (call_back)
		{
			call_back(t_list_chat);
		}
	}
}

long long DataChatMgr::GetCurTime()
{
	time_t t = time(nullptr);
	return (long long)t;
}

void DataChatMgr::SetChatWithID(int id)
{
	_IDDB_friend = id;
}
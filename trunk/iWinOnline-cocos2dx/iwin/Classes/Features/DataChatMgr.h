#ifndef __DATA_CHAT_MGR_H__
#define __DATA_CHAT_MGR_H__

#include "FileManager.h"
#include "DisplayView.h"
class ChatMessage;
struct ChatDetail
{
	int id;
	RKString msg;
	long long time;

	ChatDetail(int i, long long t , std::string m)
	{
		id = i;
		msg = m;
		time = t;
	}
};
class DataChatMgr
{
private:
	bool   _is_file_open;
	int	   _IDDB_handler;
	int	   _IDDB_friend;
	RKString _file_save;
	static DataChatMgr* m_instance;
protected:
	long long GetCurTime();

	FILE * OpenChatLogWithID(int iddb , bool read_from_first = false);
public:
	static DataChatMgr* getInstance();

	DataChatMgr();
	~DataChatMgr();

	void OnOpenDBChat(int IDDB);
	void OnCloseDBChat();

	void SaveToDBChat(int fromID, RKString chat_msg, long long time = -1);
	void LoadDBChatWithID(int fromID , const std::function<void(std::map<long long , ChatDetail> list_chat)> & call_back);

	void SetChatWithID(int id);
};

#define SaveChatMgr DataChatMgr::getInstance()

#endif // __DATA_CHAT_MGR_H__
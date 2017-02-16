#ifndef __COPHO_MGR_H__
#define __COPHO_MGR_H__

#include "Common/Common.h"
#include "Screens/GameScreens/CoTuong/CoTuongScreen.h"

class CoPhoMgr
{
private:
	std::vector<CoPhoBoard>		_list_copho;
	s64							_userId;
	int							_game_type;
	std::string					_money_type;
	static CoPhoMgr*			s_instance;
public:
	static CoPhoMgr*			getInstance();
	CoPhoMgr();
	virtual ~CoPhoMgr();

	void load(s64 userId);
	void save();
	void add(CoPhoBoard copho);
	int getNumCoPho();
	CoPhoBoard getCoPhoIdx(int idx);
};

#define GetCoPhoMgr() CoPhoMgr::getInstance()

#endif 


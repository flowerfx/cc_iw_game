#ifndef __IWIN_PLAYER_H__
#define __IWIN_PLAYER_H__
#include<string>
#include<vector>
class Player;
class iWinPlayer
{
public:
	 int IDDB ;
	 std::string name;
	 int avatar;
	 long time;
	 int  state_ready;
	 std::string lastMEssage ;
public:
	std::string toString();
	iWinPlayer();
	iWinPlayer(
		int IDDB,
		int avatar, 
		std::string name,
		std::string message, 
		int time 
		);
	iWinPlayer(iWinPlayer * chat_msg);
	iWinPlayer(Player * chat_msg);
	virtual ~iWinPlayer();
	
};


#endif //__IWIN_PLAYER_H__
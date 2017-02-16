#include "iWinPlayer.h"
#include "Models/Player.h"
iWinPlayer::iWinPlayer()
{
	IDDB = -1;
	name = "";
	avatar = 0;
	time = 0;
	lastMEssage = "";
	state_ready = 0;
}

iWinPlayer::iWinPlayer(
	int _IDDB,
	int _avatar,
	std::string _name,
	std::string _message,
	int _time) :
		IDDB(_IDDB),
		avatar(_avatar),
		name(_name),
		lastMEssage(_message),
		time(_time)
{
	state_ready = 0;
}

iWinPlayer::iWinPlayer(iWinPlayer * player)
{
	IDDB = player->IDDB;
	name = player->name;
	avatar = player->avatar;
	time = player->time;
	lastMEssage = player->lastMEssage;
	state_ready = player->state_ready;
}

iWinPlayer::iWinPlayer(Player * chat_msg)
{
	IDDB = chat_msg->IDDB;
	name = chat_msg->name;
	avatar = chat_msg->avatar;
	time = 0;
	lastMEssage = "";
	state_ready = chat_msg->isReady ? 1 : 0;
}

iWinPlayer::~iWinPlayer()
{

}

std::string iWinPlayer::toString() {
	return (
		"IDDB=" + std::to_string(IDDB) +
		" ,name=" + name + 
		" ,avatar=" + std::to_string(avatar) +
		" ,time=" + std::to_string(time) +
		" ,lastMEssage=" + lastMEssage
		);
}

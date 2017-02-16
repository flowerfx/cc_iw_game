#ifndef __UTILS_H__
#define __UTILS_H__

#include "cocos2d.h"
#include <string>
#include <vector>
#include "Common/Common.h"
USING_NS_CC;

namespace Utils
{
	extern std::vector<s64> s_popular_bet;
	extern int				s_padding;

	char _toLowerCase(char c);

	std::string toUpperCase(std::string str);

	extern Vec2 s_bounding_offset;

	std::string formatMoney_dot_ruby(s64 ruby);
	std::string formatMoney_dot_win(s64 money);

	std::string formatNumber_dot(s64 money);

	std::string formatNumber_dot_short(s64 money);

	std::string toLowerCase(std::string str);
	std::string trim(std::string str);

	s64	getMaxValidBet(s64 value);
	int	getMaxValidBetIdx(s64 value);
	int getNumberValidBetIdx();
	const std::vector<s64> & GetBetLobby();


	std::string formatNumber(s64 n);
	std::string formatNumber_win(s64 n);
	std::string formatNumber_ruby(s64 n);

	s64 GetNumberFromFormat(std::string format);

	const wchar_t * GetWChar(const char *c);
	std::string getAvataUrlPlayer(int idUser, std::string userName);
	void SetImageAvatarLink(void * data);

	int countStringLine(std::string str);

	std::string chatConvert(std::string chat);
	std::string getEmotionStr(int idx);
	void		startWaitDialog(std::string content = "");
	void		hideWaitDialog();
	void		showDialogNotice(std::string content);
	void		showToast(std::string content);

	//
	std::vector<s64> GetBetPopularNew();
	s64 GetBetValueNew(int idx);
	int GetRangeBetWithMoney(s64 &min_bet, s64 &max_bet, s64 &ave_bet, s64 curent_money , s64 min);

	s64 getMaxValidBetNew(s64 value);
	int GetIndexOfValue(s64 value);
	int GetIndexOfValue(std::vector<s64> arrBet,s64 value);

	std::string ConvertTime(s64 time);
	void replaceAllString(std::string& str, const std::string& from, const std::string& to);
	std::vector<std::string> split(std::string data, std::string token);
	
	void configPadding();

	Vec2 getPositionAtAnchorPoint(Vec2 anchor_point, cocos2d::Node * node);

	const std::string getImgUrlCorrect(const std::string & url);

	//
	void enableVibrate(bool value);
	void vibrate(int milisec);

	void vibrateShort();
	void vibrateVeryShort();
	void vibrateMedium();
	void vibrateLong();

};

#endif // __UTILS_H__
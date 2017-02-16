
#include "cocos2d.h"
#include "Screens/BaseScreen.h"
#include "Screens/ScreenManager.h"
#include "MainGame.h"
#include "Platform/Platform.h"
#include "Utils.h"
#include "time.h"
#include <iostream>
#include <clocale>
#include <cctype>

#include "Network/JsonObject/Avatar/ImageLinkContainerObject.h"
namespace Utils
{

	Vec2 s_bounding_offset = Vec2(15, 15);
	int s_padding = 0;

	std::vector<s64> s_popular_bet;

#define NUM_EMOTION 60
	const char* k_emotions[] = { ":)", "o.O", ":'(",
		"3:-)", ":(", ":-O", "8-)", ":D", ">:(", "<3", "^_^", ":v", "(U)",
		"-_-", "8|", ":P", ":/", ";)", ":*", "(y)", ":~", ":B", ":|", ":+",
		":$", ":X", ":Z", ":((", ":-|", ":-H", "(w)", ":-F", ";T", ";P",
		";D", ";O", ";G", "|-)", ":!", ":L", ":>", ":;", ";F", ";-S", ":?",
		";-X", ";-!", "/--", "&-(", ":-L", ":-R", ">-|", "P-(", ":--|",
		"X-)", ";-A", "8*", "(shit)", "(n)", "(ok)" };

	const char* k_mapEmotions[] = { "Ā", "Ġ", "İ",
		"ŀ", "Ő", "Š", "ƀ", "Ȁ", "Ģ", "Ĳ", "ł", "Œ", "Ƣ", "ģ", "Ń", "œ",
		"ţ", "ų", "Ą", "Ĕ", "Ĥ", "Ĵ", "ń", "Ŕ", "ą", "ĕ", "ĥ", "ĵ", "Ņ",
		"ŕ", "Ć", "Ė", "Ħ", "Ķ", "ņ", "Ŗ", "ć", "ė", "ħ", "ķ", "Ň", "ŗ",
		"ȹ", "Ę", "Ɯ", "Ǿ", "ň", "Ř", "ċ", "ě", "ī", "Ļ", "ŋ", "ś", "Č",
		"Ĝ", "Ĭ", "ļ", "Ƀ", "Ŝ" };
#define NUMBER_BET 39
#define NUMBER_BET_RUBY 11
	s64 POPULAR_BET_NEW[NUMBER_BET] = { 0 , 1000, 2000, 3000, 5000, 7000, 10000, 15000, 20000, 30000, 50000,
		70000, 100000, 150000, 200000, 300000, 500000, 700000, 1000000, 1500000, 2000000, 3000000, 5000000, 7000000,
		10000000, 15000000, 20000000, 30000000, 50000000, 70000000, 100000000, 150000000, 200000000, 300000000,
		500000000, 700000000, 1000000000, 1500000000, 2000000000 };

	s64 POPULAR_BET_NEW_RUBY[NUMBER_BET_RUBY] = { 1000, 2000, 3000, 5000, 10000, 20000, 50000,
		100000, 200000, 500000, 1000000 };

	std::vector<s64> s_popular_bet_new;
	std::vector<s64> s_popular_bet_new_ruby;
	const std::vector<s64> & GetBetLobby()
	{
		if (s_popular_bet.size() > 0)
		{
			return s_popular_bet;
		}
		else
		{
			if (GameController::currentMoneyType == BASE_MONEY)
			{
				if (s_popular_bet_new.size() <= 0)
				{
					std::vector<s64> v(std::begin(POPULAR_BET_NEW), std::end(POPULAR_BET_NEW));
					s_popular_bet_new = v;
				}
				return s_popular_bet_new;
			}
			else
			{
				if (s_popular_bet_new_ruby.size() <= 0)
				{
					std::vector<s64> v(std::begin(POPULAR_BET_NEW_RUBY), std::end(POPULAR_BET_NEW_RUBY));
					s_popular_bet_new_ruby = v;
				}
				return s_popular_bet_new_ruby;
			}
		}
	}


	std::string LinkGetAvatarNewByUserName = "http://iwin.me/a/i/";
	std::string LinkGetAvatarNewByID = "http://iwin.me/a/items/";

	std::string formatMoney_dot_ruby(s64 ruby)
	{
		if (ruby < 0)
		{
			return "";
		}
		else
		{
			return formatNumber_dot(ruby) + " Ruby";
		}
	}

	std::string formatMoney_dot_win(s64 money)
	{
		if (money < 0)
		{
			return "";
		}
		else
		{
			return formatNumber_dot(money) + " Win";
		}
	}

	std::string formatNumber_dot(s64 money)
	{
		std::string numWithCommas = std::to_string(money);
		int insertPosition = numWithCommas.length() - 3;
		while (insertPosition > 0) {
			numWithCommas.insert(insertPosition, ".");
			insertPosition -= 3;
		}
		return numWithCommas;
	}

	std::string formatNumber_dot_short(s64 money)
	{
		if (money < 1000000)
		{
			return formatNumber_dot(money);
		}
		else
		{
			return formatNumber(money);
		}
	}

	char _toLowerCase(char c)
	{
		if (c >= (char)'A' && c <= (char)'Z')
		{
			c += (char)'a' - (char)'A';
		}
		return c;
	}

	std::string toLowerCase(std::string str)
	{
		for (size_t i = 0; i < str.size(); i++)
		{
			str[i] = Utils::_toLowerCase(str[i]);
		}
		return str;
	}


	std::string toUpperCase(std::string str)
	{
		std::string rs;
		std::setlocale(LC_ALL, "vi_VN.UTF-8");
		for (int i = 0; i < str.length(); i++)
		{
			rs += std::toupper(str[i]);
		}
		return rs;
	}


	std::string trim(std::string str)
	{
		std::string rs = "";
		int leftIdx;
		int rightIdx;
		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] != ' ')
			{
				leftIdx = i;
				break;
			}
		}
		for (int i = str.size() - 1; i >= 0; i--)
		{
			if (str[i] != ' ')
			{
				rightIdx = i;
				break;
			}
		}
		if (leftIdx - rightIdx >= 0)
		{
			rs = "";
		}
		else
		{
			for (auto i = leftIdx; i <= rightIdx; i++)
			{
				rs += str[i];
			}
		}
		return rs;
	}

	s64 getMaxValidBet(s64 value)
	{
		auto bet = GetBetLobby();
		for (int i = 1; i < bet.size(); i++)
		{
			if (value < bet[i])
			{
				if (i == 1)
				{
					return 0;
				}
				else
				{
					return bet[i - 1];
				}
			}
		}
		return bet.back();
	}

	int getNumberValidBetIdx()
	{
		return GetBetLobby().size() - 1;
	}

	int getMaxValidBetIdx(s64 value)
	{
		auto bet = GetBetLobby();
		for (int i = 1; i < bet.size(); i++)
		{
			if (value < bet[i])
			{
				if (i == 1)
				{
					return 0;
				}
				else
				{
					return i - 1;
				}
			}
		}
		return bet.size() - 1;
	}


	std::string formatNumber_win(s64 n)
	{
		if (n < 0)
		{
			return "";
		}
		else
		{
			//@PDT: TODO
			return formatNumber(n) + " Win";
		}
	}

	std::string formatNumber_ruby(s64 n)
	{
		if (n < 0)
		{
			return "";
		}
		else
		{
			//@PDT: TODO
			return formatNumber(n) + " Ruby";
		}
	}

	std::string formatNumber(s64 n)
	{
		if (n >= 1000 || n <= -1000)
		{
			long temp = n / 1000;
			std::string numberstring = "";
			int count = 0;
			long temp_k = 0;
			long temp_m = 0;
			while (temp > 999)
			{
				count++;
				if (count == 1)
				{
					temp_k = temp % 1000;
				}
				else if (count > 1)
				{
					temp_m = temp % 1000;
				}

				temp = temp / 1000;
			}
			if (count == 0)
			{
				std::string tmpK = "";
				long k = n % 1000;
				if (k != 0)
				{
					tmpK = "." + std::to_string(k);
				}
				numberstring = std::to_string(temp) + tmpK + "K";
			}
			else if (count == 1)
			{
				std::string str = "";
				if (temp_k >= 10)
				{
					if (temp_k < 100)
					{
						str = ".0" + std::to_string(temp_k);
					}
					else if (temp_k >= 100)
					{
						str = "." + std::to_string(temp_k);
					}
					str.erase(str.size() - 1, 1);
				}
				std::string tmpK = "";
				long k = n % 1000000;
				if (k != 0)
				{
					tmpK = "." + std::to_string(k);
				}
				numberstring = std::to_string(temp) + str + "M";
			}
			else
			{
				std::string str = "";
				if (temp_m >= 10)
				{
					if (temp_m < 100)
					{
						str = ".0" + std::to_string(temp_k);
					}
					else if (temp_m >= 100)
					{
						str = "." + std::to_string(temp_m);
					}
					str.erase(str.size() - 1, 1);
				}
				std::string tmpK = "";
				long k = n % 1000000000;
				if (k != 0)
				{
					tmpK = "." + std::to_string(k);
				}
				numberstring = std::to_string(temp) + str + "B";
			}
			return numberstring;
		}
		else
			return "" + std::to_string(n);
	}

	const wchar_t * GetWChar(const char *c)
	{
		const size_t cSize = strlen(c) + 1;
		std::wstring wc(cSize, L'#');
		mbstowcs(&wc[0], c, cSize);
		return wc.data();
	}

	std::string getImageType()
	{
		cocos2d::Size game_size = GetGameSize();

		if (game_size.width < 400) {
			return "small";
		}
		else if (game_size.width >= 400 && game_size.width < 680) {
			return "small";
		}
		else if (game_size.width >= 680 && game_size.width < 880) {
			return "normal";
		}
		else {
			return "large";
		}
		return "normal";
	}

	void SetImageAvatarLink(void * data)
	{
		ImageLinkContainerObject * obj = (ImageLinkContainerObject*)data;

		LinkGetAvatarNewByUserName = ((obj->getAvatar_new() != "") ? obj->getAvatar_new() : obj->getAvatar());
		LinkGetAvatarNewByID = obj->getItems();// ((obj->getItems_new() != "") ? obj->getItems_new() : obj->getItems());

		if (LinkGetAvatarNewByUserName.back() != '/')
		{
			LinkGetAvatarNewByUserName += "/";
		}
		if (LinkGetAvatarNewByID.back() != '/')
		{
			LinkGetAvatarNewByID += "/";
		}

		delete obj;
	}

	std::string getAvataUrlPlayer(int idAvata, std::string userName)
	{
		string template_url = "";
		if (idAvata >= 0)
		{
			template_url = LinkGetAvatarNewByID + std::to_string(idAvata);
		}
		else
		{
			template_url = LinkGetAvatarNewByUserName + userName;
		}

		template_url += "/avatar?width=";
		template_url += std::to_string(400);
		template_url += "&height=";
		template_url += std::to_string(400);
		template_url += "&type=";
		template_url += getImageType();
		template_url += "&redirect=true";

		return template_url;
	}

	int countStringLine(std::string str)
	{
		if (str == "")
			return 0;
		int number_char = str.size();
		int number_line = 1;
		int i = 0;
		do
		{
			if (str[i] == 10)
			{
				number_line++;
			}
			i++;
		} while (i < number_char);
		return number_line;
	}


	std::string chatConvert(std::string chat)
	{
		std::string rs;
		for (auto i = 0; i < NUM_EMOTION; i++)
		{
			do {
				int offset = chat.find(k_emotions[i]);
				if (offset != std::string::npos)
				{
					chat.replace(offset, strlen(k_emotions[i]), k_mapEmotions[i]);
				}
				else
				{
					break;
				}
			} while (true);

		}
		return chat;
	}

	std::string getEmotionStr(int idx)
	{
		return k_emotions[idx];
	}

	void startWaitDialog(std::string content)
	{
		if (content == "")
		{
			content = LangMgr->GetString("sending_message").GetString();
		}
		ScrMgr->OnShowDialog(Platform::getAppInfo()->getName(), content, TYPE_DIALOG::DIALOG_WAITING_BUTTON);
	}

	void hideWaitDialog()
	{
		ScrMgr->OnHideDialog();
	}

	void showDialogNotice(std::string content)
	{
		ScrMgr->OnShowOKDialog(content);
	}

	void showToast(std::string content)
	{
		static_cast<CommonScreen*>(ScrMgr->GetCommonScreen())->OnShowToast(content);
	}

	std::vector<s64> GetBetPopularNew()
	{
		std::vector<s64> range;
		for (int i = 0; i < NUMBER_BET; i++)
		{
			range.push_back(POPULAR_BET_NEW[i]);
		}
		return range;
	}

	s64 GetBetValueNew(int idx)
	{
		if (idx < 0)
			idx = 0;
		if (idx >= NUMBER_BET)
			idx = NUMBER_BET - 1;
		return POPULAR_BET_NEW[idx];
	}

	int GetRangeBetWithMoney(s64 &min_bet, s64 &max_bet, s64 &ave_bet, s64 curent_money, s64 min)
	{
		int idx_min = -1;
		for (size_t i = 0; i < NUMBER_BET - 1; i++)
		{
			if (min >= POPULAR_BET_NEW[i] && min < POPULAR_BET_NEW[i + 1])
			{
				idx_min = i;
				min_bet = POPULAR_BET_NEW[i];
				break;
			}
		}
		if (idx_min < 0)
		{
			if (min >= POPULAR_BET_NEW[NUMBER_BET - 1])
			{
				min_bet = POPULAR_BET_NEW[NUMBER_BET - 1];
			}
			else
			{
				min_bet = POPULAR_BET_NEW[0];
			}
		}

		int idx_max = -1;
		for (size_t i = 0; i < NUMBER_BET - 1; i++)
		{
			if (curent_money >= POPULAR_BET_NEW[i] && curent_money < POPULAR_BET_NEW[i + 1])
			{
				idx_max = i;
				max_bet = POPULAR_BET_NEW[i];
				break;
			}
		}
		if (idx_max < 0)
		{
			if (curent_money >= POPULAR_BET_NEW[NUMBER_BET - 1])
			{
				max_bet = POPULAR_BET_NEW[NUMBER_BET - 1];
				idx_max = NUMBER_BET - 1;
			}
			else
			{
				max_bet = POPULAR_BET_NEW[0];
				idx_max = 0;
			}
		}

		int delta = abs(idx_max - idx_min);
		int ave_delta = delta / 2;
		ave_bet = POPULAR_BET_NEW[ave_delta];

		return delta;
	}

	s64 getMaxValidBetNew(s64 value)
	{
		for (size_t i = 0; i < NUMBER_BET - 1; i++)
		{
			if (value >= POPULAR_BET_NEW[i] && value < POPULAR_BET_NEW[i + 1])
			{
				return POPULAR_BET_NEW[i];
			}
		}
		if (value < POPULAR_BET_NEW[0])
		{
			return 0;
		}
		else if (value >= POPULAR_BET_NEW[NUMBER_BET - 1])
		{
			return POPULAR_BET_NEW[NUMBER_BET - 1];
		}
		return 0;
	}

	s64 GetNumberFromFormat(std::string format)
	{
		RKString str = format;
		if (format == "")
			return 0;
		str.ReplaceAll(".", "");
		str.ReplaceAll(",", "");
		str.ReplaceAll("WIN", "");
		str.ReplaceAll("RUBY", "");
		u64 number = atoll(str.GetString());
		return number;
	}

	int GetIndexOfValue(s64 value)
	{
		for (size_t i = 0; i < NUMBER_BET - 1; i++)
		{
			if (value >= POPULAR_BET_NEW[i] && value < POPULAR_BET_NEW[i + 1])
			{
				return i;
			}
		}
		if (value < POPULAR_BET_NEW[0])
		{
			return 0;
		}
		else if (value >= POPULAR_BET_NEW[NUMBER_BET - 1])
		{
			return NUMBER_BET - 1;
		}
		return 0;
	}

	int GetIndexOfValue(std::vector<s64> arrBet, s64 value)
	{
		for (size_t i = 0; i < arrBet.size() - 1; i++)
		{
			if (value >= arrBet[i] && value < arrBet[i + 1])
			{
				return i;
			}
		}
		if (value < arrBet[0])
		{
			return 0;
		}
		else if (value >= arrBet[arrBet.size() - 1])
		{
			return arrBet.size() - 1;
		}
		return 0;
	}

	std::string ConvertTime(s64 time)
	{
		time /= 1000;
		time_t t = time;
		struct tm *  tstruct;
		tstruct = localtime(&t);
		char buf[80];
		strftime(buf, sizeof(buf), "%Y/%m/%d - %X", tstruct);
		return std::string(buf);
	}


	void replaceAllString(std::string& str, const std::string& from, const std::string& to)
	{
		if (from.empty())
			return;
		std::string::size_type start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}


	vector<string> split(string data, string token)
	{
		vector<string> output;
		size_t pos = string::npos; // size_t to avoid improbable overflow
		do
		{
			pos = data.find(token);
			output.push_back(data.substr(0, pos));
			if (string::npos != pos)
				data = data.substr(pos + token.size());
		} while (string::npos != pos);
		return output;
	}

	void configPadding()
	{
		Vec2 game_size = GetGameSize();
		s_padding = game_size.y < 320 ? 6 : (game_size.y < 480 ? 8 : (game_size.y >= 540 ? 16 : 10));
	}

	Vec2 getPositionAtAnchorPoint(Vec2 anchor_point, cocos2d::Node * node)
	{

		Vec2 current_position = node->getPosition();
		Vec2 cur_anchor_point = node->getAnchorPoint();
		Vec2 current_size = Vec2(node->getContentSize().width * node->getScaleX(), node->getContentSize().height * node->getScaleY());
		//convert to bottom left
		current_position.x -= cur_anchor_point.x * current_size.x;
		current_position.y -= cur_anchor_point.y * current_size.y;
		//
		return Vec2(current_position.x + current_size.x * anchor_point.x,
			current_position.y + current_size.y * anchor_point.y);
	}

	const std::string getImgUrlCorrect(const std::string & url)
	{
		cocos2d::Size game_size = GetGameSize();
		std::string type_replace = "1";
		if (game_size.width < 400) {
			type_replace = "4";
		}
		else if (game_size.width >= 400 && game_size.width < 680) {
			type_replace = "3";
		}
		else if (game_size.width >= 680 && game_size.width < 880) {
			type_replace = "2";
		}
		else {
			type_replace = "1";
		}
		RKString temp = url;
		temp.ReplaceAll("***", type_replace.c_str());

		return temp.GetString();
	}

	bool _enableVibrate = false;

	void enableVibrate(bool value)
	{
		_enableVibrate = value;
	}

	void vibrate(int milisec)
	{
		if (_enableVibrate)
		{
			Platform::vibrate(milisec);
		}
	}

	void vibrateShort()
	{
		vibrate(500);
	}
	void vibrateVeryShort()
	{
		vibrate(300);
	}
	void vibrateMedium()
	{
		vibrate(1000);
	}
	void vibrateLong()
	{
		vibrate(2000);
	}

}

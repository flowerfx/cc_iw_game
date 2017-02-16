#ifndef _Quest_H_
#define _Quest_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "OptionList.h"
#include "ConfirmBox.h"
#include "../payment/ChargeType.h"

namespace iwinmesage
{
	enum QuestType
	{

		TYPE_ACCEPT_QUEST = 0,
		TYPE_OPEN_URL = 1,
		TYPE_INSTALL_APP = 2,
		TYPE_SUBMIT_QUEST = 3,
		TYPE_OPEN_SCREEN = 4,
		TYPE_RATE_APP = 5,
		TYPE_LIKE_FAN_PAGE = 6,
		TYPE_POST_WALL = 7,
		TYPE_WIN_FREE = 8,
		TYPE_ENABLE_CONTACT = 9,
		TYPE_ENABLE_NOTIFICATION = 10,
		/*
			mo facebook request de nhan qua
		*/
		TYPE_OPEN_FACEBOOK_REQUEST = 11,
		TYPE_SHARE_LINK_FACEBOOK = 12,
		TYPE_SHARE_LINK_FACEBOOK_NEW = 23,
		/*
		 [android] xin quyen admin cho app de chong tu dong uninstall
		*/
		TYPE_OPEN_ADMIN_ROLE = 13,
		/*
		 lac xi ngau
		*/
		TYPE_DICE = 14,
		/*
		like page G+
		*/
		TYPE_LIKE_PAGE_GOOGLE = 15,
		/*
		su kien trung thu
		*/
		TYPE_MOON_FESTIVAL = 16,
		/*
		su kien quoc khanh
		*/
		TYPE_INDEPENDENCE = 17,
		/*
		su kien halloween
		*/
		TYPE_HALLOWEEN = 18,
		/*
		hien thi o nhap yeu cau dien thong tin user
		*/
		TYPE_SEND_USER_INFO = 19,
		TYPE_INVITE_FACEBOOK_FRIENDS = 20,
		TYPE_NEW_LUCKY_CIRCLE = 21,
		TYPE_OPEN_NATIVE_URL = 22
	};

	class Quest
	{
		
CC_SYNTHESIZE(std::string, id, Id);
CC_SYNTHESIZE(QuestType, actionType, ActionType);
CC_SYNTHESIZE(std::string, title, Title);
CC_SYNTHESIZE(std::string, description, Description);
CC_SYNTHESIZE(std::string, urlImage, UrlImage);
CC_SYNTHESIZE(int, categoryId, CategoryId);
CC_SYNTHESIZE(int, maxProgressIndex, MaxProgressIndex);
CC_SYNTHESIZE(int, currentProgress, CurrentProgress);
CC_SYNTHESIZE(std::vector<std::string>, textboxInfo, TextboxInfo);
CC_SYNTHESIZE(OptionList, optionList, optionList);
CC_SYNTHESIZE(ConfirmBox, confirmBox, confirmBox);
CC_SYNTHESIZE(std::string, link, Link);
CC_SYNTHESIZE(std::string, bundleId, BundleId);
CC_SYNTHESIZE(int, screenID, ScreenID);
CC_SYNTHESIZE(int, gameId, GameId);
CC_SYNTHESIZE(std::string, buttonTitle, ButtonTitle);
CC_SYNTHESIZE(std::string, facebookAccessToken, FacebookAccessToken);
CC_SYNTHESIZE(std::string, jsonData, JsonData);
CC_SYNTHESIZE(int64_t, totalTime, TotalTime);
CC_SYNTHESIZE(int64_t, remainTime, RemainTime);
CC_SYNTHESIZE(int64_t, money, Money);
CC_SYNTHESIZE(int64_t, ruby, Ruby);
CC_SYNTHESIZE(int64_t, bonusEventId, BonusEventId);
CC_SYNTHESIZE(bool, isCapchaRequire, IsCapchaRequire);
CC_SYNTHESIZE(bool, isDisableButton, IsDisableButton);

//link captcha cho winfree
CC_SYNTHESIZE(std::string, captchaLink, CaptchaLink);
//token captcha
CC_SYNTHESIZE(std::string, token, Token);
//ky tu captcha client nhap vao
CC_SYNTHESIZE(std::string, captchaChar, CaptchaChar);
//neu screen id la SCREEN_CHARGE_MONEY , bat dung charge type nay
CC_SYNTHESIZE(std::string, chargeType, chargeType);

	public:
	
		Quest();
		Quest(Quest * qs);
		virtual ~Quest();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(const rapidjson::Document & json);

		void ReplaceTextBox(int idx, std::string);
        void ParseData(const rapidjson::Document & json);

		void changeOptionsAtIdx(int idx, bool value);
	};
}
#endif

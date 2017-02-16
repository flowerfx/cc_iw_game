#ifndef _Payment_H_
#define _Payment_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "../event/EventItem.h"

namespace iwinmesage
{
	class Payment
	{

		CC_SYNTHESIZE(std::string, title, Title);
		CC_SYNTHESIZE(std::string, desc, Desc);
		CC_SYNTHESIZE(std::string, type, Type);
		CC_SYNTHESIZE(std::string, charge_type, Charge_type);
		CC_SYNTHESIZE(std::string, icon, Icon);
		//CC_SYNTHESIZE(Payment, data, Data);
		CC_SYNTHESIZE(std::vector<Payment*>, list, List);
		CC_SYNTHESIZE(std::string, message, Message);
		CC_SYNTHESIZE(std::string, description, Description);
		CC_SYNTHESIZE(double, money, Money);
		CC_SYNTHESIZE(int, win, Win);
		CC_SYNTHESIZE(int, win_promotion, Win_promotion);
		CC_SYNTHESIZE(int, charge_by_id, Charge_by_id);
		CC_SYNTHESIZE(int, charge_by_subid, Charge_by_subid);
		CC_SYNTHESIZE(std::string, charge_id, Charge_id);
		CC_SYNTHESIZE(std::string, field_1_name, Field_1_name);
		CC_SYNTHESIZE(std::string, field_2_name, Field_2_name);
		CC_SYNTHESIZE(std::string, field_1_desc, Field_1_desc);
		CC_SYNTHESIZE(std::string, field_2_desc, Field_2_desc);
		CC_SYNTHESIZE(std::vector<Payment*>, items, Items);
		CC_SYNTHESIZE(std::string, content, Content);
		CC_SYNTHESIZE(std::string, phone, Phone);
		CC_SYNTHESIZE(std::string, confirmSMS, ConfirmSMS);
		CC_SYNTHESIZE(std::string, code, Code);
		CC_SYNTHESIZE(std::string, payload, Payload);
		CC_SYNTHESIZE(std::string, link, Link);
		CC_SYNTHESIZE(int, shopId, ShopId);
		CC_SYNTHESIZE(std::string, unit, Unit);
		CC_SYNTHESIZE(std::string, helpLink, HelpLink);
		CC_SYNTHESIZE(std::vector<EventItem>, eventItems, EventItems);
		CC_SYNTHESIZE(bool, isChooseItems, IsChooseItems);

		CC_SYNTHESIZE(bool, error, Error);

	public:
		static  const int CHARGE_BY_ID_SMS = 0;
		static  const int CHARGE_BY_ID_1_PIN = 1;
		static  const int CHARGE_BY_ID_2_PIN = 2;
		static  const int CHARGE_BY_ID_BANKING = 4;
		static  const int CHARGE_BY_ID_IAP = 5;
		static  const int CHARGE_BY_ID_GOOGLE = 9;
		static  const int CHARGE_BY_SUBID_MOBIFONE = 3;
		static  const int CHARGE_BY_SUBID_VINAPHONE = 8;
		static  const int CHARGE_BY_SUBID_VIETTEL = 5;
		static  const int CHARGE_BY_SUBID_GATE = 4;
		static  const int SHOP_ID_DANGTRONG = 1;
		Payment* data;
		Payment();
		virtual ~Payment();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif

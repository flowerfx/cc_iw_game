#ifndef __RESPONSE_AUTHORIZE_OTP_H__
#define __RESPONSE_AUTHORIZE_OTP_H__

#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "OTP.h"
namespace iwinmesage
{
	class ResponseAuthorizeOTP
	{
		// authorize thành công hay thất bại
CC_SYNTHESIZE(bool, result, Result);
		//câu báo trả về khi authorize
CC_SYNTHESIZE(std::string, desc, Desc);
		//loại OTP: google , email, phone
CC_SYNTHESIZE(int, typeOTP, TypeOTP);

	public:
		
		ResponseAuthorizeOTP();
		virtual ~ResponseAuthorizeOTP();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__RESPONSE_AUTHORIZE_OTP_H__

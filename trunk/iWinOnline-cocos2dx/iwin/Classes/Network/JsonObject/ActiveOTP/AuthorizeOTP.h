#ifndef __AUTHORIZE_OTP_H__
#define __AUTHORIZE_OTP_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "OTP.h"
namespace iwinmesage
{
	class AuthorizeOTP
	{
		
CC_SYNTHESIZE(std::string, code, Code);
CC_SYNTHESIZE(int, type, Type);
CC_SYNTHESIZE(int, purpose, Purpose);
CC_SYNTHESIZE(int, trustDevice, TrustDevice);


	public:
		
		AuthorizeOTP();
		virtual ~AuthorizeOTP();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__AUTHORIZE_OTP_H__

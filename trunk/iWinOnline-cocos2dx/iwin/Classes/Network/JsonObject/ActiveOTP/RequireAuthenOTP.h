#ifndef __REQUIRE_AUTHEN_OTP_H__
#define __REQUIRE_AUTHEN_OTP_H__
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
#include "OTP.h"
namespace iwinmesage
{
	class RequireAuthenOTP
	{
		//loại OTP : google. email, phone		
CC_SYNTHESIZE(int, typeOTP, TypeOTP);
		// nếu autho = google , data sẽ là link QR code
CC_SYNTHESIZE(std::string, data, Data);
CC_SYNTHESIZE(std::string, secretKey, SecretKey);
       // câu thông báo
CC_SYNTHESIZE(std::string, desc, Desc);
	  // mục đích authenOTP là gì, check trong PurposeOTP
CC_SYNTHESIZE(int, purpose, Purpose);
     //thời gian của code, hết thời gian phải request cái mới
CC_SYNTHESIZE(long long, expireTime, ExpireTime);


	public:
		
		RequireAuthenOTP();
		virtual ~RequireAuthenOTP();

		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif //__REQUIRE_AUTHEN_OTP_H__

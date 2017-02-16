#ifndef __OTP_H__
#define __OTP_H__

#include <string>

namespace iwinmesage
{
	enum ActiveOTP
	{
		GOOGLE_AUTH = 1,
		EMAIL_AUTH = 2,
		PHONE_AUTH = 3
	};
	enum PurposeOTP
	{
		LOGIN = 1,
		ACTIVE = 2,
		DEACTIVE = 3
	};
}
#endif //__AUTHORIZE_OTP_H__

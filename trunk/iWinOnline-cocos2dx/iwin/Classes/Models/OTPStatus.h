#ifndef __OTP_STATUS_H__
#define __OTP_STATUS_H__
#include <string>
#include "platform/CCPlatformMacros.h"
class OTPStatus
{
public:
	enum TYPE_OTP_STATUS
	{
		STATUS_DEACTIVE = 0,
		STATUS_ACTIVE
	};

	/**
	* GOOGLE_AUTH = 1,
	* EMAIL_AUTH = 2
	* PHONE_AUTH = 3
	*/
	CC_SYNTHESIZE(int, type, Type);

	 /**
	 * status = 1 : Active,
	 * status = 2 : Not active yet
	 */

	CC_SYNTHESIZE(int, status, Status);

	 /**
	 * default check radio button or not
	 */
	 
	CC_SYNTHESIZE(bool, isDefault, IsDefault);

	OTPStatus();
	virtual ~OTPStatus();
};

#endif // __ITEM_H__

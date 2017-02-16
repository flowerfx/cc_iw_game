#ifndef _GOOGLE_USER_H_
#define _GOOGLE_USER_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class GoogleUser
	{
		
	CC_SYNTHESIZE(std::string, id, Id);
	CC_SYNTHESIZE(std::string, name, Name);
	CC_SYNTHESIZE(std::string, given_name, Given_name);
	CC_SYNTHESIZE(std::string, link, Link);
	CC_SYNTHESIZE(std::string, picture, Picture_user);
    CC_SYNTHESIZE(std::string, gender, Gender);
    CC_SYNTHESIZE(std::string, locale, Locale);
    CC_SYNTHESIZE(std::string, email, Email);

	public:
		
		GoogleUser();
		virtual ~GoogleUser();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif

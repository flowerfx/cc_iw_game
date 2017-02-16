#ifndef _RESPONSE_GOOGLE_H_
#define _RESPONSE_GOOGLE_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class ResponseGoogle
	{
		
	CC_SYNTHESIZE(std::string, access_token, Access_token);
	CC_SYNTHESIZE(std::string, token_type, Token_type);
	CC_SYNTHESIZE(int, expires_in, Expires_in);
	CC_SYNTHESIZE(std::string, id_token, Id_token);

	public:
		
		ResponseGoogle();
		virtual ~ResponseGoogle();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif

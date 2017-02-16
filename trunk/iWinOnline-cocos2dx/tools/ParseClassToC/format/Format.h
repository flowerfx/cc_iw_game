#ifndef _|classname|_H_
#define _|classname|_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>
|include|
namespace iwinmesage
{
	class |classname|
	{
		|create_field|
	public:
		|static_field|
		|classname|();
		virtual ~|classname|();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
	};
}
#endif
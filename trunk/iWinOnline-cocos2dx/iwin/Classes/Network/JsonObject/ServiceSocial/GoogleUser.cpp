#include "GoogleUser.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	GoogleUser::GoogleUser()
	{

	}

	GoogleUser::~GoogleUser()
	{

	}
    
	rapidjson::Document GoogleUser::toJson()
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vid(id.c_str(), id.size());
		document.AddMember("id", vid,locator);
		rapidjson::Value vname(name.c_str(), name.size());
		document.AddMember("name", vname, locator);
		rapidjson::Value vgiven_name(given_name.c_str(), given_name.size());
		document.AddMember("given_name", vgiven_name, locator);
		rapidjson::Value vlink(link.c_str(), link.size());
		document.AddMember("link", vlink, locator);
		rapidjson::Value vpicture(picture.c_str(), picture.size());
		document.AddMember("picture", vpicture, locator);
        rapidjson::Value vgender(gender.c_str(), gender.size());
        document.AddMember("gender", vgender, locator);
        rapidjson::Value vlocale(locale.c_str(), locale.size());
        document.AddMember("locale", vlocale, locator);
        rapidjson::Value vemail(email.c_str(), email.size());
        document.AddMember("email", vemail, locator);
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void GoogleUser::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
    //    CC_SYNTHESIZE(std::string, id, Id);
    //    CC_SYNTHESIZE(std::string, name, Name);
    //    CC_SYNTHESIZE(std::string, given_name, Given_name);
    //    CC_SYNTHESIZE(std::string, link, Link);
    //    CC_SYNTHESIZE(std::string, picture, Picture_user);
    //    CC_SYNTHESIZE(std::string, gender, Gender);
    //    CC_SYNTHESIZE(std::string, locale, Locale);
    //    CC_SYNTHESIZE(std::string, email, Email);
    
	void GoogleUser::toData(rapidjson::Document & document)
	{
		if(document.HasMember("id"))
		{
			setId(document["id"].GetString());
		}
		if (document.HasMember("name"))
		{
			setName(document["name"].GetString());
		}
		if (document.HasMember("given_name"))
		{
			setGiven_name(document["given_name"].GetString());
		}
		if (document.HasMember("link"))
		{
			setLink(document["link"].GetString());
		}
		if (document.HasMember("picture"))
		{
			setPicture_user(document["picture"].GetString());
		}
        if (document.HasMember("gender"))
        {
            setGender(document["gender"].GetString());
        }
        if (document.HasMember("locale"))
        {
            setLocale(document["locale"].GetString());
        }
        if (document.HasMember("email"))
        {
            setEmail(document["email"].GetString());
        }

	}
}

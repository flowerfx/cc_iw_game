#ifndef __WIN32_LATFORM_H__
#define __WIN32_LATFORM_H__

#include "cocos2d.h"
#include "Platform/Platform.h"

USING_NS_CC;

using namespace Platform;

namespace AndroidPlatform
{
	class AndroidAppInfo :public AppInfo
	{
	public:
		AndroidAppInfo();
		virtual ~AndroidAppInfo();
		virtual std::string getName() ;
		virtual std::string getLanguage() ;
		virtual std::string getBundleID() ;
		virtual std::string getNotifyKey() ;

	};
	class AndroidDeviceInfo :public DeviceInfo
	{
	public:
		AndroidDeviceInfo();
		virtual ~AndroidDeviceInfo();
		virtual std::string getCarrier() ;
		virtual  std::string getDeviceName() ;
		virtual int getIsJailBreak() ;
		virtual std::string getNet() ;
		virtual std::string getOS() ;
		virtual std::string getOsVersion() ;
		virtual std::string getDeviceUDID() ;
        virtual std::string getMacAddress();
        virtual std::string getAdvertisingID();
        virtual std::string getAndroidID();
        virtual std::string getSimSeri();
        virtual std::string getFingerPrint();
        virtual std::string getSubscriberID();
        virtual std::string getBluetoothAddress();
	};
}
#endif

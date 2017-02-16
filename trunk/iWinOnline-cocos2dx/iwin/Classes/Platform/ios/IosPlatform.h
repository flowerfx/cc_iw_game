#ifndef __IOS_LATFORM_H__
#define __IOS_LATFORM_H__

#include "cocos2d.h"
#include "Platform/Platform.h"

USING_NS_CC;

using namespace Platform;

namespace IosPlatform
{
	class IosAppInfo :public AppInfo
	{
	public:
		IosAppInfo();
		virtual ~IosAppInfo();
		virtual std::string getName();
		virtual std::string getBundleID();
		virtual std::string getNotifyKey();

	};
	class IosDeviceInfo :public DeviceInfo
	{
	public:
		IosDeviceInfo();
		virtual ~IosDeviceInfo();
		virtual std::string getCarrier();
		virtual  std::string getDeviceName();
		virtual int getIsJailBreak();
		virtual std::string getNet();
		virtual std::string getOS();
		virtual std::string getOSVersion();
		virtual std::string getDeviceUDID();
        virtual std::string getMacAddress();
        virtual std::string getAdvertisingID();

	};
}
#endif

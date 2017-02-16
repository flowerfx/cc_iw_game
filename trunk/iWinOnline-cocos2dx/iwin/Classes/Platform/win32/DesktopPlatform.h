#ifndef __WIN32_LATFORM_H__
#define __WIN32_LATFORM_H__

#include "cocos2d.h"
#include "../Platform.h"

USING_NS_CC;

using namespace Platform;

namespace DesktopPlatform
{
	class Win32AppInfo :public AppInfo
	{
	public:
		Win32AppInfo();
		virtual ~Win32AppInfo();
		virtual std::string getName();
		virtual std::string getLanguage();
		virtual std::string getBundleID();
		virtual int getAppWidth();
		virtual int getAppHeight();
		virtual std::string getNotifyKey();

	};
	class Win32DeviceInfo :public DeviceInfo
	{
	public:
		Win32DeviceInfo();
		virtual ~Win32DeviceInfo();
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
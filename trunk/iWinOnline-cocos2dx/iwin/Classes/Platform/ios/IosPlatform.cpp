#include "RKString_Code/RKString.h"
#include "Platform/Platform.h"
#include "IosPlatform.h"
#include "Platform_ios.h"


namespace IosPlatform 
{
#pragma region  info client

	IosAppInfo::IosAppInfo()
	{

	}
	IosAppInfo::~IosAppInfo()
	{

	}
	std::string IosAppInfo::getName()
	{
		return "iWinOnline";
	}

	std::string IosAppInfo::getBundleID()
	{
        return Platform::ios::nativeGetBundleId();
	}

	std::string IosAppInfo::getNotifyKey()
	{
		return "";
	}
	IosDeviceInfo::IosDeviceInfo()
	{}
	IosDeviceInfo::~IosDeviceInfo()
	{}
	std::string IosDeviceInfo::getCarrier()
	{
        return Platform::ios::nativeGetCarrier();
	}

	std::string IosDeviceInfo::getDeviceName()
	{
        return Platform::ios::nativeGetDeviceName();
	}

	int IosDeviceInfo::getIsJailBreak()
	{
        return Platform::ios::nativeGetIsJailBreak();
	}

	std::string IosDeviceInfo::getNet()
	{
        return Platform::ios::nativeGetNet();
	}

	std::string IosDeviceInfo::getOS()
	{
		return "iOS";
	}

	std::string IosDeviceInfo::getOSVersion()
	{
        return Platform::ios::nativeGetOSVersion();
	}

	std::string IosDeviceInfo::getDeviceUDID()
	{
//		return "98-5A-EB-E0-F7-C6";
        return Platform::ios::nativeGetDeviceUDID();
	}
    
    std::string IosDeviceInfo::getMacAddress()
    {
        return Platform::ios::nativeGetMacAddress();
    }
    
    std::string IosDeviceInfo::getAdvertisingID()
    {
        return Platform::ios::nativeGetAdvertisingID();
    }
#pragma endregion



}

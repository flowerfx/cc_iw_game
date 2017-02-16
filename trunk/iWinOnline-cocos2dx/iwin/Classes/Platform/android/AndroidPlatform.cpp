#include "RKString_Code/RKString.h"
#include "Platform/Platform.h"
#include "AndroidPlatform.h"
#include "Platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#include "hellocpp/Java_org_cocos2dx_cpp_AndroidPlatform.h"

namespace AndroidPlatform 
{
#pragma region  info client

	AndroidAppInfo::AndroidAppInfo()
	{

	}
	AndroidAppInfo::~AndroidAppInfo()
	{

	}
	std::string AndroidAppInfo::getName()
	{
		return "iWinOnline";
	}

	std::string AndroidAppInfo::getLanguage()
	{
		return "vi";
	}

	std::string AndroidAppInfo::getBundleID()
	{
		return getPackageNameJNI();
	}

	std::string AndroidAppInfo::getNotifyKey()
	{
		return "";
	}
	AndroidDeviceInfo::AndroidDeviceInfo()
	{}
	AndroidDeviceInfo::~AndroidDeviceInfo()
	{}
	std::string AndroidDeviceInfo::getCarrier()
	{
		return nativeGetCarrier();
	}

	std::string AndroidDeviceInfo::getDeviceName()
	{
		return "android";
	}

	int AndroidDeviceInfo::getIsJailBreak()
	{
		return nativeGetRoot();
	}

	std::string AndroidDeviceInfo::getNet()
	{
		return nativeGetNet();
	}

	std::string AndroidDeviceInfo::getOS()
	{
		return "android";
	}

	std::string AndroidDeviceInfo::getOsVersion()
	{
		return nativeGetOsVersion();
	}

	std::string AndroidDeviceInfo::getDeviceUDID()
	{
		return "98-5A-EB-E0-F7-C6";
	}
    
    std::string AndroidDeviceInfo::getMacAddress()
    {
        return nativeGetMacAddress();
    }
    
    std::string AndroidDeviceInfo::getAdvertisingID()
    {
        return nativeGetAdvertisingID();
    }

    std::string AndroidDeviceInfo::getAndroidID()
    {
        return nativeGetAndroidID();
    }
    std::string AndroidDeviceInfo::getSimSeri()
    {
        return nativeGetSimSeri();
    }
    std::string AndroidDeviceInfo::getFingerPrint()
    {
        return nativeGetFingerPrint();
    }
    std::string AndroidDeviceInfo::getSubscriberID()
    {
        return nativeGetSubscriberID();
    }
    std::string AndroidDeviceInfo::getBluetoothAddress()
    {
        return nativeGetBluetoothAddress();
    }
#pragma endregion



}

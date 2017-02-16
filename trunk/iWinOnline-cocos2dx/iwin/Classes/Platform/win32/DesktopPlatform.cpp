#include "RKString_Code/RKString.h"
#include "../Platform.h"
#include "DesktopPlatform.h"
#include "Screens/ScreenManager.h"
#include "../../MainGame.h"
namespace DesktopPlatform
{
#pragma region  info client

	Win32AppInfo::Win32AppInfo()
	{

	}
	Win32AppInfo::~Win32AppInfo()
	{

	}
	std::string Win32AppInfo::getName()
	{
		return "win 32 cocos";
	}

	std::string Win32AppInfo::getLanguage()
	{
		return XMLMgr->GetLangDecByType(LangMgr->GetCurLanguage())->name_id.GetString();
	}

	std::string Win32AppInfo::getBundleID()
	{
		return "vn.me.iwinonline";
	}

	int Win32AppInfo::getAppWidth()
	{
		return Utility::GetGameSize().width;
	}

	int Win32AppInfo::getAppHeight()
	{
		return Utility::GetGameSize().height;
	}

	std::string Win32AppInfo::getNotifyKey()
	{
		return "";
	}
	Win32DeviceInfo::Win32DeviceInfo()
	{}
	Win32DeviceInfo::~Win32DeviceInfo()
	{}
	std::string Win32DeviceInfo::getCarrier()
	{
		return "";
	}

	std::string Win32DeviceInfo::getDeviceName()
	{
		return "Desktop";
	}

	int Win32DeviceInfo::getIsJailBreak()
	{
		return 0;
	}

	std::string Win32DeviceInfo::getNet()
	{
		return "wifi";
	}

	std::string Win32DeviceInfo::getOS()
	{
		return "Windows 7";
	}

	std::string Win32DeviceInfo::getOSVersion()
	{
		return "0";
	}

	std::string Win32DeviceInfo::getDeviceUDID()
	{
		return "98-5A-EB-E0-F7-C6";
	}
	
	std::string Win32DeviceInfo::getMacAddress()
	{
		return "08-62-66-49-A1-AD";
	}

	std::string Win32DeviceInfo::getAdvertisingID()
	{
		return "";
	}
#pragma endregion



}
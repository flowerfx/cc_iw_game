#ifndef _ClientInforObject_H_
#define _ClientInforObject_H_
#include "platform/CCPlatformMacros.h"
#include <string>
#include "json/document.h"
#include <vector>

namespace iwinmesage
{
	class ClientInforObject
	{
CC_SYNTHESIZE(std::string, device_id, DeviceID);
CC_SYNTHESIZE(std::string, name, Name);
CC_SYNTHESIZE(std::string, subscriber_Id, SubscriberID);
CC_SYNTHESIZE(std::string, sim_serial, SimSerial);
CC_SYNTHESIZE(std::string, android_id, AndroidID);
CC_SYNTHESIZE(std::string, mac_address, MacAddress);
CC_SYNTHESIZE(std::string, platform, Platform);
CC_SYNTHESIZE(bool, rooted, Rooted);
CC_SYNTHESIZE(std::string, finger_print, FingerPrint);
CC_SYNTHESIZE(std::string, os, OS);
CC_SYNTHESIZE(std::string, os_version, OSVersion);
CC_SYNTHESIZE(std::string, udid, UDID);
CC_SYNTHESIZE(std::string, bluetooth_address, BluetoothAddress);
CC_SYNTHESIZE(std::string, advertising_id, AdvertisingID);
CC_SYNTHESIZE(std::string, checksum, CheckSum);
CC_SYNTHESIZE(std::string, first_ip, FirstIP);
CC_SYNTHESIZE(std::string, last_ip, LastIP);
CC_SYNTHESIZE(std::string, channel, Channel);
CC_SYNTHESIZE(std::string, lang, Lang);
CC_SYNTHESIZE(std::string, app_version, AppVersion);
CC_SYNTHESIZE(int, width, Width);
CC_SYNTHESIZE(int, height, Height);
CC_SYNTHESIZE(std::string, carrier, Carrier);
CC_SYNTHESIZE(std::string, bundle_id, BundleID);
CC_SYNTHESIZE(std::string, location, Location);
CC_SYNTHESIZE(std::string, network, Network);
CC_SYNTHESIZE(std::string, count, Count);

	public:
		static  std::string GOWIN_BUNDLE_ID;
        static  std::string IWIN_BUNDLE_ID;

		ClientInforObject();
		virtual ~ClientInforObject();
		rapidjson::Document toJson();
		void toData(std::string json);
		void toData(rapidjson::Document & json);
        std::string toString();
	};
}
#endif

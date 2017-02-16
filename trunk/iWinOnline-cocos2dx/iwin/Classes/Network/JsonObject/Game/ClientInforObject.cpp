#include "ClientInforObject.h"
#include "json/writer.h"
#include "base/ccUTF8.h"

namespace iwinmesage
{
	std::string ClientInforObject::GOWIN_BUNDLE_ID = "vn.me.gowin";
std::string ClientInforObject::IWIN_BUNDLE_ID = "vn.mecorp.iwin";

	ClientInforObject:: ClientInforObject ()
	{

	}

	ClientInforObject::~ClientInforObject()
	{

	}

	rapidjson::Document ClientInforObject::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();

        rapidjson::Value vdevice_id(device_id.c_str(), (unsigned int)device_id.size());
        document.AddMember("device_id", vdevice_id, locator);
        rapidjson::Value vname(name.c_str(), (unsigned int)name.size());
        document.AddMember("name", vname, locator);
        rapidjson::Value vsubscriber_Id(subscriber_Id.c_str(), (unsigned int)subscriber_Id.size());
        document.AddMember("subscriber_Id", vsubscriber_Id, locator);
        rapidjson::Value vsim_serial(sim_serial.c_str(), (unsigned int)sim_serial.size());
        document.AddMember("sim_serial", vsim_serial, locator);
        rapidjson::Value vandroid_id(android_id.c_str(), (unsigned int)android_id.size());
        document.AddMember("android_id", vandroid_id, locator);
        rapidjson::Value vmac_address(mac_address.c_str(), (unsigned int)mac_address.size());
        document.AddMember("mac_address", vmac_address, locator);
        rapidjson::Value vplatform(platform.c_str(), (unsigned int)platform.size());
        document.AddMember("platform", vplatform, locator);
        document.AddMember("rooted", rooted, locator);
        rapidjson::Value vfinger_print(finger_print.c_str(), (unsigned int)finger_print.size());
        document.AddMember("finger_print", vfinger_print, locator);
        rapidjson::Value vos(os.c_str(), (unsigned int)os.size());
        document.AddMember("os", vos, locator);
        rapidjson::Value vos_version(os_version.c_str(), (unsigned int)os_version.size());
        document.AddMember("os_version", vos_version, locator);
        rapidjson::Value vudid(udid.c_str(), (unsigned int)udid.size());
        document.AddMember("udid", vudid, locator);
        rapidjson::Value vbluetooth_address(bluetooth_address.c_str(), (unsigned int)bluetooth_address.size());
        document.AddMember("bluetooth_address", vbluetooth_address, locator);
        rapidjson::Value vadvertising_id(advertising_id.c_str(), (unsigned int)advertising_id.size());
        document.AddMember("advertising_id", vadvertising_id, locator);
        rapidjson::Value vchecksum(checksum.c_str(), (unsigned int)checksum.size());
        document.AddMember("checksum", vchecksum, locator);
        rapidjson::Value vfirst_ip(first_ip.c_str(), (unsigned int)first_ip.size());
        document.AddMember("first_ip", vfirst_ip, locator);
        rapidjson::Value vlast_ip(last_ip.c_str(), (unsigned int)last_ip.size());
        document.AddMember("last_ip", vlast_ip, locator);
        rapidjson::Value vchannel(channel.c_str(), (unsigned int)channel.size());
        document.AddMember("channel", vchannel, locator);
        rapidjson::Value vlang(lang.c_str(), (unsigned int)lang.size());
        document.AddMember("lang", vlang, locator);
        rapidjson::Value vapp_version(app_version.c_str(), (unsigned int)app_version.size());
        document.AddMember("app_version", vapp_version, locator);
        document.AddMember("width", width, locator);
        document.AddMember("height", height, locator);
        rapidjson::Value vcarrier(carrier.c_str(), (unsigned int)carrier.size());
        document.AddMember("carrier", vcarrier, locator);
        rapidjson::Value vbundle_id(bundle_id.c_str(), (unsigned int)bundle_id.size());
        document.AddMember("bundle_id", vbundle_id, locator);
        rapidjson::Value vlocation(location.c_str(), (unsigned int)location.size());
        document.AddMember("location", vlocation, locator);
        rapidjson::Value vnetwork(network.c_str(), (unsigned int)network.size());
        document.AddMember("network", vnetwork, locator);
        rapidjson::Value vcount(count.c_str(), (unsigned int)count.size());
        document.AddMember("count", vcount, locator);
        
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
        std::string temp = buffer.GetString();
		return document;
	}
	void ClientInforObject::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void ClientInforObject::toData(rapidjson::Document & document)
	{
        if(document.HasMember("device_id"))
        {
            setDeviceID(document["device_id"].GetString());
        }
        if(document.HasMember("name"))
        {
            setName(document["name"].GetString());
        }
        if(document.HasMember("subscriber_Id"))
        {
            setSubscriberID(document["subscriber_Id"].GetString());
        }
        if(document.HasMember("sim_serial"))
        {
            setSimSerial(document["sim_serial"].GetString());
        }
        if(document.HasMember("android_id"))
        {
            setAndroidID(document["android_id"].GetString());
        }
        if(document.HasMember("mac_address"))
        {
            setMacAddress(document["mac_address"].GetString());
        }
        if(document.HasMember("platform"))
        {
            setPlatform(document["platform"].GetString());
        }
        if(document.HasMember("rooted"))
        {
            setRooted(document["rooted"].GetBool());
        }
        if(document.HasMember("finger_print"))
        {
            setFingerPrint(document["finger_print"].GetString());
        }
        if(document.HasMember("os"))
        {
            setOS(document["os"].GetString());
        }
        if(document.HasMember("os_version"))
        {
            setOSVersion(document["os_version"].GetString());
        }
        if(document.HasMember("udid"))
        {
            setUDID(document["udid"].GetString());
        }
        if(document.HasMember("bluetooth_address"))
        {
            setBluetoothAddress(document["bluetooth_address"].GetString());
        }
        if(document.HasMember("advertising_id"))
        {
            setAdvertisingID(document["advertising_id"].GetString());
        }
        if(document.HasMember("checksum"))
        {
            setCheckSum(document["checksum"].GetString());
        }
        if(document.HasMember("first_ip"))
        {
            setFirstIP(document["first_ip"].GetString());
        }
        if(document.HasMember("last_ip"))
        {
            setLastIP(document["last_ip"].GetString());
        }
        if(document.HasMember("channel"))
        {
            setChannel(document["channel"].GetString());
        }
        if(document.HasMember("lang"))
        {
            setLang(document["lang"].GetString());
        }
        if(document.HasMember("app_version"))
        {
            setAppVersion(document["app_version"].GetString());
        }
        if(document.HasMember("width"))
        {
            setWidth(document["width"].GetInt());
        }
        if(document.HasMember("height"))
        {
            setHeight(document["height"].GetInt());
        }
        if(document.HasMember("carrier"))
        {
            setCarrier(document["carrier"].GetString());
        }
        if(document.HasMember("bundle_id"))
        {
            setBundleID(document["bundle_id"].GetString());
        }
        if(document.HasMember("location"))
        {
            setLocation(document["location"].GetString());
        }
        if(document.HasMember("network"))
        {
            setNetwork(document["network"].GetString());
        }
        if(document.HasMember("count"))
        {
            setCount(document["count"].GetString());
        }
	}
    
    std::string ClientInforObject::toString()
    {        
        std::string result = cocos2d::StringUtils::format("ClientDeviceInfo{ device_id=%s, name=%s, subscriber_Id=%s, sim_serial=%s, android_id=%s, mac_address=%s, platform=%s, rooted=%d, finger_print=%s, os=%s, os_version=%s, udid=%s, bluetooth_address=%s, advertising_id=%s, checksum=%s, first_ip=%s, last_ip=%s, channel=%s, lang=%s, app_version=%s, width=%d, height=%d, carrier=%s, bundle_id=%s, location=%s, network=%s, count=%s}", device_id.c_str(), name.c_str(), subscriber_Id.c_str(), sim_serial.c_str(), android_id.c_str(), mac_address.c_str(), platform.c_str(), rooted, finger_print.c_str(), os.c_str(), os_version.c_str(), udid.c_str(), bluetooth_address.c_str(), advertising_id.c_str(), checksum.c_str(), first_ip.c_str(), last_ip.c_str(), channel.c_str(), lang.c_str(), app_version.c_str(), width, height, carrier.c_str(), bundle_id.c_str(), location.c_str(), network.c_str(), count.c_str());
        
        return result;
    }
}

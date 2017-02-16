#ifndef __Java_org_cocos2dx_cpp_AndroidPlatform_H__
#define __Java_org_cocos2dx_cpp_AndroidPlatform_H__


extern void nativeShowWeb(std::string url, std::string title);
extern void nativeShowSmsComposer(std::string phone_number, std::string sms_body);
extern void nativeSendSMS(std::string phone_number, std::string sms_body);
extern void nativeMakeCall(std::string phone_number);
extern std::string nativeGetAppName();
extern std::string nativeGetPhoneName();
extern std::string nativeGetVersion();
extern std::string nativeGetOsVersion();
extern std::string nativeGetNet();
extern std::string nativeGetCarrier();
extern std::string nativeGetMacAddress();
extern std::string nativeGetAdvertisingID();
extern std::string nativeGetAndroidID();
extern std::string nativeGetSimSeri();
extern std::string nativeGetFingerPrint();
extern std::string nativeGetSubscriberID();
extern std::string nativeGetBluetoothAddress();
extern int nativeGetRoot();
extern std::string nativeGetClipboardText(std::function<void(std::string)> return_handler);
extern void	nativeSetClipboardText(std::string text);
extern void nativeGGSignIn();
extern void nativeGGSignOut();
extern bool nativeIsGGConnected();
extern std::string nativeGetGGToken();
extern void nativeDismissSplash();
extern std::vector<std::string> nativeAppEmail();
extern void nativeOpenAppEmailWithName(std::string appName);
#endif /* __Java_org_cocos2dx_cpp_AndroidPlatform_H__ */
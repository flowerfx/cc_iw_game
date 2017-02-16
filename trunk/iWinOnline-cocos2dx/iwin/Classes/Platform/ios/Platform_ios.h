
#ifndef __CUSTOM_WEBVIEW_H_
#define __CUSTOM_WEBVIEW_H_

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

namespace Platform
{
    namespace ios
    {
        void nativeShowWebView(std::string url);
        void nativeHideWebView();
        void nativeMakeCall(std::string phoneNumber);
        void nativeShowSMSComposer(std::string phoneNumber, std::string body);
        std::string nativeGetBundleId();
        std::string nativeGetAppName();
        
        
        //for sign in google
        void ggSignIn();
        void ggSignOut();
        bool getIsConnected();
        void setIsConnected(bool connected);
        std::string getAccessToken();
        void setAccessToken(std::string token);
        void onGGSignInCompleted(std::string token);
        
        //generate apple verify
        typedef std::function<void(std::string)> AppleCallBack;
        void nativeGenerateAppleVerify(AppleCallBack callBack);
        
        //get device info
        std::string nativeGetCarrier();
        std::string nativeGetDeviceName();
        int nativeGetIsJailBreak();
        std::string nativeGetNet();
        std::string nativeGetOS();
        std::string nativeGetOSVersion();
        std::string nativeGetDeviceUDID();
        std::string nativeGetAdvertisingID();
        std::string nativeGetMacAddress();
        std::string nativeMakeSHA1Hash(std::string input);
        
        
        std::string nativeGetClipboard();
        void        nativeSetClipboard(std::string text);
        
        //open app email
        void nativeOpenAppEmailWithName(std::string appName);
        void nativeOpenEmail(std::string sender, std::string title);
        
        //
        void nativeLoadImageFromLibrary(const std::function<void(const char * buffer, unsigned int leng)> & call_back);
        
    }
}
#endif

#endif //__CUSTOM_WEBVIEW_H__

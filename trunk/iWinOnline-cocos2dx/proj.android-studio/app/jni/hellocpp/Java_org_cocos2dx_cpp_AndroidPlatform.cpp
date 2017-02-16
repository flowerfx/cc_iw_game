#ifndef __Java_org_cocos2dx_cpp_AndroidPlatform_H__
#define __Java_org_cocos2dx_cpp_AndroidPlatform_H__
#include "Java_org_cocos2dx_cpp_AndroidPlatform.h"
#include "cocos/platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>
#include "Platform/Platform.h"

using namespace cocos2d;

#define  CLASS_NAME "org/cocos2dx/cpp/AndroidPlatform"
std::function<void(std::string)> s_clipboard_return_handler;
extern "C" {

    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AndroidPlatform_nativeOnSMSSucceed(JNIEnv*  env, jobject thiz, jstring phone_number, jstring sms_body) {
        std::string _phone_number = JniHelper::jstring2string(phone_number);
        std::string _sms_body = JniHelper::jstring2string(sms_body);
        Platform::handleSMSSucceed(_phone_number, _sms_body);
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AndroidPlatform_nativeOnFail(JNIEnv*  env, jobject thiz) {
        Platform::handleSMSFail();
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AndroidPlatform_nativeReturnClipboardText(JNIEnv*  env, jobject thiz, jstring text) {
        std::string clipboard_text = JniHelper::jstring2string(text);
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, clipboard_text](){
            s_clipboard_return_handler(clipboard_text);
        });
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AndroidPlatform_nativeOnGGSignInCompleted(JNIEnv* env, jobject thiz, jstring ggToken) {
        std::string _ggToken = JniHelper::jstring2string(ggToken);
        Platform::onGGSignInCompleted(_ggToken);
    }
}

void nativeShowWeb(std::string url, std::string title)
{
	JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "showWeb", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring stringArg1;
        stringArg1 = t.env->NewStringUTF(url.c_str());
        jstring stringArg2 = t.env->NewStringUTF(title.c_str());

        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(t.classID);
    }
}


void nativeShowSmsComposer(std::string phone_number, std::string sms_body)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "showSMSComposer", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring stringArg1;
        stringArg1 = t.env->NewStringUTF(phone_number.c_str());
        jstring stringArg2 = t.env->NewStringUTF(sms_body.c_str());

        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(t.classID);
    }
}

void nativeSendSMS(std::string phone_number, std::string sms_body)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "sendSMS", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring stringArg1;
        stringArg1 = t.env->NewStringUTF(phone_number.c_str());
        jstring stringArg2 = t.env->NewStringUTF(sms_body.c_str());

        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(t.classID);
    }
}

void nativeMakeCall(std::string phone_number)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "makeCall", "(Ljava/lang/String;)V")) {
        jstring stringArg1;
        stringArg1 = t.env->NewStringUTF(phone_number.c_str());

        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(t.classID);
    }
}

std::string nativeGetAppName()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getAppName");
}

std::string nativeGetPhoneName()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getPhoneName");
}

std::string nativeGetVersion()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getVersion");
}

std::string nativeGetOsVersion()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getOsVersion");
}

std::string nativeGetNet()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getNet");
}

std::string nativeGetCarrier()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getCarrier");
}

int nativeGetRoot()
{
    return JniHelper::callStaticIntMethod(CLASS_NAME, "getRoot");
}

std::string nativeGetMacAddress()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getMacAddress");
}

std::string nativeGetAdvertisingID()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getAdvertisingID");
}

std::string nativeGetAndroidID()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getAndroidID");
}

std::string nativeGetSimSeri()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getSimSeri");
}

std::string nativeGetFingerPrint()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getFingerPrint");
}

std::string nativeGetSubscriberID()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getSubscriberID");
}

std::string nativeGetBluetoothAddress()
{
    return  "empty";
}

std::string nativeGetClipboardText(std::function<void(std::string)> return_handler)
{
    s_clipboard_return_handler = return_handler;
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getClipboardText");
}

void nativeSetClipboardText(std::string text)
{
    JniHelper::callStaticVoidMethod(CLASS_NAME, "setClipboardText", text);
}

void nativeGGSignIn()
{
    JniHelper::callStaticVoidMethod(CLASS_NAME, "GGSignIn");
}

void nativeGGSignOut()
{
    JniHelper::callStaticVoidMethod(CLASS_NAME, "GGSignOut");
}

bool nativeIsGGConnected()
{
    return JniHelper::callStaticBooleanMethod(CLASS_NAME, "IsGGConnected");
}

std::string nativeGetGGToken()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "GetGGToken");
}

void nativeDismissSplash()
{
    JniHelper::callStaticVoidMethod(CLASS_NAME, "DismissSplash");
}

std::vector<std::string> nativeAppEmail()
{
    return JniHelper::callStaticStringArrayMethod(CLASS_NAME, "GetAppEmail");
}

void nativeOpenAppEmailWithName(std::string appName)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "OpenAppName", "(Ljava/lang/String;)V")) {
        jstring stringArg1;
        stringArg1 = t.env->NewStringUTF(appName.c_str());

        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(t.classID);
    }
}

#endif /* __Java_org_cocos2dx_cpp_AndroidPlatform_H__ */
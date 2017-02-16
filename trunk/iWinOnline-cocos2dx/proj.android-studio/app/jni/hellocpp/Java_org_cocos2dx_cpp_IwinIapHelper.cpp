#include "Java_org_cocos2dx_cpp_IwinIapHelper.h"
#include "cocos/platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>
#define  CLASS_NAME "org/cocos2dx/cpp/IwinIapHelper"

using namespace cocos2d;


std::function<void(std::string, std::string, std::string)> handlePurchaseSuccess;
std::function<void(std::string msg)> handlePurchaseFailed;

extern "C" {

    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_IwinIapHelper_nativePurchaseSuccess(JNIEnv*  env, jobject thiz, jstring transaction_data, jstring transaction_data_sign) {
		std::string _trasaction_data = JniHelper::jstring2string(transaction_data);
		std::string _trasaction_data_sign = JniHelper::jstring2string(transaction_data_sign);
		handlePurchaseSuccess("android", _trasaction_data, _trasaction_data_sign);
    }

    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_IwinIapHelper_nativePurchaseFailed(JNIEnv*  env, jobject thiz, jstring message) {
    	std::string _message = JniHelper::jstring2string(message);
    	handlePurchaseFailed(_message);
    }
}


void nativePurchase(std::string sku, std::string payload)
{
	JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "purchase", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring stringArg1;
        stringArg1 = t.env->NewStringUTF(sku.c_str());
        jstring stringArg2 = t.env->NewStringUTF(payload.c_str());

        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);

        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(t.classID);
    }
}
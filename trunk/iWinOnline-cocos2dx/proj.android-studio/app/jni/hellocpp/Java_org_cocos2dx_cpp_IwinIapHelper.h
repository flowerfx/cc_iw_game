#ifndef __Java_org_cocos2dx_cpp_IwinIapHelper_H__
#define __Java_org_cocos2dx_cpp_IwinIapHelper_H__
#include <string>
#include <functional>

extern std::function<void(std::string, std::string, std::string)> handlePurchaseSuccess;
extern std::function<void(std::string msg)> handlePurchaseFailed;

extern void nativePurchase(std::string sku, std::string payload);

#endif /* __Java_org_cocos2dx_cpp_IwinIapHelper_H__ */
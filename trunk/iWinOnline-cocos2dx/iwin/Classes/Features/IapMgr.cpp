#include "IapMgr.h"
#include "IAP/IAPManager.h"
#include "cocos2d.h"
#include "json/writer.h"

#include "Common/GameController.h"
#include "Network/JsonObject/Payment/Payment.h"
#include "Network/JsonObject/Payment/GoogleCallbackData.h"
#include "Network/Global/Globalservice.h"
#include "FXFlatform/XFlatform.h"
#include "Platform/Platform.h"
#include "IAP/IAPManager.h"

using namespace Utility;

IapMgr* IapMgr::s_instance = nullptr;

IapMgr* IapMgr::getInstance()
{
	if (s_instance == nullptr)
	{
		s_instance = new IapMgr();
	}
	return s_instance;
}


void IapMgr::init()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	handlePurchaseSuccess = CC_CALLBACK_3(IapMgr::onPurchaseSuccess, this);
	handlePurchaseFailed = CC_CALLBACK_1(IapMgr::onPurchaseFailed, this);
#endif
#ifdef USE_IAP
    GetIAPManager()->OnInitialize();
    
//    Scheduler* scheduler = Director::getInstance()->getScheduler();
//    scheduler->schedule(CC_CALLBACK_1(IapMgr::checkFinishLoad, this ), this, 0.5f, true, "check_finish_load");
//    scheduler->resumeTarget(this);
#endif

}

void IapMgr::checkFinishLoad(float dt)
{
#ifdef USE_IAP
    int load_item_state = GetIAPManager()->StateGetAllItemProcess();
    auto finish_check = false;
    if(load_item_state == IAP::IAP_SUCCEED)
    {
        // load item success
        
        finish_check = true;
        
    }
    else if(load_item_state == IAP::IAP_FAIL)
    {
        // load item failed cmnr
        finish_check = true;
    }
    
    if(finish_check)
    {
        Scheduler* scheduler = Director::getInstance()->getScheduler();
        scheduler->unschedule("check_finish_load", this );
    }
#endif
}

void IapMgr::onPurchaseSuccess( std::string store_name, std::string transaction_data, std::string transaction_data_sign)
{
	if (store_name == "android") {
		GoogleCallbackPurchaseData purchase_data;
		purchase_data.toData(transaction_data);
		GoogleCallbackData* data = new GoogleCallbackData();
		data->setDataSignature(transaction_data_sign);
		data->setPurchaseData(purchase_data);

		//CCLOG("TTTTTTTTTTTTTTTT %s %s", transaction_data.c_str(), transaction_data_sign.c_str());

		rapidjson::Document json_doc = data->toJson();
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		json_doc.Accept(writer);
		CCLOG("AAAAAAAAAAAA %s", buffer.GetString());
		GlobalService::sendChargeMoneyForInApp( iwinmesage::Payment::CHARGE_BY_ID_GOOGLE, buffer.GetString(), GameController::getMoneyType());
	}
	else if (store_name == "apple") {
        CCLOG("token = %s", transaction_data.c_str());
        
        GlobalService::sendChargeMoneyForInApp(iwinmesage::Payment::CHARGE_BY_ID_IAP, transaction_data.c_str(), GameController::getMoneyType());
	}

}

void IapMgr::onPurchaseSuccessApple(std::string transaction)
{
    CCLOG("onPurchaseSuccessApple token = %s", transaction.c_str());
    GlobalService::sendChargeMoneyForInApp(iwinmesage::Payment::CHARGE_BY_ID_IAP, transaction, GameController::getMoneyType());
}

void IapMgr::onPurchaseFailApple(std::string msg)
{
    
}

void IapMgr::onPurchaseFailed(std::string message)
{
	CCLOG("TTTTTTTTTTTTTTTTTTTTTTTTTT onPurchaseFail %s", message.c_str());
}

void IapMgr::purchase(std::string sku, std::string payload)
{
	CCLOG("TTTTTTTTTTTTTTTTTTTTTTTTT purchase %s, %s", sku.c_str(), payload.c_str());
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	nativePurchase(sku, payload);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    std::string item_name = Platform::getAppInfo()->getBundleID();
    item_name += "." + sku;
    
    XFlatform::IAP_PurchaseItemWithID(item_name);
    
    IAP::handle_iap_success = CC_CALLBACK_1(IapMgr::onPurchaseSuccessApple, this);
#endif
}

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2dx-lib)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2dx-lib/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2dx-lib/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../lib)
$(call import-add-path,$(LOCAL_PATH)/sdkboxlib)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := \
./Classes/AnimUtils.cpp \
./Classes/AppDelegate.cpp \
./Classes/Common/ActionParallel.cpp \
./Classes/Common/BannerInvite.cpp \
./Classes/Common/ChatBoard.cpp \
./Classes/Common/CoPhoMgr.cpp \
./Classes/Common/CustomSelectedBox.cpp \
./Classes/Common/EmotionBoard.cpp \
./Classes/Common/FlyMoney.cpp \
./Classes/Common/FocusScrollView.cpp \
./Classes/Common/ChipParticle.cpp \
./Classes/Common/GameController.cpp \
./Classes/Common/IwinListView.cpp \
./Classes/Common/IwinListViewTable.cpp \
./Classes/Common/IwinScale9Sprite.cpp \
./Classes/Common/IwinScrollView.cpp \
./Classes/Common/IwinTextfield.cpp \
./Classes/Common/SpriteIapMgr.cpp \
./Classes/Common/SpriteUrl.cpp \
./Classes/Common/SpriteIap.cpp \
./Classes/Features/DataChatMgr.cpp \
./Classes/Features/FacebookMgr.cpp \
./Classes/Features/GCSignInMgr.cpp \
./Classes/Features/GGSignInMgr.cpp \
./Classes/Features/IapMgr.cpp \
./Classes/MainGame.cpp \
./Classes/Models/Achievement.cpp \
./Classes/Models/Board.cpp \
./Classes/Models/Category.cpp \
./Classes/Models/ChatMessage.cpp \
./Classes/Models/Item.cpp \
./Classes/Models/iWinPlayer.cpp \
./Classes/Models/OTPStatus.cpp \
./Classes/Models/Player.cpp \
./Classes/Models/Province.cpp \
./Classes/Models/User.cpp \
./Classes/Network/Core/BufferStream.cpp \
./Classes/Network/Core/DataInputStream.cpp \
./Classes/Network/Core/DataOutputStream.cpp \
./Classes/Network/Core/DataReader.cpp \
./Classes/Network/Core/DataWriter.cpp \
./Classes/Network/Core/IMessagehandler.cpp \
./Classes/Network/Core/JsonMessage.cpp \
./Classes/Network/Core/Message.cpp \
./Classes/Network/Core/MobileClient.cpp \
./Classes/Network/Core/ServerMgr.cpp \
./Classes/Network/Core/TEA.cpp \
./Classes/Network/Game/GameLogicHandler.cpp \
./Classes/Network/Game/GameMessageHandler.cpp \
./Classes/Network/Game/GameService.cpp \
./Classes/Network/Global/GlobalLogicHandler.cpp \
./Classes/Network/Global/GlobalMessageHandler.cpp \
./Classes/Network/Global/Globalservice.cpp \
./Classes/Network/IwinCommand.cpp \
./Classes/Network/JSONMessageHandler.cpp \
./Classes/Network/JsonObject/ActiveOTP/AuthorizeOTP.cpp \
./Classes/Network/JsonObject/ActiveOTP/RequireAuthenOTP.cpp \
./Classes/Network/JsonObject/ActiveOTP/ResponseAuthorizeOTP.cpp \
./Classes/Network/JsonObject/AppleReviewObject.cpp \
./Classes/Network/JsonObject/Avatar/Avatar.cpp \
./Classes/Network/JsonObject/Avatar/AvatarCat.cpp \
./Classes/Network/JsonObject/Avatar/AvatarCatList.cpp \
./Classes/Network/JsonObject/Avatar/AvatarList.cpp \
./Classes/Network/JsonObject/Avatar/BuyingAvatar.cpp \
./Classes/Network/JsonObject/Avatar/ImageLinkContainerObject.cpp \
./Classes/Network/JsonObject/captcha/CaptchaAnswer.cpp \
./Classes/Network/JsonObject/captcha/CaptchaInfo.cpp \
./Classes/Network/JsonObject/captcha/CaptchaResult.cpp \
./Classes/Network/JsonObject/cashoutruby/CancelGetCard.cpp \
./Classes/Network/JsonObject/cashoutruby/CancelResult.cpp \
./Classes/Network/JsonObject/cashoutruby/CardTelco.cpp \
./Classes/Network/JsonObject/cashoutruby/ChargeRate.cpp \
./Classes/Network/JsonObject/cashoutruby/PaymentHistory.cpp \
./Classes/Network/JsonObject/cashoutruby/PaymentHistoryList.cpp \
./Classes/Network/JsonObject/cashoutruby/PaymentInfo.cpp \
./Classes/Network/JsonObject/cashoutruby/PaymentResult.cpp \
./Classes/Network/JsonObject/cashoutruby/RequestHistoryCharge.cpp \
./Classes/Network/JsonObject/cashoutruby/RubyToVNDInfo.cpp \
./Classes/Network/JsonObject/ClientUI/ServerControlButtonObject.cpp \
./Classes/Network/JsonObject/ClientUI/ServerControlDialogObject.cpp \
./Classes/Network/JsonObject/CuocChauA/BetMatch.cpp \
./Classes/Network/JsonObject/CuocChauA/GetMatchWinMoney.cpp \
./Classes/Network/JsonObject/CuocChauA/League.cpp \
./Classes/Network/JsonObject/CuocChauA/LeagueList.cpp \
./Classes/Network/JsonObject/CuocChauA/Match.cpp \
./Classes/Network/JsonObject/CuocChauA/MatchList.cpp \
./Classes/Network/JsonObject/CuocChauA/MyMatch.cpp \
./Classes/Network/JsonObject/CuocChauA/MyMatchList.cpp \
./Classes/Network/JsonObject/CuocChauA/Team.cpp \
./Classes/Network/JsonObject/CuocChauA/TicketPrice.cpp \
./Classes/Network/JsonObject/CuocChauA/UserRanking.cpp \
./Classes/Network/JsonObject/CuocChauA/UserRankingList.cpp \
./Classes/Network/JsonObject/Dice/BetStars.cpp \
./Classes/Network/JsonObject/Event/BaseResultObject.cpp \
./Classes/Network/JsonObject/Event/EventAward.cpp \
./Classes/Network/JsonObject/Event/EventAwardList.cpp \
./Classes/Network/JsonObject/Event/EventInfo.cpp \
./Classes/Network/JsonObject/Event/EventItem.cpp \
./Classes/Network/JsonObject/Event/EventItemId.cpp \
./Classes/Network/JsonObject/Event/EventItemList.cpp \
./Classes/Network/JsonObject/Event/EventType.cpp \
./Classes/Network/JsonObject/Event/ShopId.cpp \
./Classes/Network/JsonObject/Event/UpgrageItem.cpp \
./Classes/Network/JsonObject/Event/UserInfo.cpp \
./Classes/Network/JsonObject/Event/UserReceiveEventItem.cpp \
./Classes/Network/JsonObject/Facebook/FacebookFriend.cpp \
./Classes/Network/JsonObject/Facebook/FacebookFriendList.cpp \
./Classes/Network/JsonObject/Facebook/FacebookFriendListPage.cpp \
./Classes/Network/JsonObject/Facebook/FacebookFriendListPageCursor.cpp \
./Classes/Network/JsonObject/Facebook/FacebookIdsList.cpp \
./Classes/Network/JsonObject/Facebook/FacebookMyInfo.cpp \
./Classes/Network/JsonObject/Facebook/FacebookPicture.cpp \
./Classes/Network/JsonObject/Facebook/FacebookPictureData.cpp \
./Classes/Network/JsonObject/Facebook/FacebookPostWallData.cpp \
./Classes/Network/JsonObject/Facebook/FacebookRequestResponse.cpp \
./Classes/Network/JsonObject/Friend/Contact.cpp \
./Classes/Network/JsonObject/Friend/FacebookRequest.cpp \
./Classes/Network/JsonObject/Friend/Friend.cpp \
./Classes/Network/JsonObject/Friend/FriendContact.cpp \
./Classes/Network/JsonObject/Friend/InviteUser.cpp \
./Classes/Network/JsonObject/Friend/Location.cpp \
./Classes/Network/JsonObject/Friend/OpenFacebookRequest.cpp \
./Classes/Network/JsonObject/Friend/RequestFacebookRequest.cpp \
./Classes/Network/JsonObject/Friend/RequestFriendFromContact.cpp \
./Classes/Network/JsonObject/Friend/RequestFriendFromFacebook.cpp \
./Classes/Network/JsonObject/Friend/RequestFriendFromLocation.cpp \
./Classes/Network/JsonObject/Friend/ResponseFacebookRequest.cpp \
./Classes/Network/JsonObject/Friend/ResponseFriendFromContact.cpp \
./Classes/Network/JsonObject/Friend/ResponseFriendFromFacebook.cpp \
./Classes/Network/JsonObject/Friend/ResponseFriendFromLocation.cpp \
./Classes/Network/JsonObject/Game/BoardOwner.cpp \
./Classes/Network/JsonObject/Game/ClientInforObject.cpp \
./Classes/Network/JsonObject/Game/CreateBoard.cpp \
./Classes/Network/JsonObject/Game/GameEvent.cpp \
./Classes/Network/JsonObject/Game/GameEventList.cpp \
./Classes/Network/JsonObject/Game/GameList.cpp \
./Classes/Network/JsonObject/Game/GameState.cpp \
./Classes/Network/JsonObject/Game/GetMinMaxBoard.cpp \
./Classes/Network/JsonObject/Game/GetOnwerBoardListResponse.cpp \
./Classes/Network/JsonObject/Game/GetOwnerBoardListRequest.cpp \
./Classes/Network/JsonObject/Guides.cpp \
./Classes/Network/JsonObject/Heartbeat/HeartBeatConfiguration.cpp \
./Classes/Network/JsonObject/heartbeat/NetworkStrength.cpp \
./Classes/Network/JsonObject/lobby/ChatLobbyItem.cpp \
./Classes/Network/JsonObject/lobby/ChatLobbyList.cpp \
./Classes/Network/JsonObject/lobby/DoPlayNow.cpp \
./Classes/Network/JsonObject/lobby/LobbyChatMessage.cpp \
./Classes/Network/JsonObject/lobby/LobbyItem.cpp \
./Classes/Network/JsonObject/lobby/LobbyList.cpp \
./Classes/Network/JsonObject/lobby/RequestLobbyList.cpp \
./Classes/Network/JsonObject/Login/AppleVerify.cpp \
./Classes/Network/JsonObject/Login/Guest.cpp \
./Classes/Network/JsonObject/Login/InvalidLoginToken.cpp \
./Classes/Network/JsonObject/Login/LoginWithToken.cpp \
./Classes/Network/JsonObject/Login/LoginWithTokenSuccess.cpp \
./Classes/Network/JsonObject/Login/MustRegisterWithToken.cpp \
./Classes/Network/JsonObject/Login/RegisterWithToken.cpp \
./Classes/Network/JsonObject/LuckyCircle/LuckyCircleQuestList.cpp \
./Classes/Network/JsonObject/LuckyCircle/LuckyCircleQuestListItem.cpp \
./Classes/Network/JsonObject/LuckyCircle/LuckyCircleResultList.cpp \
./Classes/Network/JsonObject/LuckyCircle/LuckyCircleResultListItem.cpp \
./Classes/Network/JsonObject/LuckyCircle/NewCircleInfoRequest.cpp \
./Classes/Network/JsonObject/LuckyCircle/NewCircleTurnRequest.cpp \
./Classes/Network/JsonObject/LuckyCircle/NewLuckyGetTiketsObject.cpp \
./Classes/Network/JsonObject/Mobo/APIResult.cpp \
./Classes/Network/JsonObject/Mobo/IwinUser.cpp \
./Classes/Network/JsonObject/Mobo/MoboData.cpp \
./Classes/Network/JsonObject/Mobo/MoboMap.cpp \
./Classes/Network/JsonObject/Payment/GoogleCallbackData.cpp \
./Classes/Network/JsonObject/Payment/GoogleCallbackPurchaseData.cpp \
./Classes/Network/JsonObject/Payment/GooglePurchaseData.cpp \
./Classes/Network/JsonObject/Payment/Payment.cpp \
./Classes/Network/JsonObject/Promotion/PromotionGetWin.cpp \
./Classes/Network/JsonObject/Promotion/PromotionItem.cpp \
./Classes/Network/JsonObject/Promotion/PromotionList.cpp \
./Classes/Network/JsonObject/Quest/ConfirmBox.cpp \
./Classes/Network/JsonObject/Quest/GetQuestList.cpp \
./Classes/Network/JsonObject/Quest/OptionItem.cpp \
./Classes/Network/JsonObject/Quest/OptionList.cpp \
./Classes/Network/JsonObject/Quest/Quest.cpp \
./Classes/Network/JsonObject/Quest/QuestCategory.cpp \
./Classes/Network/JsonObject/Quest/QuestCategoryList.cpp \
./Classes/Network/JsonObject/Quest/RequestQuest.cpp \
./Classes/Network/JsonObject/Quest/ResponseQuestList.cpp \
./Classes/Network/JsonObject/Quest/ScreenID.cpp \
./Classes/Network/JsonObject/ReviewApp.cpp \
./Classes/Network/JsonObject/ServiceSocial/GoogleUser.cpp \
./Classes/Network/JsonObject/ServiceSocial/ResponseGoogle.cpp \
./Classes/Network/JsonObject/SmartPhone/ButtonFeature.cpp \
./Classes/Network/JsonObject/SmartPhone/ErrorInfo.cpp \
./Classes/Network/JsonObject/SmartPhone/GetGui.cpp \
./Classes/Network/JsonObject/SmartPhone/RegisterInfor.cpp \
./Classes/Network/JsonObject/SmartPhone/RequestFeature.cpp \
./Classes/Network/JsonObject/SmartPhone/StickerInfo.cpp \
./Classes/Network/JsonObject/Stock/BuyingStock.cpp \
./Classes/Network/JsonObject/Stock/GetStockDividend.cpp \
./Classes/Network/JsonObject/Stock/MyStock.cpp \
./Classes/Network/JsonObject/Stock/MyStockList.cpp \
./Classes/Network/JsonObject/Stock/Stock.cpp \
./Classes/Network/JsonObject/Stock/StockCommand.cpp \
./Classes/Network/JsonObject/Stock/StockList.cpp \
./Classes/Network/JsonObject/taixiu/MoneyObject.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuBetRequest.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuBetResult.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuGameInfo.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuInfoRequest.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuMatch.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuMatchList.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuMatchRequest.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuUnsupportedList.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuUserBet.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuUserHistory.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuUserHistoryList.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuUserOrder.cpp \
./Classes/Network/JsonObject/taixiu/TaiXiuUserOrderList.cpp \
./Classes/Network/JsonObject/TransactionHistory/TransactionHistory.cpp \
./Classes/Network/JsonObject/TransactionHistory/TransactionHistoryDetail.cpp \
./Classes/Network/JsonObject/TransactionHistory/TransactionHistoryRequest.cpp \
./Classes/Network/Socket/Socket.cpp \
./Classes/Network/Socket/SocketConnect.cpp \
./Classes/Platform/Platform.cpp \
./Classes/Screens/ChangeAwardScreen.cpp \
./Classes/Screens/CommonScreen.cpp \
./Classes/Screens/CommonScreens/AnnouceServerLayer.cpp \
./Classes/Screens/CommonScreens/BonusMoneyLayer.cpp \
./Classes/Screens/CommonScreens/ChangePassLayer.cpp \
./Classes/Screens/CommonScreens/CommonLayer.cpp \
./Classes/Screens/CommonScreens/ConfirmCodeLayer.cpp \
./Classes/Screens/CommonScreens/DialogLayer.cpp \
./Classes/Screens/CommonScreens/EmoBoardLayer.cpp \
./Classes/Screens/CommonScreens/InviteLayer.cpp \
./Classes/Screens/CommonScreens/LeaderboardLayer.cpp \
./Classes/Screens/CommonScreens/LuckyCircleScreen.cpp \
./Classes/Screens/CommonScreens/PlayerInfoLayer.cpp \
./Classes/Screens/CommonScreens/TaiXiuLayer.cpp \
./Classes/Screens/CommonScreens/ToastLayer.cpp \
./Classes/Screens/CommonScreens/TutorialLayer.cpp \
./Classes/Screens/DialogScreen.cpp \
./Classes/Screens/EventScreen.cpp \
./Classes/Screens/FriendScreen.cpp \
./Classes/Screens/GamePlayMgr.cpp \
./Classes/Screens/GameScreens/BaiCao/BaiCaoMsgHandler.cpp \
./Classes/Screens/GameScreens/BaiCao/BaiCaoPlayer.cpp \
./Classes/Screens/GameScreens/BaiCao/BaiCaoScreen.cpp \
./Classes/Screens/GameScreens/BaiCao/FlopCardUI.cpp \
./Classes/Screens/GameScreens/CoTuong/CoTuongBoard.cpp \
./Classes/Screens/GameScreens/CoTuong/CoTuongMsgHandler.cpp \
./Classes/Screens/GameScreens/CoTuong/CoTuongPiece.cpp \
./Classes/Screens/GameScreens/CoTuong/CoTuongPlayer.cpp \
./Classes/Screens/GameScreens/CoTuong/CoTuongScreen.cpp \
./Classes/Screens/GameScreens/DealCardLayer.cpp \
./Classes/Screens/GameScreens/GameScreens.cpp \
./Classes/Screens/GameScreens/MauBinh/LabelBG.cpp \
./Classes/Screens/GameScreens/MauBinh/MauBinhMsgHandler.cpp \
./Classes/Screens/GameScreens/MauBinh/MauBinhPlayer.cpp \
./Classes/Screens/GameScreens/MauBinh/MauBinhScreen.cpp \
./Classes/Screens/GameScreens/MauBinh/MauBinhService.cpp \
./Classes/Screens/GameScreens/Phom/CardPhomBox.cpp \
./Classes/Screens/GameScreens/Phom/PhomMsgHandler.cpp \
./Classes/Screens/GameScreens/Phom/PhomScreen.cpp \
./Classes/Screens/GameScreens/Phom/PhomService.cpp \
./Classes/Screens/GameScreens/Phom/PhomUtils.cpp \
./Classes/Screens/GameScreens/TienLen/TienLenMsgHandler.cpp \
./Classes/Screens/GameScreens/TienLen/TienLenPlayer.cpp \
./Classes/Screens/GameScreens/TienLen/TienLenScreen.cpp \
./Classes/Screens/GameScreens/TienLen/TienLenService.cpp \
./Classes/Screens/GameScreens/XiZach/XiZachMsgHandler.cpp \
./Classes/Screens/GameScreens/XiZach/XiZachPlayer.cpp \
./Classes/Screens/GameScreens/XiZach/XiZachScreen.cpp \
./Classes/Screens/HUDScreen.cpp \
./Classes/Screens/IAPScreen.cpp \
./Classes/Screens/LeaderboardScreen.cpp \
./Classes/Screens/LoadingScreen.cpp \
./Classes/Screens/LobbyScreen.cpp \
./Classes/Screens/LoginScreen.cpp \
./Classes/Screens/LoginScreens/ForgetPassLayer.cpp \
./Classes/Screens/LoginScreens/LoginLayer.cpp \
./Classes/Screens/LoginScreens/OTPLoginLayer.cpp \
./Classes/Screens/LoginScreens/RegisterLayer.cpp \
./Classes/Screens/MainScreen.cpp \
./Classes/Screens/NewLobbyScreen.cpp \
./Classes/Screens/Object/Card.cpp \
./Classes/Screens/Object/CardUtils.cpp \
./Classes/Screens/Object/GameObject.cpp \
./Classes/Screens/Object/ObjectEntity.cpp \
./Classes/Screens/Object/PlayerGroup.cpp \
./Classes/Screens/Object/RectProcessTime.cpp \
./Classes/Screens/Object/SliderBet.cpp \
./Classes/Screens/Object/SpecialCardsBound.cpp \
./Classes/Screens/Object/TimeBarProcess.cpp \
./Classes/Screens/ProfileScreen.cpp \
./Classes/Screens/ProfileScreens/LeftUserProfileLayer.cpp \
./Classes/Screens/ProfileScreens/ShopAvatarLayer.cpp \
./Classes/Screens/ProfileScreens/TransferMoneyLayout.cpp \
./Classes/Screens/ScreenManager.cpp \
./Classes/Screens/BaseScreen.cpp \
./Classes/Services/DatabaseService.cpp \
./Classes/Services/GuideService.cpp \
./Classes/Services/ImageService.cpp \
./Classes/Services/ReviewService.cpp \
./Classes/Utils.cpp \
./Classes/Platform/android/AndroidPlatform.cpp \
hellocpp/sqlite3.c \
hellocpp/Java_org_cocos2dx_cpp_IwinIapHelper.cpp \
hellocpp/Java_org_cocos2dx_cpp_AndroidPlatform.cpp \
hellocpp/main.cpp 


LOCAL_C_INCLUDES := $(LOCAL_PATH)/Classes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/hellocpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/sdkboxlib

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_WHOLE_STATIC_LIBRARIES := PluginFacebook sdkbox PluginSdkboxPlay

LOCAL_STATIC_LIBRARIES := cocos2dx_static
LOCAL_STATIC_LIBRARIES += utility_static
LOCAL_STATIC_LIBRARIES += rkutils_static


# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,Utility)
$(call import-module,RKUtils)
$(call import-module,sdkbox)
$(call import-module,pluginfacebook)
$(call import-module,pluginsdkboxplay)


# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
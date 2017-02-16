#ifndef __JSON_HEADER_H__
#define __JSON_HEADER_H__

#include "Network/JsonObject/Game/GameList.h"
#include "Network/JsonObject/Game/BoardOwner.h"

#include "Network/JsonObject/captcha/CaptchaInfo.h"
#include "Network/JsonObject/captcha/CaptchaResult.h"
#include "Network/JsonObject/captcha/CaptchaAnswer.h"

#include "Network/JsonObject/Payment/Payment.h"

#include "Network/JsonObject/lobby/LobbyList.h"
#include "Network/JsonObject/lobby/ChatLobbyList.h"
#include "Network/JsonObject/lobby/RequestLobbyList.h"
#include "Network/JsonObject/lobby/DoPlayNow.h"
#include "Network/JsonObject/lobby/LobbyChatMessage.h"

#include "Network/JsonObject/Avatar/AvatarCat.h"
#include "Network/JsonObject/Avatar/AvatarCatList.h"
#include "Network/JsonObject/Avatar/AvatarList.h"
#include "Network/JsonObject/Avatar/BuyingAvatar.h"

#include "Network/JsonObject/Game/GetOnwerBoardListResponse.h"
#include "Network/JsonObject/Game/GetOwnerBoardListRequest.h"
#include "Network/JsonObject/Game/ClientInforObject.h"
#include "Network/JsonObject/Game/GetMinMaxBoard.h"

#include "Network/JsonObject/cashoutruby/RubyToVNDInfo.h"
#include "Network/JsonObject/cashoutruby/PaymentResult.h"
#include "Network/JsonObject/cashoutruby/PaymentHistoryList.h"
#include "Network/JsonObject/cashoutruby/CancelResult.h"

#include "Network/JsonObject/SmartPhone/RequestFeature.h"

#include "Network/JsonObject/taixiu/TaiXiuBetResult.h"
#include "Network/JsonObject/taixiu/TaiXiuGameInfo.h"
#include "Network/JsonObject/taixiu/TaiXiuUnsupportedList.h"
#include "Network/JsonObject/taixiu/TaiXiuUserHistoryList.h"
#include "Network/JsonObject/taixiu/TaiXiuUserOrderList.h"
#include "Network/JsonObject/taixiu/TaiXiuInfoRequest.h"
#include "Network/JsonObject/taixiu/TaiXiuBetRequest.h"
#include "Network/JsonObject/taixiu/TaiXiuMatch.h"
#include "Network/JsonObject/taixiu/TaiXiuMatchList.h"
#include "Network/JsonObject/taixiu/TaiXiuMatchRequest.h"

#include "network/JsonObject/LuckyCircle/LuckyCircleQuestList.h"
#include "network/JsonObject/LuckyCircle/LuckyCircleResultList.h"

#include "Network/JsonObject/TransactionHistory/TransactionHistory.h"
#include "Network/JsonObject/TransactionHistory/TransactionHistoryDetail.h"
#include "Network/JsonObject/TransactionHistory/TransactionHistoryRequest.h"

#include "Network/JsonObject/ActiveOTP/AuthorizeOTP.h"
#include "Network/JsonObject/ActiveOTP/RequireAuthenOTP.h"
#include "Network/JsonObject/ActiveOTP/ResponseAuthorizeOTP.h"

#include "Network/JsonObject/Friend/Contact.h"
#include "Network/JsonObject/Friend/FacebookRequest.h"
#include "Network/JsonObject/Friend/Friend.h"
#include "Network/JsonObject/Friend/FriendContact.h"
#include "Network/JsonObject/Friend/InviteUser.h"
#include "Network/JsonObject/Friend/Location.h"
#include "Network/JsonObject/Friend/OpenFacebookRequest.h"
#include "Network/JsonObject/Friend/RequestFacebookRequest.h"
#include "Network/JsonObject/Friend/RequestFriendFromContact.h"
#include "Network/JsonObject/Friend/RequestFriendFromFacebook.h"
#include "Network/JsonObject/Friend/RequestFriendFromLocation.h"
#include "Network/JsonObject/Friend/ResponseFacebookRequest.h"
#include "Network/JsonObject/Friend/ResponseFriendFromContact.h"
#include "Network/JsonObject/Friend/ResponseFriendFromFacebook.h"
#include "Network/JsonObject/Friend/ResponseFriendFromLocation.h"

#include "Network/JsonObject/Quest/RequestQuest.h"
#include "Network/JsonObject/Quest/ResponseQuestList.h"

#include "Network/JsonObject/Nohu/NoHuBetRequest.h"
#include "Network/JsonObject/Nohu/NoHuBetResult.h"
#include "Network/JsonObject/Nohu/NoHuGameInfo.h"
#include "Network/JsonObject/Nohu/NoHuHistoryRequest.h"
#include "Network/JsonObject/Nohu/NoHuListUser.h"
#include "Network/JsonObject/Nohu/NoHuUnsupportedList.h"
#include "Network/JsonObject/Nohu/NoHuUserHistoryList.h"
#include "Network/JsonObject/Nohu/NoHuUserHistoryServer.h"

#endif // __JSON_HEADER_H__


#include "platform/CCPlatformConfig.h"
#include "cocos2d.h"
#include "../../MainGame.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#include "Platform_ios.h"
#include "platform/ios/CCEAGLView-ios.h"
#include "UIKit/UIImage.h"
#include "UIKit/UIButton.h"

#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <CoreTelephony/CTCarrier.h>
#import <GameKit/GKLocalPlayer.h>
#import <GoogleSignIn/GoogleSignIn.h>
#import <AdSupport/ASIdentifierManager.h>
#import <CommonCrypto/CommonDigest.h>

#import "JBroken.h"
#import "Reachability.h"
#import "iOSDelegate.h"
@interface GGSignInWrapper: NSObject

@property (nonatomic, retain) NSString* ggtoken;
@property (nonatomic) BOOL isConnected;

+ (void) ggWrapperSignin;
+ (void) ggWrapperSignout;
+ (id) sharedInstance;
@end

@implementation GGSignInWrapper

@synthesize ggtoken;
@synthesize isConnected;

+ (id) sharedInstance {
    static GGSignInWrapper *sharedInstance = nil;
    @synchronized(self) {
        if (sharedInstance == nil)
            sharedInstance = [[self alloc] init];
    }
    return sharedInstance;
}

- (id)init {
    self = [super init];
    ggtoken = @"";
    isConnected = false;
    return self;
}

+ (void) ggWrapperSignin  {
    [[GIDSignIn sharedInstance] signIn];
}

+ (void) ggWrapperSignout {
    [[GIDSignIn sharedInstance] signOut];
}

@end

@interface WebViewWrapper : NSObject

@property (nonatomic) UIView* _view;
@property (nonatomic) UIWebView* _webView;


+ (instancetype)webViewWrapper;
- (void)initWebView;
- (void)loadUrl:(const std::string &) url;
- (void)hide;
- (void)cellButtonPressed;
@end


@implementation WebViewWrapper {
    
}

+ (instancetype)webViewWrapper {
    return [[[self alloc] init] autorelease];
}

- (instancetype)init {
    self = [super init];
    if(self) {
        [self initWebView];
    }
    return self;
}

- (void)initWebView {
    if(self._view == nullptr)
    {
        auto view = cocos2d::Director::getInstance()->getOpenGLView();
        auto eaglview = (CCEAGLView *) view->getEAGLView();
    
        int padding = 17;
    
        cocos2d::Size device_size = cocos2d::Size(eaglview.frame.size.width, eaglview.frame.size.height);//cocos2d::Director::getInstance()->getVisibleSize();
        UIImage* image = [UIImage imageNamed:@"close_button.png"];
        UIButton* btnClose = [UIButton buttonWithType:UIButtonTypeRoundedRect ];
        btnClose.frame = CGRectMake(0, 0, image.size.width, image.size.height);
        [btnClose setImage:image forState:UIControlStateNormal];
        [btnClose addTarget:self action:@selector(cellButtonPressed) forControlEvents:UIControlEventTouchUpInside];
        btnClose.center = CGPointMake(device_size.width - image.size.width / 2, image.size.height / 2);
        self._view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, device_size.width, device_size.height)];
        self._webView = [[UIWebView alloc] initWithFrame:CGRectMake(padding, padding, device_size.width - padding * 2, device_size.height - padding * 2)];
        [self._view addSubview:self._webView];
        [self._view addSubview:btnClose];
    
        [eaglview addSubview:self._view];
  
    }
    
}

- (void)cellButtonPressed
{
    [self hide];
}

-(void)loadUrl:(const std::string &)urlString
{
    self._view.hidden = false;
    NSURL *url = [NSURL URLWithString:@(urlString.c_str())];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    [self._webView loadRequest:request];
    
}

-(void)hide
{
    self._view.hidden = true;
}

@end

#define SYSTEM_VERSION_EQUAL_TO(v)                  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)
#define SYSTEM_VERSION_GREATER_THAN(v)              ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)
#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
#define SYSTEM_VERSION_LESS_THAN(v)                 ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)
#define SYSTEM_VERSION_LESS_THAN_OR_EQUAL_TO(v)     ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedDescending)

namespace Platform
{
    namespace ios{
        
        WebViewWrapper* s_webview = nullptr;
        
        void nativeShowWebView(std::string url)
        {
            if(s_webview == nullptr)
            {
                s_webview = [WebViewWrapper webViewWrapper];
                [s_webview retain];
                
            }
            [s_webview loadUrl:url];
        }
        
        void nativeHideWebView()
        {
            [s_webview hide];
        }
        
        void nativeMakeCall(std::string phoneNumber)
        {
            NSString* phone_num = [NSString stringWithUTF8String:phoneNumber.c_str()];
            NSString* url = [@"telprompt://" stringByAppendingString:phone_num];
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
        }
        
        void nativeShowSMSComposer(std::string phoneNumber, std::string body)
        {
            if( SYSTEM_VERSION_LESS_THAN(@"6.0.0"))
            {
                
            }
            else
            {
                [[UIPasteboard generalPasteboard] setString:[NSString stringWithUTF8String:body.c_str()]];
                NSString* phone_num = [NSString stringWithUTF8String:phoneNumber.c_str()];
                NSString* msg_body = [NSString stringWithUTF8String:body.c_str()];
                NSString* url = [[NSString stringWithFormat:@"sms:%@&body=%@", phone_num, msg_body] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                NSLog(@"url open: %@", url);
                if( [[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:url]] )
                {
                    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url] ];
                }

            }
        }
        
        std::string nativeGetBundleId()
        {
            return [[[NSBundle mainBundle] bundleIdentifier] UTF8String];
        }
        
        std::string nativeGetAppName()
        {
            return [[[[NSBundle mainBundle] localizedInfoDictionary] objectForKey:@"CFBundleDisplayName"] UTF8String];
        }
        
        void ggSignIn() {
            [GGSignInWrapper ggWrapperSignin];
        }
    
        void ggSignOut() {
            [GGSignInWrapper ggWrapperSignout];
        }
        
        bool getIsConnected() {
            return [[GGSignInWrapper sharedInstance] isConnected];
        }
        
        void setIsConneted(bool connected) {
            [[GGSignInWrapper sharedInstance] setIsConnected: connected ? TRUE : FALSE];
        }
        
        std::string getAccessToken() {
            return [[[GGSignInWrapper sharedInstance] ggtoken] UTF8String];
        }
        
        void setAccessToken(std::string token) {
            [[GGSignInWrapper sharedInstance] setGgtoken: [NSString stringWithUTF8String:token.c_str()]];
        }
        
        void onGGSignInCompleted(std::string token) {
            GameMgr->Network_loginWithToken(MainGame::TYPE_GOOGLE, token.c_str());
        }
        
        //get app info
        
        
        //get device info
        /**
         * Method name: nativeGetCarrier
         * Description: get name of carrier using if have
         * Parameters:
         * ref : 
         // http://stackoverflow.com/questions/21186307/getting-wrong-carrier-name-in-ios
         // https://en.wikipedia.org/wiki/Mobile_country_code#Vietnam_-_VN
         */
        std::string nativeGetCarrier()
        {
            CTTelephonyNetworkInfo *netinfo = [[CTTelephonyNetworkInfo alloc] init];
            CTCarrier *carrier = [netinfo subscriberCellularProvider];
            NSLog(@"Carrier Name: %@", [carrier carrierName]);
            NSLog(@"Mobile Country Code: %@", [carrier mobileCountryCode]);
            NSLog(@"Mobile Network Code: %@", [carrier mobileNetworkCode]);
            
            NSString* VNCountryCode = [NSString stringWithFormat:@"452"];
            
            NSString* MobiPhone = [NSString stringWithFormat:@"01"];
            NSString* MobiPhoneName = [NSString stringWithFormat:@"VN MOBIFONE"];
            
            NSString* VinaPhone = [NSString stringWithFormat:@"02"];
            NSString* VinaPhoneName = [NSString stringWithFormat:@"VINAPHONE"];
            
            NSString* Viettel = [NSString stringWithFormat:@"03"];
            NSString* ViettelName = [NSString stringWithFormat:@"VIETTEL"];
            
            NSString* VietNamMobile = [NSString stringWithFormat:@"04"];
            NSString* VietNamMobileName = [NSString stringWithFormat:@"Vietnamobile"];
            
            if([[carrier mobileCountryCode] isEqualToString:VNCountryCode])
            {
                if([[carrier mobileNetworkCode] isEqualToString:MobiPhone])
                {
                    return [MobiPhoneName UTF8String];
                }
                else if ([[carrier mobileNetworkCode] isEqualToString:VinaPhone])
                {
                    return [VinaPhoneName UTF8String];
                }
                else if ([[carrier mobileNetworkCode] isEqualToString:Viettel])
                {
                    return [ViettelName UTF8String];
                }
                else if ([[carrier mobileNetworkCode] isEqualToString:VietNamMobile])
                {
                    return [VietNamMobileName UTF8String];
                }
            }
            
            return "";
        }
        
        std::string nativeGetDeviceName()
        {
            return [[[UIDevice currentDevice] name] UTF8String];
        }
        
        int nativeGetIsJailBreak()
        {
            return [JBroken isDeviceJailbroken] ? 1 : 0;
        }
        
        /**
         * Method name: getNet
         * Description: detect network status
         * Parameters:
         * ref :
         // http://stackoverflow.com/questions/7938650/ios-detect-3g-or-wifi
         */
        std::string nativeGetNet()
        {
            Reachability *reachability = [Reachability reachabilityForInternetConnection];
            [reachability startNotifier];
            
            NetworkStatus status = [reachability currentReachabilityStatus];
            NSString* result = [NSString stringWithFormat:@""];
            
            if(status == NotReachable)
            {
                //No internet
            }
            else if (status == ReachableViaWiFi)
            {
                //WiFi
                result = [NSString stringWithFormat:@"wifi"];
            }
            else if (status == ReachableViaWWAN) 
            {
                //3G
                result = [NSString stringWithFormat:@"3g"];
            }
            
            return [result UTF8String];
        }
        
        std::string nativeGetOS()
        {
            return [[NSString stringWithFormat:@"iOS"] UTF8String];
        }
        
        std::string nativeGetOSVersion()
        {
            return [[[UIDevice currentDevice] systemVersion] UTF8String];
        }
        
        /**
         * Method name: nativeGetDeviceUDID
         * Description: detect device UDID
         * Parameters:
         * ref :
         // http://stackoverflow.com/questions/20944932/how-to-get-device-udid-in-programatically-in-ios7
         */
        std::string nativeGetDeviceUDID()
        {
            NSString* result = [NSString stringWithFormat:@""];
            
            if( SYSTEM_VERSION_LESS_THAN(@"6.0.0"))
            {
                
            }
            else
            {
                result = [[[UIDevice currentDevice] identifierForVendor] UUIDString]; // IOS 6+
                NSLog(@"UDID:: %@", result);
            }
            
            return [result UTF8String];
        }
        
        
        /**
         * Method name: nativeGenerateAppleVerify
         * Description: generate apple verify
         * Parameters:
         * ref :
         // https://developer.apple.com/reference/gamekit/gklocalplayer
         */
        void nativeGenerateAppleVerify(AppleCallBack callback)
        {
            if ([[GKLocalPlayer localPlayer] isAuthenticated])
            {
                [[GKLocalPlayer localPlayer] generateIdentityVerificationSignatureWithCompletionHandler:^(NSURL *publicKeyUrl, NSData *signature, NSData *salt, uint64_t timestamp, NSError *error) {
                    if(error != nil){
                        //login failed.
                    }else{
                        NSLog(@"signature : %@", [signature base64Encoding]);
                        NSLog(@"salt : %@", [salt base64Encoding]);
                        NSLog(@"PUBLIC_KEY_URL : %@", publicKeyUrl);
                        NSLog(@"signature : %@", [signature base64EncodedStringWithOptions:0]);
                        NSLog(@"salt : %@", [salt base64EncodedStringWithOptions:0]);
                        NSLog(@"timestamp : %lld", timestamp);
                        
                        NSLog(@"Gamecenter login success.");
                        
                        NSDictionary *params = @{@"publicKeyURL": [publicKeyUrl absoluteString],
                                                 @"timestamp": [NSString stringWithFormat:@"%llu", timestamp],
                                                 @"signature": [signature base64EncodedStringWithOptions:0],
                                                 @"salt": [salt base64EncodedStringWithOptions:0],
                                                 @"playerID": [GKLocalPlayer localPlayer].playerID,
                                                 @"bundleID": [[NSBundle mainBundle] bundleIdentifier],
                                                 @"aliasName": [[GKLocalPlayer localPlayer] alias]};
                        
                        NSError *JSONError = nil;
                        NSData *JSONPOSTData = [NSJSONSerialization dataWithJSONObject:params options:NSJSONWritingPrettyPrinted error:&JSONError];
                        
                        NSString *JSONPOSTString = [[NSString alloc] initWithData:JSONPOSTData encoding:NSUTF8StringEncoding];
                        JSONPOSTString = [JSONPOSTString stringByReplacingOccurrencesOfString:@"\n" withString:@""];
                        JSONPOSTString = [JSONPOSTString stringByTrimmingCharactersInSet:[NSCharacterSet newlineCharacterSet]];
                        if(callback)
                        {
                            callback([JSONPOSTString UTF8String]);
                        }
                    }
                }];
            }
        }
        
        std::string nativeGetAdvertisingID()
        {
            if([[ASIdentifierManager sharedManager] isAdvertisingTrackingEnabled])
            {
                return [[[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString] UTF8String];
            }
            
            return "";
        }
        
        std::string nativeGetMacAddress()
        {
            return "";
        }
        
        /**
         * create SHA1 Hash by input string
         *
         * @param string input
         */
        std::string nativeMakeSHA1Hash(std::string input)
        {
            NSString* inputStr = [NSString stringWithUTF8String: input.c_str()];
            NSData *data = [inputStr dataUsingEncoding:NSUTF8StringEncoding];
            uint8_t digest[CC_SHA1_DIGEST_LENGTH];
            
            CC_SHA1(data.bytes, data.length, digest);
            
            NSMutableString *output = [NSMutableString stringWithCapacity:CC_SHA1_DIGEST_LENGTH * 2];
            
            for (int i = 0; i < CC_SHA1_DIGEST_LENGTH; i++)
            {
                [output appendFormat:@"%02x", digest[i]];
            }
            
            return [output UTF8String];
        }
        
        std::string nativeGetClipboard()
        {
            NSString*  str=[UIPasteboard generalPasteboard].string;
            if(str)
                return [str UTF8String];
            else
                return "";
        }
        
        void nativeSetClipboard(std::string text)
        {
            [[UIPasteboard generalPasteboard] setString:[NSString stringWithUTF8String:text.c_str() ]];
        }
        
        void nativeOpenAppEmailWithName(std::string appName)
        {
            NSString *mailString = [NSString stringWithFormat:@"message://"];
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:mailString]];
        }
        
        void nativeOpenEmail(std::string sender, std::string title)
        {
            NSString* email_to = [NSString stringWithUTF8String:sender.c_str()];
            NSString* email_title = [NSString stringWithUTF8String:title.c_str()];
            [[iOSEmail defaultHelper] ShowEmail:email_to withTitle:email_title andContent:nil];
        }
        
        void nativeLoadImageFromLibrary(const std::function<void(const char * buffer, unsigned int leng)> & call_back)
        {
            [[ImageLoader defaultHelper]OnShowLibrary : ^(const char * data , size_t lenght){
                call_back(data , (unsigned int)lenght );
            }];
            
        }
    }

}

#endif


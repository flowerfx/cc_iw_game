//
//  JBroken.h
//
//  Created by Sathish Venkatisan on 12/10/13.
//
#import <Foundation/Foundation.h>

@interface JBroken : NSObject

+(float) firmwareVersion;
+(BOOL) isDeviceJailbroken;
+(BOOL) isAppCracked;
+(BOOL) isAppStoreVersion;

@end
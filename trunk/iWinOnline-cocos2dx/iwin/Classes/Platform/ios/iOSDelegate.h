#ifndef __IOS_DELEGATE_H__
#define __IOS_DELEGATE_H__

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

//begin delegate email
#import <MessageUI/MessageUI.h>
@interface iOSEmail: UIViewController<MFMailComposeViewControllerDelegate>
{
    MFMailComposeViewController *mc ;
}

+(instancetype)defaultHelper;

-(void)ShowEmail:(NSString*)email_to withTitle:(NSString*)title andContent:(NSString*)content;
@end
//end delegate email


//begin delegate image loader

@interface ImageLoader : UIViewController<UIImagePickerControllerDelegate , UINavigationControllerDelegate>
{
    UIImagePickerController * _image_picker ;
    void (^ _call_back)(const char * data , size_t leng);
}

+(instancetype)defaultHelper;

-(void)OnShowLibrary :(void(^)(const char * , size_t)) handler;

@end

//end delegate

#endif // __IOS_DELEGATE_H__


#import "iOSDelegate.h"

@implementation iOSEmail

static iOSEmail *m_instance_email = nil;

+ (iOSEmail*)defaultHelper {
    // dispatch_once will ensure that the method is only called once (thread-safe)
    static dispatch_once_t pred = 0;
    dispatch_once(&pred, ^{
        m_instance_email = [[iOSEmail alloc] init];
    });
    return m_instance_email;
}

- (id)init {
    if ((self = [super init])) {
    }
    return self;
    
}

- (void)ShowEmail:(NSString*)email_to withTitle:(NSString*)title andContent:(NSString*)content; {
    // Email Subject
    NSString *emailTitle = title;
    // Email Content
    NSString *messageBody = content;
    // To address
    NSArray *toRecipents = [NSArray arrayWithObject:email_to];
    
    
    mc = [[MFMailComposeViewController alloc] init];
    mc.mailComposeDelegate = self;
    [mc setSubject:emailTitle];
    [mc setMessageBody:messageBody isHTML:NO];
    [mc setToRecipients:toRecipents];
    
    // Present mail view controller on screen
    [self presentViewController:mc animated:YES completion:nil];
    
    self.view.hidden = NO;
    
    //add to gl view
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    [window addSubview:self.view];
    
}

- (void) mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError *)error
{
    switch (result)
    {
        case MFMailComposeResultCancelled:
            NSLog(@"Mail cancelled");
            break;
        case MFMailComposeResultSaved:
            NSLog(@"Mail saved");
            break;
        case MFMailComposeResultSent:
            NSLog(@"Mail sent");
            break;
        case MFMailComposeResultFailed:
            NSLog(@"Mail sent failure: %@", [error localizedDescription]);
            break;
        default:
            break;
    }
    
    // Close the Mail Interface
    [self dismissViewControllerAnimated:YES completion:nil];
    [self.view removeFromSuperview];
    self.view.hidden = YES;
    mc.delegate = nil;
    [mc release];
    mc = nil;
}

- (void)dealloc {
    
    [super dealloc];
}

@end //end implement ios email


@implementation ImageLoader

static ImageLoader *m_instance_image = nil;

+ (ImageLoader*)defaultHelper {
    // dispatch_once will ensure that the method is only called once (thread-safe)
    static dispatch_once_t pred = 0;
    dispatch_once(&pred, ^{
        m_instance_image = [[ImageLoader alloc] init];
    });
    return m_instance_image;
}

- (id)init {
    if ((self = [super init])) {
      
        
    }
    return self;
    
}

-(void) imagePickerController:(UIImagePickerController *)picker didFinishPickingImage:(UIImage *)image editingInfo:(NSDictionary *)editingInfo
{
    
    NSData * imgData = UIImagePNGRepresentation(image);
    
    [picker dismissViewControllerAnimated:YES completion:nil];
    
    [self.view removeFromSuperview];
    self.view.hidden = YES;
    _image_picker.delegate = nil;
    _image_picker = nil;
    _call_back((const char *)[imgData bytes] , (size_t)[imgData length]);
}

-(void) imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [self.view removeFromSuperview];
    self.view.hidden = YES;
    _image_picker.delegate = nil;
    _image_picker = nil;
}

-(void)OnShowLibrary : (void(^)(const char * , size_t)) handler
{
    _image_picker = [[UIImagePickerController alloc]init];
    _image_picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary ;
    _image_picker.delegate = self;
    
    _call_back = [handler copy];
    
    self.view.hidden = NO;
    //add to gl view
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    [window addSubview:self.view];
    
    [self presentModalViewController:_image_picker animated:TRUE];
}

- (BOOL)shouldAutorotate;
{
    return NO;
}

-(NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskPortrait;
}

- (void)dealloc {
    
    [super dealloc];
}

@end //end implement ios email


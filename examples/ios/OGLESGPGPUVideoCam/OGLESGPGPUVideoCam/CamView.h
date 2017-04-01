//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// Author: Markus Konrad <post@mkonrad.net>, Winter 2014/2015
// http://www.mkonrad.net
//
// See LICENSE file in project repository root for the license.
//

#import <UIKit/UIKit.h>

@class AVCaptureSession;

@interface CamView : UIView

@property (nonatomic, assign) AVCaptureSession *session;

@end

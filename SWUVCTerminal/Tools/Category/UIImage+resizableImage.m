//
//  UIImage+resizableImage.m
//  SWUVCTerminal
//
//  Created by hxf on 9/6/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "UIImage+resizableImage.h"

@implementation UIImage (resizableImage)
+(UIImage *)resizableImageNamed:(NSString *)name
{
    UIImage *image = [UIImage imageNamed:name];
    
    CGFloat w = image.size.width;
    CGFloat h = image.size.height;
    return [image resizableImageWithCapInsets:UIEdgeInsetsMake(0.5*h, 0.5*w,0.5*h, 0.5*w)];
}
@end

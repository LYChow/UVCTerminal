//
//  SWImagesPreview.h
//  SWUVCTerminal
//
//  Created by hxf on 9/14/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SWImage.h"

@interface SWImagesPreview : UICollectionViewController

-(void)disSelectedCollectionCell:(SWImage *)image withImageList:(NSArray *)images;

@end

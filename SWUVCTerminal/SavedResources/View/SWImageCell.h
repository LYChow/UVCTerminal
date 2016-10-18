//
//  SWImageCell.h
//  SWUVCTerminal
//
//  Created by hxf on 8/17/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import <UIKit/UIKit.h>
@class SWImageCell;
#import "SWImage.h"

@protocol SWImageCellDelegate <NSObject>

@optional
- (void)imageCellCheckingStateDidChange:(SWImageCell *)cell;

@end

@interface SWImageCell : UICollectionViewCell
@property (nonatomic, strong) SWImage *image;
@property (nonatomic, weak) id<SWImageCellDelegate> delegate;
@end

//
//  SWImageCollectionCell.m
//  SWUVCTerminal
//
//  Created by hxf on 9/18/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWImagePreviewCollectionCell.h"
@interface SWImagePreviewCollectionCell()

@property (weak, nonatomic) IBOutlet UIImageView *previewImage;

@end

@implementation SWImagePreviewCollectionCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

#pragma mark -setter 
-(void)setImageModel:(SWImage *)imageModel
{
    _imageModel=imageModel;
    UIImage *image =[UIImage imageWithContentsOfFile:SWGetAbsolutelyImagePathWithFileName(imageModel.name)];

    [self.previewImage setImage:image];
    self.previewImage.contentMode=UIViewContentModeScaleAspectFit;
}

@end

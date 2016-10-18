//
//  SWImageCell.m
//  SWUVCTerminal
//
//  Created by hxf on 8/17/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWImageCell.h"
#import "SWFileManager.h"

@interface SWImageCell()
@property (weak, nonatomic) IBOutlet UIImageView *imageView;
@property (weak, nonatomic) IBOutlet UIButton *checkingButton;
- (IBAction)changeCheckingStatus:(UIButton *)sender;

@end

@implementation SWImageCell
-(void)setImage:(SWImage *)imageModel
{
    _image=imageModel;
    NSString *filePath = SWGetAbsolutelyImagePathWithFileName(imageModel.name);
    UIImage *image = [UIImage imageWithContentsOfFile:filePath];
    self.imageView.image=image;
    
    self.checkingButton.hidden=!imageModel.isEditing;
    self.checkingButton.selected=imageModel.isChecking;
}
- (IBAction)changeCheckingStatus:(UIButton *)sender
{
    sender.selected=!sender.selected;
    self.checkingButton.selected=sender.selected;
    self.image.checking=self.checkingButton.selected;
    
    if (self.delegate && [self.delegate respondsToSelector:@selector(imageCellCheckingStateDidChange:)]) {
        [self.delegate imageCellCheckingStateDidChange:self];
    }
}
@end

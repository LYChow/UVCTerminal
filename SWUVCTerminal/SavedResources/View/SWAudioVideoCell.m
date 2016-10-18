//
//  SWVideoCell.m
//  SWUVCTerminal
//
//  Created by hxf on 8/17/16.
//  Copyright © 2016 sinowave. All rights reserved.
//

#import "SWAudioVideoCell.h"
@interface SWAudioVideoCell()
@property (weak, nonatomic) IBOutlet UIImageView *iconImageView;
@property (weak, nonatomic) IBOutlet UILabel *nameLabel;
@property (weak, nonatomic) IBOutlet UILabel *durationLabel;
@property (weak, nonatomic) IBOutlet UILabel *sizeLabel;

@property (weak, nonatomic) IBOutlet UIButton *checkingButton;
- (IBAction)changeSelectedStatus:(UIButton *)sender;

@end

@implementation SWAudioVideoCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (IBAction)changeSelectedStatus:(UIButton *)sender
{
    sender.selected=!sender.selected;
    if (self.audio)
    {
        self.audio.checking=!self.audio.checking;
    }
    if (self.video)
    {
        self.video.checking=!self.video.checking;
    }
    
    if (self.delegate && [self.delegate respondsToSelector:@selector(avCellCheckingStateDidChange:)]) {
        [self.delegate avCellCheckingStateDidChange:self];
    }
}

-(void)setAudio:(SWAudio *)audio
{
    _audio=audio;
    self.iconImageView.image=audio.coverPictureImage;
    self.nameLabel.text = audio.name;
    self.durationLabel.text = audio.duration;
    self.sizeLabel.text = audio.fileSize;
    self.checkingButton.hidden=!audio.isEditing;
    self.checkingButton.selected=audio.isChecking;
}

-(void)setVideo:(SWVideo *)video
{
    _video = video;
    self.iconImageView.image=video.coverPictureImage;
    self.nameLabel.text = video.name;
    self.durationLabel.text = video.duration;
    self.sizeLabel.text = video.fileSize;
    self.checkingButton.hidden=!video.isEditing;
    self.checkingButton.selected=video.isChecking;
}

@end

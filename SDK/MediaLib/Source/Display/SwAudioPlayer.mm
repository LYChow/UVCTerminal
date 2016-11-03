//
//  SwAudioPlayer.mm
//  SwIMediaLib
//
//  Created by 李招华 on 16/3/9.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#include "SwAudioPlayer.h"
#import "PlayAudio.h"

#pragma mark - public functions
CSwAudioPlayer::CSwAudioPlayer()
{
    playAudio* _playAudio = [[playAudio alloc] init];
    m_playAudio = ((__bridge_retained void*)_playAudio);
}

CSwAudioPlayer::~CSwAudioPlayer()
{
    playAudio* _playAudio = (__bridge_transfer playAudio*)m_playAudio;
    if (_playAudio) {
        _playAudio = nil;
    }
}

int CSwAudioPlayer::PutData(const char* data, int len, int flags)
{
    int ir = -1;

    if (!data || !m_playAudio) {
        return ir;
    }

    playAudio* _playAudio = (__bridge playAudio*)m_playAudio;
    if (!_playAudio) {
        return ir;
    }

    [_playAudio putData:(void*)data pcmDataLength:len];

    return ir;
}

#pragma mark - protected functions
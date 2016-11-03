//
//  SwAudioPlayer.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/3/9.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef SwAudioPlayer_h
#define SwAudioPlayer_h

class CSwAudioPlayer
{
#pragma mark - public functions
public:
    CSwAudioPlayer();
    virtual ~CSwAudioPlayer();

public:
    int PutData(const char* data, int len, int flags);

#pragma mark - protected functions
protected:

#pragma mark - protected members
protected:
    void* m_playAudio;
};

#endif /* SwAudioPlayer_h */

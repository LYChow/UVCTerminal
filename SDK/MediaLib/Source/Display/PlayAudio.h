//
//  PlayAudio.h
//  SwIMediaLib
//
//  Created by 李招华 on 16/2/23.
//  Copyright © 2016年 华纬讯. All rights reserved.
//

#ifndef PlayAudio_h
#define PlayAudio_h

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AudioToolbox/AudioFile.h>

#define NUM_BUFFERS 3
#define NUM_INBUF 10


@interface playAudio : NSObject {
    //音频流描述对象
    AudioStreamBasicDescription dataFormat;

    //音频队列
    AudioQueueRef queue;
    AudioQueueBufferRef buffers[NUM_BUFFERS];

	SInt64 packetIndex;
    UInt32 numPacketsToRead;
  //  UInt32 bufferByteSize;
	UInt32 BufferSize;

	char* pDataBuffer;
	UInt32 iBufHead;
	UInt32 iBufTail;
}

//定义队列为实例属性
@property AudioQueueRef queue;

//播放方法定义
-(id)init;

//定义缓存数据读取方法
-(void) audioQueueOutputWithQueue:(AudioQueueRef)audioQueue
                      queueBuffer:(AudioQueueBufferRef)audioQueueBuffer;

//数据推入接口
-(void)putData:(void*)pcm8k16bit pcmDataLength:(int)len;

@end

#endif /* PlayAudio_h */

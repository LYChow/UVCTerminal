//
//  PlayAudio.m
//  SwIMediaLib
//
//  Created by 李招华 on 16/2/23.
//  Copyright © 2016年 华纬讯. All rights reserved.
//
//#import "SwBaseWin.h"
#import "PlayAudio.h"
#define TRACET printf

#define AVCODEC_MAX_AUDIO_FRAME_SIZE  4096*2// (0x10000)/4
//static UInt32 gBufferSizeBytes=0x10000;//65536
static UInt32 gBufferSizeBytes=0x10000;//It must be pow(2,x)

@implementation playAudio

@synthesize queue;


//回调函数(Callback)的实现
static void BufferCallback(void *inUserData,AudioQueueRef inAQ,
                           AudioQueueBufferRef buffer){
    playAudio* player=(__bridge playAudio*)inUserData;

   [player audioQueueOutputWithQueue:inAQ queueBuffer:buffer];
}

//缓存数据读取方法的实现
-(void) audioQueueOutputWithQueue:(AudioQueueRef)audioQueue queueBuffer:(AudioQueueBufferRef)audioQueueBuffer{
    //NSLog(@"audioQueueOutputWithQueue");

	char* pData=pDataBuffer+iBufHead*(BufferSize);
	UInt32 len=BufferSize;//*((UInt32*)pData);
	//	pData+=4;

	int nPack=(iBufTail-iBufHead);
	if(nPack<0) nPack+=NUM_INBUF;
	TRACET("AudioPlay_FillBuffer:nPack=%d,head=%d,tail=%d\n",nPack,iBufHead,iBufTail);

	if(nPack>=NUM_BUFFERS)
	{
		memcpy(audioQueueBuffer->mAudioData, pData, BufferSize);
		if(iBufHead==(NUM_INBUF-1)) iBufHead=0;
		else iBufHead++;
	}
	//播静音数据
	else
	{
		memset(audioQueueBuffer->mAudioData,0xff,BufferSize);
	}




	audioQueueBuffer->mAudioDataByteSize = BufferSize;
	audioQueueBuffer->mPacketDescriptionCount=1;
	int ret = AudioQueueEnqueueBuffer(audioQueue, audioQueueBuffer, 0, nil); // kAudioQueueErr_BufferEmpty


  //  AudioQueueFreeBuffer(queue, audioQueueBuffer);
//    //读取包数据
//    UInt32 numBytes;
//    //    UInt32 numPackets=numPacketsToRead;
//    UInt32 numPackets=numPacketsToRead;
//
//    //成功读取时
//    numBytes=fread(inbuf, 1, numPackets*4,wavFile);
//    AudioQueueBufferRef outBufferRef=audioQueueBuffer;
//    NSData *aData=[[NSData alloc]initWithBytes:inbuf length:numBytes];
//
//    if(numBytes>0){
//        memcpy(outBufferRef->mAudioData, aData.bytes, aData.length);
//
//        outBufferRef->mAudioDataByteSize=numBytes;
//        AudioQueueEnqueueBuffer(audioQueue, outBufferRef, 0, nil);
//    }
//    else{
//    }
}


//数据推入接口
-(void)putData:(void*)pcm8k16bit pcmDataLength:(int)len {
	if (!pcm8k16bit || len<1) {
		return;
	}
	if(len>BufferSize)
	{
		printf("PlayAudio_putData:ERROR,data size=%d beyond buffer size=%d\n",len,BufferSize);
		return;
	}
	int nPack=(iBufTail-iBufHead);
	if(nPack<0) nPack+=NUM_INBUF;
	TRACET("AudioPlay_putData:nPack=%d,head=%d,tail=%d\n",nPack,iBufHead,iBufTail);

	char* pData=pDataBuffer+iBufTail*(BufferSize);

	if(iBufTail==(NUM_INBUF-1)) iBufTail=0;
	else iBufTail++;

	if (iBufTail==iBufHead) {
		if(iBufHead==(NUM_INBUF-1)) iBufHead=0;
		else iBufHead++;

	}

//	*((UInt32*)pData)=len;

	memcpy(pData,pcm8k16bit,len);

#if 0
	AudioQueueBufferRef buf;
	OSStatus ret = noErr;
	ret = AudioQueueAllocateBuffer(queue, len, &buf);
	memcpy(buf->mAudioData, pcm8k16bit, len);
	buf->mAudioDataByteSize = len;
	ret = AudioQueueEnqueueBuffer(queue, buf, 0, nil); // kAudioQueueErr_BufferEmpty
#endif
}

- (int)CalcBufferSize:(const AudioStreamBasicDescription *)format :(float) seconds
{
	int packets, frames, bytes = 0;
	@try {
		frames = (int)ceil(seconds * format->mSampleRate);

		if (format->mBytesPerFrame > 0)
			bytes = frames * format->mBytesPerFrame;
		else {
			UInt32 maxPacketSize;
			if (format->mBytesPerPacket > 0)
				maxPacketSize = format->mBytesPerPacket;	// constant packet size
			else {
				UInt32 propertySize = sizeof(maxPacketSize);
				NSAssert(AudioQueueGetProperty(queue, kAudioQueueProperty_MaximumOutputPacketSize, &maxPacketSize,
											   &propertySize), @"couldn't get queue's maximum output packet size");
			}
			if (format->mFramesPerPacket > 0)
				packets = frames / format->mFramesPerPacket;
			else
				packets = frames;	// worst-case scenario: 1 frame in a packet
			if (packets == 0)		// sanity check
				packets = 1;
			bytes = packets * maxPacketSize;
		}
	} @catch (NSException *e) {

		NSLog(@"%@", e);
		return 0;
	}
	return bytes;
}


//音频播放方法的实现
-(id) init {
    if (!(self=[super init])) {
        return nil;
    }

    //音频数据格式
    {
        dataFormat.mSampleRate = 8000;//采样频率
        dataFormat.mFormatID = kAudioFormatLinearPCM;
        dataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger|kLinearPCMFormatFlagIsPacked;
        dataFormat.mBytesPerPacket = 2;
        dataFormat.mFramesPerPacket = 1;
        dataFormat.mBytesPerFrame = 2;
        dataFormat.mChannelsPerFrame = 1;//通道数
        dataFormat.mBitsPerChannel = 16;//采样的位数
        dataFormat.mReserved = 0;
    }

    OSStatus ret = noErr;

    //创建播放用的音频队列
    ret = AudioQueueNewOutput(&dataFormat, BufferCallback, (__bridge void*)(self), CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 0, &queue);



	//创建并分配缓冲空间
	packetIndex=0;
	BufferSize=[self CalcBufferSize:&dataFormat :.04];
	for (int i=0; i< NUM_BUFFERS; i++) {
		AudioQueueAllocateBuffer(queue, BufferSize, &buffers[i]);
		memset(buffers[i]->mAudioData,0xff,BufferSize);
		buffers[i]->mAudioDataByteSize=BufferSize;
		AudioQueueEnqueueBuffer(queue, buffers[i], 0, NULL);
	}
	pDataBuffer=new char[NUM_INBUF*(BufferSize)];
	memset(pDataBuffer,0,NUM_INBUF*(BufferSize));
	iBufHead=0;
	iBufTail=0;


	Float32 gain=1.0;
	//设置音量
	ret = AudioQueueSetParameter(queue, kAudioQueueParam_Volume, gain);


    //队列处理开始，此后系统开始自动调用回调(Callback)函数
    ret = AudioQueueStart(queue, nil);

    return self;
}

- (void)dealloc
{
	AudioQueueStop(queue, true);
	AudioQueueDispose(queue, true);
	delete pDataBuffer;
	pDataBuffer=0;
}

@end
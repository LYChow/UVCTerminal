#ifndef _CAPTION_DEFINE_
#define _CAPTION_DEFINE_

#include "SWDef.h"

#pragma pack(push ,1)

const int FRAMECAPTIONLENGTHMAX = 63;

//字幕扩展结构
typedef struct tagFrameCaptionHeaderEx
{
	unsigned char Used:				1; //此结构是否启用 0 - 不启用，1 - 启用
	unsigned char VerticalPercent:	3; //垂直位置 0 - 底部, 7 - 顶部
	unsigned char FontType:			2; //字体大小 0 - 大字体  与标题大小一致
	unsigned char Background:		1; //背景色 0 - 不启用, 1 - 启用
	unsigned char Reserved:			1; //预留 0 - 无扩展结构 1 - 有扩展结构 结构追加至字幕内容之后
	tagFrameCaptionHeaderEx() { memset(this, 0x00, sizeof(*this)); }
}FrameCaptionHeaderEx, *pFrameCaptionHeaderEx;

typedef enum tagFrameCaptionHeaderEx2Flag
{
	FrameCaptionHeaderEx2Flag_NULL = 0x0000,
	FrameCaptionHeaderEx2Flag_AddCaption = 0x0001,
}FrameCaptionHeaderEx2Flag;

//字幕扩展结构
typedef struct tagFrameCaptionHeaderEx2
{
	WORD wSize;							//结构大小
	DWORD dwBackColor;					//背景色
	DWORD dwCaptionColor;				//字幕字体颜色
	DWORD dwViewWidth;					//字幕发送端视口显示的字符宽度（百分数）
	DWORD dwFlag;						//标志位 0x01 - 视频中已叠加字幕信息 不需要再叠加
	tagFrameCaptionHeaderEx2() 
	{ wSize = sizeof(*this); dwBackColor = SWRGB(255, 0, 0); dwCaptionColor = SWRGB(255, 255, 255); dwViewWidth = 0; }
	int GetFirstHeaderSize(void) { return sizeof(wSize) + sizeof(dwBackColor) + sizeof(dwCaptionColor); }
	int GetSecondHeaderSize(void) { return sizeof(wSize) + sizeof(dwBackColor) + sizeof(dwCaptionColor) + sizeof(dwViewWidth); }
	void Reset(void) { memset(this, 0x00, sizeof(*this)); }
	void Copy(const tagFrameCaptionHeaderEx2 *pHeaderEx2) 
	{	
		Reset();
		if (pHeaderEx2 == NULL || pHeaderEx2->wSize == 0 || pHeaderEx2->wSize >1024)
		{
			return;
		}
		if (pHeaderEx2->wSize >= sizeof(*this))
		{
			*this = *pHeaderEx2;
		}
		else
		{
			memcpy(this, pHeaderEx2, pHeaderEx2->wSize);
		}
	};
}FrameCaptionHeaderEx2, *pFrameCaptionHeaderEx2;

struct FRAMECAPTIONHEADER
{
	BYTE nType;	//字幕类型：0 - 固定字幕；1 - 滚动字幕
	FrameCaptionHeaderEx HeaderEx;
	int nOffsetPercentage;	//字模显示偏移量，单位为单个字符宽度
	void Reset(){ memset(this,0,sizeof(*this));}
	void ResetEx(){ nType = 0x00; nOffsetPercentage = 0; }
	FRAMECAPTIONHEADER(){Reset();}
};

struct ONE_FRAMECAPTION_INFO
{
	BYTE nAlign;			//对齐类型
	BYTE nFontSize;			//字体类型
	FrameCaptionHeaderEx HeaderEx;
	int nOffsetPercentage;	//字模显示偏移量，单位为单个字符宽度
	char szBuffer[512];		//字幕文字
	DWORD dwBackColor;		//背景色
	DWORD dwCaptionColor;	//字幕字体颜色
	DWORD dwViewWidth;
	FRAMECAPTIONHEADER CaptionHeader;
	FrameCaptionHeaderEx2 HeaderEx2;
	void Reset(){ memset(this,0,sizeof(*this));}
	ONE_FRAMECAPTION_INFO(){Reset();}
};

struct FRAMECAPTIONSCROLLINFO
{
	BOOL bScroll;	//是否滚动
	int nRepeatTimes;	//重复播放次数
	int nScrollSpeed;	//滚动字幕的滚动速度，100表示每秒一个字符宽度
	int nCharIndex;		//头字符索引
	int nCharPercentage;	//字符显示进度：0表示刚开始，100表示显示完整字符，200表示显示完整汉字
	int nLength;	//字符串长度
	int nViewWidth; //发送者的尺寸与字幕关联长度
	char szBuffer[65536];	//字幕文字缓存，用于滚动字幕
	void Reset(){ memset(this,0,szBuffer-(char*)this+2);}
	FRAMECAPTIONSCROLLINFO(){Reset();}
};

typedef struct tagSWVC3ExtendTile
{
	char szTitle[32];
	char szCaption[64];
	int nAligns;
	int nFontSize;
	int nPercentOfLeftCharOffset;
	BOOL bBackground;
	DWORD dwBackColor;		//背景色
	DWORD dwCaptionColor;	//字幕字体颜色
	DWORD dwViewWidth;
	FRAMECAPTIONHEADER CaptionHeader;
	FrameCaptionHeaderEx2 HeaderEx2;
	FrameCaptionHeaderEx CaptionHeaderEx;
	char Reserved[3];
	tagSWVC3ExtendTile() { memset(this, 0x00, sizeof(*this)); }
}SWVC3ExtendTile;

//void HandleFrameCaptionScroll(int nVideoWidth, int nVideoHeight);

#pragma pack(pop)

#endif /*_CAPTION_DEFINE_*/
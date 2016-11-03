
typedef struct _DEVInfo{
	char PCName[32];//远程机器名称
	unsigned char CodecType;//编解码器型号：0 - 内置；1-n其它型号
	unsigned char Videoer;//=1视频选择器配置,=0视频选择器未配置（最多只有一个镜头）
	unsigned char DEVNum;//设备（镜头）个数
	unsigned char CurDevNo;//当前选择设备（镜头）号(从0开始,=0xff表示初始化切换器
	//为分割画面，初始化后置为0)，用于云台地址选择与切换器选择的捆绑
	union
	{
		char DevName[16][32];//设备名字
		struct 
		{
			char DevName[20]; 
			char YTFlagEx[4]; 
			char reserved[8];
		} DevInfoEx[16];	//云台扩展信息，其中成员DevName与上一级DevName共用相同内存区域
	};
	unsigned char YTFlag[16];//云台配置标志:=1有；=0无
	unsigned char YTAddr[16];//云台地址
	
	unsigned char FirtIntoFlag;//初始化状态，第一次进入=0；以后=0xff;
	char yuntai[64];//send buffer
}DEVInfo,*PDEVInfo;

typedef struct _WaveInfo
{
	int  nCodecType;	//编码器类型
	char WaveBuf[1024]; //音频数据缓冲区
	int  nLength;		//数据长度
	char Reserved[32];	//保留
}WAVEINFO,*PWAVEINFO;


typedef struct _CodecDevInfo
{
	char UserName[32];	//登录名称
	char Password[32];	//密码
	char IPAddr[32];	//设备地址
	char EncoderName[32]; //设备名称
	int nChannel;		//通道号

}CODECDEVINFO,*PCODECDEVINFO;

typedef struct _MultiDecIdel
{
	int nLastDecNo;
	int nIdelTick;
	int bFastLogol;
}MULTIDECIDEL;

typedef struct _BitRateInfo {
	WORD BitrateVideoEnc;	//2B，视频码流（编码）比特率，kbps为单位
	WORD BitrateAudioEnc;	//2B，音频码流（编码）比特率，kbps为单位
	WORD BitrateVideoDec;	//2B，视频码流（接收）比特率，kbps为单位
	WORD BitrateAudioDec;	//2B，音频码流（接收）比特率，kbps为单位
}BITRATEINFO,*PBITRATEINFO;	//终端节点的收发带宽属性

typedef struct _CodecCapVideoFormat {
	bool bSupportAutoAdapt : 1;		//1bit，是否自动适应编码格式（用于解码端）
	bool bSupportH264_VC3 : 1;		//1bit，是否支持H264 (VC3封装)
	bool bSupportXVID_VC3 : 1;		//1bit，是否支持XVID (VC3封装)
	bool bSupportH264_Others : 1;	//1bit，是否支持H264 (非VC3封装)
	bool bSupportXVID_Others : 1;	//1bit，是否支持XVID (非VC3封装)
	bool bSupportMJPEG_Others : 1;	//1bit，是否支持MJPEG (非VC3封装)
	bool Reserved : 2;				//2bit，保留，必须置0
	bool Reserved2 : 8;				//8bit，保留，必须置0
}CODECAPVIDEOFORMAT,*PCODECAPVIDEOFORMAT;	//（2BYTE）视频编解码格式能力描述，各bit全0表示未描述

typedef struct _CodecCapVideoSize {
	bool bSupportAutoAdapt : 1;	//1bit，是否自动适应编码图像尺寸（用于解码端）
	bool bSupport128X96 : 1;	//1bit，是否支持128*96
	bool bSupport160X120 : 1;	//1bit，是否支持160*120
	bool bSupport176X128 : 1;	//1bit，是否支持176*128
	bool bSupport176X144 : 1;	//1bit，是否支持176*144
	bool bSupport256X144 : 1;	//1bit，是否支持256*144
	bool bSupport320X240 : 1;	//1bit，是否支持320*240
	bool bSupport352X240 : 1;	//1bit，是否支持352*240
	bool bSupport352X288 : 1;	//1bit，是否支持352*288
	bool bSupport352X576 : 1;	//1bit，是否支持352*576
	bool bSupport528X384 : 1;	//1bit，是否支持528*384
	bool bSupport640X480 : 1;	//1bit，是否支持640*480
	bool bSupport704X288 : 1;	//1bit，是否支持704*288
	bool bSupport704X480 : 1;	//1bit，是否支持704*480
	bool bSupport704X576 : 1;	//1bit，是否支持704*576
	bool bSupport720X288 : 1;	//1bit，是否支持720*288
	bool bSupport720X480 : 1;	//1bit，是否支持720*480
	bool bSupport720X576 : 1;	//1bit，是否支持720*576
	bool bSupport768X448 : 1;	//1bit，是否支持768*448
	bool bSupport800X600 : 1;	//1bit，是否支持800*600
	bool bSupport848X480 : 1;	//1bit，是否支持848*480
	bool bSupport1024X768 : 1;	//1bit，是否支持1024*768
	bool bSupport1280X720 : 1;	//1bit，是否支持1280*720
	bool bSupport1280X960 : 1;	//1bit，是否支持1280*960
	bool bSupport1280X1024 : 1;	//1bit，是否支持1280*1024
	bool bSupport1440X1080 : 1;	//1bit，是否支持1440*1080
	bool bSupport1600X1200 : 1;	//1bit，是否支持1600*1200
	bool bSupport1920X1080 : 1;	//1bit，是否支持1920*1080
	bool Reserved : 4;			//4bit，保留，必须置0
}CODECAPVIDEOSIZE,*PCODECAPVIDEOSIZE;	//（4BYTE）视频编解码图像尺寸能力描述，各bit全0表示未描述

typedef struct _CodecCapAudioFormat {
	bool bSupportAutoAdapt : 1;		//1bit，是否自动适应编码格式（用于解码端）
	bool bSupportPCM8k8b : 1;		//1bit，是否支持PCM8k8b
	bool bSupportPCM8k16b : 1;		//1bit，是否支持PCM8k16b 
	bool bSupportPCM16k16b : 1;		//1bit，是否支持PCM16k16b
	bool bSupportPCM32k16b : 1;		//1bit，是否支持PCM32k16b
	bool bSupportPCM44k16b : 1;		//1bit，是否支持PCM44k16b
	bool bSupportPCM48k16b : 1;		//1bit，是否支持PCM48k16b
	bool bSupportADPCM8k8b : 1;		//1bit，是否支持ADPCM8k8b
	bool bSupportADPCM8k16b : 1;	//1bit，是否支持ADPCM8k16b 
	bool bSupportADPCM16k16b : 1;	//1bit，是否支持ADPCM16k16b
	bool bSupportADPCM32k16b : 1;	//1bit，是否支持ADPCM32k16b
	bool bSupportADPCM44k16b : 1;	//1bit，是否支持ADPCM44k16b
	bool bSupportADPCM48k16b : 1;	//1bit，是否支持ADPCM48k16b
	bool bSupportAMR8k16b : 1;		//1bit，是否支持AMR
	bool bSupportG711A : 1;			//1bit，是否支持G711A
	bool bSupportG711U : 1;			//1bit，是否支持G711U
	bool bSupportG722 : 1;			//1bit，是否支持G722 
	bool bSupportG7231 : 1;			//1bit，是否支持G7231
	bool bSupportG726 : 1;			//1bit，是否支持G726 
	bool bSupportG729 : 1;			//1bit，是否支持G729 
	bool bSupportMP3 : 1;			//1bit，是否支持MP3
	bool bSupportAC3 : 1;			//1bit，是否支持AC3
	bool bSupportOGG : 1;			//1bit，是否支持OGG
	bool Reserved : 1;				//1bit，保留，必须置0
	bool Reserved2 : 8;				//8bit，保留，必须置0
}CODECAPAUDIOFORMAT,*PCODECAPAUDIOFORMAT;	//音频编解码格式能力描述，各bit全0表示未描述

typedef struct _CodecCapInfo {
	CODECAPVIDEOFORMAT	EncCapVideoFormat;	//2B，视频编码格式能力
	CODECAPVIDEOFORMAT	DecCapVideoFormat;	//2B，视频解码格式能力
	CODECAPVIDEOSIZE	EncCapVideoSize;	//4B，视频编码尺寸能力
	CODECAPVIDEOSIZE	DecCapVideoSize;	//4B，视频解码尺寸能力
	CODECAPAUDIOFORMAT	EncCapAudioFormat;	//4B，音频编码格式能力
	CODECAPAUDIOFORMAT	DecCapAudioFormat;	//4B，视频解码格式能力
	_CodecCapInfo() {ASSERT(sizeof(*this)==20);}
}CODECAPINFO,*PCODECAPINFO;	//（18BYTES）终端节点的编解码能力属性

typedef struct _NodeStatusEx {
	DWORD dwProductID;	//产品标识
	DWORD dwVersion;	//版本号
	int nShowOrderNum;	//显示顺序号，表示在终端上的列表显示位置
	BYTE TermSubType;	//设备子类型，目前设备类型TermType=2时具有子类型：0-指挥终端；1-会议终端；2-旁听会议终端（可参会的浏览终端）
	double PosX;		//坐标
	double PosY;		//坐标
	BYTE NetCondition;	//网络状况：0-默认；1-很好；2-很差
	union
	{
		BYTE DisableP2P;	//媒体流是否禁用P2P点对点传输（即强制转发或组播）：0-允许点对点；1-禁止点对点
		BYTE ForceUseVS;	//媒体流是否强制转发或组播（对于发送设备，强制发送给VS或组播地址；对于接收设备，强制从VS或组播地址接收）：0-不强制转发或组播；1-强制转发或组播
	};
	union
	{
		BYTE Flags;	//复合标志字节
		struct
		{
			bool bSupportSystemScan:1;				//是否支持系统巡检指令CMD_SYSTEMSCAN_REQUEST和CMD_SYSTEMSCAN_RESPONSE
			bool bSupportInactiveOnlineState:1;		//是否支持终端的InactiveOnline状态。终端的InactiveOnline状态是指在Online（注册在线）状态的基础上进一步细分的状态，表示设备是否活跃可用(true=不活跃)。
													//已注册在线但不活跃可用的状态通常表示可以通过某种手段使其变得活跃可用（如拨号等）
			bool bInactiveOnlineInitially:1;		//终端Node注册时，初始的状态是否为InactiveOnline状态。通常的终端该值应该为false，表示一上线即可用，只有特殊终端节点（如可视电话网关坐席、需要拨号才能上线的编码器）才需要设为true
			bool bSupportActivedByCmd301:1;			//终端是否可被新激活协议CMD301来激活（通过拨号等）
			bool bExcludeWhenSenderClearAV:1;		//是否当发送者使用CMD_CLEARALLSENDFROMME清除音视频时不包含本终端
			bool bSupportNewClearAVMode:1;			//当发送者使用CMD_CLEARALLSENDFROMME清除音视频时是否支持新的处理模式。0-老模式, 直接撤消转发关系。 1-新模式, 向终端发送通知(CMD_REMOVESENDER)由终端决定如何处理
			bool Reserved:2;	//保留bitsts
		};
	};
}NODEINFOEX,*PNODEINFOEX;	//扩展的终端属性

typedef struct _NodeStatusEx2 {
	DWORD dwAppVersion;	//应用层版本号
	DWORD dwHostIDCode;	//终端数值型的ID，该数值型ID在跨局范围内全系统唯一
	BYTE Reserved[104];	//保留
}NODEINFOEX2,*PNODEINFOEX2;	//扩展的终端属性2

typedef struct _NodeStatus {
	char MultiAddress[32];//32B //Multicast address
	char PointAddress[32];//32B //Point2Point address
	char PointAddressAudio[32];//32B //Point2Point address

	char HostName[32];	//32B //Node names	
	char Status[16];	//16B  //用于兼容老版本。
	NODEINFOEX2 NodeInfoEx2;	//112B，扩展的终端属性
	BYTE TermType;		//1B    //confTerm/ControlTerm/DisplayTerm

	char Audio[16];	//16B  //"0","1" ,"2","R" or "-", 分别表示主播、次播、接收、不收不送	
	char Video[16];	//16B  //"P" ,"M","R" or "-", 分别表示点播、广播、接收、不收不送
	char IPAddress[32];	//32B //NIC IP Address.
	union {
		char SubMask[32];	//32B 
		char SubNet[32];
	};
	char Message[32];	//32B
	char MessageEx[64];	//64B
	NODEINFOEX NodeInfoEx;	//32B，扩展的终端属性
	char DeviceName[32];	//32B，硬件终端设备名，此处与HostName相区分，HostName更多代表用户名
	BITRATEINFO BitrateInfo;	//8B，比特率信息，kbps为单位
	char PhoneNumber[20];	//20B，电话号码
	char DataStream[16];	//16B  //"P" ,"M","R" or "-", 分别表示点播、广播、接收、不收不送
	char PointAddressDataStream[32];//32B //Point2Point address
	CODECAPINFO CodecCapInfo;	//20B，编解码能力信息

	DEVInfo	DEV;		//581B
	//BYTE Reserved[64];	//64B
	char yuntai[64];	//64B  //发送数据 
	BYTE test_zero;		//0B 2002.10.05 added to test overflow.
	//follwoing item are used by internal.
	DWORD IsCall;		//4B  //internal status
	char IsCmd;			//1B  //internal status
	char Chairman;		//1B  //conference chairman
	BYTE timer;			//1B
	bool IsInConf;	//1B	//true/false
	bool IsChange;  //1B	//is change the MultiAddress
	BYTE track;		//1B  //信道
	UINT bandWidth; //4B  //带宽
	BYTE resolution;//1B  //分辨率 0=704x576,1=704x288,2=352x576,3=352x288,4=480x576.
	bool PointSwitch;//1B //点播地址是否改变。
	DWORD startTime;//4B
	DWORD endTime;	//4B
	DWORD runTime;	//4B
	//add by fxd
	char MultiAddressEx[32];//32B //Old Multicast address
	BYTE TermLevel;		//1B    //终端级别：0最高，99最低
	//end fxd
	_NodeStatus();;
}NODEINFO,*PNODEINFO;		//total 1380 bytes 
typedef struct _NodeInfoOnMcu {
	DWORD CommandCode;//4B	
	DWORD lParam;//4B
	char szParam[1024]; //1024B
	_NodeInfoOnMcu(){ memset(this,0,sizeof(*this)); };
} NODEINFOONMCU, *PNODEINFOONMCU;

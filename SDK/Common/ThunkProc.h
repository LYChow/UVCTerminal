
#pragma once


#pragma pack(push,1)

/*
结构体 _THUNKPROC
说明:
	定义了一段程序代码,代替原始函数,代码流程
		1.弹出栈顶返回地址
		2.压入自定义参数
		3.再压入原始返回地址
		4.跳转到原始函数地址


功能:
	为原始回调函数增加一个自定义参数(回调函数必须是__stdcall)
使用方法:
	1.假设原始回调函数为: LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM)
	2.定义新的回调函数: LRESULT CALLBACK ThunkWindowProc(LPVOID,HWND,UINT,WPARAM,LPARAM)
	3.定义变量: 
		_THUNKPROC _MyThunk;
	4.设置回调参数: 
		_MyThunk.SetThunkData(MyParam,ThunkWindowProc);
	5.设置回调:
		::SetWindowLong(hwnd,GWL_WNDPROC,(DWORD)&_MyThunk);

	*/

struct _THUNKPROC
{
	WORD c1;
	LPVOID _pData;
	WORD c2;
	DWORD _dwProc; 
	_THUNKPROC(LPVOID pThunkData=0,DWORD dwProc=0)
	{
		SetThunkData(pThunkData,dwProc);
	}
	void SetThunkData(LPVOID pThunkData,DWORD dwProc)
	{
		c1=0x6858;//pop eax;push _pData
		_pData=pThunkData;//
		c2=0xe950;//push eax;jmp _pProc
		_dwProc=*LPDWORD(&dwProc);
		_dwProc-=DWORD(&_dwProc)+4;
	}
};

#pragma pack(pop)

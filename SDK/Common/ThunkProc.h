
#pragma once


#pragma pack(push,1)

/*
�ṹ�� _THUNKPROC
˵��:
	������һ�γ������,����ԭʼ����,��������
		1.����ջ�����ص�ַ
		2.ѹ���Զ������
		3.��ѹ��ԭʼ���ص�ַ
		4.��ת��ԭʼ������ַ


����:
	Ϊԭʼ�ص���������һ���Զ������(�ص�����������__stdcall)
ʹ�÷���:
	1.����ԭʼ�ص�����Ϊ: LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM)
	2.�����µĻص�����: LRESULT CALLBACK ThunkWindowProc(LPVOID,HWND,UINT,WPARAM,LPARAM)
	3.�������: 
		_THUNKPROC _MyThunk;
	4.���ûص�����: 
		_MyThunk.SetThunkData(MyParam,ThunkWindowProc);
	5.���ûص�:
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

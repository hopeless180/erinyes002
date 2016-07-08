#pragma once

#include "stdafx.h"
#include "util_RC.h"

//////////////////////////////////////////////////////////////////////////
//	结构体:		DevIOParam
//	用于:		DevThread下的线程函数
//				DevIOThread, ReadDevThread, WriteDevThread
//////////////////////////////////////////////////////////////////////////
struct DevIOParam{
	//	CString DevPath 存储存储介质的物理地址
	//	目前使用的是 //./x:
	//	之前使用的是文件系统X://
	CString DevPath;

	// 存储主线程窗口句柄,用于发送消息
	HWND    hWindow;

	// 存储从主线程中得到的存储介质大小
	// 因为DWORD的限制,只有最多4G的范围
	// DWORD	DevSize;
	// 将数据类型换为更大的 Unsigned __int64
	// 0~2^64
	unsigned __int64 DevSize;
	HANDLE	hDev;
};



// 对存储介质进行读写操作的单线程
void DevIOThread(LPVOID lpPara);

// 对存储介质进行读取操作的线程.用来和 WriteDevThread 搭配使用
void ReadDevThread(LPVOID lpParam);

// 对存储介质进行写入操作的线程,用来和 ReadDevThread 搭配使用
void WriteDevThread(LPVOID lpParam);

// DevTHread.h ends here

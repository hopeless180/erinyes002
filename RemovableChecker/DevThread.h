#pragma once

#include "stdafx.h"
#include "util_RC.h"

//////////////////////////////////////////////////////////////////////////
//	�ṹ��:		DevIOParam
//	����:		DevThread�µ��̺߳���
//				DevIOThread, ReadDevThread, WriteDevThread
//////////////////////////////////////////////////////////////////////////
struct DevIOParam{
	//	CString DevPath �洢�洢���ʵ������ַ
	//	Ŀǰʹ�õ��� //./x:
	//	֮ǰʹ�õ����ļ�ϵͳX://
	CString DevPath;

	// �洢���̴߳��ھ��,���ڷ�����Ϣ
	HWND    hWindow;

	// �洢�����߳��еõ��Ĵ洢���ʴ�С
	// ��ΪDWORD������,ֻ�����4G�ķ�Χ
	// DWORD	DevSize;
	// ���������ͻ�Ϊ����� Unsigned __int64
	// 0~2^64
	unsigned __int64 DevSize;
	HANDLE	hDev;
};



// �Դ洢���ʽ��ж�д�����ĵ��߳�
void DevIOThread(LPVOID lpPara);

// �Դ洢���ʽ��ж�ȡ�������߳�.������ WriteDevThread ����ʹ��
void ReadDevThread(LPVOID lpParam);

// �Դ洢���ʽ���д��������߳�,������ ReadDevThread ����ʹ��
void WriteDevThread(LPVOID lpParam);

// DevTHread.h ends here

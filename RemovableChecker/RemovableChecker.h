
// RemovableChecker.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRemovableCheckerApp:
// �йش����ʵ�֣������ RemovableChecker.cpp
//

class CRemovableCheckerApp : public CWinApp
{
public:
	CRemovableCheckerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

BOOL FindDevicePath(CStringArray & RemovableArray);
DWORD GetNumberOfDeviceType();

extern CRemovableCheckerApp theApp;


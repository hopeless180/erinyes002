
// RemovableCheckerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemovableChecker.h"
#include "RemovableCheckerDlg.h"
#include "DevThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//	ȫ�ֽṹ�� DevIOParam ������
//	���𴫵��̺߳�������Ҫ�Ĳ�����
//	����λ��DevThread.cpp
//////////////////////////////////////////////////////////////////////////
extern struct DevIOParam DP;
extern DWORD dwMethod;

// CRemovableCheckerDlg �Ի���

CRemovableCheckerDlg::CRemovableCheckerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRemovableCheckerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	cSelectedDeviceName = 0;
	NumberOfDeviceRemovable = 0;
	strOutput = _T("");
	ThreadID1 = 0;
	ThreadID2 = 0;
	hThreadRead = 0;
	hThreadWrite = 0;
}

void CRemovableCheckerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE, m_combobox);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, m_edit);
	DDX_Control(pDX, IDC_BUTTON_START, m_bStart);
	DDX_Control(pDX, IDC_BUTTON_REFRESH, m_bRefresh);
	DDX_Control(pDX, IDC_PROGRESS_WRITE, m_pWrite);
	DDX_Control(pDX, IDC_PROGRESS_READ, m_pRead);
	DDX_Control(pDX, IDC_BUTTON_FINISH, m_bFinish);
	DDX_Control(pDX, IDC_BUTTON_RT_GOON, m_bRTGoon);
	DDX_Control(pDX, IDC_BUTTON_RT_PAUSE, m_bRTPause);
	DDX_Control(pDX, IDC_BUTTON_WT_GOON, m_bWTGoon);
	DDX_Control(pDX, IDC_BUTTON_WT_PAUSE, m_bWTPause);
}

BEGIN_MESSAGE_MAP(CRemovableCheckerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_THREAD_PROGRESS_STATUS, OnTdChangePos)
	ON_MESSAGE(WM_THREAD_ALERT, OnAMDisplayInfo)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CRemovableCheckerDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_FINISH, &CRemovableCheckerDlg::OnBnClickedButtonFinish)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE, &CRemovableCheckerDlg::OnCbnSelchangeComboDevice)
	ON_BN_CLICKED(IDC_BUTTON_START, &CRemovableCheckerDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_RT_PAUSE, &CRemovableCheckerDlg::OnBnClickedButtonRTPause)
	ON_BN_CLICKED(IDC_BUTTON_RT_GOON, &CRemovableCheckerDlg::OnBnClickedButtonRTGoon)
	ON_BN_CLICKED(IDC_BUTTON_WT_GOON, &CRemovableCheckerDlg::OnBnClickedButtonWTGoon)
	ON_BN_CLICKED(IDC_BUTTON_WT_PAUSE, &CRemovableCheckerDlg::OnBnClickedButtonWTPause)
END_MESSAGE_MAP()


// CRemovableCheckerDlg ��Ϣ�������

BOOL CRemovableCheckerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԿ�ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CRect test;
	m_bRefresh.GetClientRect(test);
	m_combobox.SetItemHeight(test.Height(), 0);

	// �������������ؼ����÷�Χ
	m_pRead.SetRange(0, PROGRESS_RANGE_MAX);
	m_pWrite.SetRange(0, PROGRESS_RANGE_MAX);

	CString			root;
	CString			output;
	ULARGE_INTEGER	lpuse;
	ULARGE_INTEGER	lptotal;

	bCheck = FindDevicePath(DevicePathArray);

	if(bCheck == FINDDEVICEPATH_FAIL)
	{
		AfxMessageBox(_T("FineDevicePath�������ִ���,�������������޷�ʶ��"));
		exit(0);
	}
	
	for (int i = DevicePathArray.GetSize() - 1; i >= 0; i --)
	{
		if (GetDriveType(DevicePathArray[i]) == DRIVE_REMOVABLE)
		{
			m_combobox.AddString(DevicePathArray[i]);
			NumberOfDeviceRemovable ++;
		}
	}

	if (NumberOfDeviceRemovable == 0)
	{
		AfxMessageBox(_T("δ�ܼ�⵽�ƶ��洢����"));
	}
	else
	{
		// Ĭ��ѡ���һ���ƶ��洢����
		m_combobox.SetCurSel(0);
		m_combobox.GetLBText(0, output);
		cSelectedDeviceName = output.GetAt(0);

		if (GetDiskFreeSpaceEx(output,&lpuse,&lptotal,&ulDeviceSpace))
		{
			output.Format(	_T("���������ƣ�\t\t\t%c\r\n")
							_T("��������\t\t\t\t%.2f\r\n")
							_T("δʹ��������\t\t\t%.2f\r\n"),
							cSelectedDeviceName,
							GB(lptotal), 
							GB(ulDeviceSpace));
			strOutput += output;
			SetDlgItemText(IDC_EDIT_OUTPUT, strOutput);
			m_bStart.EnableWindow(TRUE);
		}

	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

//  �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRemovableCheckerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRemovableCheckerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ˢ�������˵�
void CRemovableCheckerDlg::OnBnClickedButtonRefresh()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString root;
	CString output;
	ULARGE_INTEGER lpuse;
	ULARGE_INTEGER lptotal;
	BOOL bCheck;

	strOutput.Empty();
	SetDlgItemText(IDC_EDIT_OUTPUT,_T(""));
	m_combobox.ResetContent();
	DevicePathArray.RemoveAll();

	bCheck = FindDevicePath(DevicePathArray);
	//	������
	if(bCheck == FINDDEVICEPATH_FAIL)
	{
		AfxMessageBox(_T("FineDevicePath�������ִ���,�������������޷�ʶ��"));
		exit(0);
	}

	for (int i = DevicePathArray.GetSize() - 1; i >= 0; i --)
	{
		if (GetDriveType(DevicePathArray[i]) == DRIVE_REMOVABLE)
		{
			m_combobox.AddString(DevicePathArray[i]);
			NumberOfDeviceRemovable ++;
		}
	}// for() ends here

	if (NumberOfDeviceRemovable == 0)
	{
		AfxMessageBox(_T("δ�ܼ�⵽�ƶ��洢����"));
	}
	else
	{

		// Ĭ��ѡ���һ���ƶ��洢����
		m_combobox.SetCurSel(0);
		m_combobox.GetLBText(0, output);
		cSelectedDeviceName = output.GetAt(0);

		if (GetDiskFreeSpaceEx(output,&lpuse,&lptotal,&ulDeviceSpace))
		{
			output.Format(	_T("���������ƣ�\t\t\t%c\r\n")
				_T("��������\t\t\t\t%.2f\r\n")
				_T("δʹ��������\t\t\t%.2f\r\n"),
				cSelectedDeviceName,
				GB(lptotal), 
				GB(ulDeviceSpace));
			strOutput += output;
			SetDlgItemText(IDC_EDIT_OUTPUT, strOutput);
			m_bStart.EnableWindow(TRUE);
		}
	}
}

// ѡȡ�洢����
void CRemovableCheckerDlg::OnCbnSelchangeComboDevice()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BYTE index;
	CString output;
	ULARGE_INTEGER lpuse;
	ULARGE_INTEGER lptotal;

	SetDlgItemText(IDC_EDIT_OUTPUT,_T(""));
	strOutput.Empty();
	index = m_combobox.GetCurSel();
	m_combobox.GetLBText(index, output);
	cSelectedDeviceName = output.GetAt(0);
	
	if (GetDiskFreeSpaceEx(output,&lpuse,&lptotal,&ulDeviceSpace))
	{
		output.Format(	_T("���������ƣ�\t\t\t%c\r\n")
						_T("��������\t\t\t\t%.2f\r\n")
						_T("δʹ��������\t\t\t%.2f\r\n"),
						cSelectedDeviceName,
						GB(lptotal), 
						GB(ulDeviceSpace));
		strOutput += output;
	SetDlgItemText(IDC_EDIT_OUTPUT, strOutput);
	m_bStart.EnableWindow(TRUE);
	}
}

// ��ʼ���
void CRemovableCheckerDlg::OnBnClickedButtonStart()
{
	CString output;
	CString strDevicePath;
	DWORD	iBytesWritten = 0;
	DWORD	iBytesRead = 0;
	HANDLE	hWriteFile;
	HANDLE  hDev; 
	// Ϊ���ݻ���������������ݡ�ֱ����ﵽKIBIBYTE�ֽ�
	BYTE *MainBuffer = new BYTE[BUFFER_SIZE];

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	strDevicePath.Format(_T("\\\\.\\%c:"), cSelectedDeviceName);

	if (cSelectedDeviceName == 0)
	{
		AfxMessageBox(_T("��ѡ���Ӧ���̷�"));
		return;
	}

	// ����writefile�������
	hWriteFile = CreateFile(_T("WriteBuffer.txt"), 
							GENERIC_WRITE, 
							NULL, 
							NULL, 
							CREATE_ALWAYS,
							0, 
							NULL);

	if(hWriteFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("����WriteBufferʧ��---���߳�"));
		CloseHandle(hWriteFile);
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//	��������ݻ�����д������,������Ϊ���Ӳ�̶�д������ָ�����
	//srand(unsigned(time(NULL)));// �����ѭ���ڲ������ܵõ���Ҫ�Ľ��
	//for (int i = 0; i < IOUNIT; i++)
	//{
	//	MainBuffer[i] = rand()%128;
	//}// end of the for(int i...)
	//////////////////////////////////////////////////////////////////////////

	// ��0X55AAд�����������ܼ��Ӳ�̵Ķ�д����
	for (int i = 0; i < IOUNIT; i ++)
	{
		if (i % 2 == 0)
		{
			MainBuffer[i] = 0x55;
		}
		else
		{
			MainBuffer[i] = 0xAA;
		}
	}
	
	//	�����ݻ����������ݼ��ص�WriteBuffer.txt��
	//	д���СΪ512�ֽ�(IOUNIT)
	if(!WriteFile(hWriteFile, MainBuffer, IOUNIT, &iBytesWritten, NULL))
	{
		AfxMessageBox(_T("д���������ʧ�ܣ�---���߳�"));
	}

	//	�ͷ�mainbuffer�ڴ�ռ�
	free(MainBuffer);

	//	�ر�hWriteFIle���
	CloseHandle(hWriteFile);

	//	ѡ������ʽ
	CDialogEx dlgCM(IDD_DIALOG_CHOOSEMETHOD);
	dlgCM.DoModal();

	switch (dwMethod)
	{
	case :
		break;
	}

	//	������д���߳� DevIOThread
	DP.DevPath.Format(_T("\\\\.\\%c:"), cSelectedDeviceName);
	//DP.DevPath = _T("WriteBuffer.txt");

	DP.hWindow = GetSafeHwnd();
	DP.DevSize = ulDeviceSpace.HighPart << 32 | ulDeviceSpace.LowPart;

	/*hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DevIOThread, &DP, 0, &ThreadID);
	if (!hThread)
	{	
		AfxMessageBox(_T("�̴߳�������"));
		return;
	}*/

	

	hThreadWrite = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WriteDevThread, &DP, 0, &ThreadID1);
	if (!hThreadWrite)
	{	
		AfxMessageBox(_T("Write�̴߳�������"));
		return;
	}
	/*hThreadRead  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadDevThread,  &DP, 0, &ThreadID2);
	if (!hThreadRead)
	{	
	AfxMessageBox(_T("Read�̴߳�������"));
	return;
	}*/
	// ִ�����,�ȴ���Ϣ��Ӧ
}

// ��ͣ���
void CRemovableCheckerDlg::OnBnClickedButtonWTPause()
{
	if(SuspendThread(hThreadWrite)==0xFFFFFFFF)
	{
		AfxMessageBox(_T("Write����ʧ�ܣ��߳̿����Ѿ�������δ������"));
		return;
	}

	SetWindowText(_T("Write�߳��ѹ���"));
	
}

// �������
void CRemovableCheckerDlg::OnBnClickedButtonWTGoon()
{
	if(ResumeThread(hThreadWrite) == 0xffffffff)
	{
		AfxMessageBox(_T("Write�ָ�ʧ�ܣ�"));
		return;
	}
	SetWindowText(_T("Write�ָ̻߳�������֮ǰ�Ĺ���"));
}

void CRemovableCheckerDlg::OnBnClickedButtonRTPause()
{
	if(SuspendThread(hThreadRead)==0xFFFFFFFF)
	{
		AfxMessageBox(_T("Read����ʧ�ܣ��߳̿����Ѿ�������δ������"));
		return;
	}
	SetWindowText(_T("Read�߳��ѹ���"));

}

void CRemovableCheckerDlg::OnBnClickedButtonRTGoon()
{
	AfxMessageBox(_T("asd"));
	if(ResumeThread(hThreadRead) == 0xffffffff)
	{
		AfxMessageBox(_T("�ָ�ʧ�ܣ�"));
		return;
	}

	SetWindowText(_T("Read�ָ̻߳�������֮ǰ�Ĺ���"));
}

void CRemovableCheckerDlg::OnBnClickedButtonFinish()
{
	if(hThreadRead == 0 || hThreadWrite == 0)
	{
		AfxMessageBox(_T("��ȷ���߳��Ƿ񴴽�!"));
		return;
	}
	DWORD dwExitCode;
	TerminateThread(hThreadWrite, dwExitCode);
	TerminateThread(hThreadRead, dwExitCode);
}

LRESULT CRemovableCheckerDlg::OnTdChangePos(WPARAM w,LPARAM l)
{
	switch (w)
	{
	case WM_THREAD_WRITE_PROGRESS_SETPOS:
		{
			m_pWrite.SetPos(l);
			break;
		}
	case WM_THREAD_READ_PROGRESS_SETPOS:
		{
			m_pRead.SetPos(l);
			break;
		}
	default:
		{
			return 0;
		}
	}
	UpdateData(FALSE);
	return 1;
}

LRESULT CRemovableCheckerDlg::OnAMDisplayInfo(WPARAM w,LPARAM l)
{
	switch (w)
	{
	case WM_THREAD_HWRITEDEV_CREATEFILE_ERROR: 
		{
			AfxMessageBox(_T("�̺߳�����,CREATEFILE_1��������"));
			// �Ժ�������getlasterror����
			break;
		}
	case WM_THREAD_HREADFILE_CREATEFILE_ERROR:
		{
			AfxMessageBox(_T("�̺߳�����,CREATEFILE_2��������"));
			// ͬ��
			break;
		}
	case WM_THREAD_HREADDEV_CREATEFILE_ERROR:
		{
			AfxMessageBox(_T("�̺߳�����,CREATEFILE_3��������"));
			break;
		}
	case WM_THREAD_HWRITEFILE_CREATEFILE_ERROR:
		{
			AfxMessageBox(_T("�̺߳�����,CREATEFILE_4��������"));
			break;
		}
	case WM_THREAD_WRITE_WRITEFILE_ERROR:
		{
			AfxMessageBox(_T("�̺߳�����,WRITEFILE_1��������"));
			break;
		}
	case WM_THREAD_WRITE_READFILE_ERROR:
		{
			AfxMessageBox(_T("�̺߳�����,READFILE_1��������"));
			break;
		}
	case WM_THREAD_READ_WRITEFILE_ERROR:
		{
			AfxMessageBox(_T("�̺߳�����,WRITEFILE_2��������"));
			break;
		}
	case WM_THREAD_READ_READFILE_ERROR:
		{
			AfxMessageBox(_T("�̺߳�����,READFILE_2��������"));
			break;
		}
	case WM_THREAD_WRITE_SUCCESS:
		{
			AfxMessageBox(_T("д��������"));
			break;
		}
	case WM_THREAD_READ_SUCCESS:
		{
			AfxMessageBox(_T("��ȡ�������"));
			break;
		}
	default:
		{
			return 0;
		}
	}

	return 1;
}

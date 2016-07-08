
// RemovableCheckerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RemovableChecker.h"
#include "RemovableCheckerDlg.h"
#include "DevThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//	全局结构体 DevIOParam 的声明
//	负责传递线程函数所需要的参数。
//	定义位于DevThread.cpp
//////////////////////////////////////////////////////////////////////////
extern struct DevIOParam DP;
extern DWORD dwMethod;

// CRemovableCheckerDlg 对话框

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


// CRemovableCheckerDlg 消息处理程序

BOOL CRemovableCheckerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRect test;
	m_bRefresh.GetClientRect(test);
	m_combobox.SetItemHeight(test.Height(), 0);

	// 对两个进度条控件设置范围
	m_pRead.SetRange(0, PROGRESS_RANGE_MAX);
	m_pWrite.SetRange(0, PROGRESS_RANGE_MAX);

	CString			root;
	CString			output;
	ULARGE_INTEGER	lpuse;
	ULARGE_INTEGER	lptotal;

	bCheck = FindDevicePath(DevicePathArray);

	if(bCheck == FINDDEVICEPATH_FAIL)
	{
		AfxMessageBox(_T("FineDevicePath函数出现错误,所有驱动器均无法识别"));
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
		AfxMessageBox(_T("未能检测到移动存储介质"));
	}
	else
	{
		// 默认选择第一个移动存储介质
		m_combobox.SetCurSel(0);
		m_combobox.GetLBText(0, output);
		cSelectedDeviceName = output.GetAt(0);

		if (GetDiskFreeSpaceEx(output,&lpuse,&lptotal,&ulDeviceSpace))
		{
			output.Format(	_T("驱动器名称：\t\t\t%c\r\n")
							_T("总容量：\t\t\t\t%.2f\r\n")
							_T("未使用容量：\t\t\t%.2f\r\n"),
							cSelectedDeviceName,
							GB(lptotal), 
							GB(ulDeviceSpace));
			strOutput += output;
			SetDlgItemText(IDC_EDIT_OUTPUT, strOutput);
			m_bStart.EnableWindow(TRUE);
		}

	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//  如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemovableCheckerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRemovableCheckerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 刷新下拉菜单
void CRemovableCheckerDlg::OnBnClickedButtonRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
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
	//	出错检查
	if(bCheck == FINDDEVICEPATH_FAIL)
	{
		AfxMessageBox(_T("FineDevicePath函数出现错误,所有驱动器均无法识别"));
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
		AfxMessageBox(_T("未能检测到移动存储介质"));
	}
	else
	{

		// 默认选择第一个移动存储介质
		m_combobox.SetCurSel(0);
		m_combobox.GetLBText(0, output);
		cSelectedDeviceName = output.GetAt(0);

		if (GetDiskFreeSpaceEx(output,&lpuse,&lptotal,&ulDeviceSpace))
		{
			output.Format(	_T("驱动器名称：\t\t\t%c\r\n")
				_T("总容量：\t\t\t\t%.2f\r\n")
				_T("未使用容量：\t\t\t%.2f\r\n"),
				cSelectedDeviceName,
				GB(lptotal), 
				GB(ulDeviceSpace));
			strOutput += output;
			SetDlgItemText(IDC_EDIT_OUTPUT, strOutput);
			m_bStart.EnableWindow(TRUE);
		}
	}
}

// 选取存储介质
void CRemovableCheckerDlg::OnCbnSelchangeComboDevice()
{
	// TODO: 在此添加控件通知处理程序代码
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
		output.Format(	_T("驱动器名称：\t\t\t%c\r\n")
						_T("总容量：\t\t\t\t%.2f\r\n")
						_T("未使用容量：\t\t\t%.2f\r\n"),
						cSelectedDeviceName,
						GB(lptotal), 
						GB(ulDeviceSpace));
		strOutput += output;
	SetDlgItemText(IDC_EDIT_OUTPUT, strOutput);
	m_bStart.EnableWindow(TRUE);
	}
}

// 开始检测
void CRemovableCheckerDlg::OnBnClickedButtonStart()
{
	CString output;
	CString strDevicePath;
	DWORD	iBytesWritten = 0;
	DWORD	iBytesRead = 0;
	HANDLE	hWriteFile;
	HANDLE  hDev; 
	// 为数据缓冲区加入随机数据。直到其达到KIBIBYTE字节
	BYTE *MainBuffer = new BYTE[BUFFER_SIZE];

	// TODO: 在此添加控件通知处理程序代码
	strDevicePath.Format(_T("\\\\.\\%c:"), cSelectedDeviceName);

	if (cSelectedDeviceName == 0)
	{
		AfxMessageBox(_T("请选择对应的盘符"));
		return;
	}

	// 创建writefile操作句柄
	hWriteFile = CreateFile(_T("WriteBuffer.txt"), 
							GENERIC_WRITE, 
							NULL, 
							NULL, 
							CREATE_ALWAYS,
							0, 
							NULL);

	if(hWriteFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("创建WriteBuffer失败---主线程"));
		CloseHandle(hWriteFile);
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//	随机向数据缓冲区写入数据,不能作为检测硬盘读写能力的指标存在
	//srand(unsigned(time(NULL)));// 如果在循环内部，不能得到想要的结果
	//for (int i = 0; i < IOUNIT; i++)
	//{
	//	MainBuffer[i] = rand()%128;
	//}// end of the for(int i...)
	//////////////////////////////////////////////////////////////////////////

	// 用0X55AA写满缓冲区更能检测硬盘的读写能力
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
	
	//	把数据缓冲区的数据加载到WriteBuffer.txt中
	//	写入大小为512字节(IOUNIT)
	if(!WriteFile(hWriteFile, MainBuffer, IOUNIT, &iBytesWritten, NULL))
	{
		AfxMessageBox(_T("写入测试数据失败！---主线程"));
	}

	//	释放mainbuffer内存空间
	free(MainBuffer);

	//	关闭hWriteFIle句柄
	CloseHandle(hWriteFile);

	//	选择工作方式
	CDialogEx dlgCM(IDD_DIALOG_CHOOSEMETHOD);
	dlgCM.DoModal();

	switch (dwMethod)
	{
	case :
		break;
	}

	//	创建读写单线程 DevIOThread
	DP.DevPath.Format(_T("\\\\.\\%c:"), cSelectedDeviceName);
	//DP.DevPath = _T("WriteBuffer.txt");

	DP.hWindow = GetSafeHwnd();
	DP.DevSize = ulDeviceSpace.HighPart << 32 | ulDeviceSpace.LowPart;

	/*hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DevIOThread, &DP, 0, &ThreadID);
	if (!hThread)
	{	
		AfxMessageBox(_T("线程创建出错！"));
		return;
	}*/

	

	hThreadWrite = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WriteDevThread, &DP, 0, &ThreadID1);
	if (!hThreadWrite)
	{	
		AfxMessageBox(_T("Write线程创建出错！"));
		return;
	}
	/*hThreadRead  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadDevThread,  &DP, 0, &ThreadID2);
	if (!hThreadRead)
	{	
	AfxMessageBox(_T("Read线程创建出错！"));
	return;
	}*/
	// 执行完毕,等待消息响应
}

// 暂停检测
void CRemovableCheckerDlg::OnBnClickedButtonWTPause()
{
	if(SuspendThread(hThreadWrite)==0xFFFFFFFF)
	{
		AfxMessageBox(_T("Write挂起失败！线程可能已经死亡或未创建！"));
		return;
	}

	SetWindowText(_T("Write线程已挂起"));
	
}

// 继续监测
void CRemovableCheckerDlg::OnBnClickedButtonWTGoon()
{
	if(ResumeThread(hThreadWrite) == 0xffffffff)
	{
		AfxMessageBox(_T("Write恢复失败！"));
		return;
	}
	SetWindowText(_T("Write线程恢复，继续之前的工作"));
}

void CRemovableCheckerDlg::OnBnClickedButtonRTPause()
{
	if(SuspendThread(hThreadRead)==0xFFFFFFFF)
	{
		AfxMessageBox(_T("Read挂起失败！线程可能已经死亡或未创建！"));
		return;
	}
	SetWindowText(_T("Read线程已挂起"));

}

void CRemovableCheckerDlg::OnBnClickedButtonRTGoon()
{
	AfxMessageBox(_T("asd"));
	if(ResumeThread(hThreadRead) == 0xffffffff)
	{
		AfxMessageBox(_T("恢复失败！"));
		return;
	}

	SetWindowText(_T("Read线程恢复，继续之前的工作"));
}

void CRemovableCheckerDlg::OnBnClickedButtonFinish()
{
	if(hThreadRead == 0 || hThreadWrite == 0)
	{
		AfxMessageBox(_T("请确认线程是否创建!"));
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
			AfxMessageBox(_T("线程函数中,CREATEFILE_1函数错误"));
			// 以后可以添加getlasterror函数
			break;
		}
	case WM_THREAD_HREADFILE_CREATEFILE_ERROR:
		{
			AfxMessageBox(_T("线程函数中,CREATEFILE_2函数错误"));
			// 同上
			break;
		}
	case WM_THREAD_HREADDEV_CREATEFILE_ERROR:
		{
			AfxMessageBox(_T("线程函数中,CREATEFILE_3函数错误"));
			break;
		}
	case WM_THREAD_HWRITEFILE_CREATEFILE_ERROR:
		{
			AfxMessageBox(_T("线程函数中,CREATEFILE_4函数错误"));
			break;
		}
	case WM_THREAD_WRITE_WRITEFILE_ERROR:
		{
			AfxMessageBox(_T("线程函数中,WRITEFILE_1函数错误"));
			break;
		}
	case WM_THREAD_WRITE_READFILE_ERROR:
		{
			AfxMessageBox(_T("线程函数中,READFILE_1函数错误"));
			break;
		}
	case WM_THREAD_READ_WRITEFILE_ERROR:
		{
			AfxMessageBox(_T("线程函数中,WRITEFILE_2函数错误"));
			break;
		}
	case WM_THREAD_READ_READFILE_ERROR:
		{
			AfxMessageBox(_T("线程函数中,READFILE_2函数错误"));
			break;
		}
	case WM_THREAD_WRITE_SUCCESS:
		{
			AfxMessageBox(_T("写入操作完成"));
			break;
		}
	case WM_THREAD_READ_SUCCESS:
		{
			AfxMessageBox(_T("读取操作完成"));
			break;
		}
	default:
		{
			return 0;
		}
	}

	return 1;
}

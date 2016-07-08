#include "stdafx.h"
#include "DevThread.h"

//HWND mainthread;
DevIOParam DP;

unsigned __int64 uiSectorsWritten;
unsigned __int64 uiSectorsRead;

DWORD dwMethod;

CMutex mutex;
int test = 0;

////////////////////////////////////////////////////////////////
// Method:    DevIOThread
// FullName:  DevIOThread
// Access:    public 
// Returns:   void
// Qualifier:
////////////////////////////////////////////////////////////////
void DevIOThread(LPVOID lpParam)
{
	#pragma region 写入数据


	// 存储错误信息的字符串,通常接在getlasterror后
	CString		ERROROUT;

	// 结构体传递参数的重要步骤,将无类型指针转化成对应指针
	DevIOParam	*lpDP = (DevIOParam *) lpParam;

	// 把数据写入存储介质的操作句柄
	HANDLE		hWriteDev;

	// 从文件中读取数据的操作句柄
	HANDLE		hReadFile;
	
	// 从存储介质中读取数据的操作句柄
	HANDLE		hReadDev;

	// 向本地文件中写入数据的操作句柄
	HANDLE		hWriteFile;

	//////////////////////////////////////////////////////////////////////////
	//	BOOL bCheck
	//	接收返回值是BOOL型数据的函数返回值
	//	使用时尽量重定义1和0
	//	格式:函数_状态
	//////////////////////////////////////////////////////////////////////////
	BOOL		bCheck;

	//////////////////////////////////////////////////////////////////////////
	//	存储读写数据的数据缓冲区buffer
	//	用于:	WriteFile和ReadFIle
	//////////////////////////////////////////////////////////////////////////
	WCHAR		buffer[BUFFER_SIZE];

	//////////////////////////////////////////////////////////////////////////
	//	DWORD ulFileSize
	//	定义：		记录文件的大小
	//	范围：		0～4294967295，如果驱动器超过4G需要重新定义变量
	//	使用函数：	GetFIleSize()
	//////////////////////////////////////////////////////////////////////////
	// DWORD ulFileSize

	//////////////////////////////////////////////////////////////////////////
	//	unsigned __int64 i64FileSize
	//	定义:			文件的大小
	//	范围:			0~2^64
	//	使用函数:			GetFileSIzeEX()
	//////////////////////////////////////////////////////////////////////////
	unsigned __int64 i64FileSize;
	LARGE_INTEGER intbuffer;

	// 用于WriteFile()和ReadFile(),存储读写的数据量
	DWORD NumberOfBytesRead  = 0;
	DWORD NumberOfBytesWrite = 0;


	#pragma region 创建驱动器句柄,用于写入数据
    // ID:		FUNC_THREAD_WRITE_HWRITEDEV_CREATEFILE_1			CREATEFILE_1
	hWriteDev = CreateFile(	lpDP->DevPath, 
							GENERIC_WRITE, 
							NULL, 
							NULL, 
							OPEN_EXISTING, 
							0, 
							NULL); 
			
	if (hWriteDev == INVALID_HANDLE_VALUE)
	{
		PostMessage(lpDP->hWindow, WM_THREAD_ALERT, 
					WM_THREAD_HWRITEDEV_CREATEFILE_ERROR, 0);
		return;
	}
    #pragma endregion

	#pragma region 创建读取文件的句柄,用于从本地的WriteBuffer.txt中读取数据
// ID:		FUNC_THREAD_WRITE_HREADDEV_CREATEFILE_1				CREATEFILE_2
	hReadFile = CreateFile( _T("WriteBuffer.txt"), 
							GENERIC_READ,
							NULL, 
							NULL, 
							OPEN_EXISTING, 
							0, 
							NULL);

	if (hReadFile == INVALID_HANDLE_VALUE)
	{
		PostMessage(lpDP->hWindow, WM_THREAD_ALERT, 
					WM_THREAD_HREADFILE_CREATEFILE_ERROR, 0);
		return;
	}
	#pragma endregion

	# pragma region 以512字节为单位,读取1单位的数据到数据缓冲区Buffer中
	// ID:			FUNC_THREAD_WRITE_BCHECK_READFILE_1				READFILE_1
	bCheck = ReadFile(hReadFile, buffer, IOUNIT, &NumberOfBytesRead,  NULL);

	if (bCheck == READFILE_FAIL)
	{
		ERROROUT.Format(_T("%d\r\n读取WriteBuffer的ReadFile函数错误"),
						GetLastError());
		PostMessage(lpDP->hWindow, WM_THREAD_ALERT, 
					WM_THREAD_WRITE_READFILE_ERROR, 0);
		return;
	}

	# pragma endregion 

	// 关闭句柄 hReadFile ,还剩句柄 hWriteDev 没有关闭
	CloseHandle(hReadFile); 

	i64FileSize = 0;

	#pragma region 循环写入55AA的数据到存储介质中,总共512字节
	while (i64FileSize < lpDP->DevSize)
	{
		//ID:			FUNC_THREAD_WRITE_BCHECK_WRITEFILE_1		WRITEFILE_1
		bCheck = WriteFile(hWriteDev, buffer, IOUNIT, &NumberOfBytesWrite, NULL);

		if (bCheck == WRITEFILE_FAIL)
		{
			ERROROUT.Format(_T("%d\r\n写入驱动器的WriteFile函数错误"), 
							GetLastError());
			PostMessage(lpDP->hWindow, WM_THREAD_ALERT, 
						WM_THREAD_WRITE_WRITEFILE_ERROR, 0);
			return;
		}

		i64FileSize += NumberOfBytesWrite;

		if(lpDP->DevSize - i64FileSize < 1024)
		{
			break;
		}

		PostMessage(lpDP->hWindow, WM_THREAD_PROGRESS_STATUS, 
					WM_THREAD_WRITE_PROGRESS_SETPOS, 
			       (DOUBLE)i64FileSize*PROGRESS_RANGE_MAX/lpDP->DevSize);

	}// end of the while()
	#pragma endregion

	// 关闭句柄 hWriteDev ,所有句柄全部关闭
	CloseHandle(hWriteDev);

	// 向主线程发送成功的消息
	PostMessage(lpDP->hWindow, WM_THREAD_WRITE_SUCCESS, 0, 0);



	# pragma endregion 

	# pragma region 读出数据


# pragma region 创建hReadDev句柄,用于从存储介质中读取数据
	// ID:			FUNC_THREAD_READ_HREADDEV_CREATEFILE_1			CREATEFILE_3
	hReadDev = CreateFile(	lpDP->DevPath, 
							GENERIC_READ, 
							NULL, 
							NULL, 
							OPEN_EXISTING, 
							0, 
							NULL);

	if(hReadDev == INVALID_HANDLE_VALUE)
	{
		PostMessage(lpDP->hWindow, WM_THREAD_ALERT, 
					WM_THREAD_HREADDEV_CREATEFILE_ERROR, 0);
		CloseHandle(hReadDev);
		return;
	}
# pragma endregion

# pragma region 创建句柄hWriteFile,用于向本地的ReadFile
	// ID:			FUNC_THREAD_READ_HWRITEFILE_CREATEFILE_1		CREATEFILE_4
	hWriteFile = CreateFile(	_T("ReadFile.txt"),
								GENERIC_WRITE,
								NULL,
								NULL,
								CREATE_ALWAYS,
								0,
								NULL);

	if(hWriteFile == INVALID_HANDLE_VALUE)
	{
		PostMessage(lpDP->hWindow, WM_THREAD_ALERT, 
					WM_THREAD_HWRITEFILE_CREATEFILE_ERROR, 0);
		return;
	}

# pragma endregion	
	
	GetFileSizeEx(hReadFile, &intbuffer);
	i64FileSize = intbuffer.QuadPart;

#pragma region 以千字节为单位,循环从存储介质中读取,同时向ReadFile.txt写入1KB的数据
	while(i64FileSize < lpDP->DevSize)
	{
		// ID:		FUNC_THREAD_READ_BCHECK_READFILE_2				READFILE_2
		bCheck = ReadFile(hReadDev, buffer, IOUNIT, &NumberOfBytesRead, NULL);

		// 如果出错,发送消息到主线程
		if (bCheck == READFILE_FAIL)
		{
			ERROROUT.Format(_T("%d\r\n读取U盘的ReadFile函数错误"), GetLastError());
			PostMessage(lpDP->hWindow, WM_THREAD_ALERT, 
						WM_THREAD_READ_READFILE_ERROR, 0);
			return;
		}

		// ID:			FUNC_THREAD_READ_BCHECK_WRITEFILE_2				WRITEFILE_2
		bCheck = WriteFile(hWriteFile, buffer, IOUNIT, &NumberOfBytesWrite, NULL);

		// 如果出错,发送消息到主线程
		if (bCheck == WRITEFILE_FAIL)
		{
			ERROROUT.Format(_T("%d\r\n写入本地文本的WriteFile函数错误"), 
							GetLastError());
			PostMessage(lpDP->hWindow, WM_THREAD_ALERT, 
						WM_THREAD_READ_WRITEFILE_ERROR, 0);
			return;
		}

		i64FileSize += NumberOfBytesRead;
		PostMessage(lpDP->hWindow, WM_THREAD_PROGRESS_STATUS, 
					WM_THREAD_READ_PROGRESS_SETPOS, 
			       (DOUBLE)i64FileSize*PROGRESS_RANGE_MAX/lpDP->DevSize);

	}//while(i64FileSize < lpDP->DevSize) ends here

#pragma endregion

	// 向主线程发送成功的消息
	PostMessage(lpDP->hWindow, WM_THREAD_ALERT, WM_THREAD_READ_SUCCESS, 0);

	// 关闭在前面创建的两个句柄
	CloseHandle(hWriteFile);
	CloseHandle(hReadDev);
	


	# pragma endregion 
}

void WriteDevThread(LPVOID lpParam)
{
	// 读取结构体中的数据,存放到指针中
	DevIOParam *lpDP = (DevIOParam *) lpParam;
	HANDLE hDev = lpDP->hDev;
	HANDLE hReadFile;
	HANDLE hWriteDev;
	BYTE byBuffer[BUFFER_SIZE];
	DWORD  iBytesWritten = 0;
	DWORD  iBytesRead = 0;
	BOOL bCheck;

	uiSectorsWritten = 0;
	hWriteDev = CreateFile(lpDP->DevPath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (hWriteDev == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("hWriteDev句柄创建失败"));
		return;
	}
	hReadFile = CreateFile(	_T("WriteBuffer.txt"), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(hReadFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("hReadFile句柄创建失败"));
		return;
	}

	bCheck = ReadFile(hReadFile, byBuffer, IOUNIT, &iBytesRead, NULL);
	CloseHandle(hReadFile);

	if (bCheck == READFILE_FAIL)
	{
		AfxMessageBox(_T("ReadFile错误 by DevWriteThread"));
		return;
	}
	
	while (uiSectorsWritten < TESTMAXIUM / IOUNIT)
	{
		if (test == 0)
		{
		//mutex.Lock();
		test ++;
		//SetFilePointer(hWriteDev, uiSectorsWritten*512, 0, FILE_BEGIN);

		bCheck = WriteFile(hWriteDev, byBuffer, IOUNIT, &iBytesWritten, NULL);
		int x = GetLastError();
		if(bCheck == WRITEFILE_FAIL)

		{
			AfxMessageBox(_T("WriteFile错误 by DevWriteThread"));
			return;
		}
		PostMessage(lpDP->hWindow, WM_THREAD_PROGRESS_STATUS, 
					WM_THREAD_WRITE_PROGRESS_SETPOS, (DOUBLE)uiSectorsWritten*100/TESTMAXIUM);
		uiSectorsWritten ++;
		//mutex.Unlock();
		test = 0;
		}
	}
	CloseHandle(hWriteDev);
	PostMessage(lpDP->hWindow, WM_THREAD_ALERT, WM_THREAD_WRITE_SUCCESS, 0);
}

void ReadDevThread(LPVOID lpParam)
{
	DevIOParam *lpDP = (DevIOParam *) lpParam;
	HANDLE hWriteFile;
	HANDLE hReadDev;
	BYTE byBuffer[BUFFER_SIZE];
	DWORD iBytesWritten;
	DWORD iBytesRead;
	BOOL bCheck;

	uiSectorsRead = 0;

	hReadDev = CreateFile(lpDP->DevPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(hReadDev == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("hReadDev创建句柄失败"));
		return;
	}

	hWriteFile = CreateFile(_T("ReadFile.txt"), GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, 0, NULL);
	if (hWriteFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("hWriteFile创建句柄失败"));
		return;
	}

	while (uiSectorsRead < TESTMAXIUM/*lpDP->DevSize*/ / IOUNIT)
	{
		if(uiSectorsRead < uiSectorsWritten)
		{
			//mutex.Lock();
			if(test == 0)
			{
			test ++;

			//SetFilePointer(hReadDev, uiSectorsRead*512, 0, FILE_BEGIN);
			bCheck = ReadFile(hReadDev, byBuffer, IOUNIT, &iBytesRead, NULL);
			if (bCheck = READFILE_FAIL)
			{
				AfxMessageBox(_T("ReadFile 错误 by DevReadThread"));
				return;
			}

			WriteFile(hWriteFile, byBuffer, IOUNIT, &iBytesWritten, NULL);
			if(bCheck = WRITEFILE_FAIL)
			{
				AfxMessageBox(_T("WriteFile 错误 by DevReadThread"));
				return;
			}
			PostMessage(lpDP->hWindow, WM_THREAD_PROGRESS_STATUS, 
						WM_THREAD_READ_PROGRESS_SETPOS, (DOUBLE)uiSectorsRead*100/TESTMAXIUM);
			uiSectorsRead ++;
			test = 0;
			}
			//mutex.Unlock();
		}
		else
		{
			Sleep(1);
		}
	}

	CloseHandle(hReadDev);
	CloseHandle(hWriteFile);
	PostMessage(lpDP->hWindow, WM_THREAD_ALERT, WM_THREAD_READ_SUCCESS, 0);
}
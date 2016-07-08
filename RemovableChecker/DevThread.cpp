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
	#pragma region д������


	// �洢������Ϣ���ַ���,ͨ������getlasterror��
	CString		ERROROUT;

	// �ṹ�崫�ݲ�������Ҫ����,��������ָ��ת���ɶ�Ӧָ��
	DevIOParam	*lpDP = (DevIOParam *) lpParam;

	// ������д��洢���ʵĲ������
	HANDLE		hWriteDev;

	// ���ļ��ж�ȡ���ݵĲ������
	HANDLE		hReadFile;
	
	// �Ӵ洢�����ж�ȡ���ݵĲ������
	HANDLE		hReadDev;

	// �򱾵��ļ���д�����ݵĲ������
	HANDLE		hWriteFile;

	//////////////////////////////////////////////////////////////////////////
	//	BOOL bCheck
	//	���շ���ֵ��BOOL�����ݵĺ�������ֵ
	//	ʹ��ʱ�����ض���1��0
	//	��ʽ:����_״̬
	//////////////////////////////////////////////////////////////////////////
	BOOL		bCheck;

	//////////////////////////////////////////////////////////////////////////
	//	�洢��д���ݵ����ݻ�����buffer
	//	����:	WriteFile��ReadFIle
	//////////////////////////////////////////////////////////////////////////
	WCHAR		buffer[BUFFER_SIZE];

	//////////////////////////////////////////////////////////////////////////
	//	DWORD ulFileSize
	//	���壺		��¼�ļ��Ĵ�С
	//	��Χ��		0��4294967295���������������4G��Ҫ���¶������
	//	ʹ�ú�����	GetFIleSize()
	//////////////////////////////////////////////////////////////////////////
	// DWORD ulFileSize

	//////////////////////////////////////////////////////////////////////////
	//	unsigned __int64 i64FileSize
	//	����:			�ļ��Ĵ�С
	//	��Χ:			0~2^64
	//	ʹ�ú���:			GetFileSIzeEX()
	//////////////////////////////////////////////////////////////////////////
	unsigned __int64 i64FileSize;
	LARGE_INTEGER intbuffer;

	// ����WriteFile()��ReadFile(),�洢��д��������
	DWORD NumberOfBytesRead  = 0;
	DWORD NumberOfBytesWrite = 0;


	#pragma region �������������,����д������
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

	#pragma region ������ȡ�ļ��ľ��,���ڴӱ��ص�WriteBuffer.txt�ж�ȡ����
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

	# pragma region ��512�ֽ�Ϊ��λ,��ȡ1��λ�����ݵ����ݻ�����Buffer��
	// ID:			FUNC_THREAD_WRITE_BCHECK_READFILE_1				READFILE_1
	bCheck = ReadFile(hReadFile, buffer, IOUNIT, &NumberOfBytesRead,  NULL);

	if (bCheck == READFILE_FAIL)
	{
		ERROROUT.Format(_T("%d\r\n��ȡWriteBuffer��ReadFile��������"),
						GetLastError());
		PostMessage(lpDP->hWindow, WM_THREAD_ALERT, 
					WM_THREAD_WRITE_READFILE_ERROR, 0);
		return;
	}

	# pragma endregion 

	// �رվ�� hReadFile ,��ʣ��� hWriteDev û�йر�
	CloseHandle(hReadFile); 

	i64FileSize = 0;

	#pragma region ѭ��д��55AA�����ݵ��洢������,�ܹ�512�ֽ�
	while (i64FileSize < lpDP->DevSize)
	{
		//ID:			FUNC_THREAD_WRITE_BCHECK_WRITEFILE_1		WRITEFILE_1
		bCheck = WriteFile(hWriteDev, buffer, IOUNIT, &NumberOfBytesWrite, NULL);

		if (bCheck == WRITEFILE_FAIL)
		{
			ERROROUT.Format(_T("%d\r\nд����������WriteFile��������"), 
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

	// �رվ�� hWriteDev ,���о��ȫ���ر�
	CloseHandle(hWriteDev);

	// �����̷߳��ͳɹ�����Ϣ
	PostMessage(lpDP->hWindow, WM_THREAD_WRITE_SUCCESS, 0, 0);



	# pragma endregion 

	# pragma region ��������


# pragma region ����hReadDev���,���ڴӴ洢�����ж�ȡ����
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

# pragma region �������hWriteFile,�����򱾵ص�ReadFile
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

#pragma region ��ǧ�ֽ�Ϊ��λ,ѭ���Ӵ洢�����ж�ȡ,ͬʱ��ReadFile.txtд��1KB������
	while(i64FileSize < lpDP->DevSize)
	{
		// ID:		FUNC_THREAD_READ_BCHECK_READFILE_2				READFILE_2
		bCheck = ReadFile(hReadDev, buffer, IOUNIT, &NumberOfBytesRead, NULL);

		// �������,������Ϣ�����߳�
		if (bCheck == READFILE_FAIL)
		{
			ERROROUT.Format(_T("%d\r\n��ȡU�̵�ReadFile��������"), GetLastError());
			PostMessage(lpDP->hWindow, WM_THREAD_ALERT, 
						WM_THREAD_READ_READFILE_ERROR, 0);
			return;
		}

		// ID:			FUNC_THREAD_READ_BCHECK_WRITEFILE_2				WRITEFILE_2
		bCheck = WriteFile(hWriteFile, buffer, IOUNIT, &NumberOfBytesWrite, NULL);

		// �������,������Ϣ�����߳�
		if (bCheck == WRITEFILE_FAIL)
		{
			ERROROUT.Format(_T("%d\r\nд�뱾���ı���WriteFile��������"), 
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

	// �����̷߳��ͳɹ�����Ϣ
	PostMessage(lpDP->hWindow, WM_THREAD_ALERT, WM_THREAD_READ_SUCCESS, 0);

	// �ر���ǰ�洴�����������
	CloseHandle(hWriteFile);
	CloseHandle(hReadDev);
	


	# pragma endregion 
}

void WriteDevThread(LPVOID lpParam)
{
	// ��ȡ�ṹ���е�����,��ŵ�ָ����
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
		AfxMessageBox(_T("hWriteDev�������ʧ��"));
		return;
	}
	hReadFile = CreateFile(	_T("WriteBuffer.txt"), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(hReadFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("hReadFile�������ʧ��"));
		return;
	}

	bCheck = ReadFile(hReadFile, byBuffer, IOUNIT, &iBytesRead, NULL);
	CloseHandle(hReadFile);

	if (bCheck == READFILE_FAIL)
	{
		AfxMessageBox(_T("ReadFile���� by DevWriteThread"));
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
			AfxMessageBox(_T("WriteFile���� by DevWriteThread"));
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
		AfxMessageBox(_T("hReadDev�������ʧ��"));
		return;
	}

	hWriteFile = CreateFile(_T("ReadFile.txt"), GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, 0, NULL);
	if (hWriteFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("hWriteFile�������ʧ��"));
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
				AfxMessageBox(_T("ReadFile ���� by DevReadThread"));
				return;
			}

			WriteFile(hWriteFile, byBuffer, IOUNIT, &iBytesWritten, NULL);
			if(bCheck = WRITEFILE_FAIL)
			{
				AfxMessageBox(_T("WriteFile ���� by DevReadThread"));
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
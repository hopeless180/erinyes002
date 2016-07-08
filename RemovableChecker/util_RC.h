#pragma once

//////////////////////////////////////////////////////////////////////////
//	����ֿ鹦��
//	ʹ�÷���:
//		#pragma region NAME
//		��������
//		#pragma endregion NAME
//	ǰ��:		ò����ҪVC8����
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//		�������ռ䷶Χ�����ֵ
//		���ó�0~100,����԰ٷֱȵ���ʽ���
//////////////////////////////////////////////////////////////////////////
#define PROGRESS_RANGE_MAX								(100)


//////////////////////////////////////////////////////////////////////////
//		����WriteFIle��ReadFile�����ݻ�����Buffer�Ĵ�С�����ڳ�ʼ�������ڴ�ռ�
//////////////////////////////////////////////////////////////////////////
#define BUFFER_SIZE										(512)


//////////////////////////////////////////////////////////////////////////
//		1 Kibibyte = 1024 bytes��������������д�Ļ�����λ
//////////////////////////////////////////////////////////////////////////
#define KIBIBYTE										(1024)

//////////////////////////////////////////////////////////////////////////
//		1 Sector   = 512 Bytes 
//		  0X55AA   = 4	 Bytes
//		�������������ƶ��洢���ʵĻ�����λ
//////////////////////////////////////////////////////////////////////////
#define IOUNIT											(512)


//////////////////////////////////////////////////////////////////////////
//		BOOL��������ر������ض���
//////////////////////////////////////////////////////////////////////////
#define WRITEFILE_FAIL									(0)
#define READFILE_FAIL									(0)
#define WRITEFILE_SUCCESS								(1)
#define READFILE_SUCCESS								(1)
#define FINDDEVICEPATH_SUCCESS							(1)
#define	FINDDEVICEPATH_FAIL								(0)
#define MYFUNC_SUCCESS									(1)
#define MYFUNC_FAIL										(0)

//		��������,����Ϊ100MB
#define TESTMAXIUM										(100*1024*1024)

//////////////////////////////////////////////////////////////////////////
//	����ѡ��������еķ�ʽ
//////////////////////////////////////////////////////////////////////////
#define MYMETHOD_UNIFYTHREAD							(1)
#define MYMETHOD_SEPARATETHREAD							(2)
#define MYMETHOD_NULL									(0)

//////////////////////////////////////////////////////////////////////////
//		��Ϣ:
//      ����_�ļ�_�������ߺ���_��Ϣ����1_��Ϣ����2
//////////////////////////////////////////////////////////////////////////
#define WM_THREAD_PROGRESS_STATUS						(WM_USER+100)
#define WM_THREAD_WRITE_PROGRESS_SETPOS					(WM_USER+101)
#define WM_THREAD_READ_PROGRESS_SETPOS					(WM_USER+102)
#define WM_THREAD_ALERT									(WM_USER+200)
#define WM_THREAD_HWRITEDEV_CREATEFILE_ERROR			(WM_USER+201)
#define WM_THREAD_HREADFILE_CREATEFILE_ERROR			(WM_USER+202)
#define WM_THREAD_WRITE_READFILE_ERROR					(WM_USER+203)
#define WM_THREAD_WRITE_WRITEFILE_ERROR					(WM_USER+204)
#define WM_THREAD_WRITE_SUCCESS							(WM_USER+205)
#define WM_THREAD_HREADDEV_CREATEFILE_ERROR				(WM_USER+206)
#define WM_THREAD_HWRITEFILE_CREATEFILE_ERROR			(WM_USER+207)
#define WM_THREAD_READ_READFILE_ERROR					(WM_USER+208)
#define WM_THREAD_READ_WRITEFILE_ERROR					(WM_USER+209)
#define	WM_THREAD_READ_SUCCESS							(WM_USER+210)



//////////////////////////////////////////////////////////////////////////
//	�������ULARGE_INTEGER����ת�����ʺ��û��鿴��Gibibyte����
//	���������		ULARGE_INTEGER x
//	���أ�			С�������2λ�ĸ�����,��λ��Gibibyte
//////////////////////////////////////////////////////////////////////////
#define GB(x) ((x.HighPart << 2) + ((DOUBLE)x.LowPart) / 1024 / 1024 / 1024)
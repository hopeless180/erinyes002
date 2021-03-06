#pragma once

//////////////////////////////////////////////////////////////////////////
//	代码分块功能
//	使用方法:
//		#pragma region NAME
//		代码内容
//		#pragma endregion NAME
//	前提:		貌似需要VC8以上
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//		进度条空间范围的最大值
//		设置成0~100,结果以百分比的形式表达
//////////////////////////////////////////////////////////////////////////
#define PROGRESS_RANGE_MAX								(100)


//////////////////////////////////////////////////////////////////////////
//		用于WriteFIle和ReadFile的数据缓冲区Buffer的大小，用于初始化申请内存空间
//////////////////////////////////////////////////////////////////////////
#define BUFFER_SIZE										(512)


//////////////////////////////////////////////////////////////////////////
//		1 Kibibyte = 1024 bytes，在这里用作读写的基本单位
//////////////////////////////////////////////////////////////////////////
#define KIBIBYTE										(1024)

//////////////////////////////////////////////////////////////////////////
//		1 Sector   = 512 Bytes 
//		  0X55AA   = 4	 Bytes
//		在这里做测试移动存储介质的基本单位
//////////////////////////////////////////////////////////////////////////
#define IOUNIT											(512)


//////////////////////////////////////////////////////////////////////////
//		BOOL变量及相关变量的重定义
//////////////////////////////////////////////////////////////////////////
#define WRITEFILE_FAIL									(0)
#define READFILE_FAIL									(0)
#define WRITEFILE_SUCCESS								(1)
#define READFILE_SUCCESS								(1)
#define FINDDEVICEPATH_SUCCESS							(1)
#define	FINDDEVICEPATH_FAIL								(0)
#define MYFUNC_SUCCESS									(1)
#define MYFUNC_FAIL										(0)

//		测试用量,现在为100MB
#define TESTMAXIUM										(100*1024*1024)

//////////////////////////////////////////////////////////////////////////
//	用于选择程序运行的方式
//////////////////////////////////////////////////////////////////////////
#define MYMETHOD_UNIFYTHREAD							(1)
#define MYMETHOD_SEPARATETHREAD							(2)
#define MYMETHOD_NULL									(0)

//////////////////////////////////////////////////////////////////////////
//		消息:
//      种类_文件_程序块或者函数_消息描述1_消息描述2
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
//	将传入的ULARGE_INTEGER数据转换成适合用户查看的Gibibyte类型
//	传入参数：		ULARGE_INTEGER x
//	返回：			小数点后有2位的浮点数,单位是Gibibyte
//////////////////////////////////////////////////////////////////////////
#define GB(x) ((x.HighPart << 2) + ((DOUBLE)x.LowPart) / 1024 / 1024 / 1024)

// RemovableCheckerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "util_RC.h"
#include "afxdialogex.h"




	
// CRemovableCheckerDlg �Ի���
class CRemovableCheckerDlg : public CDialogEx
{
// ����
public:
	CRemovableCheckerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REMOVABLECHECKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnCbnSelchangeComboDevice();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonFinish();
	afx_msg void OnBnClickedButtonWTPause();
	afx_msg void OnBnClickedButtonWTGoon();
	afx_msg void OnBnClickedButtonRTGoon();
	afx_msg void OnBnClickedButtonRTPause();
	afx_msg LRESULT OnTdChangePos(WPARAM w,LPARAM l);
	afx_msg LRESULT OnAMDisplayInfo(WPARAM w, LPARAM l);

	
private:
	wchar_t			cSelectedDeviceName;
	CString			strOutput;
	ULARGE_INTEGER	ulDeviceSpace;
	DWORD			NumberOfDeviceRemovable;
	CStringArray	DevicePathArray;
	BOOL			bCheck;
	HANDLE hThreadWrite;
	HANDLE hThreadRead;
protected:
	HANDLE hThread;
	DWORD ThreadID1;
	DWORD ThreadID2;
public:
	CButton m_bStart;
	CComboBox m_combobox;
	CEdit m_edit;
	CButton m_bRefresh;
	CProgressCtrl m_pWrite;
	CProgressCtrl m_pRead;
	CButton m_bFinish;
	CButton m_bRTGoon;
	CButton m_bRTPause;
	CButton m_bWTGoon;
	CButton m_bWTPause;
};


#pragma once


// ChooseMethodDlg �Ի���

class ChooseMethodDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ChooseMethodDlg)

public:
	ChooseMethodDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ChooseMethodDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_CHOOSEMETHOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonUnifythread();
};

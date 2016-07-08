#pragma once


// ChooseMethodDlg 对话框

class ChooseMethodDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ChooseMethodDlg)

public:
	ChooseMethodDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ChooseMethodDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHOOSEMETHOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonUnifythread();
};

// ChooseMethodDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemovableChecker.h"
#include "ChooseMethodDlg.h"
#include "afxdialogex.h"

extern DWORD dwMethod;

// ChooseMethodDlg �Ի���

IMPLEMENT_DYNAMIC(ChooseMethodDlg, CDialogEx)

ChooseMethodDlg::ChooseMethodDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ChooseMethodDlg::IDD, pParent)
{

}

ChooseMethodDlg::~ChooseMethodDlg()
{
}

void ChooseMethodDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ChooseMethodDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_UNIFYTHREAD, &ChooseMethodDlg::OnBnClickedButtonUnifythread)
END_MESSAGE_MAP()


// ChooseMethodDlg ��Ϣ�������


void ChooseMethodDlg::OnBnClickedButtonUnifythread()
{
	dwMethod = MYMETHOD_UNIFYTHREAD;
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

// SidLoginDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include "SidLoginDlg.h"
#include "afxdialogex.h"
#include"setup.h"
#include"websocket.h"

// SidLoginDlg 对话框

IMPLEMENT_DYNAMIC(SidLoginDlg, CDialogEx)

SidLoginDlg::SidLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIDLOGINDIALOG, pParent)
{

}

SidLoginDlg::~SidLoginDlg()
{
}

void SidLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SIDEDIT, m_SidEdit);
}


BEGIN_MESSAGE_MAP(SidLoginDlg, CDialogEx)
	ON_BN_CLICKED(ID_SIDLOGIN, &SidLoginDlg::OnBnClickedSidlogin)
END_MESSAGE_MAP()


// SidLoginDlg 消息处理程序


void SidLoginDlg::OnBnClickedSidlogin()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sid;
	m_SidEdit.GetWindowText(sid);
	gConfig.sid = CW2A(sid.GetString());
	for (int i = 0; i < gGamer.size(); i++) {
		gGamer[i]->start();
	}
	CDialog::OnOK();
}


BOOL SidLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_SidEdit.SetWindowText(CString(gConfig.sid.c_str()));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


// bbngameDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include "bbngameDlg.h"
#include "afxdialogex.h"
#include <winsock2.h>
#include<WS2tcpip.h>
#include <string>
#include "httpsocket.h"
#include "setup.h"
#include"rule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CbbngameDlg 对话框



CbbngameDlg::CbbngameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BBNGAME_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CbbngameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAME_EDIT, m_NameEdit);
	DDX_Control(pDX, IDC_PASSWD_EDIT, m_PasswdEdit);
}

BEGIN_MESSAGE_MAP(CbbngameDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CbbngameDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CbbngameDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CbbngameDlg 消息处理程序

BOOL CbbngameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	if (!gConfig.getNetConfig()) {
		MessageBox(_T("读入配置文件出错"), _T("出错!"));
		OnCancel();		
	}
	gRuleConfig.init();
	gConfig.readConfig();
	if (gConfig.userName.size() > 0)	m_NameEdit.SetWindowText(CString(gConfig.userName.c_str()));

	//测试时用，以后删去
	m_NameEdit.SetWindowText(_T("test"));
	m_PasswdEdit.SetWindowText(_T("test"));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CbbngameDlg::OnPaint()
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
HCURSOR CbbngameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CbbngameDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString nameStr;
	CString pwdStr;

	
	m_NameEdit.GetWindowText(nameStr);
	m_PasswdEdit.GetWindowText(pwdStr);

	if (nameStr.GetLength() == 0 || pwdStr.GetLength() == 0) {
		MessageBox(_T("账号密码不能为空"), _T(""));
		return;
	}
	gConfig.userName = CT2A(nameStr.GetBuffer());
	std::string pwd = CT2A(pwdStr.GetBuffer());

	HttpSocket *http;
	http = new HttpSocket(gConfig.dns, gConfig.port);
	int ret = http->login(gConfig.userName, pwd);
	delete http;

	if (ret < 0) {
		MessageBox(_T("连接失败"), _T(""));
	}
	else if (ret == 1) {
		MessageBox(_T("服务器异常"), _T(""));
	}
	else if (ret == 2) {
		MessageBox(_T("账号不存在"), _T(""));
	}
	else if (ret == 3) {
		MessageBox(_T("密码错误"), _T(""));
	}
	else if (ret == 4) {
		MessageBox(_T("账号过期"), _T(""));
	}
	else if (ret == 0) {
		CDialogEx::OnOK();
		gamedlg.DoModal();
	}
	else {
		MessageBox(_T("登录失败"), _T(""));
	}
	//CDialogEx::OnOK();
}


void CbbngameDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}

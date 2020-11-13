// CSetupRuleDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include "CRuleDlg.h"
#include "CMoneyRuleDlg.h"
#include "CSetupRuleDlg.h"
#include "afxdialogex.h"


// CSetupRuleDlg 对话框

IMPLEMENT_DYNAMIC(CSetupRuleDlg, CDialogEx)

CSetupRuleDlg::CSetupRuleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RULESETDIALOG, pParent)
{

}

CSetupRuleDlg::~CSetupRuleDlg()
{
}

void CSetupRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RULESETTAB, m_RuleSetTab);
}


BEGIN_MESSAGE_MAP(CSetupRuleDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_RULESETTAB, &CSetupRuleDlg::OnTcnSelchangeRulesettab)
END_MESSAGE_MAP()


// CSetupRuleDlg 消息处理程序


BOOL CSetupRuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect tabRect;    // 标签控件客户区的Rect   
   // 获取标签控件客户区Rect，并对其调整，以适合放置标签页   
	m_RuleSetTab.GetClientRect(&tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 40;
	tabRect.bottom -= 1;

	CSize size, size1;
	size.cx = 150;
	size.cy = 40;
	size1 = m_RuleSetTab.SetItemSize(size);

	CFont m_tabFont;
	m_tabFont.CreatePointFont(200, _T("宋书"));
	m_RuleSetTab.SetFont(&m_tabFont);
	m_RuleSetTab.InsertItem(0, _T("切入点设置"));
	m_RuleSetTab.InsertItem(1, _T("注码设置"));

	m_BetRuleDlg = new CRuleDlg(this);
	m_BetRuleDlg->Create(IDD_RULEDLG, &m_RuleSetTab);
	m_BetRuleDlg->SetWindowPos(
		NULL,
		tabRect.left,
		tabRect.top,
		tabRect.Width(),
		tabRect.Height(),
		SWP_SHOWWINDOW);

	m_MoneyRuleDlg = new CMoneyRuleDlg(this);
	m_MoneyRuleDlg->Create(IDD_MONEYRULEDLG, &m_RuleSetTab);
	m_MoneyRuleDlg->SetWindowPos(
		NULL,
		tabRect.left,
		tabRect.top,
		tabRect.Width(),
		tabRect.Height(),
		SWP_HIDEWINDOW);

	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSetupRuleDlg::OnTcnSelchangeRulesettab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CRect tabRect;    // 标签控件客户区的Rect   
   // 获取标签控件客户区Rect，并对其调整，以适合放置标签页   
	m_RuleSetTab.GetClientRect(&tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 40;
	tabRect.bottom -= 1;

	switch (m_RuleSetTab.GetCurSel()){
		case 0:
			m_BetRuleDlg->SetWindowPos(
				NULL,
				tabRect.left,
				tabRect.top,
				tabRect.Width(),
				tabRect.Height(),
				SWP_SHOWWINDOW);
			m_MoneyRuleDlg->SetWindowPos(
				NULL,
				tabRect.left,
				tabRect.top,
				tabRect.Width(),
				tabRect.Height(),
				SWP_HIDEWINDOW);
			break;
		case 1:
			m_BetRuleDlg->SetWindowPos(
				NULL,
				tabRect.left,
				tabRect.top,
				tabRect.Width(),
				tabRect.Height(),
				SWP_HIDEWINDOW);
			m_MoneyRuleDlg->SetWindowPos(
				NULL,
				tabRect.left,
				tabRect.top,
				tabRect.Width(),
				tabRect.Height(),
				SWP_SHOWWINDOW);
			break;
		default:
			break;
	}
}



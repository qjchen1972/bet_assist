// CMoneyRuleDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include "CMoneyRuleDlg.h"
#include"rule.h"
#include "afxdialogex.h"


// CMoneyRuleDlg 对话框

IMPLEMENT_DYNAMIC(CMoneyRuleDlg, CDialogEx)

CMoneyRuleDlg::CMoneyRuleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MONEYRULEDLG, pParent)
{
	m_pParent = pParent;
}

CMoneyRuleDlg::~CMoneyRuleDlg()
{
}

void CMoneyRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MONEYLIST, m_MoneyList);
	DDX_Control(pDX, IDC_MONEYDLG_MONEYEDIT, m_MoneyEdit);
	DDX_Control(pDX, IDC_MONEYDLG_WINEDIT, m_WinNumEdit);
	DDX_Control(pDX, IDC_MONEYDL_LOSEEDIT, m_LoseNumEdit);
	DDX_Control(pDX, IDC_MONEYDL_MIDEDIT, m_MidNumEdit);
	DDX_Control(pDX, IDC_NEWBETRULEFILEEDIT, m_NewBetFileEdit);
	DDX_Control(pDX, IDC_BETRULECOMBO, m_BetRuleCombo);
	DDX_Control(pDX, IDC_ADDMONEYBTN, m_AddMoneyBtn);
	DDX_Control(pDX, IDC_UPDATEMONEYBTN, m_UpdateMoneyBtn);
	DDX_Control(pDX, IDC_DELMONEYBTN, m_DelMoneyBtn);
}


BEGIN_MESSAGE_MAP(CMoneyRuleDlg, CDialogEx)
	ON_BN_CLICKED(IDC_NEWBETRULEBTN, &CMoneyRuleDlg::OnBnClickedNewbetrulebtn)
	ON_BN_CLICKED(IDC_ADDMONEYBTN, &CMoneyRuleDlg::OnBnClickedAddmoneybtn)
	ON_BN_CLICKED(IDC_UPDATEMONEYBTN, &CMoneyRuleDlg::OnBnClickedUpdatemoneybtn)
	ON_BN_CLICKED(IDC_DELMONEYBTN, &CMoneyRuleDlg::OnBnClickedDelmoneybtn)
	ON_BN_CLICKED(IDC_CLOSEBETRULEBTN, &CMoneyRuleDlg::OnBnClickedClosebetrulebtn)
	ON_BN_CLICKED(IDC_SAVENEWBETRULEBTN, &CMoneyRuleDlg::OnBnClickedSavenewbetrulebtn)
	ON_NOTIFY(NM_CLICK, IDC_MONEYLIST, &CMoneyRuleDlg::OnNMClickMoneylist)
	ON_CBN_SELCHANGE(IDC_BETRULECOMBO, &CMoneyRuleDlg::OnCbnSelchangeBetrulecombo)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_DELOLDBETRULEBTN, &CMoneyRuleDlg::OnBnClickedDeloldbetrulebtn)
END_MESSAGE_MAP()


// CMoneyRuleDlg 消息处理程序


BOOL CMoneyRuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	m_MoneyList.GetClientRect(&rect);
	
	m_MoneyList.SetExtendedStyle(m_MoneyList.GetExtendedStyle() | LVS_EX_GRIDLINES |
		LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	m_MoneyList.SetTextBkColor(RGB(255, 255, 255)); //设置文本背景颜色
	m_MoneyList.SetTextColor(RGB(10, 10, 80)); //设置文本颜色

	CFont m_oFont;
	m_oFont.CreatePointFont(210, _T("隶书"));
	m_MoneyList.SetFont(&m_oFont);
	m_MoneyEdit.SetFont(&m_oFont);
	m_WinNumEdit.SetFont(&m_oFont);
	m_LoseNumEdit.SetFont(&m_oFont);
	m_MidNumEdit.SetFont(&m_oFont);

	CFont m_tFont;
	m_tFont.CreatePointFont(190, _T("宋书"));
	m_NewBetFileEdit.SetFont(&m_tFont);

	//m_DoorList.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	m_MoneyList.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 100);	//插入第1列的列名
	m_MoneyList.InsertColumn(1, _T("下注金额"), LVCFMT_LEFT, 100); // 插入第2列的列名
	m_MoneyList.InsertColumn(2, _T("赢跳转"), LVCFMT_LEFT, 100); // 插入第2列的列名
	m_MoneyList.InsertColumn(3, _T("输跳转"), LVCFMT_LEFT, 100); // 插入第2列的列名
	m_MoneyList.InsertColumn(4, _T("和跳转"), LVCFMT_LEFT, rect.Width() - 400); // 插入第2列的列名
	
	for (int i = 0; i < gRuleConfig.betRuleFileList.size(); i++) {
		CString  str(gRuleConfig.betRuleFileList[i].c_str());
		m_BetRuleCombo.InsertString(i, str);
	}
	procInit();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CMoneyRuleDlg::procInit() {
	m_UpdateMoneyBtn.EnableWindow(false);
	m_MoneyEdit.SetWindowText(_T(""));
	m_WinNumEdit.SetWindowText(_T(""));
	m_LoseNumEdit.SetWindowText(_T(""));
	m_MidNumEdit.SetWindowText(_T(""));
	m_curRow = -1;
}

void CMoneyRuleDlg::GetRuleFromFile(int index) {

	m_BetRuleCombo.SetCurSel(index);
	std::vector<betRule_t> rules;
	gRuleConfig.readBetRuleFile(gRuleConfig.betRuleFileList[index], rules);
	m_MoneyList.DeleteAllItems();
	for (int i = 0; i < rules.size(); i++) {
		m_MoneyList.InsertItem(i, _T(""));
		CString str;
		str.Format(_T("%d"), rules[i].id);
		m_MoneyList.SetItemText(i, 0, str);
		str.Format(_T("%d"), rules[i].money);
		m_MoneyList.SetItemText(i, 1, str);
		str.Format(_T("%d"), rules[i].winId);
		m_MoneyList.SetItemText(i, 2, str);
		str.Format(_T("%d"), rules[i].loseId);
		m_MoneyList.SetItemText(i, 3, str);
		str.Format(_T("%d"), rules[i].midId);
		m_MoneyList.SetItemText(i, 4, str);
	}
	CString str(gRuleConfig.betRuleFileList[index].c_str());
	m_NewBetFileEdit.SetWindowText(str);
}


void CMoneyRuleDlg::OnBnClickedNewbetrulebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MoneyList.DeleteAllItems();
	m_BetRuleCombo.SetCurSel(-1);
	m_NewBetFileEdit.SetWindowText(_T(""));
	procInit();
}


void CMoneyRuleDlg::OnBnClickedAddmoneybtn()
{
	// TODO: 在此添加控件通知处理程序代码
	int size = m_MoneyList.GetItemCount();
	m_MoneyList.InsertItem(size, _T(""));

	CString str;
	str.Format(_T("%d"), size);
	m_MoneyList.SetItemText(size, 0, str);

	m_MoneyEdit.GetWindowText(str);
	m_MoneyList.SetItemText(size, 1, str);
	m_WinNumEdit.GetWindowText(str);
	m_MoneyList.SetItemText(size, 2, str);
	m_LoseNumEdit.GetWindowText(str);
	m_MoneyList.SetItemText(size, 3, str);
	m_MidNumEdit.GetWindowText(str);
	m_MoneyList.SetItemText(size, 4, str);
	procInit();
}


void CMoneyRuleDlg::OnBnClickedUpdatemoneybtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_MoneyEdit.GetWindowText(str);
	m_MoneyList.SetItemText(m_curRow, 1, str);
	m_WinNumEdit.GetWindowText(str);
	m_MoneyList.SetItemText(m_curRow, 2, str);
	m_LoseNumEdit.GetWindowText(str);
	m_MoneyList.SetItemText(m_curRow, 3, str);
	m_MidNumEdit.GetWindowText(str);
	m_MoneyList.SetItemText(m_curRow, 4, str);
	procInit();
}

void CMoneyRuleDlg::resetId() {

	for (int i = 0; i < m_MoneyList.GetItemCount(); i++) {
		CString str;
		str.Format(_T("%d"), i);
		m_MoneyList.SetItemText(i, 0, str);
	}
}

void CMoneyRuleDlg::OnBnClickedDelmoneybtn()
{
	// TODO: 在此添加控件通知处理程序代码
	int size = m_MoneyList.GetItemCount();
	for (int i = size - 1; i >= 0; i--) {
		if (m_MoneyList.GetCheck(i)) m_MoneyList.DeleteItem(i);
	}
	resetId();
	procInit();
}


void CMoneyRuleDlg::OnBnClickedClosebetrulebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	::SendMessage(m_pParent->GetSafeHwnd(), WM_CLOSE, 0, 0);
}


void CMoneyRuleDlg::OnBnClickedSavenewbetrulebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	if (m_MoneyList.GetItemCount() == 0) {
		MessageBox(_T("空的规则"), _T(""));
		return;
	}
	CString str;
	m_NewBetFileEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		MessageBox(_T("存盘文件为空"), _T(""));
		return;
	}
	std::string  name = CT2A(str.GetBuffer());
	bool isSave = false;
	int oldIndex = -1;
	for (int i = 0; i < gRuleConfig.betRuleFileList.size(); i++) {
		if (name == gRuleConfig.betRuleFileList[i]) {
			isSave = true;
			oldIndex = i;
			break;
		}
	}

	if (isSave) {
		if (MessageBox(
			_T("切入码文件已经存在,需要继续吗？"),
			_T("警告"),
			MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
			return;
	}

	std::vector<betRule_t> rules;
	for (int i = 0; i < m_MoneyList.GetItemCount(); i++) {
		betRule_t one;
		one.id = atoi(CT2A(m_MoneyList.GetItemText(i, 0).GetBuffer()));
		one.money = atoi(CT2A(m_MoneyList.GetItemText(i, 1).GetBuffer()));
		one.winId = atoi(CT2A(m_MoneyList.GetItemText(i, 2).GetBuffer()));
		one.loseId = atoi(CT2A(m_MoneyList.GetItemText(i, 3).GetBuffer()));
		one.midId = atoi(CT2A(m_MoneyList.GetItemText(i, 4).GetBuffer()));
		rules.push_back(one);
	}
	gRuleConfig.writeBetRuleFile(name, rules);
	if (!isSave) {
		CString  str(name.c_str());
		int index = m_BetRuleCombo.GetCount();
		m_BetRuleCombo.InsertString(index, str);
		gRuleConfig.betRuleFileList.push_back(name);
		oldIndex = index;
	}
	GetRuleFromFile(oldIndex);
	procInit();
	MessageBox(_T("保存规则成功"), _T(""));	
}



void CMoneyRuleDlg::OnNMClickMoneylist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	if (pNMItemActivate->iItem >= m_MoneyList.GetItemCount()) return;	
	m_MoneyEdit.SetWindowText(m_MoneyList.GetItemText(pNMItemActivate->iItem, 1));
	m_WinNumEdit.SetWindowText(m_MoneyList.GetItemText(pNMItemActivate->iItem, 2));
	m_LoseNumEdit.SetWindowText(m_MoneyList.GetItemText(pNMItemActivate->iItem, 3));
	m_MidNumEdit.SetWindowText(m_MoneyList.GetItemText(pNMItemActivate->iItem, 4));
	m_curRow = pNMItemActivate->iItem;
	m_UpdateMoneyBtn.EnableWindow(true);
}


void CMoneyRuleDlg::OnCbnSelchangeBetrulecombo()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_BetRuleCombo.GetCurSel();
	GetRuleFromFile(nIndex);
	procInit();
}


void CMoneyRuleDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(rect, RGB(234, 242, 255));
}


void CMoneyRuleDlg::OnBnClickedDeloldbetrulebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = m_BetRuleCombo.GetCurSel();
	if (index < 0) {
		MessageBox(_T("没有选中规则"), _T(""));
		return;
	}
	if (MessageBox(
		_T("将永久删去注码文件,需要继续吗？"),
		_T("警告"),
		MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
		return;


	gRuleConfig.delBetRuleFile(gRuleConfig.betRuleFileList[index]);
	gRuleConfig.loadBetRuleFileList();
	m_BetRuleCombo.ResetContent();
	for (int i = 0; i < gRuleConfig.betRuleFileList.size(); i++) {
		CString  str(gRuleConfig.betRuleFileList[i].c_str());
		m_BetRuleCombo.InsertString(i, str);
	}
	m_MoneyList.DeleteAllItems();
	m_BetRuleCombo.SetCurSel(-1);
	m_NewBetFileEdit.SetWindowText(_T(""));
	procInit();
}

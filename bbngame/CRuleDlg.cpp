// CRuleDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include "CRuleDlg.h"
#include "afxdialogex.h"
#include"rule.h"

// CRuleDlg 对话框

IMPLEMENT_DYNAMIC(CRuleDlg, CDialogEx)

CRuleDlg::CRuleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RULEDLG, pParent)
{
	m_pParent = pParent;
}

CRuleDlg::~CRuleDlg()
{
}

void CRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DOORLIST, m_DoorList);
	DDX_Control(pDX, IDC_RULEFileCOMBO, m_RuleFileComb);
	DDX_Control(pDX, IDC_RULEFileEDIT, m_SaveRuleNameEdit);
	DDX_Control(pDX, IDC_DELRULEBTN, m_DelRuleBtn);
	DDX_Control(pDX, IDC_UPDATERULEBTN, m_UpdateRuleBtn);
	DDX_Control(pDX, IDC_RULEDLG_RULEEDIT, m_RuleEdit);
	DDX_Control(pDX, IDC_RULEDLG_BETEDIT, m_BetEdit);
	DDX_Control(pDX, IDC_ADDRULEBTN, m_AddRuleBtn);
	DDX_Control(pDX, IDC_DELOLDRULEBTN, m_DelOldRuleBtn);
	DDX_Control(pDX, IDC_TIPEDIT, m_TipEdit);
}


BEGIN_MESSAGE_MAP(CRuleDlg, CDialogEx)
	ON_BN_CLICKED(IDC_ADDRULEBTN, &CRuleDlg::OnBnClickedAddrulebtn)
	ON_BN_CLICKED(IDC_UPDATERULEBTN, &CRuleDlg::OnBnClickedUpdaterulebtn)
	ON_BN_CLICKED(IDC_DELRULEBTN, &CRuleDlg::OnBnClickedDelrulebtn)
	ON_NOTIFY(NM_CLICK, IDC_DOORLIST, &CRuleDlg::OnNMClickDoorlist)
	ON_BN_CLICKED(IDC_SAVENEWRULEBTN, &CRuleDlg::OnBnClickedSavenewrulebtn)
	ON_BN_CLICKED(IDC_CLOSERULEBTN, &CRuleDlg::OnBnClickedCloserulebtn)
	ON_CBN_SELCHANGE(IDC_RULEFileCOMBO, &CRuleDlg::OnCbnSelchangeRulefilecombo)
	ON_BN_CLICKED(IDC_NEWRULEBTN, &CRuleDlg::OnBnClickedNewrulebtn)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_DELOLDRULEBTN, &CRuleDlg::OnBnClickedDeloldrulebtn)
END_MESSAGE_MAP()


// CRuleDlg 消息处理程序


BOOL CRuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rect;
	m_DoorList.GetClientRect(&rect);

	// TODO:  在此添加额外的初始化

	m_DoorList.SetExtendedStyle(m_DoorList.GetExtendedStyle() | LVS_EX_GRIDLINES |
		LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_AUTOAUTOARRANGE);

	m_DoorList.SetTextBkColor(RGB(255, 255, 255)); //设置文本背景颜色
	m_DoorList.SetTextColor(RGB(10, 10, 80)); //设置文本颜色

	CFont m_oFont;
	m_oFont.CreatePointFont(210, _T("隶书"));
	
	m_RuleEdit.SetFont(&m_oFont);
	m_BetEdit.SetFont(&m_oFont);
	m_DoorList.SetFont(&m_oFont);
	

	CFont m_tFont;
	m_tFont.CreatePointFont(190, _T("宋书"));
	m_TipEdit.SetFont(&m_tFont);
	m_SaveRuleNameEdit.SetFont(&m_tFont);
	
	m_DoorList.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 100);	//插入第1列的列名
	m_DoorList.InsertColumn(1, _T("切入点"), LVCFMT_LEFT, 500); // 插入第2列的列名
	m_DoorList.InsertColumn(2, _T("下注方向"), LVCFMT_LEFT, rect.Width() - 600); // 插入第2列的列名

	for (int i = 0; i < gRuleConfig.inRuleFileList.size(); i++) {
		CString  str(gRuleConfig.inRuleFileList[i].c_str());
		m_RuleFileComb.InsertString(i, str);
	}
	procInit();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CRuleDlg::procInit() {
	m_UpdateRuleBtn.EnableWindow(false);
	m_RuleEdit.SetWindowText(_T(""));
	m_BetEdit.SetWindowText(_T(""));
	m_curRow = -1;
}

void CRuleDlg::resetId() {

	for (int i = 0; i < m_DoorList.GetItemCount(); i++) {
		CString str;
		str.Format(_T("%d"), i);
		m_DoorList.SetItemText(i, 0, str);
	}
}

void CRuleDlg::GetRuleFromFile(int index) {
	m_RuleFileComb.SetCurSel(index);

	std::vector<inRule_t> rules;
	gRuleConfig.readInRuleFile(gRuleConfig.inRuleFileList[index], rules);
	m_DoorList.DeleteAllItems();
	for (int i = 0; i < rules.size(); i++) {
		m_DoorList.InsertItem(i, _T(""));
		CString str;
		str.Format(_T("%d"), rules[i].id);
		m_DoorList.SetItemText(i, 0, str);
		m_DoorList.SetItemText(i, 1, CString(rules[i].scene.c_str()));
		m_DoorList.SetItemText(i, 2, CString(rules[i].action.c_str()));

	}
	CString str(gRuleConfig.inRuleFileList[index].c_str());
	m_SaveRuleNameEdit.SetWindowText(str);
}

void CRuleDlg::OnBnClickedAddrulebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	int size = m_DoorList.GetItemCount();
	m_DoorList.InsertItem(size, _T(""));
	CString str;
	str.Format(_T("%d"), size);
	m_DoorList.SetItemText(size, 0, str);
	m_RuleEdit.GetWindowText(str);
	m_DoorList.SetItemText(size, 1, str);
	m_BetEdit.GetWindowText(str);
	m_DoorList.SetItemText(size, 2, str);
	procInit();
}


void CRuleDlg::OnBnClickedUpdaterulebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_curRow < 0) return;
	CString str;
	m_RuleEdit.GetWindowText(str);
	m_DoorList.SetItemText(m_curRow, 1, str);
	m_BetEdit.GetWindowText(str);
	m_DoorList.SetItemText(m_curRow, 2, str);
	procInit();
}


void CRuleDlg::OnBnClickedDelrulebtn()
{
	// TODO: 在此添加控件通知处理程序代码

	int size = m_DoorList.GetItemCount();
	for (int i = size - 1; i >= 0; i--) {
		bool p = m_DoorList.GetCheck(i);
		if (p) m_DoorList.DeleteItem(i);
	}
	resetId();
	procInit();
}


void CRuleDlg::OnNMClickDoorlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pNMItemActivate->iItem >= m_DoorList.GetItemCount()) return;
	*pResult = 0;
	m_RuleEdit.SetWindowText(m_DoorList.GetItemText(pNMItemActivate->iItem, 1));
	m_BetEdit.SetWindowText(m_DoorList.GetItemText(pNMItemActivate->iItem, 2));
	m_curRow = pNMItemActivate->iItem;
	m_UpdateRuleBtn.EnableWindow(true);
}


void CRuleDlg::OnBnClickedSavenewrulebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_DoorList.GetItemCount() == 0) {
		MessageBox(_T("空的规则"), _T(""));
		return;
	}
	CString str;
	m_SaveRuleNameEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		MessageBox(_T("存盘文件为空"),_T(""));
		return;
	}
	std::string  name = CT2A(str.GetBuffer());	
	bool isSave = false;
	int oldIndex = -1;
	for (int i = 0; i < gRuleConfig.inRuleFileList.size(); i++) {
		if (name == gRuleConfig.inRuleFileList[i]) {
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

	std::vector<inRule_t> rules;
	for (int i = 0; i < m_DoorList.GetItemCount(); i++) {
		inRule_t one;
		one.id = atoi(CT2A(m_DoorList.GetItemText(i, 0).GetBuffer()));
		one.scene = CT2A(m_DoorList.GetItemText(i, 1).GetBuffer());
		one.action = CT2A(m_DoorList.GetItemText(i, 2).GetBuffer());
		rules.push_back(one);
	}
	gRuleConfig.writeInRuleFile(name, rules);	
	//m_DoorList.DeleteAllItems();
	if (!isSave) {
		CString  str(name.c_str());
		int index = m_RuleFileComb.GetCount();
		m_RuleFileComb.InsertString(index, str);
		gRuleConfig.inRuleFileList.push_back(name);
		oldIndex = index;
	}
	GetRuleFromFile(oldIndex);	
	procInit();
	MessageBox(_T("保存规则成功"), _T(""));	
}



void CRuleDlg::OnBnClickedCloserulebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	::SendMessage(m_pParent->GetSafeHwnd(), WM_CLOSE, 0, 0);
}


void CRuleDlg::OnCbnSelchangeRulefilecombo()
{
	// TODO: 在此添加控件通知处理程序代码

	int nIndex = m_RuleFileComb.GetCurSel();
	GetRuleFromFile(nIndex);
	procInit();
}


void CRuleDlg::OnBnClickedNewrulebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_DoorList.DeleteAllItems();
	m_RuleFileComb.SetCurSel(-1);
	m_SaveRuleNameEdit.SetWindowText(_T(""));
	procInit();
}


void CRuleDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(rect, RGB(234, 242, 255));

	m_TipEdit.SetWindowText(_T("请用小写输入 b=庄   p=闲   h=和"));
}


void CRuleDlg::OnBnClickedDeloldrulebtn()
{
	// TODO: 在此添加控件通知处理程序代码

	int index = m_RuleFileComb.GetCurSel();
	if (index < 0) {
		MessageBox(_T("没有选中规则"), _T(""));
		return;
	}
	if (MessageBox(
		_T("将永久删去切入码文件,需要继续吗？"),
		_T("警告"),
		MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
		return;


	gRuleConfig.delInRuleFile(gRuleConfig.inRuleFileList[index]);
	gRuleConfig.loadInRuleFileList();
	m_RuleFileComb.ResetContent();
	for (int i = 0; i < gRuleConfig.inRuleFileList.size(); i++) {
		CString  str(gRuleConfig.inRuleFileList[i].c_str());
		m_RuleFileComb.InsertString(i, str);
	}
	m_DoorList.DeleteAllItems();
	m_RuleFileComb.SetCurSel(-1);
	m_SaveRuleNameEdit.SetWindowText(_T(""));
	procInit();
}

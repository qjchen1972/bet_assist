// PartDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include "PartDlg.h"
#include "afxdialogex.h"
#include"userdata.h"

// PartDlg 对话框

IMPLEMENT_DYNAMIC(PartDlg, CDialogEx)

PartDlg::PartDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PARTDIALOG, pParent)
{

}

PartDlg::~PartDlg()
{
}

void PartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEATILLIST, m_DetailList);
}


BEGIN_MESSAGE_MAP(PartDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// PartDlg 消息处理程序


BOOL PartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	m_DetailList.GetClientRect(&rect);
	m_DetailList.SetExtendedStyle(m_DetailList.GetExtendedStyle() | LVS_EX_GRIDLINES |
		LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);

	m_DetailList.SetTextBkColor(RGB(255, 255, 255)); //设置文本背景颜色
	m_DetailList.SetTextColor(RGB(10, 10, 80)); //设置文本颜色

	CFont m_oFont;
	m_oFont.CreatePointFont(160, _T("隶书"));
	m_DetailList.SetFont(&m_oFont);

	m_DetailList.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 80);
	m_DetailList.InsertColumn(1, _T("开始时间"), LVCFMT_LEFT, 160);
	m_DetailList.InsertColumn(2, _T("结束时间"), LVCFMT_LEFT, 160);
	m_DetailList.InsertColumn(3, _T("现金收益"), LVCFMT_LEFT, 100);
	m_DetailList.InsertColumn(4, _T("流水"), LVCFMT_LEFT, 100);
	m_DetailList.InsertColumn(5, _T("下注次数"), LVCFMT_LEFT, 80);
	m_DetailList.InsertColumn(6, _T("赢的次数"), LVCFMT_LEFT, 80);
	m_DetailList.InsertColumn(7, _T("输的次数"), LVCFMT_LEFT, 80);
	m_DetailList.InsertColumn(8, _T("最高峰值"), LVCFMT_LEFT, 100);
	m_DetailList.InsertColumn(9, _T("最低峰值"), LVCFMT_LEFT, 100);
	m_DetailList.InsertColumn(10, _T("累计收益"), LVCFMT_LEFT, rect.Width() - 1040);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



void  PartDlg::showDetail() {

	std::vector<Stat_t> set;
	gUserData->getPartStat(set);

	if (set.size() == 0) return;
	Tool tool;

	float realProfit = 0;
	m_DetailList.DeleteAllItems();
	CString str;


	for (int i = 0; i < set.size(); i++) {

		m_DetailList.InsertItem(i, _T(""));
		str.Format(_T("%d"), i + 1);
		m_DetailList.SetItemText(i, 0, str);
		std::string temp = tool.getTime(set[i].startTime);
		m_DetailList.SetItemText(i, 1, CString(temp.c_str()));
		temp = tool.getTime(set[i].endTime);
		m_DetailList.SetItemText(i, 2, CString(temp.c_str()));
		str.Format(_T("%.2f"), set[i].profit);
		m_DetailList.SetItemText(i, 3, str);
		str.Format(_T("%.2f"), set[i].bet);
		m_DetailList.SetItemText(i, 4, str);
		str.Format(_T("%d"), set[i].betNum);
		m_DetailList.SetItemText(i, 5, str);
		str.Format(_T("%d"), set[i].winNum);
		m_DetailList.SetItemText(i, 6, str);
		str.Format(_T("%d"), set[i].loseNum);
		m_DetailList.SetItemText(i, 7, str);
		str.Format(_T("%.2f"), set[i].maxProfit);
		m_DetailList.SetItemText(i, 8, str);
		str.Format(_T("%.2f"), set[i].minProfit);
		m_DetailList.SetItemText(i, 9, str);
		realProfit += set[i].profit;
		str.Format(_T("%.2f"), realProfit);
		m_DetailList.SetItemText(i, 10, str);
	}
}



void PartDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	showDetail();
}

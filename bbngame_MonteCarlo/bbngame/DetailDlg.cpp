// DetailDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include "DetailDlg.h"
#include "afxdialogex.h"
#include"websocket.h"
#include"userdata.h"


// DetailDlg 对话框

IMPLEMENT_DYNAMIC(DetailDlg, CDialogEx)

DetailDlg::DetailDlg(int index, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DETAILDIALOG, pParent)
{
	nIndex = index;
}

DetailDlg::~DetailDlg()
{
}

void DetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEATILLIST, m_DetailList);
}


BEGIN_MESSAGE_MAP(DetailDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// DetailDlg 消息处理程序


BOOL DetailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// TODO:  在此添加额外的初始化
	CRect rect;
	m_DetailList.GetClientRect(&rect);
	m_DetailList.SetExtendedStyle(m_DetailList.GetExtendedStyle() | LVS_EX_GRIDLINES |
		LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);

	m_DetailList.SetTextBkColor(RGB(255, 255, 255)); //设置文本背景颜色
	m_DetailList.SetTextColor(RGB(10, 10, 80)); //设置文本颜色

	CFont m_oFont;
	m_oFont.CreatePointFont(160, _T("宋书"));
	m_DetailList.SetFont(&m_oFont);

	m_DetailList.InsertColumn(0, _T("游戏序号"), LVCFMT_LEFT, 80);
	m_DetailList.InsertColumn(1, _T("时间"), LVCFMT_LEFT, 160);
	m_DetailList.InsertColumn(2, _T("方向"), LVCFMT_LEFT, 60);
	m_DetailList.InsertColumn(3, _T("金额"), LVCFMT_LEFT, 100);
	m_DetailList.InsertColumn(4, _T("下注方式"), LVCFMT_LEFT, 80);
	m_DetailList.InsertColumn(5, _T("结果"), LVCFMT_LEFT, 100);
	m_DetailList.InsertColumn(6, _T("闲家牌"), LVCFMT_LEFT, 160);
	m_DetailList.InsertColumn(7, _T("庄家牌"), LVCFMT_LEFT, 160);
	m_DetailList.InsertColumn(8, _T("累计收益"), LVCFMT_LEFT, rect.Width() - 900);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void DetailDlg::showDetail() {
	std::vector<Round_t> set;
	if (!gGamer[nIndex]) return;
	if (!gGamer[nIndex]->pGameProcer) return;
	gGamer[nIndex]->pGameProcer->gameData.allProcer.getProfitSet(set);
	if (set.size() == 0) return;
	Tool tool;

	float realProfit = 0;
	m_DetailList.DeleteAllItems();
	CString str;

	for (int i = 0; i < set.size(); i++) {
		if (set[i].type != FAKEBET_TYPE && set[i].type != REALBET_TYPE) continue;
		m_DetailList.InsertItem(i, _T(""));

		m_DetailList.SetItemText(i, 0, CString(set[i].roundNo.c_str()));
		std::string temp = tool.getTime(set[i].time);
		m_DetailList.SetItemText(i, 1, CString(temp.c_str()));
		int bet;
		if (set[i].b1 > 0) {
			temp = "庄";
			bet = set[i].b1;
		}
		else if (set[i].b2 > 0) {
			temp = "闲";
			bet = set[i].b2;
		}
		else {
			temp = "和";
			bet = set[i].b3;
		}
		m_DetailList.SetItemText(i, 2, CString(temp.c_str()));
		
		str.Format(_T("%d"), bet);
		m_DetailList.SetItemText(i, 3, str);
		if (set[i].type == FAKEBET_TYPE) {
			temp = "模拟下注";
		}
		else {
			temp = "真实下注";
			if(!set[i].isSmallBet)	realProfit += set[i].payoff;
		}
		m_DetailList.SetItemText(i, 4, CString(temp.c_str()));
		str.Format(_T("%.2f"), set[i].payoff);
		m_DetailList.SetItemText(i, 5, str);

		std::string bankerCard = "";
		std::string playerCard = "";
		set[i].getCard(bankerCard, playerCard);

		m_DetailList.SetItemText(i, 6, CString(playerCard.c_str()));
		m_DetailList.SetItemText(i, 7, CString(bankerCard.c_str()));

		str.Format(_T("%.2f"), realProfit);
		m_DetailList.SetItemText(i, 8, str);
	}
}


void DetailDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	showDetail();
}

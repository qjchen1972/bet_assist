// GameShowDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include "GameShowDlg.h"
#include "afxdialogex.h"
#include"setup.h"
#include"websocket.h"
#include"DetailDlg.h"

// GameShowDlg 对话框

IMPLEMENT_DYNAMIC(GameShowDlg, CDialogEx)

GameShowDlg::GameShowDlg(int index, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GAMESHOWDLG, pParent)
{
	nIndex = index;
}

GameShowDlg::~GameShowDlg()
{
	if (map) delete map;
	if (detailDlg) delete detailDlg;
}

void GameShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAMECHECK, m_NameCheck);
	DDX_Control(pDX, IDC_ROADMAP, m_RoadCtrl);
	DDX_Control(pDX, IDC_DETAILBTN, m_DetailBtn);
	DDX_Control(pDX, IDC_PROFITEDIT, m_ProfitEdit);
	DDX_Control(pDX, IDC_BETEDIT, m_BetEdit);
	DDX_Control(pDX, IDC_BETCOLORSTATIC, m_BetColorCtrl);
	DDX_Control(pDX, IDC_STATUSSTATIC, m_StatusStatic);
	DDX_Control(pDX, IDC_BANKERSTATIC, m_BankerCtrl);
	DDX_Control(pDX, IDC_PLAYERSTATIC, m_PlayerCtrl);
	DDX_Control(pDX, IDC_MIDSTATIC, m_MidCtrl);
}


BEGIN_MESSAGE_MAP(GameShowDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_NAMECHECK, &GameShowDlg::OnBnClickedNamecheck)
	ON_BN_CLICKED(IDC_DETAILBTN, &GameShowDlg::OnBnClickedDetailbtn)
END_MESSAGE_MAP()


// GameShowDlg 消息处理程序


void GameShowDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	drawRoad(false);
	setStatus();
	drawBet(false);
	setProfit();
}


BOOL GameShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CString caption(gConfig.gameSite[nIndex].name.c_str());
	m_NameCheck.SetWindowText(caption);
	CRect rect;
	m_RoadCtrl.GetClientRect(&rect);
	map = new RoadImage();
	map->setRect(rect);
	m_NameCheck.SetCheck(1);
	
	CFont font;
	font.CreatePointFont(150, _T("宋体"));
	m_StatusStatic.SetFont(&font);

	detailDlg = new DetailDlg(nIndex);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



void GameShowDlg::OnBnClickedNamecheck()
{
	// TODO: 在此添加控件通知处理程序代码
	int type = m_NameCheck.GetCheck();
	setGameCtrl(type);
}

void GameShowDlg::setGameCtrl(int type) {

	//if (gGamer[nIndex]->pGameProcer->gameStatus.status == INIT) return;
	if (type) {
		if (!gGamer[nIndex]->pGameProcer->gameData.isStop) return;
		gGamer[nIndex]->pGameProcer->gameData.isStop = false;
		//if (gConfig.sid.size() == 0) return;
		//gGamer[nIndex]->pGameProcer->set_status(gGamer[nIndex]->pGameProcer->gameStatus.status);
	}
	else {
		if (gGamer[nIndex]->pGameProcer->gameData.isStop) return;
		gGamer[nIndex]->pGameProcer->gameData.isStop = true;
		//gGamer[nIndex]->pGameProcer->set_status(gGamer[nIndex]->pGameProcer->gameStatus.status);
	}
	if (gConfig.sid.size() == 0) return;
	gGamer[nIndex]->pGameProcer->set_status(gGamer[nIndex]->pGameProcer->gameStatus.status);
}

void GameShowDlg::OnBnClickedDetailbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	if (detailDlg) detailDlg->DoModal();
}

void GameShowDlg::updateNum(CDC* dc, CRect rect, int num, COLORREF color) {

	dc->FillSolidRect(rect, color);
	CPen pen(PS_SOLID, 2, RGB(255, 255, 255));
	CPen *oldpen = dc->SelectObject(&pen);
	COLORREF oldbc = dc->GetBkColor();
	COLORREF oldtc = dc->GetTextColor();
	dc->SetTextColor(RGB(234, 242, 255));
	dc->SetBkColor(color);
	CString str;
	str.Format(_T("%d"), num);
	dc->DrawText(str, rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	dc->SelectObject(oldpen);
	dc->SetTextColor(oldtc);
	dc->SetBkColor(oldbc);
}

void GameShowDlg::drawRoad(bool ClientMode, int init) {

	std::vector<Round_t> road;
	if (!gGamer[nIndex]) return;
	if (!gGamer[nIndex]->pGameProcer) return;

	if (gConfig.useNew)
		gGamer[nIndex]->pGameProcer->gameData.getNewRound(road);
	else
		gGamer[nIndex]->pGameProcer->gameData.getLastRounds(road, map->size);

	CRect rect;
	if (ClientMode) {
		if (road.size() >= map->size || road.size() == 0 || init) {
			CDC* pDC = m_RoadCtrl.GetDC();
			map->drawRoad(pDC, road);
			m_RoadCtrl.ReleaseDC(pDC);

			bankerNum = playerNum = midNum = 0;
			for (int i = 0; i < road.size(); i++) {
				if (i >= map->size) break;
				if (road[i].cardAns == BANKER) bankerNum++;
				else if (road[i].cardAns == PLAYER) playerNum++;
				else midNum++;
			}

			pDC = m_BankerCtrl.GetDC();
			m_BankerCtrl.GetClientRect(&rect);
			updateNum(pDC, rect, bankerNum, BANKERCOLOR);
			m_BankerCtrl.ReleaseDC(pDC);

			pDC = m_PlayerCtrl.GetDC();
			m_PlayerCtrl.GetClientRect(&rect);
			updateNum(pDC, rect, playerNum, PLAYERCOLOR);
			m_PlayerCtrl.ReleaseDC(pDC);

			pDC = m_MidCtrl.GetDC();
			m_MidCtrl.GetClientRect(&rect);
			updateNum(pDC, rect, midNum, MIDCOLOR);
			m_MidCtrl.ReleaseDC(pDC);
		}
		else {

			CDC* pDC = m_RoadCtrl.GetDC();
			map->draw_ans(pDC, road[0], road.size() - 1);
			m_RoadCtrl.ReleaseDC(pDC);

			if (road[0].cardAns == BANKER) {
				bankerNum++;
				pDC = m_BankerCtrl.GetDC();
				m_BankerCtrl.GetClientRect(&rect);
				updateNum(pDC, rect, bankerNum, BANKERCOLOR);
				m_BankerCtrl.ReleaseDC(pDC);
			}
			else if (road[0].cardAns == PLAYER) {
				playerNum++;
				pDC = m_PlayerCtrl.GetDC();
				m_PlayerCtrl.GetClientRect(&rect);
				updateNum(pDC, rect, playerNum, PLAYERCOLOR);
				m_PlayerCtrl.ReleaseDC(pDC);
			}
			else {
				midNum++;
				pDC = m_MidCtrl.GetDC();
				m_MidCtrl.GetClientRect(&rect);
				updateNum(pDC, rect, midNum, MIDCOLOR);
				m_MidCtrl.ReleaseDC(pDC);
			}
		}
	}
	else {
		PAINTSTRUCT ps;
		CDC* pDC = m_RoadCtrl.BeginPaint(&ps);
		map->drawRoad(pDC, road);
		m_RoadCtrl.EndPaint(&ps);

		bankerNum = playerNum = midNum = 0;
		for (int i = 0; i < road.size(); i++) {
			if (i >= map->size) break;
			if (road[i].cardAns == BANKER) bankerNum++;
			else if (road[i].cardAns == PLAYER) playerNum++;
			else midNum++;
		}

		CRect rect;

		pDC = m_BankerCtrl.BeginPaint(&ps);		
		m_BankerCtrl.GetClientRect(&rect);
		updateNum(pDC, rect, bankerNum, BANKERCOLOR);
		m_BankerCtrl.EndPaint(&ps);


		pDC = m_PlayerCtrl.BeginPaint(&ps);
		m_PlayerCtrl.GetClientRect(&rect);
		updateNum(pDC, rect, playerNum, PLAYERCOLOR);
		m_PlayerCtrl.EndPaint(&ps);

		pDC = m_MidCtrl.BeginPaint(&ps);
		m_MidCtrl.GetClientRect(&rect);
		updateNum(pDC, rect, midNum, MIDCOLOR);
		m_MidCtrl.EndPaint(&ps);
	}
}

void GameShowDlg::setRoad(int init) {
	drawRoad(true, init);
}

void GameShowDlg::setStatus() {

	if (!gGamer[nIndex]) return;
	if (!gGamer[nIndex]->pGameProcer) return;

	std::string status = gGamer[nIndex]->pGameProcer->gameStatus.get_strStatus();
	m_StatusStatic.SetWindowText(CString(status.c_str()));

}

void GameShowDlg::setProfit() {

	if (!gGamer[nIndex]) return;
	if (!gGamer[nIndex]->pGameProcer) return;
	float profit = gGamer[nIndex]->pGameProcer->gameData.tempProcer.realStat.profit;
	CString str;
	str.Format(_T("%.2f"), profit);
	m_ProfitEdit.SetWindowText(str);
}

void GameShowDlg::drawBet(bool ClientMode) {
	
	if (!gGamer[nIndex]) return;
	if (!gGamer[nIndex]->pGameProcer) return;

	int money, b1, b2, b3;
	if (gGamer[nIndex]->pGameProcer->gameData.roundIsOver) {
		money = 0;
		b1 = b2 = b3 = 0;
	}
	else {
		money =
			gGamer[nIndex]->pGameProcer->gameData.cur_round.b1 +
			gGamer[nIndex]->pGameProcer->gameData.cur_round.b2 +
			gGamer[nIndex]->pGameProcer->gameData.cur_round.b3;
		b1 = gGamer[nIndex]->pGameProcer->gameData.cur_round.b1;
		b2 = gGamer[nIndex]->pGameProcer->gameData.cur_round.b2;
		b3 = gGamer[nIndex]->pGameProcer->gameData.cur_round.b3;
	}

	CString str;
	str.Format(_T("%d"), money);
	m_BetEdit.SetWindowText(str);
	CRect rect;
	m_BetColorCtrl.GetClientRect(&rect);
	CRect rc(
		rect.left + 1,
		rect.top + 1,
		rect.right - 1,
		rect.bottom - 1);
	CDC* dc;
	PAINTSTRUCT ps;
	CBrush brush;

	if (ClientMode) 
		dc = m_BetColorCtrl.GetDC();
	else 
		dc = m_BetColorCtrl.BeginPaint(&ps);
	
	if (b1 > 0) {
		brush.CreateSolidBrush(BANKERCOLOR);
		CBrush *oldbrush = dc->SelectObject(&brush);
		dc->Ellipse(rc);
		dc->SelectObject(oldbrush);
	}
	else if (b2 > 0) {
		brush.CreateSolidBrush(PLAYERCOLOR);
		CBrush *oldbrush = dc->SelectObject(&brush);
		dc->Ellipse(rc);
		dc->SelectObject(oldbrush);
	}
	else if (b3 > 0) {
		brush.CreateSolidBrush(MIDCOLOR);
		CBrush *oldbrush = dc->SelectObject(&brush);
		dc->Ellipse(rc);
		dc->SelectObject(oldbrush);
	}
	else {
		dc->FillSolidRect(rc, RGB(255, 255, 255));
	}

	if (ClientMode) m_BetColorCtrl.ReleaseDC(dc);	
	else m_BetColorCtrl.EndPaint(&ps);
	
}

void GameShowDlg::setBet() {
	drawBet(true);
}